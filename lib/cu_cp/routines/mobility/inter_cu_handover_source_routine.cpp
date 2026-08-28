// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "inter_cu_handover_source_routine.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace ocucp;

inter_cu_handover_source_routine::inter_cu_handover_source_routine(cu_cp_rrc_handover_command    command_,
                                                                   ue_manager&                   ue_mng_,
                                                                   du_processor_repository&      du_db_,
                                                                   cu_up_processor_repository&   cu_up_db_,
                                                                   ngap_control_message_handler& ngap_,
                                                                   xnap_interface*               xnap_,
                                                                   ocudulog::basic_logger&       logger_) :
  command(std::move(command_)),
  ue_index(command.ue_index),
  ue_mng(ue_mng_),
  du_db(du_db_),
  cu_up_db(cu_up_db_),
  ngap(ngap_),
  xnap(xnap_),
  logger(logger_)
{
}

void inter_cu_handover_source_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  if (ue_mng.find_du_ue(ue_index) == nullptr) {
    CORO_EARLY_RETURN(false);
  }

  // Unpack Handover Command PDU at RRC, to get RRC Reconfig PDU.
  ho_reconfig_pdu =
      ue_mng.find_du_ue(ue_index)->get_rrc_ue()->handle_rrc_handover_command(std::move(command.rrc_container));
  if (ho_reconfig_pdu.empty()) {
    logger.warning("ue={}: Could not unpack Handover Command PDU", ue_index);
    CORO_EARLY_RETURN(false);
  }

  // Send RRC Reconfiguration to UE.
  ue_context_mod_request.ue_index      = ue_index;
  ue_context_mod_request.rrc_container = ho_reconfig_pdu.copy();

  // Stop data transmission for the UE on the source DU (see TS 38.473 section 8.3.4.2).
  ue_context_mod_request.tx_action_ind = f1ap_tx_action_ind::stop;

  CORO_AWAIT_VALUE(ue_context_mod_response,
                   du_db.get_du_processor(ue_mng.find_du_ue(ue_index)->get_du_index())
                       .get_f1ap_handler()
                       .handle_ue_context_modification_request(ue_context_mod_request));

  if (not ue_context_mod_response.success) {
    CORO_EARLY_RETURN(false);
  }

  // Get the PDCP state from the CU-UP and give it the forwarding tunnels to send the held data to.
  fill_e1ap_bearer_context_modification_request();

  CORO_AWAIT_VALUE(bearer_mod_resp,
                   cu_up_db.find_cu_up_processor(ue_mng.find_du_ue(ue_index)->get_cu_up_index())
                       ->get_e1ap_bearer_context_manager()
                       .handle_bearer_context_modification_request(bearer_mod_req));

  if (not bearer_mod_resp.success) {
    CORO_EARLY_RETURN(false);
  }

  if (not fill_status_transfer()) {
    CORO_EARLY_RETURN(false);
  }

  if (xnap == nullptr) {
    // Send PDCP state to AMF.
    ngap.handle_ul_ran_status_transfer(status_transfer);
  } else {
    // Send PDCP state to XN-C peer CU-CP.
    xnap->handle_sn_status_transfer_required(status_transfer);
  }

  CORO_RETURN(true);
}

void inter_cu_handover_source_routine::fill_e1ap_bearer_context_modification_request()
{
  bearer_mod_req.ue_index = ue_index;
  bearer_mod_req.ng_ran_bearer_context_mod_request.emplace();
  const auto& pdu_sessions = ue_mng.find_du_ue(ue_index)->get_up_resource_manager().get_pdu_sessions();
  for (const auto& pdu_session_id : pdu_sessions) {
    e1ap_pdu_session_res_to_modify_item e1ap_pdu_session_item;
    e1ap_pdu_session_item.pdu_session_id = pdu_session_id;

    // The forwarding tunnels accepted for this PDU session, if any.
    const auto forwarding_info     = command.data_forwarding_info_from_target.find(pdu_session_id);
    const bool has_forwarding_info = forwarding_info != command.data_forwarding_info_from_target.end();

    // Give the CU-UP the PDU session level forwarding tunnel, together with the QoS flows to be forwarded over it.
    bool forwarding_tunnel_programmed = false;
    if (has_forwarding_info and forwarding_info->second.pdu_session_level_dl_data_forwarding_info.has_value()) {
      e1ap_data_forwarding_info session_forwarding_info;
      session_forwarding_info.dl_data_forwarding = forwarding_info->second.pdu_session_level_dl_data_forwarding_info;
      session_forwarding_info.data_forwarding_to_ng_ran_qos_flow_info_list =
          forwarding_info->second.qos_flows_accepted_for_data_forwarding_list;
      logger.info("ue={}: Forwarding {} over the PDU session level tunnel. tnl_info={}",
                  ue_index,
                  pdu_session_id,
                  session_forwarding_info.dl_data_forwarding.value());
      e1ap_pdu_session_item.pdu_session_data_forwarding_info = std::move(session_forwarding_info);
      forwarding_tunnel_programmed                           = true;
    }

    const up_pdu_session_context& pdu_session =
        ue_mng.find_du_ue(ue_index)->get_up_resource_manager().get_pdu_session_context(pdu_session_id);
    // Fill DRBs
    for (const std::pair<const drb_id_t, up_drb_context>& drb : pdu_session.drbs) {
      e1ap_drb_to_modify_item_ng_ran drb_to_mod;
      drb_to_mod.drb_id                 = drb.first;
      drb_to_mod.pdcp_sn_status_request = true;

      e1ap_pdu_session_item.drb_to_modify_list_ng_ran.emplace(drb.first, drb_to_mod);
    }

    // This node proposes DL forwarding for every QoS flow it hands over, so a PDU session left without any tunnel
    // means the data still held for the UE is lost. The 5GC may report QoS flows as accepted for forwarding and
    // still provide no endpoint to send them to.
    if (not forwarding_tunnel_programmed) {
      logger.info(
          "ue={}: Not forwarding the data of {}. Cause: no forwarding tunnel was provided", ue_index, pdu_session_id);
    }

    bearer_mod_req.ng_ran_bearer_context_mod_request->pdu_session_res_to_modify_list.emplace(pdu_session_id,
                                                                                             e1ap_pdu_session_item);
  }
}

bool inter_cu_handover_source_routine::fill_status_transfer()
{
  status_transfer.ue_index = ue_index;

  for (const auto& pdu_session_modified : bearer_mod_resp.pdu_session_resource_modified_list) {
    const up_pdu_session_context& pdu_session_ctx =
        ue_mng.find_du_ue(ue_index)->get_up_resource_manager().get_pdu_session_context(
            pdu_session_modified.pdu_session_id);
    for (const e1ap_drb_modified_item_ng_ran& drb_modified : pdu_session_modified.drb_modified_list_ng_ran) {
      if (drb_modified.pdcp_sn_status_info.has_value()) {
        auto drb_ctx = pdu_session_ctx.drbs.find(drb_modified.drb_id);
        if (drb_ctx == pdu_session_ctx.drbs.end()) {
          return false;
        }
        cu_cp_drbs_subject_to_status_transfer_item drb_item;
        drb_item.drb_id                     = drb_modified.drb_id;
        drb_item.drb_status_dl.sn_size      = drb_ctx->second.pdcp_cfg.tx.sn_size;
        drb_item.drb_status_dl.dl_count.hfn = drb_modified.pdcp_sn_status_info->pdcp_status_transfer_dl.hfn;
        drb_item.drb_status_dl.dl_count.sn  = drb_modified.pdcp_sn_status_info->pdcp_status_transfer_dl.pdcp_sn;
        drb_item.drb_status_ul.sn_size      = drb_ctx->second.pdcp_cfg.rx.sn_size;
        drb_item.drb_status_ul.ul_count.hfn = drb_modified.pdcp_sn_status_info->pdcp_status_transfer_ul.count_value.hfn;
        drb_item.drb_status_ul.ul_count.sn =
            drb_modified.pdcp_sn_status_info->pdcp_status_transfer_ul.count_value.pdcp_sn;
        status_transfer.drbs_subject_to_status_transfer_list.emplace(drb_modified.drb_id, drb_item);
      }
    }
  }
  return true;
}
