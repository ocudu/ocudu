// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "inter_cu_handover_execution_target_routine.h"
#include "../pdu_session_routine_helpers.h"
#include "ocudu/cu_cp/inter_cu_handover_messages.h"
#include "ocudu/security/security.h"
#include "ocudu/support/async/coroutine.h"
#include "ocudu/xnap/xnap.h"

using namespace ocudu;
using namespace ocucp;

inter_cu_handover_execution_target_routine::inter_cu_handover_execution_target_routine(
    cu_cp_ue*                                                    ue_,
    const std::optional<xnap_handover_target_execution_context>& xnap_ho_target_execution_ctxt_,
    e1ap_bearer_context_manager&                                 e1ap_,
    ngap_interface&                                              ngap_,
    xnap_interface*                                              xnap_,
    ocudulog::basic_logger&                                      logger_) :
  ue(ue_),
  xnap_ho_target_execution_ctxt(xnap_ho_target_execution_ctxt_),
  e1ap(e1ap_),
  ngap(ngap_),
  xnap(xnap_),
  logger(logger_)
{
}

void inter_cu_handover_execution_target_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (ue == nullptr) {
    logger.warning("\"{}\" failed. Cause: UE got removed", name());
    CORO_EARLY_RETURN();
  }

  if (is_xn_handover() && xnap == nullptr) {
    logger.warning("\"{}\" failed. Cause: XNAP interface (xnc_peer_index={}) is nullptr",
                   name(),
                   xnap_ho_target_execution_ctxt->xnc_index);
    CORO_EARLY_RETURN();
  }

  logger.debug("ue={}: \"{}\" started...", ue->get_ue_index(), name());

  if (!is_xn_handover()) {
    // Await for NGAP DL Status transfer.
    CORO_AWAIT_VALUE(sn_status, ngap.handle_dl_ran_status_transfer_required(ue->get_ue_index()));
  } else {
    // Await for SN Status transfer from source XN-C.
    CORO_AWAIT_VALUE(sn_status, xnap->handle_sn_status_transfer_expected(ue->get_ue_index()));
  }

  if (not sn_status.has_value()) {
    logger.warning("ue={}: \"{}\" failed. Cause: Failed to receive {} Status Transfer",
                   ue->get_ue_index(),
                   name(),
                   is_xn_handover() ? "SN" : "DL RAN");
    CORO_EARLY_RETURN();
  }

  // Inform CU-UP of the current PDCP state.
  bearer_context_modification_request.ue_index = ue->get_ue_index();
  fill_e1ap_bearer_context_modification_request();
  CORO_AWAIT(e1ap.handle_bearer_context_modification_request(bearer_context_modification_request));

  // Notify RRC UE to await ReconfigurationComplete.
  if (!initialize_reconfiguration_timeout()) {
    logger.warning(
        "ue={}: \"{}\" failed. Cause: Failed to initialize reconfiguration timeout", ue->get_ue_index(), name());
    CORO_EARLY_RETURN();
  }
  CORO_AWAIT_VALUE(reconf_result,
                   ue->get_rrc_ue()->handle_handover_reconfiguration_complete_expected(0, reconf_timeout));
  if (!reconf_result) {
    logger.warning(
        "ue={}: \"{}\" failed. Cause: Failed to receive RRC Reconfiguration Complete", ue->get_ue_index(), name());
    CORO_EARLY_RETURN();
  }

  if (!is_xn_handover()) {
    // Send handover notify from here. Use UE selected PLMN in the NR-CGI in case of MOCN.
    ngap.get_ngap_control_message_handler().handle_inter_cu_ho_rrc_recfg_complete(
        ue->get_ue_index(),
        {ue->get_ue_context().plmn, ue->get_rrc_ue()->get_cell_context().cgi.nci},
        ue->get_rrc_ue()->get_cell_context().tac);
  } else {
    // Prepare Path Switch Request.
    path_switch_request = fill_path_switch_request(xnap_ho_target_execution_ctxt.value(),
                                                   ue->get_rrc_ue()->get_cell_context(),
                                                   ue->get_ue_context().plmn,
                                                   ue->get_security_manager().get_security_context());

    // Send Path Switch Request from here.
    CORO_AWAIT_VALUE(path_switch_response,
                     ngap.get_ngap_control_message_handler().handle_path_switch_request_required(path_switch_request));
    if (std::holds_alternative<cu_cp_path_switch_request_failure>(path_switch_response)) {
      logger.warning("ue={}: \"{}\" failed. Cause: Path Switch Request rejected by AMF", ue->get_ue_index(), name());
      CORO_EARLY_RETURN();
    }

    // Inform CU-UP of new UL NG-U tunnel endpoints and any PDU sessions released by the AMF.
    fill_e1ap_bearer_context_tunnel_update_request(std::get<cu_cp_path_switch_request_ack>(path_switch_response));
    if (!tunnel_context_modification_request.ng_ran_bearer_context_mod_request->pdu_session_res_to_modify_list.empty() ||
        !tunnel_context_modification_request.ng_ran_bearer_context_mod_request->pdu_session_res_to_rem_list.empty()) {
      CORO_AWAIT(e1ap.handle_bearer_context_modification_request(tunnel_context_modification_request));
    }

    // Request for the release of the UE Context at the source CU-CP.
    if (!xnap->handle_ue_context_release_required(ue->get_ue_index())) {
      logger.warning("ue={}: \"{}\" failed. Cause: Failed to transmit UE Context Release", ue->get_ue_index(), name());
      CORO_EARLY_RETURN();
    }

    CORO_EARLY_RETURN();
  }

  CORO_RETURN();
}

void inter_cu_handover_execution_target_routine::fill_e1ap_bearer_context_modification_request()
{
  auto& ng_request = bearer_context_modification_request.ng_ran_bearer_context_mod_request;
  ng_request.emplace();
  slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_modify_item>& pdu_sessions =
      ng_request->pdu_session_res_to_modify_list;
  for (const cu_cp_drbs_subject_to_status_transfer_item& ngap_drb : sn_status->drbs_subject_to_status_transfer_list) {
    const up_drb_context& drb_ctx = ue->get_up_resource_manager().get_drb_context(ngap_drb.drb_id);
    pdu_session_id_t      psi     = drb_ctx.pdu_session_id;

    if (not pdu_sessions.contains(psi)) {
      e1ap_pdu_session_res_to_modify_item ps_item;
      ps_item.pdu_session_id = psi;
      pdu_sessions.emplace(psi, ps_item);
    }
    e1ap_pdu_session_res_to_modify_item& ps_item = pdu_sessions[psi];
    e1ap_drb_to_modify_item_ng_ran       drb_item;
    drb_item.drb_id = ngap_drb.drb_id;
    drb_item.pdcp_sn_status_info.emplace();

    drb_item.pdcp_sn_status_info->pdcp_status_transfer_ul.count_value.hfn     = ngap_drb.drb_status_ul.ul_count.hfn;
    drb_item.pdcp_sn_status_info->pdcp_status_transfer_ul.count_value.pdcp_sn = ngap_drb.drb_status_ul.ul_count.sn;

    drb_item.pdcp_sn_status_info->pdcp_status_transfer_dl.hfn     = ngap_drb.drb_status_dl.dl_count.hfn;
    drb_item.pdcp_sn_status_info->pdcp_status_transfer_dl.pdcp_sn = ngap_drb.drb_status_dl.dl_count.sn;

    drb_item.pdcp_cfg.emplace();
    fill_e1ap_drb_pdcp_config(*drb_item.pdcp_cfg, drb_ctx.pdcp_cfg);
    drb_item.pdcp_cfg->pdcp_reest = true;

    ps_item.drb_to_modify_list_ng_ran.emplace(drb_item.drb_id, drb_item);
  }
}

bool inter_cu_handover_execution_target_routine::initialize_reconfiguration_timeout()
{
  std::optional<std::chrono::milliseconds> t304_ms = ue->get_rrc_ue()->get_cell_context().timers.t304;
  if (!t304_ms.has_value()) {
    report_fatal_error("T304 not configured in cell context");
  }

  reconf_timeout =
      t304_ms.value() + std::chrono::milliseconds{/*We add 1s of extra time for the UE to reestablish*/ 1000};

  return true;
}

cu_cp_path_switch_request inter_cu_handover_execution_target_routine::fill_path_switch_request(
    const xnap_handover_target_execution_context& target_execution_ctxt,
    const rrc_cell_context&                       cell_context,
    const plmn_identity&                          selected_plmn,
    const security::security_context&             security_context)
{
  cu_cp_path_switch_request path_switch_req;
  path_switch_req.ue_index              = target_execution_ctxt.ue_index;
  path_switch_req.source_amf_ue_ngap_id = target_execution_ctxt.amf_ue_id;

  path_switch_req.user_location_info.nr_cgi = {selected_plmn, cell_context.cgi.nci};
  path_switch_req.user_location_info.tai    = {selected_plmn, cell_context.tac};

  path_switch_req.supported_enc_algos = security_context.supported_enc_algos;
  path_switch_req.supported_int_algos = security_context.supported_int_algos;

  for (const auto& pdu_session_item : target_execution_ctxt.pdu_session_res_admitted_list) {
    cu_cp_pdu_session_res_to_be_switched_dl_item pdu_session_to_be_switched_item;
    pdu_session_to_be_switched_item.pdu_session_id     = pdu_session_item.pdu_session_id;
    pdu_session_to_be_switched_item.dl_ngu_up_tnl_info = pdu_session_item.dl_ngu_up_tnl_info;
    for (const auto& qos_flow : pdu_session_item.qos_flows_setup_list) {
      pdu_session_to_be_switched_item.qos_flow_accepted_list.push_back(qos_flow.qos_flow_id);
    }

    path_switch_req.pdu_session_res_to_be_switched_dl_list.push_back(pdu_session_to_be_switched_item);
  }

  path_switch_req.pdu_session_res_failed_to_setup_list_ps_req = target_execution_ctxt.pdu_session_failed_to_setup_list;

  return path_switch_req;
}

void inter_cu_handover_execution_target_routine::fill_e1ap_bearer_context_tunnel_update_request(
    const cu_cp_path_switch_request_ack& ack)
{
  tunnel_context_modification_request.ue_index = ue->get_ue_index();
  auto& ng_request = tunnel_context_modification_request.ng_ran_bearer_context_mod_request;
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
