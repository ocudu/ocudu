// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "inter_cu_conditional_handover_target_execution_routine.h"
#include "../pdu_session_routine_helpers.h"
#include "ocudu/cu_cp/inter_cu_handover_messages.h"
#include "ocudu/security/security.h"
#include "ocudu/support/async/coroutine.h"
#include "ocudu/xnap/xnap.h"

using namespace ocudu;
using namespace ocucp;

inter_cu_conditional_handover_target_execution_routine::inter_cu_conditional_handover_target_execution_routine(
    cu_cp_ue*                                     ue_,
    const xnap_handover_target_execution_context& execution_ctxt_,
    e1ap_bearer_context_manager&                  e1ap_,
    ngap_interface&                               ngap_,
    xnap_interface*                               xnap_,
    f1ap_ue_context_manager&                      f1ap_,
    cu_cp_ue_context_release_handler&             ue_ctx_release_handler_,
    mobility_manager&                             mobility_mng_,
    ocudulog::basic_logger&                       logger_) :
  ue(ue_),
  execution_ctxt(execution_ctxt_),
  e1ap(e1ap_),
  ngap(ngap_),
  xnap(xnap_),
  f1ap(f1ap_),
  ue_ctx_release_handler(ue_ctx_release_handler_),
  mobility_mng(mobility_mng_),
  logger(logger_)
{
}

void inter_cu_conditional_handover_target_execution_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (ue == nullptr) {
    logger.warning("\"{}\" failed. Cause: UE got removed", name());
    CORO_EARLY_RETURN();
  }

  if (xnap == nullptr) {
    logger.warning("\"{}\" failed. Cause: XNAP interface is nullptr", name());
    CORO_EARLY_RETURN();
  }

  logger.debug("ue={}: \"{}\" started...", ue->get_ue_index(), name());

  // Step 1: Await RRCReconfigurationComplete - the UE autonomously attached to the target cell.
  if (!initialize_reconfiguration_timeout()) {
    logger.warning(
        "ue={}: \"{}\" failed. Cause: Failed to initialize reconfiguration timeout", ue->get_ue_index(), name());
    CORO_EARLY_RETURN();
  }
  CORO_AWAIT_VALUE(reconf_result,
                   ue->get_rrc_ue()->handle_handover_reconfiguration_complete_expected(0, reconf_timeout, false));
  if (!reconf_result) {
    logger.debug("ue={}: \"{}\" did not receive RRCReconfigurationComplete. CHO to this target failed/canceled",
                 execution_ctxt.ue_index,
                 name());
    release_cmd                      = {};
    release_cmd.ue_index             = execution_ctxt.ue_index;
    release_cmd.cause                = ngap_cause_radio_network_t::unspecified;
    release_cmd.requires_rrc_message = false;
    CORO_AWAIT_VALUE(release_complete, ue_ctx_release_handler.handle_ue_context_release_command(release_cmd));
    CORO_EARLY_RETURN();
  }

  // Step 2: Send HandoverSuccess to the source CU-CP.
  xnap->handle_handover_success_required(ue->get_ue_index(), ue->get_rrc_ue()->get_cell_context().cgi);

  // Step 3: Await SN Status Transfer from the source CU-CP.
  CORO_AWAIT_VALUE(sn_status, xnap->handle_sn_status_transfer_expected(ue->get_ue_index()));
  if (!sn_status.has_value()) {
    logger.warning("ue={}: \"{}\" failed. Cause: Failed to receive SN Status Transfer", ue->get_ue_index(), name());
    CORO_EARLY_RETURN();
  }

  // Step 4: Apply UP AS security keys and PDCP SN status to CU-UP via E1AP bearer context modification.
  bearer_context_modification_request.ue_index = ue->get_ue_index();
  if (!fill_e1ap_bearer_context_modification_request()) {
    logger.warning(
        "ue={}: \"{}\" failed. Cause: Failed to build bearer context modification request", ue->get_ue_index(), name());
    CORO_EARLY_RETURN();
  }
  CORO_AWAIT_VALUE(bearer_context_modification_response,
                   e1ap.handle_bearer_context_modification_request(bearer_context_modification_request));
  if (!bearer_context_modification_response.success) {
    logger.warning(
        "ue={}: \"{}\" failed. Cause: CU-UP bearer context modification unsuccessful", ue->get_ue_index(), name());
    CORO_EARLY_RETURN();
  }

  // Step 5: Send Path Switch Request to AMF.
  path_switch_req = fill_path_switch_request();
  CORO_AWAIT_VALUE(path_switch_response,
                   ngap.get_ngap_control_message_handler().handle_path_switch_request_required(path_switch_req));
  if (!std::holds_alternative<cu_cp_path_switch_request_ack>(path_switch_response)) {
    logger.warning("ue={}: \"{}\" failed. Cause: Path Switch Request rejected by AMF", ue->get_ue_index(), name());
    ue_context_release_request = {.ue_index                         = ue->get_ue_index(),
                                  .pdu_session_res_list_cxt_rel_req = ue->get_up_resource_manager().get_pdu_sessions(),
                                  .cause = ngap_cause_radio_network_t::ho_fail_in_target_5_gc_ngran_node_or_target_sys};
    CORO_AWAIT(ngap.handle_ue_context_release_request(ue_context_release_request));
    CORO_EARLY_RETURN();
  }

  // Step 6: Inform CU-UP of new UL NG-U tunnel endpoints and any PDU sessions released by the AMF.
  fill_e1ap_bearer_context_tunnel_update_request(std::get<cu_cp_path_switch_request_ack>(path_switch_response));
  if (!tunnel_context_modification_request.ng_ran_bearer_context_mod_request->pdu_session_res_to_modify_list.empty() ||
      !tunnel_context_modification_request.ng_ran_bearer_context_mod_request->pdu_session_res_to_rem_list.empty()) {
    CORO_AWAIT(e1ap.handle_bearer_context_modification_request(tunnel_context_modification_request));
  }

  // Step 7: Send UE Context Release to source CU-CP.
  if (!xnap->handle_ue_context_release_required(ue->get_ue_index())) {
    logger.warning("ue={}: \"{}\" failed. Cause: Failed to transmit UE Context Release", ue->get_ue_index(), name());
  }

  // Step 8: Send Reconfiguration Complete Indicator to DU.
  ue_context_mod_request.ue_index               = ue->get_ue_index();
  ue_context_mod_request.rrc_recfg_complete_ind = f1ap_rrc_recfg_complete_ind::true_value;
  CORO_AWAIT(f1ap.handle_ue_context_modification_request(ue_context_mod_request));

  mobility_mng.get_metrics_handler().aggregate_successful_handover_execution();

  // Clear the CHO target role context; the UE is now the serving UE on this CU and may become a CHO source.
  ue->get_cho_context().reset();

  // Re-arm auto-CHO on the new serving cell.
  mobility_mng.trigger_auto_conditional_handover(ue->get_ue_index());

  CORO_RETURN();
}

bool inter_cu_conditional_handover_target_execution_routine::fill_e1ap_bearer_context_modification_request()
{
  // Push updated UP AS keys to CU-UP so it can re-key after the handover.
  const security::sec_as_config sec_cfg = ue->get_security_manager().get_up_as_config();
  bearer_context_modification_request.security_info.emplace();
  bearer_context_modification_request.security_info->security_algorithm.ciphering_algo = sec_cfg.cipher_algo;
  bearer_context_modification_request.security_info->security_algorithm.integrity_protection_algorithm =
      sec_cfg.integ_algo;
  auto k_enc = byte_buffer::create(sec_cfg.k_enc);
  if (!k_enc.has_value()) {
    logger.warning("ue={}: Failed to allocate UP security encryption key", ue->get_ue_index());
    return false;
  }
  bearer_context_modification_request.security_info->up_security_key.encryption_key = std::move(k_enc.value());
  if (sec_cfg.k_int.has_value()) {
    auto k_int = byte_buffer::create(sec_cfg.k_int.value());
    if (!k_int.has_value()) {
      logger.warning("ue={}: Failed to allocate UP security integrity key", ue->get_ue_index());
      return false;
    }
    bearer_context_modification_request.security_info->up_security_key.integrity_protection_key =
        std::move(k_int.value());
  }

  // Apply PDCP SN status received in the SN Status Transfer.
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
  return true;
}

bool inter_cu_conditional_handover_target_execution_routine::initialize_reconfiguration_timeout()
{
  // If the source signalled a CHO window duration, use it as the primary timeout; it covers
  // the full time until the UE triggers and completes the conditional handover.
  if (execution_ctxt.cho_timeout.count() > 0) {
    reconf_timeout = execution_ctxt.cho_timeout;
    return true;
  }

  // Fall back to T304 + 1s guard when no CHO window was signalled.
  std::optional<std::chrono::milliseconds> t304_ms = ue->get_rrc_ue()->get_cell_context().timers.t304;
  if (!t304_ms.has_value()) {
    logger.warning("ue={}: T304 not configured in cell context, using default 2000ms", ue->get_ue_index());
    t304_ms = std::chrono::milliseconds{2000};
  }

  reconf_timeout =
      t304_ms.value() + std::chrono::milliseconds{/*We add 1s of extra time for the UE to reestablish*/ 1000};

  return true;
}

void inter_cu_conditional_handover_target_execution_routine::fill_e1ap_bearer_context_tunnel_update_request(
    const cu_cp_path_switch_request_ack& ack)
{
  tunnel_context_modification_request.ue_index = ue->get_ue_index();
  auto& ng_request                             = tunnel_context_modification_request.ng_ran_bearer_context_mod_request;
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

cu_cp_path_switch_request inter_cu_conditional_handover_target_execution_routine::fill_path_switch_request()
{
  cu_cp_path_switch_request path_switch_request;
  path_switch_request.ue_index              = execution_ctxt.ue_index;
  path_switch_request.source_amf_ue_ngap_id = execution_ctxt.amf_ue_id;

  path_switch_request.user_location_info.nr_cgi = {ue->get_ue_context().plmn,
                                                   ue->get_rrc_ue()->get_cell_context().cgi.nci};
  path_switch_request.user_location_info.tai    = {ue->get_ue_context().plmn, ue->get_rrc_ue()->get_cell_context().tac};

  path_switch_request.supported_enc_algos = ue->get_security_manager().get_security_context().supported_enc_algos;
  path_switch_request.supported_int_algos = ue->get_security_manager().get_security_context().supported_int_algos;

  for (const auto& pdu_session_item : execution_ctxt.pdu_session_res_admitted_list) {
    cu_cp_pdu_session_res_to_be_switched_dl_item pdu_session_to_be_switched_item;
    pdu_session_to_be_switched_item.pdu_session_id     = pdu_session_item.pdu_session_id;
    pdu_session_to_be_switched_item.dl_ngu_up_tnl_info = pdu_session_item.dl_ngu_up_tnl_info;
    for (const auto& qos_flow : pdu_session_item.qos_flows_setup_list) {
      pdu_session_to_be_switched_item.qos_flow_accepted_list.push_back(qos_flow.qos_flow_id);
    }

    path_switch_request.pdu_session_res_to_be_switched_dl_list.push_back(pdu_session_to_be_switched_item);
  }

  path_switch_request.pdu_session_res_failed_to_setup_list_ps_req = execution_ctxt.pdu_session_failed_to_setup_list;

  return path_switch_request;
}
