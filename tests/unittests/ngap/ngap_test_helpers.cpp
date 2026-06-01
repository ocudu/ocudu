// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/ngap/ngap_configuration.h"
#include "ocudu/ngap/ngap_factory.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/support/async/async_test_utils.h"

using namespace ocudu;
using namespace ocucp;

ngap_test::ngap_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &ctrl_worker;
    cucfg.ngap.ngaps.push_back(cu_cp_configuration::ngap_config{
        &n2_gw,
        {supported_tracking_area{
            7,
            {plmn_item{plmn_identity::test_value(),
                       std::vector<s_nssai_t>{s_nssai_t{slice_service_type{1}, slice_differentiator{}}}}}}}});
    return cucfg;
  }())
{
  test_logger.set_level(ocudulog::basic_levels::debug);
  ngap_logger.set_level(ocudulog::basic_levels::debug);
  ocudulog::init();

  ngap_configuration ngap_cfg{};
  ngap_cfg.gnb_id                      = cu_cp_cfg.node.gnb_id;
  ngap_cfg.ran_node_name               = cu_cp_cfg.node.ran_node_name;
  ngap_cfg.supported_tas               = cu_cp_cfg.ngap.ngaps.front().supported_tas;
  ngap_cfg.request_pdu_session_timeout = cu_cp_cfg.ue.request_pdu_session_timeout;
  ngap = create_ngap(ngap_cfg, cu_cp_notifier, *cu_cp_cfg.ngap.ngaps.front().n2_gw, timers, ctrl_worker);

  cu_cp_notifier.connect_ngap(ngap->get_ngap_ue_context_removal_handler());
  n2_gw.attach_handler(&dummy_amf);

  // Initiate N2 TNL association to AMF.
  report_fatal_error_if_not(ngap->handle_amf_tnl_connection_request(), "Unable to establish connection to AMF");
}

ngap_test::~ngap_test()
{
  // flush logger after each test
  ocudulog::flush();
}

bool ngap_test::run_ng_setup()
{
  // Launch NG setup procedure
  test_logger.info("Launch ng setup request procedure...");
  async_task<ngap_ng_setup_result>         t = ngap->handle_ng_setup_request(1);
  lazy_task_launcher<ngap_ng_setup_result> t_launcher(t);

  // Inject NG setup response message.
  ngap_message ng_setup_response = generate_ng_setup_response();
  test_logger.info("Injecting NGSetupResponse");
  ngap->handle_message(ng_setup_response);

  if (!std::holds_alternative<ngap_ng_setup_response>(t.get())) {
    test_logger.error("NG Setup procedure failed");
    return false;
  }

  return true;
}

cu_cp_ue_index_t ngap_test::create_ue(rnti_t rnti)
{
  // Create UE in UE manager.
  cu_cp_ue_index_t ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
  if (ue_index == cu_cp_ue_index_t::invalid) {
    test_logger.error("Failed to create UE");
    return cu_cp_ue_index_t::invalid;
  }
  if (ue_mng.ue_admission_limit_reached()) {
    test_logger.error("Failed to create UE. UE not servable");
    ue_mng.remove_ue(ue_index);
    return cu_cp_ue_index_t::invalid;
  }

  if (not ue_mng.update_ue_context(ue_index, int_to_gnb_du_id(0), MIN_PCI, rnti, MIN_DU_CELL_INDEX)) {
    test_logger.error("ue={}: Failed to update UE context with pci={} rnti={} pcell_index={}",
                      ue_index,
                      MIN_PCI,
                      rnti_t::MIN_CRNTI,
                      MIN_DU_CELL_INDEX);
    return cu_cp_ue_index_t::invalid;
  }

  if (!ue_mng.set_plmn(ue_index, plmn_identity::test_value())) {
    test_logger.error("ue={}: Failed to set PLMN", ue_index);
    ue_mng.remove_ue(ue_index);
    return cu_cp_ue_index_t::invalid;
  }

  // Inject UE creation at NGAP.
  test_ues.emplace(ue_index, test_ue(ue_index));
  test_ue& new_test_ue = test_ues.at(ue_index);

  ue_mng.get_ngap_rrc_ue_adapter(ue_index).connect_rrc_ue(new_test_ue.rrc_ue_handler);

  // Generate and inject valid initial UE message.
  cu_cp_initial_ue_message msg = generate_initial_ue_message(ue_index);
  ngap->handle_initial_ue_message(msg);

  new_test_ue.ran_ue_id =
      uint_to_ran_ue_id(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id);

  return ue_index;
}

cu_cp_ue_index_t ngap_test::create_ue_without_init_ue_message(rnti_t rnti)
{
  // Create UE in UE manager.
  cu_cp_ue_index_t ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
  if (ue_index == cu_cp_ue_index_t::invalid) {
    test_logger.error("Failed to create UE");
    return cu_cp_ue_index_t::invalid;
  }
  if (ue_mng.ue_admission_limit_reached()) {
    test_logger.error("Failed to create UE. UE not servable");
    ue_mng.remove_ue(ue_index);
    return cu_cp_ue_index_t::invalid;
  }

  if (not ue_mng.update_ue_context(ue_index, int_to_gnb_du_id(0), MIN_PCI, rnti, MIN_DU_CELL_INDEX)) {
    test_logger.error("ue={}: Failed to update UE context with pci={} rnti={} pcell_index={}",
                      ue_index,
                      MIN_PCI,
                      rnti_t::MIN_CRNTI,
                      MIN_DU_CELL_INDEX);
    return cu_cp_ue_index_t::invalid;
  }
  if (!ue_mng.set_plmn(ue_index, plmn_identity::test_value())) {
    test_logger.error("ue={}: Failed to set PLMN", ue_index);
    ue_mng.remove_ue(ue_index);
    return cu_cp_ue_index_t::invalid;
  }

  // Inject UE creation at NGAP
  test_ues.emplace(ue_index, test_ue(ue_index));
  test_ue& new_test_ue = test_ues.at(ue_index);

  ue_mng.get_ngap_rrc_ue_adapter(ue_index).connect_rrc_ue(new_test_ue.rrc_ue_handler);

  return ue_index;
}

void ngap_test::run_dl_nas_transport(cu_cp_ue_index_t ue_index)
{
  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = uint_to_amf_ue_id(test_rng::uniform_int<uint64_t>(16, 128));
  ue.amf_ue_id = uint_to_amf_ue_id(
      test_rng::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));

  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);
}

void ngap_test::run_ul_nas_transport(cu_cp_ue_index_t ue_index)
{
  cu_cp_ul_nas_transport ul_nas_transport = generate_ul_nas_transport_message(ue_index);
  ngap->handle_ul_nas_transport_message(ul_nas_transport);
}

void ngap_test::run_initial_context_setup(cu_cp_ue_index_t ue_index)
{
  auto& ue = test_ues.at(ue_index);

  ngap_message init_context_setup_request =
      generate_valid_initial_context_setup_request_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(init_context_setup_request);
}

bool ngap_test::enable_ue_security(cu_cp_ue_index_t ue_index)
{
  // Setup security.
  byte_buffer key_buf = make_byte_buffer("45cbc3f8a81193fd5c5229300d59edf812e998a115ec4e0ce903ba89367e2628").value();
  security::sec_key sk_gnb = {};
  std::copy(key_buf.begin(), key_buf.end(), sk_gnb.begin());

  // Initialize security context and capabilities.
  security::security_context init_sec_ctx = {};
  init_sec_ctx.k                          = sk_gnb;
  std::fill(init_sec_ctx.supported_int_algos.begin(), init_sec_ctx.supported_int_algos.end(), true);
  std::fill(init_sec_ctx.supported_enc_algos.begin(), init_sec_ctx.supported_enc_algos.end(), true);

  // Mark security as enabled.
  if (!ue_mng.find_ue(ue_index)->get_security_manager().init_security_context(init_sec_ctx)) {
    return false;
  }

  if (!ue_mng.find_ue(ue_index)->get_security_manager().finalize_security_context()) {
    return false;
  }

  return true;
}

void ngap_test::run_pdu_session_resource_setup(cu_cp_ue_index_t ue_index, pdu_session_id_t pdu_session_id)
{
  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue.amf_ue_id.value(),
      ue.ran_ue_id.value(),
      {{pdu_session_id, {pdu_session_type_t::ipv4, {{uint_to_qos_flow_id(1), 9}}}}});
  ngap->handle_message(pdu_session_resource_setup_request);
}

void ngap_test::add_pdu_session_to_up_manager(cu_cp_ue_index_t        ue_index,
                                              pdu_session_id_t        pdu_session_id,
                                              pdu_session_type_t      pdu_session_type,
                                              up_transport_layer_info ul_ngu_up_tnl_info,
                                              drb_id_t                drb_id,
                                              qos_flow_id_t           qos_flow_id)
{
  auto&                                        up_mng = ue_mng.find_ue(ue_index)->get_up_resource_manager();
  up_config_update_result                      result;
  up_pdu_session_context_update                ctxt_update{pdu_session_id, pdu_session_type, ul_ngu_up_tnl_info};
  std::map<qos_flow_id_t, up_qos_flow_context> qos_flows;
  qos_flows[qos_flow_id]         = {qos_flow_id, {}};
  ctxt_update.drb_to_add[drb_id] = {drb_id, pdu_session_id, {}, false, {}, {}, qos_flows, {}, {}, {}};

  result.pdu_sessions_added_list.push_back(ctxt_update);

  up_mng.apply_config_update(result);
}

void ngap_test::tick()
{
  timers.tick();
  ctrl_worker.run_pending_tasks();
}
