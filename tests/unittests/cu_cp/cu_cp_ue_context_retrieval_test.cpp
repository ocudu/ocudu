// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/cu_cp/routines/mobility/ue_context_retrieval_helpers.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/rrc/rrc_ue_test_helpers.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture for the old NG-RAN node side of the UE context retrieval procedure (TS 38.423 section 8.2.4), i.e. this
/// node answering a peer that wants to take over one of our UEs.
class cu_cp_ue_context_retrieval_test : public ::testing::Test
{
protected:
  cu_cp_ue_context_retrieval_test() :
    cu_cp_cfg([this]() {
      cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
      cucfg.services.timers         = &timers;
      cucfg.services.cu_cp_executor = &cu_worker;
      return cucfg;
    }()),
    ue_cfg([this]() {
      return ue_manager_config{.gnb_id              = cu_cp_cfg.node.gnb_id,
                               .max_nof_ues         = cu_cp_cfg.admission.max_nof_ues,
                               .drb_config          = cu_cp_cfg.bearers.drb_config,
                               .max_nof_drbs_per_ue = cu_cp_cfg.admission.max_nof_drbs_per_ue,
                               .int_algo_pref_list  = cu_cp_cfg.security.int_algo_pref_list,
                               .enc_algo_pref_list  = cu_cp_cfg.security.enc_algo_pref_list,
                               .enable_rrc_metrics  = cu_cp_cfg.metrics.layers_cfg.enable_rrc_metrics,
                               .ue                  = cu_cp_cfg.ue};
    }()),
    ue_dependencies([this]() {
      return ue_manager_dependencies{
          .timers = timers, .cu_cp_executor = cu_worker, .logger = ocudulog::fetch_basic_logger("CU-CP")};
    }())
  {
    ocudulog::init();
  }

  /// Creates a UE that is fully attached: RRC UE, security context and one PDU session with one DRB.
  cu_cp_ue* create_attached_ue()
  {
    const cu_cp_ue_index_t ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
    report_fatal_error_if_not(ue_index != cu_cp_ue_index_t::invalid, "Failed to add UE");
    report_fatal_error_if_not(
        ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, source_pci, source_c_rnti, MIN_DU_CELL_INDEX),
        "Failed to update UE context");
    report_fatal_error_if_not(ue_mng.set_plmn(ue_index, plmn_identity::test_value()), "Failed to set PLMN");

    cu_cp_ue* ue = ue_mng.find_du_ue(ue_index);
    ue->set_rrc_ue(rrc_ue);

    report_fatal_error_if_not(
        ue->get_security_manager().init_security_context(generate_security_context(ue->get_security_manager())),
        "Failed to init security context");

    ue->get_up_resource_manager().set_up_context(make_up_context());

    return ue;
  }

  static up_context make_up_context()
  {
    up_context ctx;

    up_pdu_session_context pdu_session{
        uint_to_pdu_session_id(1),
        pdu_session_type_t::ipv4,
        up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"), gtpu_teid_t{12345}}};

    up_drb_context drb;
    drb.drb_id         = drb_id_t::drb1;
    drb.pdu_session_id = uint_to_pdu_session_id(1);
    drb.s_nssai        = s_nssai_t{.sst = slice_service_type{1}, .sd = slice_differentiator::create(1).value()};
    up_qos_flow_context qos_flow;
    qos_flow.qfi = uint_to_qos_flow_id(1);
    drb.qos_flows.emplace(uint_to_qos_flow_id(1), qos_flow);
    pdu_session.drbs.emplace(drb_id_t::drb1, drb);

    ctx.pdu_sessions.emplace(uint_to_pdu_session_id(1), pdu_session);

    return ctx;
  }

  /// Builds a request identifying the UE by the RRC Reestablishment UE Context ID, as a peer would send it.
  xnap_retrieve_ue_context_request make_request(cu_cp_ue_index_t ue_index) const
  {
    xnap_retrieve_ue_context_request request;
    request.ue_index      = ue_index;
    request.ue_context_id = xnap_ue_context_id_for_rrc_reest{.c_rnti = source_c_rnti, .fail_cell_pci = source_pci};
    request.mac_i         = {0xab, 0xcd};
    request.target_nci    = target_nci;

    cu_cp_served_cell_info target_cell;
    target_cell.nr_cgi  = nr_cell_global_id_t{plmn_identity::test_value(), target_nci};
    target_cell.nr_pci  = target_pci;
    request.target_cell = target_cell;

    return request;
  }

  /// Builds a request identifying the UE by the RRC Resume UE Context ID, as a peer would send it.
  xnap_retrieve_ue_context_request make_resume_request(cu_cp_ue_index_t ue_index) const
  {
    xnap_retrieve_ue_context_request request = make_request(ue_index);
    request.ue_context_id    = xnap_ue_context_id_for_rrc_resume{.i_rnti = short_i_rnti_t::from_uint(0x1234).value(),
                                                                 .allocated_c_rnti = to_rnti(0x4602),
                                                                 .access_pci       = target_pci};
    request.rrc_resume_cause = resume_cause_t::mo_data;

    return request;
  }

  static constexpr pci_t  source_pci    = 1;
  static constexpr pci_t  target_pci    = 42;
  static constexpr rnti_t source_c_rnti = to_rnti(0x4601);
  const nr_cell_identity  target_nci    = nr_cell_identity::create(0x19b0).value();
  /// SSB ARFCN the peer advertised for the target cell.
  static constexpr uint32_t target_ssb_arfcn = 632628;
  /// Address of the SCTP association with the AMF serving the UE.
  const transport_layer_address amf_addr = transport_layer_address::create_from_string("10.12.1.100");
  const guami_t guami{.plmn = plmn_identity::test_value(), .amf_set_id = 1, .amf_pointer = 1, .amf_region_id = 1};

  timer_manager           timers;
  manual_task_worker      cu_worker{128};
  cu_cp_configuration     cu_cp_cfg;
  ue_manager_config       ue_cfg;
  ue_manager_dependencies ue_dependencies;
  ue_manager              ue_mng{ue_cfg, ue_dependencies};
  dummy_rrc_ue            rrc_ue;
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("CU-CP");
};

TEST_F(cu_cp_ue_context_retrieval_test, when_ue_context_is_retrievable_then_context_is_collected)
{
  cu_cp_ue* ue = create_attached_ue();

  const xnap_retrieve_ue_context_response response = collect_ue_context_for_retrieval(
      make_request(ue->get_ue_index()), *ue, guami, amf_ue_id_t::min, amf_addr, target_ssb_arfcn, logger);

  ASSERT_TRUE(response.success);
  ASSERT_EQ(response.guami.plmn, guami.plmn);
  ASSERT_EQ(response.ue_context_info.amf_ue_id, to_underlying(amf_ue_id_t::min));
  ASSERT_EQ(response.ue_context_info.amf_addr, amf_addr) << "The peer was not told which AMF serves the UE";
  ASSERT_EQ(response.ue_context_info.pdu_session_res_to_be_setup_list.size(), 1);
  ASSERT_EQ(response.ue_context_info.pdu_session_res_to_be_setup_list[uint_to_pdu_session_id(1)]
                .qos_flow_setup_request_items.size(),
            1);
}

TEST_F(cu_cp_ue_context_retrieval_test, when_ue_context_is_retrieved_then_local_ue_keys_are_left_untouched)
{
  cu_cp_ue* ue = create_attached_ue();

  const security::security_context sec_context_before = ue->get_security_manager().get_security_context();

  const xnap_retrieve_ue_context_response response = collect_ue_context_for_retrieval(
      make_request(ue->get_ue_index()), *ue, guami, amf_ue_id_t::min, amf_addr, target_ssb_arfcn, logger);
  ASSERT_TRUE(response.success);

  // The UE stays in service at this node until the peer confirms the retrieval, so deriving KgNB* for the target cell
  // must not rotate the keys the UE is currently using.
  const security::security_context sec_context_after = ue->get_security_manager().get_security_context();
  ASSERT_EQ(sec_context_after.k, sec_context_before.k) << "The local UE's KgNB was rotated by the retrieval";
  ASSERT_EQ(sec_context_after.ncc, sec_context_before.ncc) << "The local UE's NCC was changed by the retrieval";

  // The transferred key must be the derived KgNB*, not the key still in use locally.
  ASSERT_NE(response.ue_context_info.security_context.k, sec_context_before.k)
      << "The transferred key was not derived for the target cell";
}

TEST_F(cu_cp_ue_context_retrieval_test, when_mac_i_verification_fails_then_retrieval_is_rejected)
{
  cu_cp_ue* ue = create_attached_ue();

  rrc_ue.mac_i_valid = false;

  const xnap_retrieve_ue_context_response response = collect_ue_context_for_retrieval(
      make_request(ue->get_ue_index()), *ue, guami, amf_ue_id_t::min, amf_addr, target_ssb_arfcn, logger);

  ASSERT_FALSE(response.success);
  ASSERT_TRUE(response.cause.has_value());
}

TEST_F(cu_cp_ue_context_retrieval_test, when_ue_context_id_identifies_a_resume_then_context_is_collected)
{
  cu_cp_ue* ue = create_attached_ue();

  const xnap_retrieve_ue_context_response response = collect_ue_context_for_retrieval(
      make_resume_request(ue->get_ue_index()), *ue, guami, amf_ue_id_t::min, amf_addr, target_ssb_arfcn, logger);

  ASSERT_TRUE(response.success);
  ASSERT_EQ(response.ue_context_info.pdu_session_res_to_be_setup_list.size(), 1);

  // A resume identity carries no source cell or C-RNTI, so the token is a ResumeMAC-I verified against the context of
  // the cell the UE was suspended in.
  ASSERT_TRUE(rrc_ue.resume_mac_i_verified) << "The ResumeMAC-I was not verified";
  ASSERT_FALSE(rrc_ue.short_mac_i_verified) << "A resume identity was verified as a reestablishment";
}

TEST_F(cu_cp_ue_context_retrieval_test, when_resume_mac_i_verification_fails_then_retrieval_is_rejected)
{
  cu_cp_ue* ue = create_attached_ue();

  rrc_ue.mac_i_valid = false;

  const xnap_retrieve_ue_context_response response = collect_ue_context_for_retrieval(
      make_resume_request(ue->get_ue_index()), *ue, guami, amf_ue_id_t::min, amf_addr, target_ssb_arfcn, logger);

  ASSERT_TRUE(rrc_ue.resume_mac_i_verified);
  ASSERT_FALSE(response.success);
  ASSERT_TRUE(response.cause.has_value());
}

TEST_F(cu_cp_ue_context_retrieval_test, when_ue_has_no_pdu_sessions_then_retrieval_is_rejected)
{
  cu_cp_ue* ue = create_attached_ue();

  // A UE that is not fully attached has no context worth transferring.
  ue->get_up_resource_manager().set_up_context(up_context{});

  const xnap_retrieve_ue_context_response response = collect_ue_context_for_retrieval(
      make_request(ue->get_ue_index()), *ue, guami, amf_ue_id_t::min, amf_addr, target_ssb_arfcn, logger);

  ASSERT_FALSE(response.success);
  ASSERT_EQ(response.cause, xnap_cause_t{xnap_cause_radio_network_t::non_relocation_of_context});
}

TEST_F(cu_cp_ue_context_retrieval_test, when_ue_has_no_amf_ue_id_then_retrieval_is_rejected)
{
  cu_cp_ue* ue = create_attached_ue();

  const xnap_retrieve_ue_context_response response = collect_ue_context_for_retrieval(
      make_request(ue->get_ue_index()), *ue, guami, amf_ue_id_t::invalid, amf_addr, target_ssb_arfcn, logger);

  ASSERT_FALSE(response.success);
  ASSERT_EQ(response.cause, xnap_cause_t{xnap_cause_radio_network_t::non_relocation_of_context});
}

TEST_F(cu_cp_ue_context_retrieval_test, when_target_cell_ssb_arfcn_is_unknown_then_retrieval_is_rejected)
{
  cu_cp_ue* ue = create_attached_ue();

  // Deriving KgNB* takes the ARFCN of the target cell, so an unknown one is rejected (TS 33.501 section 6.11).
  const xnap_retrieve_ue_context_response response = collect_ue_context_for_retrieval(
      make_request(ue->get_ue_index()), *ue, guami, amf_ue_id_t::min, amf_addr, std::nullopt, logger);

  ASSERT_FALSE(response.success);
  ASSERT_EQ(response.cause, xnap_cause_t{xnap_cause_radio_network_t::cell_not_available});
}

TEST_F(cu_cp_ue_context_retrieval_test, when_target_cell_is_not_served_by_the_peer_then_retrieval_is_rejected)
{
  cu_cp_ue* ue = create_attached_ue();

  // Deriving KgNB* takes the PCI of the target cell, so a cell the peer did not advertise is rejected.
  xnap_retrieve_ue_context_request request = make_request(ue->get_ue_index());
  request.target_cell.reset();

  const xnap_retrieve_ue_context_response response =
      collect_ue_context_for_retrieval(request, *ue, guami, amf_ue_id_t::min, amf_addr, target_ssb_arfcn, logger);

  ASSERT_FALSE(response.success);
  ASSERT_EQ(response.cause, xnap_cause_t{xnap_cause_radio_network_t::cell_not_available});
}
