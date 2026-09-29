// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "test_doubles/mock_amf.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/e1ap/e1ap_cu_cp_test_messages.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

static std::vector<std::vector<supported_tracking_area>> make_amf_test_config()
{
  // Common.
  slice_service_type sst     = slice_service_type{1};
  s_nssai_t          s_nssai = s_nssai_t{sst, slice_differentiator{}};

  // AMF 1.
  plmn_item               plmn_1         = plmn_item{plmn_identity::test_value(), std::vector<s_nssai_t>{s_nssai}};
  tac_t                   tac_1          = 7;
  supported_tracking_area supported_ta_1 = supported_tracking_area{tac_1, {plmn_1}};

  // AMF 2.
  plmn_item               plmn_2 = plmn_item{plmn_identity::parse("99902").value(), std::vector<s_nssai_t>{s_nssai}};
  tac_t                   tac_2  = 7;
  supported_tracking_area supported_ta_2 = supported_tracking_area{tac_2, {plmn_2}};

  return {{supported_ta_1}, {supported_ta_2}};
}

class cu_cp_mocn_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_mocn_test() :
    cu_cp_test_environment(cu_cp_test_env_params{/*max_nof_cu_ups*/ 8,
                                                 /*max_nof_dus*/ 8,
                                                 /*max_nof_ues*/ 8192,
                                                 /*max_nof_drbs_per_ue*/ 8,
                                                 /*amf_config*/ make_amf_test_config()})
  {
    OCUDU_TEST_REQUIREMENTS("MVP-FUNC-RANS-16-1");
  }
};

//----------------------------------------------------------------------------------//
// CU-CP to AMF connection handling                                                 //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_mocn_test, when_cu_cp_is_created_then_it_is_not_connected_to_any_amf)
{
  ngap_message ngap_pdu;
  ASSERT_FALSE(get_amf().try_pop_rx_pdu(ngap_pdu))
      << "The CU-CP should not send a message to the NG interface before being started";

  ASSERT_FALSE(get_amf(1).try_pop_rx_pdu(ngap_pdu))
      << "The CU-CP should not send a message to the NG interface before being started";

  ASSERT_FALSE(get_cu_cp().get_ng_handler().amfs_are_connected());
}

TEST_F(cu_cp_mocn_test, when_cu_cp_starts_then_it_initiates_ng_setup_procedures_and_blocks_waiting_for_responses)
{
  // Enqueue AMF NG Setup Response for first AMF as an auto-reply to CU-CP.
  get_amf().enqueue_next_tx_pdu(generate_ng_setup_response());

  // Enqueue AMF NG Setup Response for second AMF as an auto-reply to CU-CP.
  get_amf(1).enqueue_next_tx_pdu(generate_ng_setup_response(plmn_identity::parse("99902").value()));

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connections.
  get_cu_cp().start();

  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf().try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_TRUE(get_amf(1).try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_TRUE(get_cu_cp().get_ng_handler().amfs_are_connected());
}

TEST_F(cu_cp_mocn_test, when_one_ng_setup_fails_then_cu_cp_is_not_in_amf_connected_state)
{
  // Enqueue AMF NG Setup Response for first AMF as an auto-reply to CU-CP.
  get_amf().enqueue_next_tx_pdu(generate_ng_setup_response());

  // Enqueue AMF NG Setup Failure for second AMF as an auto reply to CU-CP.
  get_amf(1).enqueue_next_tx_pdu(generate_ng_setup_failure());

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  get_cu_cp().start();

  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf().try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_TRUE(get_amf(1).try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_FALSE(get_cu_cp().get_ng_handler().amfs_are_connected());
}

//----------------------------------------------------------------------------------//
// DU connection handling                                                           //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_mocn_test, when_new_f1_setup_request_is_received_and_ngs_are_setup_then_f1_setup_is_accepted)
{
  // Run NG setups to completion.
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
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request(
      du_id,
      {test_helpers::served_cell_item_info{
          .sib1_str =
              "d20405c04300100e6640400000e000cd80018056109a000200046402051320c6b6c61b3704020000080800041a23524121304269"
              "2f1200000464e35b63224f80b0664abff0124e9106e28dc61b8e372c6fbf56c70ea008580d003058b68228"}}));

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

TEST_F(cu_cp_mocn_test,
       when_ng_setup_for_amf_supporting_the_dus_plmn_is_not_successful_then_f1_setup_is_accepted_with_no_cell_activated)
{
  // Enqueue AMF NG Setup Failure for first AMF as an auto-reply to CU-CP.
  get_amf().enqueue_next_tx_pdu(generate_ng_setup_failure());

  // Enqueue AMF NG Setup Response for second AMF as an auto-reply to CU-CP.
  get_amf(1).enqueue_next_tx_pdu(generate_ng_setup_response(plmn_identity::parse("99902").value()));

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  get_cu_cp().start();

  // Establish TNL connection between DU and CU-CP and start F1 setup procedure.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = *ret;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request());
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));

  // The CU-CP accepts the DU, but keeps all of its cells deactivated.
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(f1ap_pdu.pdu.successful_outcome().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::f1_setup_resp);
  ASSERT_FALSE(f1ap_pdu.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list_present);
}

TEST_F(cu_cp_mocn_test, when_ng_setup_for_amf_supporting_the_dus_plmn_is_successful_then_f1_setup_is_accepted)
{
  // Enqueue AMF NG Setup Response for first AMF as an auto-reply to CU-CP.
  get_amf().enqueue_next_tx_pdu(generate_ng_setup_response());

  // Enqueue AMF NG Setup Failure for second AMF as an auto-reply to CU-CP.
  get_amf(1).enqueue_next_tx_pdu(generate_ng_setup_failure());

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  get_cu_cp().start();

  // Establish TNL connection between DU and CU-CP and start F1 setup procedure.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned    du_idx = *ret;
  gnb_du_id_t du_id  = int_to_gnb_du_id(0x11);
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request(
      du_id,
      {test_helpers::served_cell_item_info{
          .sib1_str =
              "d20405c04300100e6640400000e000cd80018056109a000200046402051320c6b6c61b3704020000080800041a23524121304269"
              "2f1200000464e35b63224f80b0664abff0124e9106e28dc61b8e372c6fbf56c70ea008580d003058b68228"}}));
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));

  // The CU-CP should accept F1 setup.
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
}

TEST_F(cu_cp_mocn_test, when_the_amfs_connect_one_after_the_other_then_each_activates_the_cells_of_its_plmn)
{
  // Neither AMF is reachable yet, so the CU-CP starts without a core.
  get_amf().drop_connection();
  get_amf(1).drop_connection();
  get_cu_cp().start();

  // A DU with one cell per AMF PLMN.
  test_helpers::served_cell_item_info cell_a;
  test_helpers::served_cell_item_info cell_b;
  cell_b.plmn_id  = plmn_identity::parse("99902").value();
  cell_b.nci      = nr_cell_identity::create(gnb_id_t{411, 22}, 1).value();
  cell_b.pci      = 7;
  cell_b.sib1_str = test_helpers::create_sib1_hex_string(cell_b.plmn_id);

  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = *ret;
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x11), {cell_a, cell_b}));

  // The CU-CP accepts the DU and keeps both cells deactivated.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_FALSE(f1ap_pdu.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list_present);

  // Each AMF activates the cell of its own PLMN when it connects.
  for (const auto& [amf_idx, activated_cell] : {std::make_pair(0U, cell_a), std::make_pair(1U, cell_b)}) {
    ASSERT_TRUE(reconnect_amf(amf_idx)) << "AMF " << amf_idx << " did not reconnect within expected time";

    ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}))
        << "no activation update after AMF " << amf_idx << " connected";
    ASSERT_TRUE(test_helpers::is_valid_gnb_cu_configuration_update(f1ap_pdu));
    const auto& cu_cfg_upd = f1ap_pdu.pdu.init_msg().value.gnb_cu_cfg_upd();
    ASSERT_TRUE(cu_cfg_upd->cells_to_be_activ_list_present);
    ASSERT_EQ(cu_cfg_upd->cells_to_be_activ_list.size(), 1U) << "only the cells of the connected PLMN are activated";
    ASSERT_EQ(cu_cfg_upd->cells_to_be_activ_list[0]->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.to_number(),
              activated_cell.nci.value());

    f1ap_message ack = test_helpers::generate_gnb_cu_configuration_update_acknowledgement({});
    ack.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack()->transaction_id = cu_cfg_upd->transaction_id;
    get_du(du_idx).push_ul_pdu(ack);
  }
}

//----------------------------------------------------------------------------------//
// CU-UP connection handling                                                        //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_mocn_test, when_new_e1_setup_request_is_received_and_ngs_are_setup_then_e1_setup_is_accepted)
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

//----------------------------------------------------------------------------------//
//  UE connection handling                                                          //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_mocn_test, when_ngs_f1_e1_are_setup_then_ues_can_attach)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(
      du_idx,
      int_to_gnb_du_id(0x11),
      {test_helpers::served_cell_item_info{
          .sib1_str =
              "d20405c04300100e6640400000e000cd80018056109a000200046402051320c6b6c61b3704020000080800041a23524121304269"
              "2f1200000464e35b63224f80b0664abff0124e9106e28dc61b8e372c6fbf56c70ea008580d003058b68228"}}));

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

  // Verify F1AP DL RRC Message is sent with RRC Setup.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu));

  // Check UE is created.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1);
  ASSERT_EQ(report.ues[0].rnti, crnti);
}

TEST_F(cu_cp_mocn_test, when_ngs_f1_e1_are_setup_and_ue_selects_second_plmn_then_ue_can_attach)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(
      du_idx,
      int_to_gnb_du_id(0x11),
      {test_helpers::served_cell_item_info{
          .sib1_str =
              "d20405c04300100e6640400000e000cd80018056109a000200046402051320c6b6c61b3704020000080800041a23524121304269"
              "2f1200000464e35b63224f80b0664abff0124e9106e28dc61b8e372c6fbf56c70ea008580d003058b68228"}}));

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

  // Verify F1AP DL RRC Message is sent with RRC Setup.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu));

  // Send RRC Setup Complete.
  // > Generate UL DCCH message (containing RRC Setup Complete).
  byte_buffer pdu = test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete(2));
  // > Generate UL RRC Message (containing RRC Setup Complete) with PDCP SN=0.
  get_du(du_idx).push_rrc_ul_dcch_message(du_ue_f1ap_id, srb_id_t::srb1, std::move(pdu));

  // CU-CP should send an NGAP Initial UE Message to the correct AMF.
  ngap_message ngap_pdu;
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{1000}, 1))
      << "CU-CP did not send the Initial UE Message to the AMF";
  ASSERT_TRUE(test_helpers::is_valid_init_ue_message(ngap_pdu)) << "Invalid Initial UE Message";

  // Verify that user location info contains the UE-selected PLMN (second PLMN), not the cell's primary PLMN.
  const auto& init_ue_msg = ngap_pdu.pdu.init_msg().value.init_ue_msg();
  ASSERT_EQ(init_ue_msg->user_location_info.type(),
            asn1::ngap::user_location_info_c::types_opts::options::user_location_info_nr);
  const auto&   user_loc_info = init_ue_msg->user_location_info.user_location_info_nr();
  plmn_identity second_plmn   = plmn_identity::parse("99902").value();
  ASSERT_EQ(user_loc_info.nr_cgi.plmn_id.to_number(), second_plmn.to_bcd())
      << "NR-CGI PLMN should be the UE-selected PLMN, not the cell's primary PLMN";
  ASSERT_EQ(user_loc_info.tai.plmn_id.to_number(), second_plmn.to_bcd())
      << "TAI PLMN should be the UE-selected PLMN, not the cell's primary PLMN";

  // Check UE is created.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1);
  ASSERT_EQ(report.ues[0].rnti, crnti);
}

TEST_F(cu_cp_mocn_test, when_cells_of_different_plmns_share_an_nci_then_ue_is_routed_to_the_amf_of_its_cell)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Setup DU with one cell per PLMN. The NCI is only unique within a PLMN, so both cells may use the same one.
  test_helpers::served_cell_item_info cell_a;
  test_helpers::served_cell_item_info cell_b;
  cell_b.plmn_id  = plmn_identity::parse("99902").value();
  cell_b.pci      = 1;
  cell_b.sib1_str = test_helpers::create_sib1_hex_string(cell_b.plmn_id);
  ASSERT_EQ(cell_a.nci, cell_b.nci);

  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();
  ASSERT_TRUE(this->run_f1_setup(du_idx, int_to_gnb_du_id(0x11), {cell_a, cell_b}));

  // Setup CU-UP.
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = ret.value();
  ASSERT_TRUE(this->run_e1_setup(cu_up_idx));

  // Create UE in cell A.
  gnb_du_ue_f1ap_id_t du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti         = to_rnti(0x4601);
  get_du(du_idx).push_ul_pdu(test_helpers::generate_init_ul_rrc_message_transfer(du_ue_f1ap_id, crnti, cell_a.plmn_id));
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu));

  // UE selects the only PLMN of cell A.
  byte_buffer pdu = test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete(1));
  get_du(du_idx).push_rrc_ul_dcch_message(du_ue_f1ap_id, srb_id_t::srb1, std::move(pdu));

  // CU-CP should send the Initial UE Message to the AMF of cell A's PLMN.
  ngap_message ngap_pdu;
  ASSERT_FALSE(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{100}, 1))
      << "Initial UE Message was sent to the AMF of cell B's PLMN";
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds{1000}, 0))
      << "CU-CP did not send the Initial UE Message to the AMF of cell A's PLMN";
  ASSERT_TRUE(test_helpers::is_valid_init_ue_message(ngap_pdu)) << "Invalid Initial UE Message";
  const auto& user_loc_info = ngap_pdu.pdu.init_msg().value.init_ue_msg()->user_location_info.user_location_info_nr();
  ASSERT_EQ(user_loc_info.nr_cgi.plmn_id.to_number(), cell_a.plmn_id.to_bcd());
}
