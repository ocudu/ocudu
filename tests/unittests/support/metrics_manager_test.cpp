// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "apps/services/buffer_pool/metrics/buffer_pool_metrics.h"
#include "apps/services/metrics/metrics_channel.h"
#include "apps/services/metrics/metrics_consumer.h"
#include "apps/services/metrics/metrics_manager.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "ocudu/adt/unique_function.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include "ocudu/support/timers.h"
#include <array>
#include <atomic>
#include <future>
#include <gtest/gtest.h>
#include <memory>
#include <numeric>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

using namespace ocudu;
using namespace ocudu::app_services;

// The deferred fan-out task must stay within the unique_task small-buffer optimization for small
// metric types: its closure captures the metric, the channel pointer and the stop token. Growing
// any of the three (e.g. adding one more capture) silently pushes every metric type onto the heap;
// this assertion keeps the smallest production metric type on the allocation-free path.
static_assert(sizeof(buffer_pool_metrics_impl) + sizeof(metrics_channel<buffer_pool_metrics_impl>*) +
                      sizeof(stop_event_token) <=
                  default_unique_task_buffer_size,
              "metrics fan-out closure no longer fits the unique_task small buffer");

namespace {

struct test_metric final {
  int value = 0;
};

struct other_test_metric final {
  int value = 0;
};

/// \brief Metric type that counts how many times it is copied and moved.
///
/// Verifies the zero-copy contract of the push path: an rvalue report must travel to the consumers
/// without a single copy.
struct copy_counting_metric final {
  int value = 0;

  copy_counting_metric() = default;
  explicit copy_counting_metric(int value_) : value(value_) {}

  copy_counting_metric(const copy_counting_metric& other) :
    value(other.value), copies(other.copies + 1), moves(other.moves)
  {
  }
  copy_counting_metric(copy_counting_metric&& other) noexcept :
    value(other.value), copies(other.copies), moves(other.moves + 1)
  {
  }
  copy_counting_metric& operator=(const copy_counting_metric&) = default;
  copy_counting_metric& operator=(copy_counting_metric&&)      = default;

  unsigned copies = 0;
  unsigned moves  = 0;
};

/// \brief Metric type that cannot be copied.
///
/// Compile-time proof that the delivery path never requires copying a metric: a producer holding
/// the only instance can hand it over to the channel.
struct move_only_metric final {
  int                  value = 0;
  std::unique_ptr<int> token;
};

/// \brief Metric type larger than the small buffer of a deferred task.
///
/// Exercises the heap-allocating path of the deferred fan-out task: the closure does not fit the
/// unique_task small buffer and the report must still be delivered.
struct large_metric final {
  int                            value = 0;
  std::array<unsigned char, 256> padding{};
};
static_assert(sizeof(large_metric) > default_unique_task_buffer_size,
              "large_metric is expected to exceed the unique_task small buffer");

/// Producer that pushes metrics through the base \c metrics_notifier<MetricType>& interface,
/// exactly like the production producers (resource usage, executor, buffer pool, ...). This
/// exercises the real contract that dispatches through the typed notifier reference rather than
/// through the concrete channel.
class test_metrics_producer : public metrics_producer
{
public:
  explicit test_metrics_producer(metrics_notifier<test_metric>& notifier_) : notifier(notifier_) {}

  void push(test_metric metric) { notifier.on_new_metric(metric); }

  void on_new_report_period() override {}

private:
  metrics_notifier<test_metric>& notifier;
};

template <typename MetricType>
class recording_consumer : public metrics_consumer<MetricType>
{
public:
  void handle_metric(const MetricType& metric) override { received_values.push_back(metric.value); }

  std::vector<int> received_values;
};

/// Consumer that records the copy/move counters of the received report.
class copy_counting_metric_consumer : public metrics_consumer<copy_counting_metric>
{
public:
  void handle_metric(const copy_counting_metric& metric) override
  {
    received = true;
    copies   = metric.copies;
    moves    = metric.moves;
  }

  bool     received = false;
  unsigned copies   = 0;
  unsigned moves    = 0;
};

class rejecting_task_executor : public task_executor
{
public:
  [[nodiscard]] bool execute([[maybe_unused]] unique_task task) override { return true; }

  [[nodiscard]] bool defer([[maybe_unused]] unique_task task) override { return false; }
};

/// Executor that accepts tasks but parks them in a queue, mimicking a busy metrics executor. The
/// parked tasks only run when the test calls \c run_all, which lets a test hold a metrics report
/// in flight for as long as it needs.
class holding_task_executor : public task_executor
{
public:
  [[nodiscard]] bool execute(unique_task task) override
  {
    tasks.push_back(std::move(task));
    return true;
  }

  [[nodiscard]] bool defer(unique_task task) override
  {
    tasks.push_back(std::move(task));
    return true;
  }

  /// Runs and destroys every parked task.
  void run_all()
  {
    std::vector<unique_task> parked;
    parked.swap(tasks);
    for (auto& task : parked) {
      task();
    }
  }

  std::vector<unique_task> tasks;
};

/// Producer that counts how many report periods the periodic controller commanded it to report.
class counting_producer : public metrics_producer
{
public:
  void on_new_report_period() override { ++nof_periods; }

  unsigned nof_periods = 0;
};

/// Consumer that records into storage owned by the test (not by the channel), so the test can read
/// the result after the manager, and with it the channel, has been destroyed.
template <typename MetricType>
class externally_recording_consumer : public metrics_consumer<MetricType>
{
public:
  explicit externally_recording_consumer(std::atomic<int>& sink_) : sink(sink_) {}

  void handle_metric(const MetricType& metric) override { sink.store(metric.value); }

private:
  std::atomic<int>& sink;
};

constexpr int test_metric_value       = 7;
constexpr int dispatched_metric_value = 42;
constexpr int report_period_ms        = 100;

/// Creates the channels first and the manager last, mirroring the lifetime topology of an
/// application: the manager takes ownership of the channels, so it is the one tearing them down
/// (after draining every in-flight report).
class metrics_fixture
{
public:
  metrics_fixture() { ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::warning); }

  /// Creates a channel for the given metric type in the collection owned by this fixture.
  template <typename MetricType>
  metrics_channel<MetricType>& add_channel()
  {
    return make_channel<MetricType>(channels);
  }

  /// Creates a metrics manager connected to the channels created so far.
  metrics_manager& start_manager()
  {
    manager.emplace(ocudulog::fetch_basic_logger("TEST"),
                    executor,
                    std::move(channels),
                    timers,
                    std::chrono::milliseconds{report_period_ms});
    return *manager;
  }

  inline_task_executor                     executor;
  timer_manager                            timers{1};
  app_services::metrics_channel_collection channels;
  std::optional<metrics_manager>           manager;
};

} // namespace

TEST(metrics_manager_test, dispatches_concrete_metric_to_typed_consumer)
{
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<test_metric>();
  auto  consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  fixture.start_manager();
  channel.on_new_metric(test_metric{dispatched_metric_value});

  EXPECT_EQ(consumer_ptr->received_values, std::vector<int>{dispatched_metric_value});
}

TEST(metrics_manager_test, dispatches_metric_pushed_through_metrics_notifier_base_reference)
{
  // Models the production contract: the producer only holds a `metrics_notifier<T>&`, not the
  // concrete channel. This guarantees the dispatch path used in production is the one under test.
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<test_metric>();
  auto  consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  fixture.start_manager();

  test_metrics_producer producer(channel);
  producer.push(test_metric{dispatched_metric_value});

  EXPECT_EQ(consumer_ptr->received_values, std::vector<int>{dispatched_metric_value});
}

TEST(metrics_manager_test, dispatches_to_all_registered_consumers_of_the_channel)
{
  metrics_fixture fixture;

  auto& channel    = fixture.add_channel<test_metric>();
  auto  first      = std::make_unique<recording_consumer<test_metric>>();
  auto  second     = std::make_unique<recording_consumer<test_metric>>();
  auto  third      = std::make_unique<recording_consumer<test_metric>>();
  auto* first_ptr  = first.get();
  auto* second_ptr = second.get();
  auto* third_ptr  = third.get();
  channel.add_consumer(std::move(first));
  channel.add_consumer(std::move(second));
  channel.add_consumer(std::move(third));

  fixture.start_manager();
  channel.on_new_metric(test_metric{dispatched_metric_value});

  EXPECT_EQ(first_ptr->received_values, std::vector<int>{dispatched_metric_value});
  EXPECT_EQ(second_ptr->received_values, std::vector<int>{dispatched_metric_value});
  EXPECT_EQ(third_ptr->received_values, std::vector<int>{dispatched_metric_value});
}

TEST(metrics_manager_test, channels_of_distinct_metric_types_do_not_cross_dispatch)
{
  metrics_fixture fixture;

  auto& test_channel   = fixture.add_channel<test_metric>();
  auto& other_channel  = fixture.add_channel<other_test_metric>();
  auto  test_consumer  = std::make_unique<recording_consumer<test_metric>>();
  auto  other_consumer = std::make_unique<recording_consumer<other_test_metric>>();
  auto* test_ptr       = test_consumer.get();
  auto* other_ptr      = other_consumer.get();
  test_channel.add_consumer(std::move(test_consumer));
  other_channel.add_consumer(std::move(other_consumer));

  fixture.start_manager();
  test_channel.on_new_metric(test_metric{dispatched_metric_value});
  other_channel.on_new_metric(other_test_metric{test_metric_value});

  EXPECT_EQ(test_ptr->received_values, std::vector<int>{dispatched_metric_value});
  EXPECT_EQ(other_ptr->received_values, std::vector<int>{test_metric_value});
}

TEST(metrics_manager_test, does_not_dispatch_when_no_consumer_is_registered)
{
  metrics_fixture fixture;

  auto& channel = fixture.add_channel<test_metric>();

  fixture.start_manager();
  EXPECT_NO_THROW(channel.on_new_metric(test_metric{dispatched_metric_value}));
}

TEST(metrics_manager_test, does_not_dispatch_after_stop_is_requested)
{
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<test_metric>();
  auto  consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  auto& manager = fixture.start_manager();
  manager.start();
  manager.stop();
  channel.on_new_metric(test_metric{dispatched_metric_value});

  EXPECT_TRUE(consumer_ptr->received_values.empty());
}

TEST(metrics_manager_test, dispatch_works_again_after_a_stop_start_cycle)
{
  // The manager must survive a stop()/start() cycle: start() resets the stop request, and reports
  // submitted afterwards reach the consumers again.
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<test_metric>();
  auto  consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  auto& manager = fixture.start_manager();
  manager.start();
  manager.stop();
  channel.on_new_metric(test_metric{test_metric_value});
  EXPECT_TRUE(consumer_ptr->received_values.empty());

  manager.start();
  channel.on_new_metric(test_metric{dispatched_metric_value});
  EXPECT_EQ(consumer_ptr->received_values, std::vector<int>{dispatched_metric_value});
}

TEST(metrics_manager_test, stop_blocks_until_inflight_deferred_report_completes)
{
  // The stop token captured by the deferred task must keep stop() blocked until the task has
  // actually run: otherwise the manager could tear the channels down while an in-flight report
  // is still about to fan out to their consumers.
  app_services::metrics_channel_collection channels;
  auto&                                    channel      = make_channel<test_metric>(channels);
  auto                                     consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto*                                    consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::warning);
  holding_task_executor executor;
  timer_manager         timers{1};
  metrics_manager       manager(ocudulog::fetch_basic_logger("TEST"),
                          executor,
                          std::move(channels),
                          timers,
                          std::chrono::milliseconds{report_period_ms});

  // Queue a report and keep it parked inside the executor.
  channel.on_new_metric(test_metric{dispatched_metric_value});
  ASSERT_EQ(executor.tasks.size(), 1U);
  EXPECT_TRUE(consumer_ptr->received_values.empty());

  std::atomic<bool> stop_returned{false};
  std::thread       stopper([&manager, &stop_returned]() {
    manager.stop();
    stop_returned.store(true, std::memory_order_release);
  });

  // While the report is parked, stop() must not return and the report must not be lost.
  std::this_thread::sleep_for(std::chrono::milliseconds{200});
  EXPECT_FALSE(stop_returned.load(std::memory_order_acquire));
  EXPECT_TRUE(consumer_ptr->received_values.empty());

  // Running the parked report releases the captured token, which unblocks stop().
  executor.run_all();
  stopper.join();
  EXPECT_TRUE(stop_returned.load(std::memory_order_acquire));
  EXPECT_EQ(consumer_ptr->received_values, std::vector<int>{dispatched_metric_value});
}

TEST(metrics_manager_test, destroying_the_manager_without_stop_drains_inflight_reports)
{
  // Destroys the manager in a controlled thread: the destructor blocks in stop() while the
  // report stays parked and completes only after the report has run. Destroying in a separate
  // thread lets the test observe the block itself, not just its outcome.
  // The consumer records into storage owned by this test, because the consumer itself is
  // destroyed together with the manager that owns the channel.
  app_services::metrics_channel_collection channels;
  auto&                                    channel = make_channel<test_metric>(channels);
  std::atomic<int>                         received_value{0};
  channel.add_consumer(std::make_unique<externally_recording_consumer<test_metric>>(received_value));

  ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::warning);
  holding_task_executor executor;
  timer_manager         timers{1};

  // Runs the parked report when the test releases it. The promise orders the two accesses to
  // the task queue.
  std::promise<void> report_released;
  std::thread        runner([&executor, release = report_released.get_future()]() mutable {
    release.wait();
    executor.run_all();
  });

  std::optional<metrics_manager> manager;
  manager.emplace(ocudulog::fetch_basic_logger("TEST"),
                  executor,
                  std::move(channels),
                  timers,
                  std::chrono::milliseconds{report_period_ms});
  // No start()/stop(): destruction alone must drain the queue.
  channel.on_new_metric(test_metric{dispatched_metric_value});
  ASSERT_EQ(executor.tasks.size(), 1U);
  EXPECT_EQ(received_value.load(), 0);

  std::promise<void> destruction_finished;
  auto               finished = destruction_finished.get_future();
  std::thread        destroyer([&manager, &destruction_finished]() mutable {
    manager.reset();
    destruction_finished.set_value();
  });

  // While the report stays parked, destruction must not complete: the destructor is blocked
  // waiting for the report's stop token to be released.
  EXPECT_EQ(finished.wait_for(std::chrono::milliseconds{100}), std::future_status::timeout);

  // Releasing the parked report lets the destructor unblock and complete.
  report_released.set_value();
  destroyer.join();
  runner.join();

  EXPECT_EQ(finished.wait_for(std::chrono::milliseconds{0}), std::future_status::ready);
  EXPECT_EQ(received_value.load(), dispatched_metric_value);

  // Note: the channel reference must not be used from here on: the manager owned the channel
  // and it has been destroyed together with it.
}

TEST(metrics_manager_test, periodic_controller_commands_producers_every_report_period)
{
  // Drives the timer clock by hand (one tick == 1 ms), so the test is deterministic: the producers
  // must be commanded exactly once per elapsed report period and not at all before the first one.
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<test_metric>();
  auto  producer     = std::make_unique<counting_producer>();
  auto* producer_ptr = producer.get();
  channel.add_producer(std::move(producer));

  auto& manager = fixture.start_manager();
  manager.start();

  // One tick short of the first period: nothing must have been commanded yet.
  for (int i = 0; i != report_period_ms - 1; ++i) {
    fixture.timers.tick();
  }
  EXPECT_EQ(producer_ptr->nof_periods, 0U);

  // Complete the first period and two more.
  for (int p = 0; p != 3; ++p) {
    fixture.timers.tick();
    EXPECT_EQ(producer_ptr->nof_periods, p + 1U);
    for (int i = 1; i != report_period_ms; ++i) {
      fixture.timers.tick();
    }
  }

  // After stop() the timer is disarmed: ticking further must not command the producers again.
  manager.stop();
  for (int i = 0; i != 4 * report_period_ms; ++i) {
    fixture.timers.tick();
  }
  EXPECT_EQ(producer_ptr->nof_periods, 3U);
}

TEST(metrics_manager_test, reports_failed_dispatch_when_executor_cannot_defer)
{
  app_services::metrics_channel_collection channels;
  auto&                                    channel      = make_channel<test_metric>(channels);
  auto                                     consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto*                                    consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::warning);
  rejecting_task_executor executor;
  timer_manager           timers{1};
  metrics_manager         manager(ocudulog::fetch_basic_logger("TEST"),
                          executor,
                          std::move(channels),
                          timers,
                          std::chrono::milliseconds{report_period_ms});

  EXPECT_NO_THROW(channel.on_new_metric(test_metric{dispatched_metric_value}));
  EXPECT_TRUE(consumer_ptr->received_values.empty());
}

TEST(metrics_manager_test, channel_not_connected_to_a_manager_drops_reports)
{
  // Models a unit whose producers are built before the application creates the metrics manager:
  // the channel exists, but no manager has connected it to a runtime context yet, so the report
  // must be dropped without crashing.
  app_services::metrics_channel_collection channels;
  auto&                                    channel      = make_channel<test_metric>(channels);
  auto                                     consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto*                                    consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  EXPECT_NO_THROW(channel.on_new_metric(test_metric{dispatched_metric_value}));
  EXPECT_TRUE(consumer_ptr->received_values.empty());
}

TEST(metrics_manager_test, rvalue_report_reaches_consumers_without_a_single_copy)
{
  // The push path takes the report by value and moves it into the deferred task: an rvalue report
  // (the usual producer pattern) must reach the consumers with zero copies and a single move
  // (into the deferred task - the by-value parameter is direct-initialized from the prvalue).
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<copy_counting_metric>();
  auto  consumer     = std::make_unique<copy_counting_metric_consumer>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  fixture.start_manager();
  channel.on_new_metric(copy_counting_metric{dispatched_metric_value});

  ASSERT_TRUE(consumer_ptr->received);
  // Zero copies: the by-value parameter is direct-initialized from the prvalue and the report
  // only moves onwards (into the deferred task closure and, with it, into the unique_task).
  EXPECT_EQ(consumer_ptr->copies, 0U);
  EXPECT_GE(consumer_ptr->moves, 1U);
}

TEST(metrics_manager_test, lvalue_report_makes_exactly_one_copy_at_the_call_boundary)
{
  // A producer that submits an lvalue pays for exactly one copy, when the by-value parameter of
  // on_new_metric is initialized; from there the report is moved along.
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<copy_counting_metric>();
  auto  consumer     = std::make_unique<copy_counting_metric_consumer>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  fixture.start_manager();
  copy_counting_metric report{dispatched_metric_value};
  channel.on_new_metric(report);

  ASSERT_TRUE(consumer_ptr->received);
  // Exactly one copy, at the call boundary; from there the report only moves onwards.
  EXPECT_EQ(consumer_ptr->copies, 1U);
  EXPECT_GE(consumer_ptr->moves, 1U);
}

TEST(metrics_manager_test, channel_supports_move_only_metric_types)
{
  // Compile- and run-time proof that the delivery path never requires copying: a producer holding
  // the only instance of a move-only report can hand it over to the channel.
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<move_only_metric>();
  auto  consumer     = std::make_unique<recording_consumer<move_only_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  fixture.start_manager();
  move_only_metric report;
  report.value = dispatched_metric_value;
  report.token = std::make_unique<int>(1);
  channel.on_new_metric(std::move(report));

  EXPECT_EQ(consumer_ptr->received_values, std::vector<int>{dispatched_metric_value});
}

TEST(metrics_manager_test, channels_of_the_same_metric_type_dispatch_independently)
{
  // Each builder owns its channel: a report pushed into one channel must never reach the consumers
  // of another channel of the same metric type. Channels are not merged by type.
  metrics_fixture fixture;

  auto& first_channel  = fixture.add_channel<test_metric>();
  auto& second_channel = fixture.add_channel<test_metric>();
  auto  first          = std::make_unique<recording_consumer<test_metric>>();
  auto  second         = std::make_unique<recording_consumer<test_metric>>();
  auto* first_ptr      = first.get();
  auto* second_ptr     = second.get();
  first_channel.add_consumer(std::move(first));
  second_channel.add_consumer(std::move(second));

  fixture.start_manager();
  first_channel.on_new_metric(test_metric{dispatched_metric_value});

  EXPECT_EQ(first_ptr->received_values, std::vector<int>{dispatched_metric_value});
  EXPECT_TRUE(second_ptr->received_values.empty());
}

TEST(metrics_manager_test, channel_reference_stays_valid_as_the_collection_grows)
{
  // The collection stores the channels behind unique_ptrs, so growing it (and reallocating the
  // vector) must not invalidate the references make_channel handed out to the producers.
  metrics_fixture fixture;

  auto& channel = fixture.add_channel<test_metric>();
  for (unsigned i = 0; i != 64; ++i) {
    fixture.add_channel<other_test_metric>();
  }
  auto  consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  fixture.start_manager();
  channel.on_new_metric(test_metric{dispatched_metric_value});

  EXPECT_EQ(consumer_ptr->received_values, std::vector<int>{dispatched_metric_value});
}

TEST(metrics_manager_test, periodic_controller_commands_producers_of_every_channel)
{
  // The periodic controller must command the producers of every channel, not only those of the
  // first one.
  metrics_fixture fixture;

  auto& first_channel  = fixture.add_channel<test_metric>();
  auto& second_channel = fixture.add_channel<other_test_metric>();
  auto  first          = std::make_unique<counting_producer>();
  auto  second         = std::make_unique<counting_producer>();
  auto* first_ptr      = first.get();
  auto* second_ptr     = second.get();
  first_channel.add_producer(std::move(first));
  second_channel.add_producer(std::move(second));

  auto& manager = fixture.start_manager();
  manager.start();

  for (int i = 0; i != report_period_ms; ++i) {
    fixture.timers.tick();
  }
  EXPECT_EQ(first_ptr->nof_periods, 1U);
  EXPECT_EQ(second_ptr->nof_periods, 1U);
}

TEST(metrics_manager_test, multiple_reports_are_delivered_in_order)
{
  // Reports of the same channel are delivered sequentially, in push order, without losses.
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<test_metric>();
  auto  consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  fixture.start_manager();
  channel.on_new_metric(test_metric{1});
  channel.on_new_metric(test_metric{2});
  channel.on_new_metric(test_metric{3});

  EXPECT_EQ(consumer_ptr->received_values, (std::vector<int>{1, 2, 3}));
}

TEST(metrics_manager_test, channel_supports_multiple_producers)
{
  // Several producers can share one channel: the periodic controller sees both of them, and the
  // reports of each reach the shared consumers.
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<test_metric>();
  auto  consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  auto  first_producer  = std::make_unique<test_metrics_producer>(channel);
  auto  second_producer = std::make_unique<test_metrics_producer>(channel);
  auto* first_ptr       = first_producer.get();
  auto* second_ptr      = second_producer.get();
  channel.add_producer(std::move(first_producer));
  channel.add_producer(std::move(second_producer));

  fixture.start_manager();
  EXPECT_EQ(channel.get_producers().size(), 2U);

  first_ptr->push(test_metric{1});
  second_ptr->push(test_metric{2});

  EXPECT_EQ(consumer_ptr->received_values, (std::vector<int>{1, 2}));
}

TEST(metrics_manager_test, large_metric_exceeding_the_task_small_buffer_is_still_delivered)
{
  // Metric types whose closure does not fit the unique_task small buffer take the heap-allocating
  // path of the deferred task; the report must be delivered all the same.
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<large_metric>();
  auto  consumer     = std::make_unique<recording_consumer<large_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  fixture.start_manager();
  large_metric report;
  report.value = dispatched_metric_value;
  channel.on_new_metric(std::move(report));

  EXPECT_EQ(consumer_ptr->received_values, std::vector<int>{dispatched_metric_value});
}

TEST(metrics_manager_test, reports_pushed_from_another_thread_are_delivered)
{
  // The production push contexts are not the application thread (e.g. the executor metrics
  // backend pushes from a timer thread): reports pushed across a thread boundary must all be
  // delivered. The inline executor runs the fan-out synchronously on the pushing thread.
  metrics_fixture fixture;

  auto& channel      = fixture.add_channel<test_metric>();
  auto  consumer     = std::make_unique<recording_consumer<test_metric>>();
  auto* consumer_ptr = consumer.get();
  channel.add_consumer(std::move(consumer));

  fixture.start_manager();

  constexpr int nof_reports = 100;
  std::thread   pusher([&channel]() {
    for (int i = 0; i != nof_reports; ++i) {
      channel.on_new_metric(test_metric{i});
    }
  });
  pusher.join();

  std::vector<int> expected(nof_reports);
  std::iota(expected.begin(), expected.end(), 0);
  EXPECT_EQ(consumer_ptr->received_values, expected);
}

TEST(metrics_manager_test, manager_over_an_empty_channel_collection_starts_and_stops)
{
  // Degenerate application configuration with every metric disabled: no channel exists, and the
  // manager must still start and stop cleanly.
  metrics_fixture fixture;

  auto& manager = fixture.start_manager();
  EXPECT_NO_THROW(manager.start());
  EXPECT_NO_THROW(manager.stop());
}
