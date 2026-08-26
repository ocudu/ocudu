// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "retrieved_context_helpers.h"
#include "../pdu_session_routine_helpers.h"
#include "ocudu/ran/cause/e1ap_cause_converters.h"

using namespace ocudu;
using namespace ocucp;

bool ocudu::ocucp::fill_retrieved_context_bearer_setup_request(e1ap_bearer_context_setup_request& request,
                                                               cu_cp_ue&                          ue,
                                                               const up_config_update&            next_config,
                                                               const ue_configuration&            ue_cfg,
                                                               const security_indication_t& default_security_indication,
                                                               ocudulog::basic_logger&      logger)
{
  const cu_cp_ue_index_t                    ue_index          = ue.get_ue_index();
  const cu_cp_ue_context_retrieval_context& retrieval_context = ue.get_context_retrieval_context().value();
  const security::sec_as_config             sec_info          = ue.get_security_manager().get_up_as_config();

  request.ue_index = ue_index;

  request.security_info.security_algorithm.ciphering_algo                 = sec_info.cipher_algo;
  request.security_info.security_algorithm.integrity_protection_algorithm = sec_info.integ_algo;
  auto k_enc_buffer                                                       = byte_buffer::create(sec_info.k_enc);
  if (not k_enc_buffer.has_value()) {
    logger.warning("ue={}: Unable to allocate byte_buffer", ue_index);
    return false;
  }
  request.security_info.up_security_key.encryption_key = std::move(k_enc_buffer.value());
  if (sec_info.k_int.has_value()) {
    auto k_int_buffer = byte_buffer::create(sec_info.k_int.value());
    if (not k_int_buffer.has_value()) {
      logger.warning("ue={}: Unable to allocate byte_buffer", ue_index);
      return false;
    }
    request.security_info.up_security_key.integrity_protection_key = std::move(k_int_buffer.value());
  }

  request.ue_dl_aggregate_maximum_bit_rate = ue.get_ue_ambr().dl;
  request.serving_plmn                     = ue.get_ue_context().plmn;
  request.activity_notif_level             = "ue"; // TODO: Remove hardcoded value
  if (request.activity_notif_level == "ue") {
    request.ue_inactivity_timer = ue_cfg.inactivity_timer;
  }

  fill_e1ap_pdu_session_res_to_setup_list(request.pdu_session_res_to_setup_list,
                                          logger,
                                          next_config,
                                          retrieval_context.pdu_session_res_to_be_setup_list,
                                          ue_cfg,
                                          default_security_indication);

  return true;
}

cu_cp_path_switch_request
ocudu::ocucp::fill_retrieved_context_path_switch_request(cu_cp_ue&                                 ue,
                                                         const e1ap_bearer_context_setup_response& setup_response)
{
  const cu_cp_ue_context_retrieval_context& retrieval_context = ue.get_context_retrieval_context().value();
  const rrc_cell_context&                   cell_context      = ue.get_rrc_ue()->get_cell_context();
  const plmn_identity&                      selected_plmn     = ue.get_ue_context().plmn;

  cu_cp_path_switch_request path_switch_req;
  path_switch_req.ue_index = ue.get_ue_index();
  // The AMF identifies the UE by the ID it has at the peer, which is why the retrieval reports it.
  path_switch_req.source_amf_ue_ngap_id = retrieval_context.amf_ue_id;

  path_switch_req.user_location_info.nr_cgi   = {selected_plmn, cell_context.cgi.nci};
  path_switch_req.user_location_info.tai      = {selected_plmn, cell_context.tac};
  path_switch_req.user_location_info.tac_list = cell_context.tac_list;

  const security::security_context& sec_context = ue.get_security_manager().get_security_context();
  path_switch_req.supported_enc_algos           = sec_context.supported_enc_algos;
  path_switch_req.supported_int_algos           = sec_context.supported_int_algos;

  // Report the sessions the CU-UP actually admitted, with the DL NG-U endpoints it allocated.
  for (const auto& pdu_session : setup_response.pdu_session_resource_setup_list) {
    cu_cp_pdu_session_res_to_be_switched_dl_item switched_item;
    switched_item.pdu_session_id     = pdu_session.pdu_session_id;
    switched_item.dl_ngu_up_tnl_info = pdu_session.ng_dl_up_tnl_info;
    for (const auto& drb : pdu_session.drb_setup_list_ng_ran) {
      for (const auto& qos_flow : drb.flow_setup_list) {
        switched_item.qos_flow_accepted_list.push_back(qos_flow.qos_flow_id);
      }
    }

    path_switch_req.pdu_session_res_to_be_switched_dl_list.push_back(switched_item);
  }

  for (const auto& failed_session : setup_response.pdu_session_resource_failed_list) {
    path_switch_req.pdu_session_res_failed_to_setup_list_ps_req.push_back(
        cu_cp_pdu_session_with_cause_item{failed_session.pdu_session_id, e1ap_to_ngap_cause(failed_session.cause)});
  }

  return path_switch_req;
}

void ocudu::ocucp::fill_retrieved_context_tunnel_update_request(e1ap_bearer_context_modification_request& request,
                                                                cu_cp_ue_index_t                          ue_index,
                                                                const cu_cp_path_switch_request_ack&      ack)
{
  request.ue_index = ue_index;
  auto& ng_request = request.ng_ran_bearer_context_mod_request;
  ng_request.emplace();

  for (const auto& switched_session : ack.pdu_session_res_switched_list) {
    if (!switched_session.ul_ngu_up_tnl_info.has_value()) {
      continue;
    }
    e1ap_pdu_session_res_to_modify_item ps_item;
    ps_item.pdu_session_id    = switched_session.pdu_session_id;
    ps_item.ng_ul_up_tnl_info = switched_session.ul_ngu_up_tnl_info;
    ng_request->pdu_session_res_to_modify_list.emplace(ps_item.pdu_session_id, ps_item);
  }

  for (const auto& released_session : ack.pdu_session_res_released_list) {
    ng_request->pdu_session_res_to_rem_list.push_back(released_session.pdu_session_id);
  }
}

void ocudu::ocucp::apply_retrieved_context_up_config_update(cu_cp_ue& ue, const up_config_update& next_config)
{
  up_config_update_result result;
  for (const auto& pdu_session_to_add : next_config.pdu_sessions_to_setup_list) {
    result.pdu_sessions_added_list.push_back(pdu_session_to_add.second);
  }
  ue.get_up_resource_manager().apply_config_update(result);
}

void ocudu::ocucp::start_retrieved_context_location_reporting(cu_cp_ue&                        ue,
                                                              ngap_location_reporting_handler& loc_report_handler,
                                                              ocudulog::basic_logger&          logger)
{
  const cu_cp_ue_context_retrieval_context& retrieval_context = ue.get_context_retrieval_context().value();
  if (!retrieval_context.location_report_info.has_value()) {
    return;
  }
  const location_report_request& loc_req = retrieval_context.location_report_info.value();

  using event_type = location_report_request::event_type;
  if (loc_req.location_reporting_type == event_type::nulltype) {
    logger.warning("ue={}: Ignoring the location reporting the peer requested. Cause: Unknown event type",
                   ue.get_ue_index());
    return;
  }

  if (loc_req.location_reporting_type != event_type::direct) {
    ue.get_location_manager().configure_location_reporting(loc_req);
  }

  if (loc_req.location_reporting_type == event_type::direct ||
      loc_req.location_reporting_type == event_type::change_of_serve_cell ||
      loc_req.location_reporting_type == event_type::change_of_serving_cell_and_ue_presence_in_the_area_of_interest) {
    const rrc_cell_context& cell_context  = ue.get_rrc_ue()->get_cell_context();
    const plmn_identity&    selected_plmn = ue.get_ue_context().plmn;

    cu_cp_user_location_info_nr user_location_info;
    user_location_info.nr_cgi   = {selected_plmn, cell_context.cgi.nci};
    user_location_info.tai      = {selected_plmn, cell_context.tac};
    user_location_info.tac_list = cell_context.tac_list;

    loc_report_handler.handle_location_report_transmission(
        ue.get_location_manager().get_direct_location_report(ue.get_ue_index(), user_location_info, loc_req));
  }
}
