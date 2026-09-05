// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/e1ap/e1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ngap/ngap_message.h"
#include "ocudu/ran/plmn_identity.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class cu_cp_intra_du_handover_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  explicit cu_cp_intra_du_handover_test(
      cu_cp_test_env_params                         prms,
      const std::optional<location_report_request>& location_reporting_request = std::nullopt) :
    cu_cp_test_environment(std::move(prms))
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Setup DU.
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(
        this->run_f1_setup(du_idx,
                           int_to_gnb_du_id(0x11),
                           {{.nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0U).value(), .pci = 0, .tac = 7},
                            {.nci = nr_cell_identity::create(gnb_id_t{411, 22}, 1U).value(), .pci = 2, .tac = 7}}));

    // Setup CU-UP.
    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));

    // Connect UE 0x4601.
    EXPECT_TRUE(
        cu_cp_test_environment::attach_ue(du_idx,
                                          cu_up_idx,
                                          du_ue_id,
                                          crnti,
                                          amf_ue_id,
                                          cu_up_e1ap_id,
                                          psi,
                                          drb_id_t::drb1,
                                          qfi,
                                          test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete()),
                                          make_byte_buffer("00070e00cc6fcda5").value(),
                                          std::nullopt,
                                          true,
                                          location_reporting_request));
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);

    EXPECT_NE(ue_ctx, nullptr);
  }

  explicit cu_cp_intra_du_handover_test(
      const std::optional<location_report_request>& location_reporting_request = std::nullopt) :
    cu_cp_intra_du_handover_test(cu_cp_test_env_params{}, location_reporting_request)
  {
  }

  [[nodiscard]] bool send_rrc_measurement_report_and_await_ue_context_setup_request()
  {
    // Inject UL RRC Message (containing RRC Measurement Report) and wait for UE Context Setup Request.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        ue_ctx->du_ue_id.value(),
        ue_ctx->cu_ue_id.value(),
        srb_id_t::srb1,
        make_byte_buffer("000800410004015f741fe0804bf183fcaa6e9699").value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Setup Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request_with_ue_capabilities(f1ap_pdu),
                              "Invalid UE Context Setup Request");

    target_cu_ue_id =
        int_to_gnb_cu_ue_f1ap_id(f1ap_pdu.pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id);

    return true;
  }

  [[nodiscard]] bool send_intra_cell_ho_command_and_await_ue_context_setup_request()
  {
    // Inject UL RRC Message (containing RRC Measurement Report) and wait for UE Context Setup Request.
    get_cu_cp().get_command_handler().get_mobility_command_handler().trigger_handover(
        pci_t{0}, crnti, pci_t{0}, plmn_identity::parse("00101").value(), 7);
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Setup Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request_with_ue_capabilities(f1ap_pdu),
                              "Invalid UE Context Setup Request");

    target_cu_ue_id =
        int_to_gnb_cu_ue_f1ap_id(f1ap_pdu.pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id);

    return true;
  }

  [[nodiscard]] bool send_ue_context_setup_failure()
  {
    // Inject UE Context Setup Failure.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_setup_failure(target_cu_ue_id, target_du_ue_id));
    return true;
  }

  [[nodiscard]] bool send_ue_context_setup_response_and_await_ue_context_modification_request()
  {
    // Inject UE Context Setup Response and await UE Context Modification Request.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_setup_response(
        target_cu_ue_id,
        target_du_ue_id,
        crnti,
        make_byte_buffer("5c06c0060030258380f80408d07810000929dc601349798002692f"
                         "1200000464c6b6c61b3704020000080800041a235246c0134978"
                         "90000023271adb19127c03033255878092748837146e30dc71b963"
                         "7dfab6387580221603400c162300e20981950001ff0000000003"
                         "06e10840000402ca0041904000040d31a01100102000e388844800"
                         "4080038e2221400102000e3888c60004080038e24060088681aa"
                         "b2420e0008206102860e4a60c9a3670e8f00000850000800b50001"
                         "000850101800b50102000850202800b50203000850303800b503"
                         "0b8c8b5040c00504032014120d00505036014160e0050603a0141a"
                         "120c506a0496302a72fd159e26f2681d2083c5df81821c000000"
                         "38ffd294a5294f28160000219760000000000005000001456aa280"
                         "23800c00041000710804e20070101084000e21009c200e040220"
                         "8001c420138401c0c042100038840270c038200882000710804e18"
                         "004000000410c04080c100e0d0000e388000000400800100c001"
                         "0120044014c00004620090e3800c")
            .value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");
    {
      // Check if tx_action_ind is set to stop
      report_fatal_error_if_not(f1ap_pdu.pdu.init_msg().value.ue_context_mod_request()->tx_action_ind_present &&
                                    f1ap_pdu.pdu.init_msg().value.ue_context_mod_request()->tx_action_ind ==
                                        asn1::f1ap::tx_action_ind_e::stop,
                                "Invalid TxActionInd");

      const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
      report_fatal_error_if_not(
          test_helpers::is_valid_rrc_reconfiguration(test_helpers::extract_dl_dcch_msg(rrc_container), false, {}, {}),
          "Invalid RRC Reconfiguration");
    }
    return true;
  }

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  [[nodiscard]] bool send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request(
      const std::string& rrc_reconfig_complete_buffer)
  {
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        target_du_ue_id, target_cu_ue_id, srb_id_t::srb1, make_byte_buffer(rrc_reconfig_complete_buffer).value()));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");

    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_failure_and_await_bearer_context_release_command()
  {
    // Inject Bearer Context Modification Failure and wait for UE Context Release Command.
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_modification_failure(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_release_command(e1ap_pdu),
                              "Invalid Bearer Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_release_complete_and_await_ue_context_release_command()
  {
    // Inject Bearer Context Release Complete and wait for UE Context Release Command.
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_release_complete(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_modification_response_and_await_ue_context_modification_request()
  {
    // Inject Bearer Context Modification Response and wait for RRC Reconfiguration.
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_modification_response(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");
    return true;
  }

  [[nodiscard]] bool send_ue_context_modification_response()
  {
    // Inject UE Context Modification Response and wait for UE Context Release Command.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_modification_response(
        ue_ctx->du_ue_id.value(), ue_ctx->cu_ue_id.value(), crnti));
    return true;
  }

  [[nodiscard]] bool timeout_rrc_reconfiguration_and_await_f1ap_ue_context_release_command()
  {
    // Fail RRC Reconfiguration (UE doesn't respond) and wait for F1AP UE Context Release Command.
    if (tick_until(std::chrono::milliseconds(3000), [&]() { return false; }, false)) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_rrc_reconfiguration_complete()
  {
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        target_du_ue_id, target_cu_ue_id, srb_id_t::srb1, make_byte_buffer("80000800db659eb2").value()));
    return true;
  }

  [[nodiscard]] bool send_ue_context_modification_response_and_await_ue_context_release_command()
  {
    // Inject UE Context Modification Response and wait for UE Context Release Command.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_modification_response(target_du_ue_id, target_cu_ue_id, crnti));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_f1ap_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id,
                                                           gnb_du_ue_f1ap_id_t du_ue_f1ap_id)
  {
    // Inject F1AP UE Context Release Complete.
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_complete(cu_ue_f1ap_id, du_ue_f1ap_id));
    return true;
  }

  [[nodiscard]] bool timeout_handover_ue_release_timer_and_await_ngap_ue_context_release_request()
  {
    // Fail RRC Reconfiguration (UE doesn't respond) and wait for NGAP UE Context Release Request.
    if (tick_until(std::chrono::milliseconds(6400), [&]() { return false; }, false)) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu),
                              "Failed to receive NGAP UE Context Release Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_request(ngap_pdu),
                              "Invalid NGAP UE Context Release Request");
    return true;
  }

  [[nodiscard]] bool send_ngap_ue_context_release_command_and_await_bearer_context_release_command()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Inject NGAP UE Context Release Command and wait for Bearer Context Release Command
    get_amf().push_tx_pdu(generate_valid_ue_context_release_command_with_amf_ue_ngap_id(ue_ctx->amf_ue_id.value()));
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu),
                              "Failed to receive Bearer Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_release_command(e1ap_pdu),
                              "Invalid Bearer Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command()
  {
    // Inject Bearer Context Release Complete and wait for F1AP UE Context Release Command
    get_cu_up(cu_up_idx).push_tx_pdu(
        generate_bearer_context_release_complete(ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value()));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");
    return true;
  }

  [[nodiscard]] bool send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete()
  {
    // Inject F1AP UE Context Release Complete and wait for N1AP UE Context Release Command
    if (!send_f1ap_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value())) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu), "Failed to receive UE Context Release Complete");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_complete(ngap_pdu),
                              "Invalid UE Context Release Complete");
    return true;
  }

  [[nodiscard]] bool
  reestablish_target_ue(gnb_du_ue_f1ap_id_t new_du_ue_id, rnti_t new_crnti, rnti_t old_crnti, pci_t old_pci)
  {
    // Send Initial UL RRC Message (containing RRC Reestablishment Request) to CU-CP.
    byte_buffer rrc_container = test_helpers::pack_ul_ccch_msg(
        test_helpers::create_rrc_reestablishment_request(old_crnti, old_pci, "1111010001000010"));
    f1ap_message f1ap_init_ul_rrc_msg = test_helpers::generate_init_ul_rrc_message_transfer(
        new_du_ue_id, new_crnti, plmn_identity::test_value(), {}, std::move(rrc_container));
    get_du(du_idx).push_ul_pdu(f1ap_init_ul_rrc_msg);

    // Wait for DL RRC message transfer (F1AP UE Context Release Command).
    bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Context Release Command");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_release_command(f1ap_pdu),
                              "Invalid UE Context Release Command");

    // Wait for DL RRC message transfer (with RRC Reestablishment / RRC Setup / RRC Reject).
    result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "F1AP DL RRC Message Transfer with Msg4 not sent to DU");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu), "Invalid Msg4");

    auto& dl_rrc_msg = *f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
    report_fatal_error_if_not(int_to_gnb_du_ue_f1ap_id(dl_rrc_msg.gnb_du_ue_f1ap_id) == new_du_ue_id,
                              "Invalid gNB-DU-UE-F1AP-ID");

    // Inject F1AP UE Context Release Complete for target UE.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(1), int_to_gnb_du_ue_f1ap_id(1)));

    ue_context old_ue;
    old_ue.du_ue_id = new_du_ue_id;
    old_ue.cu_ue_id = int_to_gnb_cu_ue_f1ap_id(dl_rrc_msg.gnb_cu_ue_f1ap_id);
    old_ue.crnti    = new_crnti;

    // EVENT: Send RRC Reestablishment Complete.
    // > Generate UL-DCCH message (containing RRC Reestablishment Complete).
    byte_buffer pdu = test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_reestablishment_complete());
    // > Prepend PDCP header and append MAC.
    report_error_if_not(pdu.prepend(std::array<uint8_t, 2>{0x00U, 0x00U}), "bad alloc");
    report_error_if_not(pdu.append(std::array<uint8_t, 4>{0x85, 0xc1, 0x04, 0xf1}), "bad alloc");
    // > Send UL RRC Message to CU-CP.
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ul_rrc_message_transfer(new_du_ue_id, *old_ue.cu_ue_id, srb_id_t::srb1, std::move(pdu)));

    // STATUS: CU-CP sends E1AP Bearer Context Modification Request.
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(0, e1ap_pdu),
                              "E1AP BearerContextModificationRequest NOT sent");

    gnb_cu_cp_ue_e1ap_id_t cu_cp_e1ap_id =
        int_to_gnb_cu_cp_ue_e1ap_id(e1ap_pdu.pdu.init_msg().value.bearer_context_mod_request()->gnb_cu_cp_ue_e1ap_id);

    // EVENT: Inject E1AP Bearer Context Modification Response
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));

    // STATUS: CU-CP sends F1AP UE Context Modification Request to DU.
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "F1AP UEContextModificationRequest NOT sent");
    report_fatal_error_if_not(
        f1ap_pdu.pdu.init_msg().value.ue_context_mod_request()->drbs_to_be_modified_list_present,
        "UE Context Modification Request for RRC Reestablishment must contain DRBs to be modified");
    report_fatal_error_if_not(
        not f1ap_pdu.pdu.init_msg().value.ue_context_mod_request()->drbs_to_be_setup_mod_list_present,
        "UE Context Modification Request for RRC Reestablishment must not contain DRBs to be setup");

    gnb_cu_ue_f1ap_id_t new_cu_ue_id =
        int_to_gnb_cu_ue_f1ap_id(f1ap_pdu.pdu.init_msg().value.ue_context_mod_request()->gnb_cu_ue_f1ap_id);

    // EVENT: Inject F1AP UE Context Modification Response
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_modification_response(new_du_ue_id, new_cu_ue_id, new_crnti));

    // STATUS: CU-CP sends E1AP Bearer Context Modification Request.
    report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(0, e1ap_pdu),
                              "E1AP BearerContextModificationRequest NOT sent");

    // EVENT: CU-UP sends E1AP Bearer Context Modification Response
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));

    // STATUS: CU-CP sends F1AP DL RRC Message Transfer (containing RRC Reconfiguration).
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu), "F1AP DL RRC Message NOT sent");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                              "Invalid DL RRC Message Transfer");

    // EVENT: DU sends F1AP UL RRC Message Transfer (containing RRC Reconfiguration Complete).
    pdu = test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_reconfiguration_complete(1U));
    // > Prepend PDCP header and append MAC.
    report_error_if_not(pdu.prepend(std::array<uint8_t, 2>{0x00U, 0x01U}), "bad alloc");
    report_error_if_not(pdu.append(std::array<uint8_t, 4>{0xf1, 0x21, 0x02, 0x5e}), "bad alloc");
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ul_rrc_message_transfer(new_du_ue_id, *old_ue.cu_ue_id, srb_id_t::srb1, std::move(pdu)));

    return true;
  }

  void verify_ngap_location_report(const ngap_message& msg) const
  {
    ASSERT_TRUE(test_helpers::is_valid_location_report(msg));
    const auto& report = msg.pdu.init_msg().value.location_report();
    EXPECT_EQ(report->amf_ue_ngap_id, to_underlying(amf_ue_id));
    // Verify the configured event type is echoed back.
    EXPECT_EQ(report->location_report_request_type.event_type,
              asn1::ngap::event_type_opts::options::change_of_serve_cell);
    EXPECT_EQ(report->location_report_request_type.report_area.value, asn1::ngap::report_area_opts::options::cell);
    // Verify user location info points to the target cell (PCI 2, NCI index 1).
    ASSERT_EQ(report->user_location_info.type(),
              asn1::ngap::user_location_info_c::types_opts::options::user_location_info_nr);
    const auto& loc = report->user_location_info.user_location_info_nr();
    EXPECT_EQ(loc.nr_cgi.nr_cell_id.to_number(), nr_cell_identity::create(gnb_id_t{411, 22}, 1U).value().value());
    EXPECT_EQ(loc.nr_cgi.plmn_id.to_number(), plmn_identity::test_value().to_bcd());
    EXPECT_EQ(loc.tai.tac.to_number(), 7U);
    EXPECT_EQ(loc.tai.plmn_id.to_number(), plmn_identity::test_value().to_bcd());
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t du_ue_id  = gnb_du_ue_f1ap_id_t::min;
  rnti_t              crnti     = to_rnti(0x4601);
  pci_t               pci       = 0;
  amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
      test_rng::uniform_int<uint64_t>(to_underlying(amf_ue_id_t::min), to_underlying(amf_ue_id_t::max)));
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;

  const ue_context* ue_ctx = nullptr;

  gnb_du_ue_f1ap_id_t target_du_ue_id = int_to_gnb_du_ue_f1ap_id(1);
  gnb_cu_ue_f1ap_id_t target_cu_ue_id;

  pdu_session_id_t psi = uint_to_pdu_session_id(1);
  qos_flow_id_t    qfi = uint_to_qos_flow_id(1);

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
  e1ap_message e1ap_pdu;
};

TEST_F(cu_cp_intra_du_handover_test, when_ue_context_setup_fails_then_ho_fails)
{
  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Failure.
  ASSERT_TRUE(send_ue_context_setup_failure());

  // STATUS: Target UE should be removed from DU.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1) << "UE should be removed";
}

TEST_F(cu_cp_intra_du_handover_test, when_bearer_context_modification_fails_then_ho_fails)
{
  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await UE Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response());

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request("80000800db659eb2"));

  // Inject Bearer Context Modification Failure and await Bearer Context Release Command.
  ASSERT_TRUE(send_bearer_context_modification_failure_and_await_bearer_context_release_command());

  // Inject Bearer Context Release Complete and await F1AP UE Context Release Command.
  ASSERT_TRUE(send_bearer_context_release_complete_and_await_ue_context_release_command());

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(target_cu_ue_id, target_du_ue_id));

  // STATUS: Target UE should be removed from DU.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1) << "UE should be removed";
}

TEST_F(cu_cp_intra_du_handover_test, when_rrc_reconfiguration_fails_then_ho_fails)
{
  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response());

  // Let the RRC Reconfiguration timeout and await F1AP UE Context Release Command for target UE.
  ASSERT_TRUE(timeout_rrc_reconfiguration_and_await_f1ap_ue_context_release_command());

  // Inject F1AP UE Context Release Complete for target UE.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(target_cu_ue_id, target_du_ue_id));

  // STATUS: Target UE should be removed from DU.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1) << "Target UE should be removed";
}

TEST_F(cu_cp_intra_du_handover_test, when_ho_succeeds_then_source_ue_is_removed)
{
  // Check that the metrics report doesn't contain a requested/successful handover execution.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 0U);
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 0U);

  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await UE Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response());

  // Check that the metrics report contains a requested handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 1U);

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request("80000800db659eb2"));

  // Inject Bearer Context Modification Response and await UE Context Modification Request.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await UE Context Release Command.
  ASSERT_TRUE(send_ue_context_modification_response_and_await_ue_context_release_command());

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));

  // Check that the metrics report contains a successful handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 1U);

  // STATUS: Source UE should be removed from DU.
  ASSERT_EQ(report.ues.size(), 1) << "Source UE should be removed";
}

/// Fixture that attaches the UE with cell-change location reporting configured via Initial Context Setup.
class cu_cp_intra_du_handover_initial_context_setup_location_reporting_test : public cu_cp_intra_du_handover_test
{
public:
  cu_cp_intra_du_handover_initial_context_setup_location_reporting_test() :
    cu_cp_intra_du_handover_test([] {
      location_report_request req;
      req.location_reporting_type = location_report_request::event_type::change_of_serve_cell;
      return req;
    }())
  {
  }
};

TEST_F(cu_cp_intra_du_handover_initial_context_setup_location_reporting_test,
       when_cell_change_reporting_configured_in_initial_context_setup_then_location_report_is_sent_after_ho)
{
  // Unlike the Location Reporting Control case, no immediate location report is sent when location reporting is
  // configured via Initial Context Setup.

  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await UE Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response());

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request("80000800db659eb2"));

  // Inject Bearer Context Modification Response and await UE Context Modification Request.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await UE Context Release Command.
  // The location report is sent to the AMF before the source UE is released.
  ASSERT_TRUE(send_ue_context_modification_response_and_await_ue_context_release_command());

  // Check that an NGAP Location Report was sent to the AMF after intra DU handover.
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_NO_FATAL_FAILURE(verify_ngap_location_report(ngap_pdu));

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));
}

TEST_F(cu_cp_intra_du_handover_test,
       when_cell_change_reporting_configured_then_location_report_is_sent_immediately_and_after_ho)
{
  // Configure cell change location reporting.
  get_amf().push_tx_pdu(
      generate_location_reporting_control_message_with_cell_change(amf_ue_id, ue_ctx->ran_ue_id.value()));

  // Check that location report was sent upon configuration of cell change reporting (3GPP TS 38.413 8.12.1.2).
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_location_report(ngap_pdu));

  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await UE Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response());

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request("80000800db659eb2"));

  // Inject Bearer Context Modification Response and await UE Context Modification Request.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response and await UE Context Release Command.
  ASSERT_TRUE(send_ue_context_modification_response_and_await_ue_context_release_command());

  // Check that an NGAP Location Report was sent to the AMF after intra DU handover.
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_NO_FATAL_FAILURE(verify_ngap_location_report(ngap_pdu));

  // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));
}

TEST_F(cu_cp_intra_du_handover_test, when_ho_fails_and_ue_is_gone_then_source_and_target_ue_are_removed)
{
  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await UE Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response());

  // Let the RRC Reconfiguration timeout and await F1AP UE Context Release Command for target UE.
  ASSERT_TRUE(timeout_rrc_reconfiguration_and_await_f1ap_ue_context_release_command());

  // Inject F1AP UE Context Release Complete for target UE.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(target_cu_ue_id, target_du_ue_id));

  // Let the UE release timer timeout and await NGAP UE Context Release Request for source UE.
  ASSERT_TRUE(timeout_handover_ue_release_timer_and_await_ngap_ue_context_release_request());

  // Inject NGAP UE Context Release Command and await E1AP Bearer Context Release Command
  ASSERT_TRUE(send_ngap_ue_context_release_command_and_await_bearer_context_release_command());

  // Inject Bearer Context Release Complete and await F1AP UE Context Release Command
  ASSERT_TRUE(send_bearer_context_release_complete_and_await_f1ap_ue_context_release_command());

  // Inject F1AP UE Context Release Complete and await NGAP UE Context Release Complete
  ASSERT_TRUE(send_f1ap_ue_context_release_complete_and_await_ngap_ue_context_release_complete());

  // STATUS: Source and target UE should be removed from DU.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0) << "Target UE should be removed";
}

TEST_F(cu_cp_intra_du_handover_test, when_ho_fails_then_reestablishment_to_source_ue_succeeds)
{
  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await Bearer Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response());

  // Let the RRC Reconfiguration timeout and await F1AP UE Context Release Command for target UE.
  ASSERT_TRUE(timeout_rrc_reconfiguration_and_await_f1ap_ue_context_release_command());

  // Inject F1AP UE Context Release Complete for target UE.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(target_cu_ue_id, target_du_ue_id));

  // Inject RRC Reestablishment Request for source UE.
  gnb_du_ue_f1ap_id_t new_du_ue_id = int_to_gnb_du_ue_f1ap_id(2);
  rnti_t              new_crnti    = to_rnti(0x4602);
  ASSERT_TRUE(reestablish_ue(du_idx, cu_up_idx, new_du_ue_id, new_crnti, crnti, pci)) << "Reestablishment failed";

  // STATUS: Target UE should be removed from DU.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1) << "Target UE should be removed";
}

TEST_F(cu_cp_intra_du_handover_test, when_ho_fails_then_reestablishment_to_target_ue_succeeds)
{
  // Inject Measurement Report and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_rrc_measurement_report_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await UE Context Modification Request.
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  ASSERT_TRUE(send_ue_context_modification_response());

  // Inject RRC Reestablishment Request for target UE.
  gnb_du_ue_f1ap_id_t new_du_ue_id = int_to_gnb_du_ue_f1ap_id(2);
  rnti_t              new_crnti    = to_rnti(0x4602);
  ASSERT_TRUE(reestablish_target_ue(new_du_ue_id, new_crnti, crnti, 0)) << "Reestablishment failed";

  // STATUS: Source UE should be removed from DU.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1) << "Source UE should be removed";
}

TEST_F(cu_cp_intra_du_handover_test, intra_cell_ho_test)
{
  // Check that the metrics report doesn't contain a requested/successful handover execution.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 0U);
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 0U);

  // Inject intra-cell HO command and await F1AP UE Context Setup Request.
  ASSERT_TRUE(send_intra_cell_ho_command_and_await_ue_context_setup_request());

  // Inject UE Context Setup Response and await UE Context Modification Request.
  target_du_ue_id = int_to_gnb_du_ue_f1ap_id(2);
  crnti           = to_rnti(0x4602);
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  crnti = to_rnti(0x4602);
  ASSERT_TRUE(send_ue_context_modification_response());

  // Check that the metrics report contains a requested handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 1U);
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 0U);

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request("80000800795ae600"));

  // Inject Bearer Context Modification Response and await UE Context Modification Request.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // STATUS: Source UE is still present in DU.
  ASSERT_EQ(report.ues.size(), 2) << "Source UE should be still present";

  // // Inject UE Context Modification Response and await UE Context Release Command.
  ASSERT_TRUE(send_ue_context_modification_response_and_await_ue_context_release_command());

  // // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));

  // // Check that the metrics report contains a successful handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 1U);

  // // STATUS: Source UE should be removed from DU.
  ASSERT_EQ(report.ues.size(), 1) << "Source UE should be removed";
}

class cu_cp_intra_cell_handover_after_drb_id_wraparound_test : public cu_cp_intra_du_handover_test
{
public:
  [[nodiscard]] bool
  setup_pdu_session(pdu_session_id_t psi_,
                    drb_id_t         drb_id_,
                    qos_flow_id_t    qfi_,
                    byte_buffer      rrc_reconfiguration_complete = make_byte_buffer("00070e00cc6fcda5").value(),
                    bool             is_initial_session_          = false)
  {
    return cu_cp_test_environment::setup_pdu_session(du_idx,
                                                     cu_up_idx,
                                                     du_ue_id,
                                                     crnti,
                                                     cu_up_e1ap_id,
                                                     psi_,
                                                     drb_id_,
                                                     qfi_,
                                                     std::move(rrc_reconfiguration_complete),
                                                     is_initial_session_);
  }

  [[nodiscard]] bool release_pdu_session(pdu_session_id_t psi_,
                                         drb_id_t         drb_id_,
                                         qos_flow_id_t    qfi_,
                                         byte_buffer      rrc_reconfiguration_complete_)
  {
    ocudu_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
    ocudu_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");
    ocudu_assert(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                 "there are still E1AP messages to pop from CU-UP");

    // Inject NGAP PDU Session Resource Release Command and await Bearer Context Modification Request
    get_amf().push_tx_pdu(
        generate_valid_pdu_session_resource_release_command(amf_ue_id, ue_ctx->ran_ue_id.value(), psi_));
    bool result = this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive Bearer Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                              "Invalid Bearer Context Modification Request");

    // Inject Bearer Context Modification Response and wait for UE Context Modification Request
    get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(
        ue_ctx->cu_cp_e1ap_id.value(), ue_ctx->cu_up_e1ap_id.value(), {{psi_, {drb_test_params{drb_id_, qfi_}}}}, {}));
    result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "Failed to receive UE Context Modification Request");
    report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                              "Invalid UE Context Modification Request");

    // Inject UE Context Modification Response and await RRC Reconfiguration
    get_du(du_idx).push_ul_pdu(
        test_helpers::generate_ue_context_modification_response(du_ue_id, ue_ctx->cu_ue_id.value(), crnti));
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu),
                              "Failed to receive F1AP DL RRC Message (containing RRC Reconfiguration)");
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                              "Invalid DL RRC Message Transfer");
    {
      const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
      report_fatal_error_if_not(
          test_helpers::is_valid_rrc_reconfiguration(test_helpers::extract_dl_dcch_msg(rrc_container), {}, {}),
          "Invalid RRC Reconfiguration");
    }

    // Inject RRC Reconfiguration Complete and await PDU Session Resource Release Response
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        du_ue_id, ue_ctx->cu_ue_id.value(), srb_id_t::srb1, std::move(rrc_reconfiguration_complete_)));
    report_fatal_error_if_not(this->wait_for_ngap_tx_pdu(ngap_pdu, std::chrono::milliseconds(10000)),
                              "Failed to receive PDU Session Resource Release Response");
    report_fatal_error_if_not(test_helpers::is_valid_pdu_session_resource_release_response(ngap_pdu),
                              "Invalid PDU Session Resource Release Response");
    return true;
  }

  void increment_count_and_transaction_id()
  {
    count++;
    if (transaction_id < 3) {
      ++transaction_id;
    } else {
      transaction_id = 0;
    }
  }

  unsigned transaction_id = 0;
  uint8_t  count          = 8;
};

TEST_F(cu_cp_intra_cell_handover_after_drb_id_wraparound_test,
       when_too_many_stale_drb_ids_are_present_intra_cell_handover_will_be_triggerred_proactively)
{
  // UE is already attached and one PDU session with one DRB is setup
  // No intra-cell handover has been triggered yet.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 0U);
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 0U);

  // Add and remove PDU session 2-23 with one DRB (22 times):
  for (uint8_t i = 2; i <= 23; i++) {
    ASSERT_TRUE(
        cu_cp_test_environment::setup_pdu_session(du_idx,
                                                  cu_up_idx,
                                                  du_ue_id,
                                                  crnti,
                                                  cu_up_e1ap_id,
                                                  uint_to_pdu_session_id(i),
                                                  uint_to_drb_id(i),
                                                  uint_to_qos_flow_id(i),
                                                  generate_rrc_reconfiguration_complete_pdu(transaction_id, count),
                                                  false));

    increment_count_and_transaction_id();
    ASSERT_TRUE(release_pdu_session(uint_to_pdu_session_id(i),
                                    uint_to_drb_id(i),
                                    uint_to_qos_flow_id(i),
                                    generate_rrc_reconfiguration_complete_pdu(transaction_id, count)));
    increment_count_and_transaction_id();

    // Check that intra-cell HO was not triggered yet
    ocudu_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
    ocudu_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU ");
    ocudu_assert(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                 "there are still E1AP messages to pop from CU-UP");
  }

  // 1 DRB ID is still in use.
  // 22 DRB IDs are stale - used and released by PDU Sessions 2-23.
  // 6 DRB IDs are left available, but UE may need add up to 7 more DRBs.
  // Next PDU Session Resource Setup Procedure with one DRB should trigger intra-cell HO at the end.
  uint16_t next_drb_id = 24;
  ASSERT_TRUE(setup_pdu_session(uint_to_pdu_session_id(next_drb_id),
                                uint_to_drb_id(next_drb_id),
                                uint_to_qos_flow_id(next_drb_id),
                                generate_rrc_reconfiguration_complete_pdu(transaction_id, count),
                                false));
  increment_count_and_transaction_id();

  // intra-cell HO is triggered
  report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu), "Failed to receive UE Context Setup Request");
  report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request_with_ue_capabilities(f1ap_pdu),
                            "Invalid UE Context Setup Request");

  // Inject UE Context Setup Response and await UE Context Modification Request.
  target_du_ue_id = int_to_gnb_du_ue_f1ap_id(2);
  target_cu_ue_id = int_to_gnb_cu_ue_f1ap_id(1);
  crnti           = to_rnti(0x4602);
  ASSERT_TRUE(send_ue_context_setup_response_and_await_ue_context_modification_request());

  // Inject UE Context Modification Response.
  crnti = to_rnti(0x4602);
  ASSERT_TRUE(send_ue_context_modification_response());

  // Check that the metrics report contains a requested handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_handover_executions_requested, 1U);
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 0U);

  // Inject RRC Reconfiguration Complete and await Bearer Context Modification Request.
  ASSERT_TRUE(send_rrc_reconfiguration_complete_and_await_bearer_context_modification_request("00000a00ff014561"));

  // Inject Bearer Context Modification Response and await UE Context Modification Request.
  ASSERT_TRUE(send_bearer_context_modification_response_and_await_ue_context_modification_request());

  // STATUS: Source UE is still present in DU.
  ASSERT_EQ(report.ues.size(), 2) << "Source UE should be still present";

  // // Inject UE Context Modification Response and await UE Context Release Command.
  ASSERT_TRUE(send_ue_context_modification_response_and_await_ue_context_release_command());

  // // Inject F1AP UE Context Release Complete.
  ASSERT_TRUE(send_f1ap_ue_context_release_complete(ue_ctx->cu_ue_id.value(), ue_ctx->du_ue_id.value()));

  // // Check that the metrics report contains a successful handover execution.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 1U);

  // // STATUS: Source UE should be removed from DU.
  ASSERT_EQ(report.ues.size(), 1) << "Source UE should be removed";

  // report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.mobility.nof_successful_handover_executions, 1U);
}

///////////////////////////////////////////////////////////////////////////////
//                             Admission Rejection
///////////////////////////////////////////////////////////////////////////////
class cu_cp_intra_du_handover_admission_limit_test : public cu_cp_intra_du_handover_test
{
public:
  cu_cp_intra_du_handover_admission_limit_test() :
    cu_cp_intra_du_handover_test(cu_cp_test_env_params{/*max_nof_cu_ups*/ 8,
                                                       /*max_nof_dus*/ 8,
                                                       /*max_nof_ues*/ 1})
  {
  }
};

TEST_F(cu_cp_intra_du_handover_admission_limit_test, when_target_ue_is_not_servable_then_handover_fails)
{
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
      ue_ctx->du_ue_id.value(),
      ue_ctx->cu_ue_id.value(),
      srb_id_t::srb1,
      make_byte_buffer("000800410004015f741fe0804bf183fcaa6e9699").value()));

  ASSERT_FALSE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{100}));

  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1) << "Target UE should be cleaned up on admission rejection";
  ASSERT_EQ(report.ues[0].rnti, crnti);
}
