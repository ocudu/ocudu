// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
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

/// Starts the release of UE 0 and resumes UE 0 through a new F1-connection before the release completes.
static async_task<cu_cp_ue_index_t> start_release_and_resume(f1ap_cu&                          f1ap,
                                                             dummy_f1ap_du_processor_notifier& du_processor_notifier)
{
  f1ap.handle_message(test_helpers::generate_init_ul_rrc_message_transfer(int_to_gnb_du_ue_f1ap_id(1)));

  f1ap_ue_context_release_command release_cmd;
  release_cmd.ue_index           = cu_cp_ue_index_t::min;
  release_cmd.cause              = f1ap_cause_radio_network_t::normal_release;
  async_task<cu_cp_ue_index_t> t = f1ap.handle_ue_context_release_command(release_cmd);

  du_processor_notifier.next_resumed_ue_index = cu_cp_ue_index_t::min;
  f1ap.handle_message(test_helpers::generate_init_ul_rrc_message_transfer(int_to_gnb_du_ue_f1ap_id(2)));
  return t;
}

TEST_F(f1ap_cu_test, when_ue_resumes_before_release_complete_then_old_ue_context_is_removed_after_release_complete)
{
  async_task<cu_cp_ue_index_t>         t = start_release_and_resume(*f1ap, du_processor_notifier);
  lazy_task_launcher<cu_cp_ue_index_t> t_launcher(t);
  ASSERT_FALSE(t.ready());
  ASSERT_EQ(f1ap->get_nof_ues(), 2);

  // The DU confirms the release of the old F1-connection.
  f1ap->handle_message(
      test_helpers::generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(1)));
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), cu_cp_ue_index_t::min);
  ASSERT_EQ(f1ap->get_nof_ues(), 1);

  // The resumed UE uses the new F1-connection with a new gNB-CU UE F1AP ID (TS 38.401 section 8.6.2).
  f1ap->handle_dl_rrc_message_transfer(
      {.ue_index = cu_cp_ue_index_t::min, .srb_id = srb_id_t::srb1, .rrc_container = make_byte_buffer("1234").value()});
  const auto& dl_rrc_msg = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.dl_rrc_msg_transfer();
  ASSERT_EQ(dl_rrc_msg->gnb_cu_ue_f1ap_id, 1);
  ASSERT_EQ(dl_rrc_msg->gnb_du_ue_f1ap_id, 2);
}

TEST_F(f1ap_cu_test, when_f1ap_stops_while_replaced_ue_context_is_released_then_release_is_cancelled)
{
  async_task<cu_cp_ue_index_t>         t = start_release_and_resume(*f1ap, du_processor_notifier);
  lazy_task_launcher<cu_cp_ue_index_t> t_launcher(t);
  ASSERT_FALSE(t.ready());

  // Stop F1AP while the release of the replaced UE context is in flight.
  async_task<void>         stop_task = f1ap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);

  ASSERT_TRUE(stop_task.ready());
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), cu_cp_ue_index_t::invalid);
  ASSERT_EQ(f1ap->get_nof_ues(), 1);
}

TEST_F(f1ap_cu_test, when_other_release_caller_removes_ue_then_release_of_replaced_ue_context_completes)
{
  f1ap->handle_message(test_helpers::generate_init_ul_rrc_message_transfer(int_to_gnb_du_ue_f1ap_id(1)));

  f1ap_ue_context_release_command release_cmd;
  release_cmd.ue_index                   = cu_cp_ue_index_t::min;
  release_cmd.cause                      = f1ap_cause_radio_network_t::normal_release;
  async_task<cu_cp_ue_index_t>         t = f1ap->handle_ue_context_release_command(release_cmd);
  lazy_task_launcher<cu_cp_ue_index_t> t_launcher(t);

  // A second release caller waits for the same release and removes the UE when the release completes.
  async_task<void>         t2 = launch_async([this, release_cmd](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(f1ap->handle_ue_context_release_command(release_cmd));
    f1ap->remove_ue_context(release_cmd.ue_index);
    CORO_RETURN();
  });
  lazy_task_launcher<void> t2_launcher(t2);

  // The UE resumes through a new F1-connection before the release completes.
  du_processor_notifier.next_resumed_ue_index = cu_cp_ue_index_t::min;
  f1ap->handle_message(test_helpers::generate_init_ul_rrc_message_transfer(int_to_gnb_du_ue_f1ap_id(2)));
  ASSERT_EQ(f1ap->get_nof_ues(), 2);

  // The DU confirms the release of the old F1-connection.
  f1ap->handle_message(
      test_helpers::generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(1)));
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), cu_cp_ue_index_t::min);
  ASSERT_TRUE(t2.ready());
  ASSERT_EQ(f1ap->get_nof_ues(), 0);
}
