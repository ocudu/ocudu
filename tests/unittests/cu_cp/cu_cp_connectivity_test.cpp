// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/e1ap/e1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/asn1/rrc_nr/dl_ccch_msg.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg_ies.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ngap/ngap_message.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;
using namespace ocucp;

class cu_cp_connectivity_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_connectivity_test() : cu_cp_test_environment(cu_cp_test_env_params{}) {}
};

//----------------------------------------------------------------------------------//
// CU-CP to AMF connection handling                                                 //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_connectivity_test, when_cu_cp_is_created_then_it_is_not_connected_to_amf)
{
  ngap_message ngap_pdu;
  ASSERT_FALSE(get_amf().try_pop_rx_pdu(ngap_pdu))
      << "The CU-CP should not send a message to the NG interface before being started";

  ASSERT_FALSE(get_cu_cp().get_ng_handler().amfs_are_connected());
}

TEST_F(cu_cp_connectivity_test, when_cu_cp_starts_then_it_initiates_ng_setup_procedure_and_blocks_waiting_for_response)
{
  // Enqueue AMF NG Setup Response as an auto-reply to CU-CP.
  ngap_message ng_setup_resp = generate_ng_setup_response();
  get_amf().enqueue_next_tx_pdu(ng_setup_resp);

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  ASSERT_TRUE(get_cu_cp().start());

  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf().try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_TRUE(get_cu_cp().get_ng_handler().amfs_are_connected());
}

TEST_F(cu_cp_connectivity_test, when_ng_setup_fails_then_cu_cp_is_not_in_amf_connected_state)
{
  // Enqueue AMF NG Setup Response as an auto reply to CU-CP.
  ngap_message ng_setup_fail = generate_ng_setup_failure();
  get_amf().enqueue_next_tx_pdu(ng_setup_fail);

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  ASSERT_FALSE(get_cu_cp().start());

  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf().try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_FALSE(get_cu_cp().get_ng_handler().amfs_are_connected());
}

TEST_F(cu_cp_connectivity_test, when_amf_is_not_reachable_on_startup_then_cu_cp_starts_and_connects_once_amf_is_up)
{
  // Simulate an AMF that is not reachable yet.
  get_amf().drop_connection();

  // The CU-CP must start, even though the N2 TNL connection could not be established.
  ASSERT_TRUE(get_cu_cp().start());
  ASSERT_FALSE(get_cu_cp().get_ng_handler().amfs_are_connected());

  ngap_message ngap_pdu;
  ASSERT_FALSE(get_amf().try_pop_rx_pdu(ngap_pdu)) << "CU-CP sent an NG Setup Request without a TNL connection";

  // The AMF becomes reachable. The scheduled reconnection must establish the TNL connection and run the NG Setup.
  ASSERT_TRUE(reconnect_amf(0)) << "CU-CP did not retry the AMF connection";
  ASSERT_TRUE(tick_until(
      std::chrono::milliseconds{1000}, [this]() { return get_cu_cp().get_ng_handler().amfs_are_connected(); }, false));
}

TEST_F(cu_cp_connectivity_test, when_amf_connection_is_lost_and_no_dus_are_connected_then_no_amfs_are_connected)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Drop AMF connection.
  ASSERT_TRUE(drop_amf_connection(0));

  // Check that CU-CP is still running but not connected to AMF.
  ASSERT_FALSE(this->get_cu_cp().get_ng_handler().amfs_are_connected());
}

TEST_F(cu_cp_connectivity_test, when_amf_connection_is_lost_then_connected_ues_are_released_and_new_ues_are_rejected)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Setup CU-UP.
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = ret.value();
  ASSERT_TRUE(this->run_e1_setup(cu_up_idx));

  // Create UE.
  gnb_du_ue_f1ap_id_t du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti         = to_rnti(0x4601);
  ASSERT_TRUE(connect_new_ue(du_idx, du_ue_f1ap_id, crnti));

  ASSERT_TRUE(drop_amf_connection(0));

  // TEST: Verify that connected UEs are released.
  {
    // Verify F1AP UE Context Release Command is sent to DU.
    f1ap_message f1ap_pdu;
    ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
    ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(f1ap_pdu));

    // Verify RRC Reject is sent to UE.
    const auto&                 ue_rel = f1ap_pdu.pdu.init_msg().value.ue_context_release_cmd();
    asn1::rrc_nr::dl_ccch_msg_s ccch;
    {
      asn1::cbit_ref bref{ue_rel->rrc_container};
      ASSERT_EQ(ccch.unpack(bref), asn1::OCUDUASN_SUCCESS);
    }
    ASSERT_EQ(ccch.msg.c1().type().value, asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::rrc_reject);

    // DU sends F1AP UE Context Release Complete.
    auto rel_complete = test_helpers::generate_ue_context_release_complete(
        int_to_gnb_cu_ue_f1ap_id(ue_rel->gnb_cu_ue_f1ap_id), du_ue_f1ap_id);
    get_du(du_idx).push_ul_pdu(rel_complete);

    // TEST: Verify UE is removed in CU-CP.
    auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
    ASSERT_TRUE(report.ues.empty());
  }

  // TEST: Verify that Cells are deactivated in DU.
  {
    // Verify F1AP GNB CU Configuration Update is sent to DU.
    f1ap_message f1ap_pdu;
    ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
    ASSERT_TRUE(test_helpers::is_valid_gnb_cu_configuration_update(f1ap_pdu));

    // DU sends F1AP UE Context Release Complete.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_gnb_cu_configuration_update_acknowledgement({}));
  }

  // TEST: Verify that new UE connection is rejected.
  {
    // Send Initial UL RRC Message.
    gnb_du_ue_f1ap_id_t du_ue_f1ap_id_2 = int_to_gnb_du_ue_f1ap_id(1);
    rnti_t              crnti_2         = to_rnti(0x4602);
    get_du(du_idx).push_ul_pdu(test_helpers::generate_init_ul_rrc_message_transfer(du_ue_f1ap_id_2, crnti_2));

    // TEST: UE Context Release Command is sent to DU.
    f1ap_message f1ap_pdu;
    ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
    ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(f1ap_pdu));

    // TEST: RRC Reject is sent to the UE within F1AP UE Context Release Command message.
    const auto&                 ue_rel = f1ap_pdu.pdu.init_msg().value.ue_context_release_cmd();
    asn1::rrc_nr::dl_ccch_msg_s ccch;
    {
      asn1::cbit_ref bref{ue_rel->rrc_container};
      ASSERT_EQ(ccch.unpack(bref), asn1::OCUDUASN_SUCCESS);
    }
    ASSERT_EQ(ccch.msg.c1().type().value, asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::rrc_reject);

    // DU sends F1AP UE Context Release Complete.
    auto rel_complete = test_helpers::generate_ue_context_release_complete(
        int_to_gnb_cu_ue_f1ap_id(ue_rel->gnb_cu_ue_f1ap_id), du_ue_f1ap_id);
    get_du(du_idx).push_ul_pdu(rel_complete);

    // TEST: Verify UE is removed in CU-CP.
    auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
    ASSERT_TRUE(report.ues.empty());
  }
}

TEST_F(cu_cp_connectivity_test,
       when_amf_connection_is_lost_and_gnb_cu_configuration_update_times_out_then_cell_deactivation_completes)
{
  // This test reproduces an std::bad_optional_access exception in log after GNBCU Configuration Update timeout.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Drop AMF connection - triggers cell_deactivation_routine.
  ASSERT_TRUE(drop_amf_connection(0));

  // Cell deactivation sends GNB-CU Configuration Update to the DU.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "GNB-CU Configuration Update not sent to DU";
  ASSERT_TRUE(test_helpers::is_valid_gnb_cu_configuration_update(f1ap_pdu));

  // Do NOT inject a DU response - let the F1AP transaction timer fire.
  ASSERT_FALSE(tick_until(
      this->get_cu_cp_cfg().f1ap.proc_timeout + std::chrono::milliseconds{1000}, [&]() { return false; }, false));

  // After the timeout the routine should have completed gracefully without crashing.
  ASSERT_FALSE(this->get_cu_cp().get_ng_handler().amfs_are_connected());
}

TEST_F(cu_cp_connectivity_test,
       when_amf_reconnects_and_gnb_cu_configuration_update_times_out_then_cell_activation_completes)
{
  // This test reproduces an std::bad_optional_access exception in log after GNBCU Configuration Update timeout.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Drop AMF connection - triggers cell_deactivation_routine which moves cells to deactivated_cells.
  ASSERT_TRUE(drop_amf_connection(0));

  // Acknowledge the GNB-CU Configuration Update so cell_deactivation_routine completes cleanly,
  // allowing amf_connection_loss_routine to proceed to the reconnection step.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "GNB-CU Configuration Update not sent to DU";
  ASSERT_TRUE(test_helpers::is_valid_gnb_cu_configuration_update(f1ap_pdu));
  get_du(du_idx).push_ul_pdu(test_helpers::generate_gnb_cu_configuration_update_acknowledgement({}));

  // Reconnect AMF - completes NG re-setup and triggers cell_activation_routine.
  ASSERT_TRUE(reconnect_amf(0)) << "AMF did not reconnect within expected time";

  // Cell activation sends GNB-CU Configuration Update to the DU.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}))
      << "GNB-CU Configuration Update not sent to DU after AMF reconnection";
  ASSERT_TRUE(test_helpers::is_valid_gnb_cu_configuration_update(f1ap_pdu));

  // Do NOT inject a DU response - let the F1AP transaction timer fire.
  ASSERT_FALSE(tick_until(
      this->get_cu_cp_cfg().f1ap.proc_timeout + std::chrono::milliseconds{1000}, [&]() { return false; }, false));

  // After the timeout the routine should have completed gracefully without crashing.
  ASSERT_TRUE(this->get_cu_cp().get_ng_handler().amfs_are_connected());
}

TEST_F(cu_cp_connectivity_test, when_amf_connection_is_lost_and_ue_release_times_out_then_cell_deactivation_completes)
{
  // This test reproduces a bug where cell_deactivation_routine would hang forever.
  // The scenario is:
  // 1. AMF initiates UE release (F1AP UE Context Release in progress)
  // 2. AMF connection drops while release is still in progress
  // 3. Cell deactivation tries to release the same UE again
  // 4. The second release fails (already marked for release) and the task fails
  // 5. With the old boolean tracking, ue_released was never set to true, causing infinite loop

  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Setup CU-UP.
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = ret.value();
  ASSERT_TRUE(this->run_e1_setup(cu_up_idx));

  // Create and authenticate UE.
  gnb_du_ue_f1ap_id_t du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti         = to_rnti(0x4601);
  ASSERT_TRUE(connect_new_ue(du_idx, du_ue_f1ap_id, crnti));
  ASSERT_TRUE(authenticate_ue(du_idx, du_ue_f1ap_id, uint_to_amf_ue_id(0)));

  // Step 1: AMF initiates UE release - inject NGAP UE Context Release Command.
  // This starts a release procedure that will send F1AP UE Context Release Command.
  auto* ue_ctx = find_ue_context(du_idx, du_ue_f1ap_id);
  ASSERT_NE(ue_ctx, nullptr);
  get_amf().push_tx_pdu(generate_valid_ue_context_release_command_with_amf_ue_ngap_id(ue_ctx->amf_ue_id.value()));

  // Wait for F1AP UE Context Release Command to be sent to DU.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(f1ap_pdu));

  // Step 2: DO NOT complete the F1AP release - leave it in progress.
  // Now drop AMF connection - this triggers cell_deactivation_routine.
  ASSERT_TRUE(drop_amf_connection(0));

  // Step 3: Cell deactivation will try to release the same UE again.
  // With the old implementation, this would fail and ue_released would never be set to true.
  // Wait for the F1AP timeout to expire (the first release will timeout).
  ASSERT_FALSE(tick_until(
      this->get_cu_cp_cfg().f1ap.proc_timeout + std::chrono::milliseconds{1000}, [&]() { return false; }, false));

  // After timeout, the cell deactivation routine should complete and send GNB CU Configuration Update.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}))
      << "Cell deactivation should proceed even after UE release timeout";
  ASSERT_TRUE(test_helpers::is_valid_gnb_cu_configuration_update(f1ap_pdu));

  // DU acknowledges configuration update.
  get_du(du_idx).push_ul_pdu(test_helpers::generate_gnb_cu_configuration_update_acknowledgement({}));

  // Verify CU-CP is not connected to AMF but still running.
  ASSERT_FALSE(this->get_cu_cp().get_ng_handler().amfs_are_connected());
}

TEST_F(cu_cp_connectivity_test, when_amf_connection_is_lost_then_all_ue_releases_are_started_in_parallel)
{
  // This test verifies that cell_deactivation_routine releases all UEs in parallel, not sequentially.
  // All F1AP UE Context Release Commands should be sent out before any response is received.
  // If releases were sequential, only one command would be sent at a time, waiting for the response
  // before sending the next one.

  const unsigned nof_ues = 20;

  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Setup CU-UP.
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = ret.value();
  ASSERT_TRUE(this->run_e1_setup(cu_up_idx));

  // Create 20 UEs.
  for (unsigned i = 0; i < nof_ues; ++i) {
    ASSERT_TRUE(connect_new_ue(du_idx, int_to_gnb_du_ue_f1ap_id(i), to_rnti(0x4601 + i)));
  }

  // Verify all UEs are created.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), nof_ues);

  // Drop AMF connection - this triggers cell_deactivation_routine which should release all UEs.
  ASSERT_TRUE(drop_amf_connection(0));

  // TEST: Verify all F1AP UE Context Release Commands are sent BEFORE we respond to any.
  // If the releases are parallel, all commands should be sent without waiting for responses.
  // If sequential, only 1 command would appear, and the next only after we respond.
  std::vector<f1ap_message> release_commands(nof_ues);
  for (unsigned i = 0; i < nof_ues; ++i) {
    ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, release_commands[i], std::chrono::milliseconds{1000}))
        << "Expected F1AP UE Context Release Command for UE " << i
        << " before any response was sent - releases may not be parallel";
    ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(release_commands[i]));
  }

  // Generate responses for all UEs and shuffle them to simulate out-of-order completion.
  std::vector<f1ap_message> release_responses;
  for (unsigned i = 0; i < nof_ues; ++i) {
    release_responses.push_back(test_helpers::generate_ue_context_release_complete(release_commands[i]));
  }
  std::mt19937 rng(42); // Fixed seed for reproducibility.
  std::shuffle(release_responses.begin(), release_responses.end(), rng);

  // Simulate DU responses arriving over ~5 seconds (well within 10s F1AP proc timeout).
  const unsigned delay_between_responses_ms = 5000 / nof_ues; // 250ms between each response.
  for (const auto& response : release_responses) {
    tick_until(std::chrono::milliseconds{delay_between_responses_ms}, [&]() { return false; }, false);
    get_du(du_idx).push_ul_pdu(response);
  }

  // After all UE releases complete, cell deactivation should send GNB CU Configuration Update.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}))
      << "Cell deactivation should complete and send GNB CU Configuration Update after all UEs are released";
  ASSERT_TRUE(test_helpers::is_valid_gnb_cu_configuration_update(f1ap_pdu));

  // DU acknowledges configuration update.
  get_du(du_idx).push_ul_pdu(test_helpers::generate_gnb_cu_configuration_update_acknowledgement({}));

  // Verify all UEs are removed.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_TRUE(report.ues.empty());

  // Verify CU-CP is not connected to AMF but still running.
  ASSERT_FALSE(this->get_cu_cp().get_ng_handler().amfs_are_connected());
}

//----------------------------------------------------------------------------------//
// DU connection handling                                                           //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_connectivity_test, when_new_f1_setup_request_is_received_and_ng_is_setup_then_f1_setup_is_accepted)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Verify no DUs detected.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_TRUE(report.dus.empty());

  // Establish TNL connection between DU and CU-CP.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = *ret;

  // Verify that DU was created but without gNB-DU-Id yet, as that value will come in the F1 Setup Request.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.dus.size(), 1);
  ASSERT_EQ(report.dus[0].id, gnb_du_id_t::invalid);
  ASSERT_TRUE(report.dus[0].cells.empty());

  // Send F1 Setup Request.
  gnb_du_id_t du_id = int_to_gnb_du_id(0x55);
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request(du_id));

  // Ensure the F1 Setup Response is received and correct.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}))
      << "F1 Setup Response was not received by the DU";
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(f1ap_pdu.pdu.successful_outcome().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::f1_setup_resp);

  // Verify DU has assigned DU id.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.dus.size(), 1);
  ASSERT_EQ(report.dus[0].id, du_id);
  ASSERT_EQ(report.dus[0].cells.size(), 1);
}

TEST_F(cu_cp_connectivity_test, when_dus_with_duplicate_du_ids_connect_then_f1_setup_is_rejected)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish two TNL connections between two DUs and the CU-CP.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = *ret;
  ret             = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx2 = *ret;

  // DU1 sends F1 Setup Request.
  gnb_du_id_t du_id = int_to_gnb_du_id(0x55);
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request(du_id));

  // DU2 sends F1 Setup Request.
  gnb_du_id_t du_id2 = int_to_gnb_du_id(0x55);
  get_du(du_idx2).push_ul_pdu(test_helpers::generate_f1_setup_request(du_id2));

  // Ensure the F1 Setup Response is received and correct for first DU.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(f1ap_pdu.pdu.successful_outcome().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::f1_setup_resp);

  // Ensure the F1 Setup Failure is received for second DU.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx2, f1ap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu.pdu.unsuccessful_outcome().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(cu_cp_connectivity_test, when_a_du_with_non_matching_gnb_id_connects_then_f1_setup_is_rejected)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish two TNL connections between DU and the CU-CP.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = *ret;

  // DU sends F1 Setup Request.
  gnb_du_id_t  du_id    = int_to_gnb_du_id(0x55);
  f1ap_message f1ap_pdu = test_helpers::generate_f1_setup_request(
      du_id, {{.nci = nr_cell_identity::create(0x0).value()}}); // the gnb-id does not match.
  get_du(du_idx).push_ul_pdu(f1ap_pdu);

  // Ensure the F1 Setup Failure is received for the DU.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu.pdu.unsuccessful_outcome().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(cu_cp_connectivity_test, when_f1_setup_request_meas_timing_omits_freq_and_timing_then_f1_setup_is_rejected)
{
  // Run NG setup to completion.
  run_ng_setup();

  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = *ret;

  // Start from a valid F1 Setup Request and replace its meas_timing_cfg with one that omits
  // freq-and-timing in every MeasTiming entry.
  gnb_du_id_t  du_id    = int_to_gnb_du_id(0x55);
  f1ap_message f1ap_pdu = test_helpers::generate_f1_setup_request(du_id);
  f1ap_pdu.pdu.init_msg()
      .value.f1_setup_request()
      ->gnb_du_served_cells_list[0]
      ->gnb_du_served_cells_item()
      .served_cell_info.meas_timing_cfg = test_helpers::create_meas_timing_cfg_no_freq_and_timing();
  get_du(du_idx).push_ul_pdu(f1ap_pdu);

  // CU-CP must respond with F1 Setup Failure.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu.pdu.unsuccessful_outcome().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(cu_cp_connectivity_test, when_max_nof_dus_connected_reached_then_cu_cp_rejects_new_du_connections)
{
  for (unsigned idx = 0; idx < this->get_test_env_params().max_nof_dus; idx++) {
    auto ret = connect_new_du();
    ASSERT_TRUE(ret.has_value());
  }

  auto ret = connect_new_du();
  ASSERT_FALSE(ret.has_value());

  // Verify that no DUs are created.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.dus.size(), this->get_test_env_params().max_nof_dus);
}

TEST_F(
    cu_cp_connectivity_test,
    when_max_nof_dus_connected_reached_and_du_connection_drops_then_du_is_removed_from_cu_cp_and_new_du_connection_is_accepted)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish TNL connection and F1 Setup for max number of DUs.
  for (unsigned idx = 0; idx < this->get_test_env_params().max_nof_dus; idx++) {
    auto ret = connect_new_du();
    ASSERT_TRUE(ret.has_value());
    unsigned du_idx = *ret;
    get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request());
    f1ap_message f1ap_pdu;
    ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  }

  // Drop DU connection.
  ASSERT_TRUE(this->drop_du_connection(0));

  // A new DU can now be created.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
}

TEST_F(cu_cp_connectivity_test, when_ng_setup_is_not_successful_then_f1_setup_is_rejected)
{
  // Enqueue AMF NG Setup Response as an auto reply to CU-CP.
  ngap_message ng_setup_fail = generate_ng_setup_failure();
  get_amf().enqueue_next_tx_pdu(ng_setup_fail);

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  ASSERT_FALSE(get_cu_cp().start());

  // Establish TNL connection between DU and CU-CP and start F1 setup procedure.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = *ret;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request());
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));

  // The CU-CP should reject F1 setup.
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::unsuccessful_outcome);
}

TEST_F(cu_cp_connectivity_test, when_du_connection_is_lost_then_connected_ues_are_released)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Setup CU-UP.
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = ret.value();
  ASSERT_TRUE(this->run_e1_setup(cu_up_idx));

  // Create UE.
  gnb_du_ue_f1ap_id_t    du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t                 crnti         = to_rnti(0x4601);
  amf_ue_id_t            amf_ue_id     = amf_ue_id_t::min;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;
  ASSERT_TRUE(attach_ue(du_idx, cu_up_idx, du_ue_f1ap_id, crnti, amf_ue_id, cu_up_e1ap_id));

  ASSERT_TRUE(drop_du_connection(0));

  // TEST: Verify that connected UEs are released.
  {
    // TEST: Verify NGReset is sent to AMF (NG Reset Ack is injected automatically).
    ngap_message ngap_pdu;
    ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{1000}))
        << "NG Reset was not received by the AMF";
    ASSERT_TRUE(test_helpers::is_valid_ng_reset(ngap_pdu));

    // TEST: Verify E1Reset is sent to CU-UP (E1 Reset Ack is injected automatically).
    e1ap_message e1ap_pdu;
    ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(du_idx, e1ap_pdu, std::chrono::milliseconds{1000}))
        << "E1 Reset was not received by the CU-UP";
    ASSERT_TRUE(test_helpers::is_valid_e1_reset(e1ap_pdu));

    // TEST: Verify UE is removed in CU-CP.
    auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
    ASSERT_TRUE(report.ues.empty());
  }
}

//----------------------------------------------------------------------------------//
// CU-UP connection handling                                                        //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_connectivity_test, when_new_e1_setup_request_is_received_and_ng_is_setup_then_e1_setup_is_accepted)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish TNL connection between CU-CP and CU-UP.
  auto ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = *ret;

  // CU-UP sends E1 Setup Request.
  get_cu_up(cu_up_idx).push_tx_pdu(generate_valid_cu_up_e1_setup_request());

  // Ensure the E1 Setup Response is received and correct.
  e1ap_message e1ap_pdu;
  ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu, std::chrono::milliseconds{1000}))
      << "E1 Setup Response was not received by the CU-UP";
  ASSERT_EQ(e1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(e1ap_pdu.pdu.successful_outcome().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_up_e1_setup_resp);
}

TEST_F(cu_cp_connectivity_test, when_max_nof_cu_ups_connected_reached_then_cu_cp_rejects_new_cu_up_connections)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish TNL connection between CU-CP and CU-UP.
  for (unsigned i = 0; i != this->get_test_env_params().max_nof_cu_ups; ++i) {
    auto ret = connect_new_cu_up();
    ASSERT_TRUE(ret.has_value());
  }

  // The last one is rejected.
  auto ret = connect_new_cu_up();
  ASSERT_FALSE(ret.has_value());
}

TEST_F(
    cu_cp_connectivity_test,
    when_max_nof_cu_ups_connected_reached_and_cu_up_connection_drops_then_cu_up_is_removed_from_cu_cp_and_new_cu_up_connection_is_accepted)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish TNL connection and E1 Setup for max number of CU-UPs.
  for (unsigned idx = 0; idx < this->get_test_env_params().max_nof_dus; idx++) {
    auto ret = connect_new_cu_up();
    ASSERT_TRUE(ret.has_value());
    unsigned cu_up_idx = *ret;
    get_cu_up(cu_up_idx).push_tx_pdu(generate_valid_cu_up_e1_setup_request());
    e1ap_message e1ap_pdu;
    ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu, std::chrono::milliseconds{1000}));
  }

  // Drop DU connection.
  ASSERT_TRUE(this->drop_cu_up_connection(0));

  // A new CU-UP can now be created.
  auto ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
}

TEST_F(cu_cp_connectivity_test,
       when_e1_release_request_is_received_and_no_ues_are_connected_then_release_response_is_sent)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Setup CU-UP.
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = ret.value();
  ASSERT_TRUE(this->run_e1_setup(cu_up_idx));

  // Check no UEs.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_TRUE(report.ues.empty());

  // CU-UP sends E1 Release Request.
  get_cu_up(cu_up_idx).push_tx_pdu(generate_valid_e1_release_request());

  // Ensure the E1 Release Response is received and correct.
  e1ap_message e1ap_pdu;
  ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu, std::chrono::milliseconds{1000}))
      << "E1 Release Response was not received by the CU-UP";
  ASSERT_EQ(e1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(e1ap_pdu.pdu.successful_outcome().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::e1_release_resp);
}

TEST_F(cu_cp_connectivity_test,
       when_e1_release_request_is_received_and_ues_are_connected_then_ues_are_released_and_release_response_is_sent)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Setup CU-UP.
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = ret.value();
  ASSERT_TRUE(this->run_e1_setup(cu_up_idx));

  ASSERT_TRUE(
      attach_ue(du_idx, cu_up_idx, gnb_du_ue_f1ap_id_t(0), to_rnti(0x4601), amf_ue_id_t(0), gnb_cu_up_ue_e1ap_id_t(0)));

  // CU-UP sends E1 Release Request.
  get_cu_up(cu_up_idx).push_tx_pdu(generate_valid_e1_release_request());

  // TEST: Verify NGAP UE Context Release Request is sent to NGAP.
  ngap_message ngap_pdu;
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_request(ngap_pdu));

  // AMF sends NGAP UE Context Release Command.
  get_amf().push_tx_pdu(generate_valid_ue_context_release_command_with_amf_ue_ngap_id(amf_ue_id_t(0)));

  // TEST: Verify F1AP UE Context Release Command is sent to DU.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(f1ap_pdu));

  // DU sends F1AP UE Context Release Complete.
  get_du(du_idx).push_ul_pdu(
      test_helpers::generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0)));

  // TEST: Verify NGAP UE Context Release Complete is sent to AMF.
  ngap_message ngap_pdu2;
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu2, std::chrono::milliseconds{1000}));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_complete(ngap_pdu2));

  // TEST: Verify E1 Release Response is sent to CU-UP.
  e1ap_message e1ap_pdu;
  ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu, std::chrono::milliseconds{1000}))
      << "E1 Release Response was not received by the CU-UP";
  ASSERT_EQ(e1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(e1ap_pdu.pdu.successful_outcome().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::e1_release_resp);
}

//----------------------------------------------------------------------------------//
//  UE connection handling                                                          //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_connectivity_test, when_ng_f1_e1_are_setup_then_ues_can_attach)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Setup CU-UP.
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = ret.value();
  ASSERT_TRUE(this->run_e1_setup(cu_up_idx));

  // Check no UEs.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_TRUE(report.ues.empty());

  // Create UE by sending Initial UL RRC Message.
  gnb_du_ue_f1ap_id_t du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti         = to_rnti(0x4601);
  get_du(du_idx).push_ul_pdu(test_helpers::generate_init_ul_rrc_message_transfer(du_ue_f1ap_id, crnti));
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.dus[0].rrc_metrics.attempted_rrc_connection_establishments.get_count(establishment_cause_t::mo_sig),
            1);

  // Verify F1AP DL RRC Message is sent with RRC Setup.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu));

  // Check UE is created.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1);
  ASSERT_EQ(report.ues[0].rnti, crnti);

  // Send RRC Setup Complete.
  // > Generate UL RRC Message (containing RRC Setup Complete) with PDCP SN=0.
  get_du(du_idx).push_rrc_ul_dcch_message(
      du_ue_f1ap_id, srb_id_t::srb1, test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete()));
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.dus[0].rrc_metrics.attempted_rrc_connection_establishments.get_count(establishment_cause_t::mo_sig),
            1);
  ASSERT_EQ(report.dus[0].rrc_metrics.successful_rrc_connection_establishments.get_count(establishment_cause_t::mo_sig),
            1);
}

TEST_F(cu_cp_connectivity_test, when_e1_is_not_setup_then_new_ues_are_rejected)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx));

  // Send Initial UL RRC Message.
  gnb_du_ue_f1ap_id_t ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti      = to_rnti(0x4601);
  get_du(du_idx).push_ul_pdu(test_helpers::generate_init_ul_rrc_message_transfer(ue_f1ap_id, crnti));

  // TEST: F1AP UE Context Command is sent to DU.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd);
  const auto& ue_rel = f1ap_pdu.pdu.init_msg().value.ue_context_release_cmd();
  ASSERT_EQ(int_to_gnb_du_ue_f1ap_id(ue_rel->gnb_du_ue_f1ap_id), ue_f1ap_id);
  ASSERT_TRUE(ue_rel->srb_id_present);
  ASSERT_EQ(int_to_srb_id(ue_rel->srb_id), srb_id_t::srb0);

  // TEST: RRC Reject is sent to UE.
  asn1::rrc_nr::dl_ccch_msg_s ccch;
  {
    asn1::cbit_ref bref{ue_rel->rrc_container};
    ASSERT_EQ(ccch.unpack(bref), asn1::OCUDUASN_SUCCESS);
  }
  ASSERT_EQ(ccch.msg.c1().type().value, asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::rrc_reject);

  // TEST: UE is not destroyed in CU-CP until UE Context Release Complete is received.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1);
  ASSERT_EQ(report.ues[0].rnti, crnti);

  // DU sends F1AP UE Context Release Complete.
  auto rel_complete = test_helpers::generate_ue_context_release_complete(
      int_to_gnb_cu_ue_f1ap_id(ue_rel->gnb_cu_ue_f1ap_id), ue_f1ap_id);
  get_du(du_idx).push_ul_pdu(rel_complete);

  // TEST: Verify UE is removed in CU-CP.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_TRUE(report.ues.empty());

  // Verify no NGAP PDU was sent when a UE is rejected.
  ngap_message ngap_pdu;
  ASSERT_FALSE(this->get_amf().try_pop_rx_pdu(ngap_pdu));
}
