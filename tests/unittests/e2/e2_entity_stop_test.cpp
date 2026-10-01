// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "common/e2_test_helpers.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>
#include <thread>

using namespace ocudu;

namespace {

/// \brief Task executor that rejects the next deferred tasks on demand, and forwards everything else.
///
/// Rejection mimics the real executors: task_executor::defer() takes the task by value, so a rejected task is
/// destroyed before defer() returns.
class flaky_task_executor : public task_executor
{
public:
  explicit flaky_task_executor(task_executor& delegate_) : delegate(delegate_) {}

  /// Makes the next \c n deferred tasks be rejected.
  void reject_next(unsigned n) { n_rejects = n; }

  unsigned nof_rejected() const { return rejected_count; }

  [[nodiscard]] bool execute(unique_task task) override { return delegate.execute(std::move(task)); }

  [[nodiscard]] bool defer(unique_task task) override
  {
    if (n_rejects > 0) {
      --n_rejects;
      ++rejected_count;
      // Note: task is destroyed here, as it would be by an executor that failed to enqueue it.
      return false;
    }
    return delegate.defer(std::move(task));
  }

private:
  task_executor& delegate;
  unsigned       n_rejects      = 0;
  unsigned       rejected_count = 0;
};

/// E2 connection client that records when the E2 TNL association is set up and torn down.
class observing_e2_connection_client : public e2_connection_client
{
public:
  std::unique_ptr<e2_message_notifier>
  handle_e2_connection_request(std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier_) override
  {
    e2_rx_pdu_notifier   = std::move(e2_rx_pdu_notifier_);
    connection_requested = true;
    return std::make_unique<dummy_e2_tx_pdu_notifier>(last_tx_e2_pdu, [this]() {
      tnl_removed = true;
      // Notify the E2 interface that the Rx path got disconnected as well.
      e2_rx_pdu_notifier.reset();
    });
  }

  e2_message        last_tx_e2_pdu;
  std::atomic<bool> connection_requested{false};
  std::atomic<bool> tnl_removed{false};

private:
  std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier;
};

} // namespace

/// Fixture that builds an E2 DU agent on top of an executor whose dispatches can be made to fail.
class e2_entity_stop_test : public e2_test_base
{
protected:
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    cfg                  = config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    du_metrics                      = std::make_unique<dummy_e2_du_metrics>();
    f1ap_ue_id_mapper               = std::make_unique<dummy_f1ap_ue_id_translator>();
    factory                         = timer_factory{timers, task_worker};
    du_rc_param_configurator        = std::make_unique<dummy_du_configurator>();
    auto owned_collector            = std::make_unique<e2_node_component_config_collector>(task_worker, 1);
    node_component_config_collector = owned_collector.get();
    e2agent                         = create_e2_du_agent(cfg,
                                 e2ap_dependencies{.e2_client                      = *observing_client,
                                                                           .e2_metrics_var                 = &(*du_metrics),
                                                                           .f1ap_ue_id_translator          = &(*f1ap_ue_id_mapper),
                                                                           .du_configurator                = &(*du_rc_param_configurator),
                                                                           .timers                         = factory,
                                                                           .e2_exec                        = *flaky_exec,
                                                                           .node_component_config_provider = std::move(owned_collector),
                                                                           .logger = ocudulog::fetch_basic_logger("TEST")});
  }

  void TearDown() override
  {
    // Destroy the agent before the executor and the connection client that it points to.
    e2agent.reset();
    ocudulog::flush();
  }

  /// Runs pending tasks until \c predicate holds. Returns false if it did not hold before the timeout.
  template <typename Predicate>
  bool run_until(Predicate predicate)
  {
    for (unsigned i = 0; i != 10000; ++i) {
      if (predicate()) {
        return true;
      }
      task_worker.run_pending_tasks();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return predicate();
  }

  std::unique_ptr<observing_e2_connection_client> observing_client = std::make_unique<observing_e2_connection_client>();
  std::unique_ptr<flaky_task_executor>            flaky_exec       = std::make_unique<flaky_task_executor>(task_worker);

  /// Set by the thread that calls stop(). Fixture members, so that they outlive the test body.
  std::atomic<bool> stop_returned{false};
  std::atomic<bool> tnl_removed_on_return{false};
};

/// \brief stop() must not return before the RIC disconnection it dispatched has run.
///
/// A rejected defer destroys the task it was given. If that task held the only stop token, its destruction signals the
/// stop event, and stop() returns while the disconnect coroutine has not even been dispatched.
TEST_F(e2_entity_stop_test, stop_does_not_return_before_ric_disconnection_runs)
{
  // Deliver a minimal F1 component config so the setup coroutine proceeds without suspending.
  node_component_config_collector->deliver(e2_node_component_interface_type::f1, byte_buffer{}, byte_buffer{});
  task_worker.run_pending_tasks();

  // Establish the E2 TNL association, so that stopping has something to tear down.
  e2agent->start();
  ASSERT_TRUE(observing_client->connection_requested) << "the E2 TNL association was never established";

  // Complete the E2 setup, so that the main control loop is left idle before stop() is called. Otherwise the stop
  // coroutine would queue behind a suspended setup routine and never run.
  const unsigned transaction_id = get_transaction_id(observing_client->last_tx_e2_pdu.pdu).value();
  e2_message     setup_response = generate_e2_setup_response(transaction_id);
  setup_response.pdu.successful_outcome()
      .value.e2setup_resp()
      ->ran_functions_accepted[0]
      ->ran_function_id_item()
      .ran_function_id = e2sm_kpm_asn1_packer::ran_func_id;
  e2agent->get_e2_interface().handle_message(setup_response);
  ASSERT_TRUE(e2agent->is_ric_connected());
  ASSERT_FALSE(observing_client->tnl_removed) << "the E2 TNL association was torn down before stop() was called";

  // Reject only the stop task, so that a rejection cannot be consumed by an unrelated dispatch.
  flaky_exec->reject_next(1);

  // stop() blocks until the stop event is signalled, so it has to run on its own thread.
  std::thread stopper([this]() {
    e2agent->stop();
    tnl_removed_on_return = observing_client->tnl_removed.load();
    stop_returned         = true;
  });

  if (not run_until([this]() { return stop_returned.load(); })) {
    // stop() is stuck, so the stopper thread can neither be joined nor safely left behind: it is blocked inside
    // e2agent->stop(), so tearing the fixture down would destroy an object that has a live call on it, and would also
    // release the last stop token and wake the thread onto freed memory. The stop event is local to stop(), so it
    // cannot be signalled from here either. Report the failure and end the process before any cleanup can run.
    ADD_FAILURE() << "stop() did not return: the dispatched stop task chain never completed";
    stopper.detach();
    std::fflush(nullptr);
    std::_Exit(1);
  }
  stopper.join();

  // Guard against a vacuous pass: the test only means anything if a dispatch was actually rejected.
  ASSERT_EQ(flaky_exec->nof_rejected(), 1U) << "the stop task was not rejected, so the retry path was not exercised";

  ASSERT_TRUE(tnl_removed_on_return) << "stop() returned before the RIC disconnection ran";
}
