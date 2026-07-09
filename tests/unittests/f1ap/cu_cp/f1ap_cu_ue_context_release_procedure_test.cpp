// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Test the f1 UE context release procedure (gNB-CU initiated)
TEST_F(f1ap_cu_test, when_ue_release_command_received_then_procedure_succeeds)
{
  // Action 1: Add UE
  test_logger.info("Injecting Initial UL RRC message");
  f1ap_message init_ul_rrc_msg = test_helpers::generate_init_ul_rrc_message_transfer(int_to_gnb_du_ue_f1ap_id(41255));
  f1ap->handle_message(init_ul_rrc_msg);

  // Action 2: Start UE Context Release procedure
  test_logger.info("Starting UE Context Release procedure");
  f1ap_ue_context_release_command f1ap_ue_ctxt_rel_cmd_msg;
  f1ap_ue_ctxt_rel_cmd_msg.ue_index = cu_cp_ue_index_t::min;
  f1ap_ue_ctxt_rel_cmd_msg.cause    = f1ap_cause_radio_network_t::unspecified;

  // launch F1 UE context release procedure
  async_task<cu_cp_ue_index_t>         t = f1ap->handle_ue_context_release_command(f1ap_ue_ctxt_rel_cmd_msg);
  lazy_task_launcher<cu_cp_ue_index_t> t_launcher(t);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action 3: Inject UE Context Release Complete message
  test_logger.info("Injecting UE Context Release Complete message");
  f1ap_message ue_ctxt_rel_complete_msg =
      test_helpers::generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(41255));
  f1ap->handle_message(ue_ctxt_rel_complete_msg);

  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), cu_cp_ue_index_t::min);
}

TEST_F(f1ap_cu_test, when_f1ap_stopped_while_ue_context_release_in_flight_then_teardown_does_not_crash)
{
  // Action 1: Add UE
  f1ap_message init_ul_rrc_msg = test_helpers::generate_init_ul_rrc_message_transfer(int_to_gnb_du_ue_f1ap_id(41255));
  f1ap->handle_message(init_ul_rrc_msg);

  // Action 2: Start UE Context Release procedure, leaving its transaction unanswered.
  f1ap_ue_context_release_command f1ap_ue_ctxt_rel_cmd_msg;
  f1ap_ue_ctxt_rel_cmd_msg.ue_index = cu_cp_ue_index_t::min;
  f1ap_ue_ctxt_rel_cmd_msg.cause    = f1ap_cause_radio_network_t::unspecified;

  async_task<cu_cp_ue_index_t>         t = f1ap->handle_ue_context_release_command(f1ap_ue_ctxt_rel_cmd_msg);
  lazy_task_launcher<cu_cp_ue_index_t> t_launcher(t);
  ASSERT_FALSE(t.ready());

  // Action 3: Stop F1AP while the UE Context Release Procedure's transaction is still in flight.
  async_task<void>         stop_task = f1ap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);

  ASSERT_TRUE(stop_task.ready());
  ASSERT_TRUE(t.ready());
}
