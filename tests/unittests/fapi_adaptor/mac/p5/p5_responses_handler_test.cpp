// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "p5_responses_handler.h"
#include "p5_transaction_outcome_manager.h"
#include "ocudu/fapi/common/error_indication.h"
#include "ocudu/fapi/p5/p5_messages.h"
#include "ocudu/mac/mac_cell_timing_context.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/timers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;

namespace {

/// Executor that rejects every task, as a full or stopped executor does.
class rejecting_executor : public task_executor
{
public:
  [[nodiscard]] bool execute(unique_task task) override { return false; }
  [[nodiscard]] bool defer(unique_task task) override { return false; }
};

/// Observes the outcome of a transaction without waiting for it.
using outcome_observer = protocol_transaction_outcome_observer<bool>;

/// Transactions of the P5 procedures, with an observer subscribed to each of them.
class p5_transactions_fixture : public ::testing::Test
{
protected:
  /// Timeout of the transactions, which never expires as the timers are not ticked.
  static constexpr std::chrono::milliseconds timeout{1000};

  p5_transactions_fixture() : transaction_manager(timer_factory{timers, executor})
  {
    param_observer.subscribe_to(transaction_manager.param_response_outcome, timeout);
    config_observer.subscribe_to(transaction_manager.config_response_outcome, timeout);
    stop_observer.subscribe_to(transaction_manager.stop_response_outcome, timeout);
    start_observer.subscribe_to(transaction_manager.start_outcome, timeout);
  }

  /// Returns true when the observer holds the given outcome.
  static bool has_outcome(const outcome_observer& observer, bool outcome)
  {
    return observer.complete() && observer.successful() && observer.response() == outcome;
  }

  /// Returns true when no transaction has been completed.
  bool no_transaction_completed() const
  {
    return !param_observer.complete() && !config_observer.complete() && !stop_observer.complete() &&
           !start_observer.complete();
  }

  timer_manager                  timers;
  manual_task_worker             executor{16, false, true};
  p5_transaction_outcome_manager transaction_manager;
  outcome_observer               param_observer;
  outcome_observer               config_observer;
  outcome_observer               stop_observer;
  outcome_observer               start_observer;
};

class p5_responses_handler_fixture : public p5_transactions_fixture
{
protected:
  ~p5_responses_handler_fixture() override
  {
    // The handler waits for the tasks it deferred before it is destroyed, so they have to run.
    run_deferred_tasks();
  }

  /// Runs the tasks that the handler deferred to the FAPI control executor.
  void run_deferred_tasks() { executor.run_pending_tasks(); }

  p5_responses_handler handler{ocudulog::fetch_basic_logger("FAPI"), transaction_manager, executor};
};

} // namespace

TEST_F(p5_responses_handler_fixture, param_response_without_error_completes_the_param_transaction_successfully)
{
  handler.on_param_response(fapi::param_response{.error_code = fapi::error_code_id::msg_ok});
  run_deferred_tasks();

  EXPECT_TRUE(has_outcome(param_observer, true));
  EXPECT_FALSE(config_observer.complete());
  EXPECT_FALSE(stop_observer.complete());
  EXPECT_FALSE(start_observer.complete());
}

TEST_F(p5_responses_handler_fixture, param_response_with_an_error_completes_the_param_transaction_with_failure)
{
  handler.on_param_response(fapi::param_response{.error_code = fapi::error_code_id::msg_invalid_config});
  run_deferred_tasks();

  EXPECT_TRUE(has_outcome(param_observer, false));
}

TEST_F(p5_responses_handler_fixture, config_response_without_error_completes_the_config_transaction_successfully)
{
  handler.on_config_response(fapi::config_response{.error_code = fapi::error_code_id::msg_ok});
  run_deferred_tasks();

  EXPECT_TRUE(has_outcome(config_observer, true));
  EXPECT_FALSE(param_observer.complete());
}

TEST_F(p5_responses_handler_fixture, config_response_with_an_error_completes_the_config_transaction_with_failure)
{
  handler.on_config_response(fapi::config_response{.error_code = fapi::error_code_id::msg_invalid_state});
  run_deferred_tasks();

  EXPECT_TRUE(has_outcome(config_observer, false));
}

TEST_F(p5_responses_handler_fixture, stop_indication_completes_the_stop_transaction_successfully)
{
  handler.on_stop_indication(fapi::stop_indication{});
  run_deferred_tasks();

  EXPECT_TRUE(has_outcome(stop_observer, true));
}

TEST_F(p5_responses_handler_fixture, transactions_are_completed_by_the_fapi_control_executor)
{
  handler.on_param_response(fapi::param_response{.error_code = fapi::error_code_id::msg_ok});

  EXPECT_FALSE(param_observer.complete()) << "The outcome must not be set in the caller's context";
  run_deferred_tasks();
  EXPECT_TRUE(param_observer.complete());
}

TEST_F(p5_responses_handler_fixture, first_slot_indication_completes_the_start_transaction_successfully)
{
  handler.handle_slot_indication(mac_cell_timing_context{});
  run_deferred_tasks();

  EXPECT_TRUE(has_outcome(start_observer, true));
}

TEST_F(p5_responses_handler_fixture, slot_indications_after_the_first_one_do_no_work)
{
  handler.handle_slot_indication(mac_cell_timing_context{});
  run_deferred_tasks();
  ASSERT_TRUE(has_outcome(start_observer, true));

  handler.handle_slot_indication(mac_cell_timing_context{});

  EXPECT_FALSE(executor.has_pending_tasks());
}

TEST_F(p5_responses_handler_fixture, config_response_rearms_the_slot_indication_that_completes_the_start_transaction)
{
  handler.handle_slot_indication(mac_cell_timing_context{});
  run_deferred_tasks();
  ASSERT_TRUE(has_outcome(start_observer, true));

  // A new cell start begins with a CONFIG.response, and the next slot indication has to complete it again.
  handler.on_config_response(fapi::config_response{.error_code = fapi::error_code_id::msg_ok});
  run_deferred_tasks();
  start_observer.subscribe_to(transaction_manager.start_outcome, timeout);
  handler.handle_slot_indication(mac_cell_timing_context{});
  run_deferred_tasks();

  EXPECT_TRUE(has_outcome(start_observer, true));
}

namespace {

/// Message identifier of an ERROR.indication and the transactions that it must fail.
struct error_indication_param {
  fapi::message_type_id message_id;
  bool                  fails_start_transaction;
  bool                  fails_stop_transaction;
};

class p5_responses_handler_error_fixture : public p5_responses_handler_fixture,
                                           public ::testing::WithParamInterface<error_indication_param>
{};

} // namespace

TEST_P(p5_responses_handler_error_fixture, error_indication_fails_only_the_transaction_of_the_failed_request)
{
  handler.on_error_indication(fapi::error_indication{.slot          = slot_point(subcarrier_spacing::kHz30, 1, 3),
                                                     .message_id    = GetParam().message_id,
                                                     .error_code    = fapi::error_code_id::msg_invalid_state,
                                                     .expected_slot = std::nullopt});
  run_deferred_tasks();

  EXPECT_EQ(GetParam().fails_start_transaction, has_outcome(start_observer, false));
  EXPECT_EQ(GetParam().fails_stop_transaction, has_outcome(stop_observer, false));
  // No ERROR.indication completes the PARAM and CONFIG transactions, as those requests are answered by a response.
  EXPECT_FALSE(param_observer.complete());
  EXPECT_FALSE(config_observer.complete());
}

INSTANTIATE_TEST_SUITE_P(message_ids,
                         p5_responses_handler_error_fixture,
                         ::testing::Values(error_indication_param{.message_id = fapi::message_type_id::start_request,
                                                                  .fails_start_transaction = true,
                                                                  .fails_stop_transaction  = false},
                                           error_indication_param{.message_id = fapi::message_type_id::stop_request,
                                                                  .fails_start_transaction = false,
                                                                  .fails_stop_transaction  = true},
                                           error_indication_param{.message_id = fapi::message_type_id::param_request,
                                                                  .fails_start_transaction = false,
                                                                  .fails_stop_transaction  = false},
                                           error_indication_param{.message_id = fapi::message_type_id::config_request,
                                                                  .fails_start_transaction = false,
                                                                  .fails_stop_transaction  = false},
                                           error_indication_param{.message_id = fapi::message_type_id::dl_tti_request,
                                                                  .fails_start_transaction = false,
                                                                  .fails_stop_transaction  = false},
                                           error_indication_param{.message_id = fapi::message_type_id::tx_data_request,
                                                                  .fails_start_transaction = false,
                                                                  .fails_stop_transaction  = false}),
                         [](const ::testing::TestParamInfo<error_indication_param>& param_info) {
                           return fmt::format("message_{}", static_cast<unsigned>(param_info.param.message_id));
                         });

namespace {

/// Handler whose FAPI control executor rejects every task, so it has to drop every message.
class p5_responses_handler_rejecting_executor_fixture : public p5_transactions_fixture
{
protected:
  rejecting_executor   rejecting;
  p5_responses_handler handler{ocudulog::fetch_basic_logger("FAPI"), transaction_manager, rejecting};
};

} // namespace

TEST_F(p5_responses_handler_rejecting_executor_fixture, param_response_is_dropped)
{
  handler.on_param_response(fapi::param_response{.error_code = fapi::error_code_id::msg_ok});

  EXPECT_TRUE(no_transaction_completed());
}

TEST_F(p5_responses_handler_rejecting_executor_fixture, config_response_is_dropped)
{
  handler.on_config_response(fapi::config_response{.error_code = fapi::error_code_id::msg_ok});

  EXPECT_TRUE(no_transaction_completed());
}

TEST_F(p5_responses_handler_rejecting_executor_fixture, stop_indication_is_dropped)
{
  handler.on_stop_indication(fapi::stop_indication{});

  EXPECT_TRUE(no_transaction_completed());
}

TEST_F(p5_responses_handler_rejecting_executor_fixture, slot_indication_is_dropped)
{
  handler.handle_slot_indication(mac_cell_timing_context{});

  EXPECT_TRUE(no_transaction_completed());
}

TEST_F(p5_responses_handler_rejecting_executor_fixture, error_indication_is_dropped)
{
  for (fapi::message_type_id message_id : {fapi::message_type_id::start_request, fapi::message_type_id::stop_request}) {
    handler.on_error_indication(fapi::error_indication{.slot          = std::nullopt,
                                                       .message_id    = message_id,
                                                       .error_code    = fapi::error_code_id::msg_invalid_state,
                                                       .expected_slot = std::nullopt});
  }

  EXPECT_TRUE(no_transaction_completed());
}
