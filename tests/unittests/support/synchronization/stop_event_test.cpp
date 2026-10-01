// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/synchronization/stop_event.h"
#include <gtest/gtest.h>
#include <thread>

using namespace ocudu;

template <typename T>
class stop_event_test : public ::testing::Test
{
protected:
  using Type = T;
  std::vector<std::unique_ptr<std::thread>> workers;
  Type                                      ev;
};

using stop_event_types = ::testing::Types<stop_event_source, rt_stop_event_source>;
TYPED_TEST_SUITE(stop_event_test, stop_event_types);

TYPED_TEST(stop_event_test, no_observers_stop_is_noop)
{
  this->ev.stop();
}

TYPED_TEST(stop_event_test, stop_blocks_until_observer_is_reset)
{
  auto obs = this->ev.get_token();

  ASSERT_FALSE(obs.is_stop_requested());

  // Call stop from another thread to avoid deadlock.
  std::atomic<bool> finished{false};
  this->workers.push_back(std::make_unique<std::thread>([this, &finished]() {
    this->ev.stop();
    finished = true;
  }));

  // Stop was requested, but event source did not unlock.
  while (not obs.is_stop_requested()) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
  ASSERT_FALSE(finished);

  // Now stop() will unlock.
  obs.reset();
  this->workers[0]->join();
  ASSERT_TRUE(finished);
}

TYPED_TEST(stop_event_test, multiple_stop_block_until_all_observers_are_reset)
{
  std::vector<std::invoke_result_t<decltype(&TestFixture::Type::get_token), typename TestFixture::Type>> observers;
  const int num_observers = 10;
  const int num_stoppers  = 5;

  for (int i = 0; i < num_observers; i++) {
    observers.emplace_back(this->ev.get_token());
  }
  std::vector<std::atomic<bool>> finished(num_stoppers);
  for (int i = 0; i < num_stoppers; i++) {
    this->workers.push_back(std::make_unique<std::thread>([this, &finished, i]() {
      this->ev.stop();
      finished[i] = true;
    }));
  }

  while (not observers[0].is_stop_requested()) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }

  for (int i = 0; i != num_observers; i++) {
    ASSERT_TRUE(observers[i].is_stop_requested()) << "All observers should be the stop() signal at this point";
  }
  for (int i = 0; i != num_stoppers; i++) {
    ASSERT_FALSE(finished[i]) << "No stopper should have finished yet";
  }

  // All except one observer are reset.
  for (int i = 0; i < num_observers - 1; i++) {
    observers[i].reset();
  }
  for (int i = 0; i != num_stoppers; i++) {
    ASSERT_FALSE(finished[i]) << "No stopper should have finished yet";
  }

  // All stoppers should unlock now.
  observers[num_observers - 1].reset();
  for (int i = 0; i != num_stoppers; i++) {
    this->workers[i]->join();
    ASSERT_TRUE(finished[i]) << "All stoppers should have finished now";
  }
}

TYPED_TEST(stop_event_test, reset_on_event_not_stopped_is_noop)
{
  auto obs = this->ev.get_token();
  ASSERT_EQ(this->ev.nof_tokens_approx(), 1);
  this->ev.reset();
  ASSERT_EQ(this->ev.nof_tokens_approx(), 1);
}

TYPED_TEST(stop_event_test, reset_waits_for_ungoing_stop)
{
  auto obs = this->ev.get_token();
  this->workers.push_back(std::make_unique<std::thread>([this]() { this->ev.stop(); }));
  while (not obs.is_stop_requested()) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }

  std::atomic<bool> reset_finished{false};
  this->workers.push_back(std::make_unique<std::thread>([this, &reset_finished]() {
    this->ev.reset();
    reset_finished = true;
  }));

  ASSERT_FALSE(reset_finished);
  obs.reset();
  this->workers[0]->join();
  this->workers[1]->join();
}

TYPED_TEST(stop_event_test, token_to_already_stopped_event_is_empty)
{
  this->ev.stop();
  auto obs = this->ev.get_token();
  ASSERT_TRUE(obs.is_stop_requested());
}

TEST(stop_event_dtor_test, event_concurrent_destruction_many_threads)
{
  // Note: I use std::unique_ptr, because with std::optional, ASAN/TSAN do not catch any heap-use-after-free.
  std::unique_ptr<stop_event_source> ev;
  const unsigned                     nof_workers = 4;
  for (unsigned i = 0, nof_runs = 1000; i != nof_runs; i++) {
    std::vector<std::thread> workers;
    workers.reserve(nof_workers);
    ev = std::make_unique<stop_event_source>();

    for (unsigned j = 0; j != nof_workers - 1; j++) {
      workers.emplace_back([tk = ev->get_token()]() mutable { tk.reset(); });
    }
    workers.emplace_back([&ev]() { ev.reset(); });

    while (not workers.empty()) {
      workers.back().join();
      workers.pop_back();
    }
  }
}
