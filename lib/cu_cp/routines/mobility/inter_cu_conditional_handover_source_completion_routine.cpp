// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "inter_cu_conditional_handover_source_completion_routine.h"
#include "mobility_helpers.h"
#include "ocudu/ran/cause/ngap_cause.h"

using namespace ocudu;
using namespace ocucp;

inter_cu_conditional_handover_source_completion_routine::inter_cu_conditional_handover_source_completion_routine(
    cu_cp_ue_index_t                  source_ue_index_,
    peer_xnap_ue_id_t                 winner_peer_xnap_ue_id_,
    ue_manager&                       ue_mng_,
    cu_up_processor_repository&       cu_up_db_,
    xnap_interface*                   winner_xnap_,
    xnap_repository*                  xnap_db_,
    cu_cp_ue_context_release_handler& release_handler_,
    ocudulog::basic_logger&           logger_) :
  source_ue_index(source_ue_index_),
  winner_peer_xnap_ue_id(winner_peer_xnap_ue_id_),
  ue_mng(ue_mng_),
  cu_up_db(cu_up_db_),
  winner_xnap(winner_xnap_),
  xnap_db(xnap_db_),
  release_handler(release_handler_),
  logger(logger_)
{
}

void inter_cu_conditional_handover_source_completion_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  {
    cu_cp_ue* source_ue = ue_mng.find_du_ue(source_ue_index);
    if (source_ue == nullptr || !source_ue->get_cho_context().has_value()) {
      logger.warning("ue={}: \"{}\" aborted: source UE or CHO context missing", source_ue_index, name());
      CORO_EARLY_RETURN();
    }

    cancel_cho_candidates(*source_ue, ue_mng, xnap_db, cu_cp_ue_index_t::invalid, winner_peer_xnap_ue_id);

    // Clear CHO context now to prevent stale callbacks.
    source_ue->get_cho_context()->clear();
  }

  // Step 2: Query CU-UP for PDCP SN status (per TS 38.423, source sends SN Status Transfer AFTER HandoverSuccess).
  fill_e1ap_bearer_modification_request_pdcp_sn_query();
  if (ue_mng.find_du_ue(source_ue_index) == nullptr) {
    logger.warning("ue={}: \"{}\" source UE gone before CU-UP query", source_ue_index, name());
    CORO_EARLY_RETURN();
  }

  CORO_AWAIT_VALUE(bearer_mod_resp,
                   cu_up_db.find_cu_up_processor(ue_mng.find_du_ue(source_ue_index)->get_cu_up_index())
                       ->get_e1ap_bearer_context_manager()
                       .handle_bearer_context_modification_request(bearer_mod_req));
  if (!bearer_mod_resp.success) {
    logger.warning("ue={}: \"{}\" failed: CU-UP PDCP SN query failed", source_ue_index, name());
    CORO_EARLY_RETURN();
  }

  if (!fill_status_transfer()) {
    logger.warning("ue={}: \"{}\" failed: could not build SN Status Transfer payload", source_ue_index, name());
    CORO_EARLY_RETURN();
  }

  // Step 3: Send SN Status Transfer to the winning target CU-CP via Xn.
  winner_xnap->handle_sn_status_transfer_required(status_transfer);

  // Step 4: Release source UE context.
  if (ue_mng.find_du_ue(source_ue_index) != nullptr) {
    release_cmd                      = {};
    release_cmd.ue_index             = source_ue_index;
    release_cmd.cause                = ngap_cause_radio_network_t::unspecified;
    release_cmd.requires_rrc_message = false;
    CORO_AWAIT_VALUE(release_complete, release_handler.handle_ue_context_release_command(release_cmd));
  }

  logger.info("ue={}: \"{}\" completed successfully", source_ue_index, name());

  CORO_RETURN();
}

void inter_cu_conditional_handover_source_completion_routine::fill_e1ap_bearer_modification_request_pdcp_sn_query()
{
  bearer_mod_req.ue_index = source_ue_index;
  bearer_mod_req.ng_ran_bearer_context_mod_request.emplace();
  cu_cp_ue* source_ue = ue_mng.find_du_ue(source_ue_index);
  if (source_ue == nullptr) {
    return;
  }
  const auto& pdu_sessions = source_ue->get_up_resource_manager().get_pdu_sessions();
  for (const auto& pdu_session_id : pdu_sessions) {
    e1ap_pdu_session_res_to_modify_item e1ap_pdu_session_item;
    e1ap_pdu_session_item.pdu_session_id = pdu_session_id;

    const up_pdu_session_context& pdu_session =
        source_ue->get_up_resource_manager().get_pdu_session_context(pdu_session_id);
    for (const auto& [drb_id, drb_ctx] : pdu_session.drbs) {
      e1ap_drb_to_modify_item_ng_ran drb_to_mod;
      drb_to_mod.drb_id                 = drb_id;
      drb_to_mod.pdcp_sn_status_request = true;
      e1ap_pdu_session_item.drb_to_modify_list_ng_ran.emplace(drb_id, drb_to_mod);
    }
    bearer_mod_req.ng_ran_bearer_context_mod_request->pdu_session_res_to_modify_list.emplace(pdu_session_id,
                                                                                             e1ap_pdu_session_item);
  }
}

bool inter_cu_conditional_handover_source_completion_routine::fill_status_transfer()
{
  status_transfer.ue_index = source_ue_index;

  cu_cp_ue* source_ue = ue_mng.find_du_ue(source_ue_index);
  if (source_ue == nullptr) {
    return false;
  }

  for (const auto& pdu_session_modified : bearer_mod_resp.pdu_session_resource_modified_list) {
    const up_pdu_session_context& pdu_session_ctx =
        source_ue->get_up_resource_manager().get_pdu_session_context(pdu_session_modified.pdu_session_id);
    for (const e1ap_drb_modified_item_ng_ran& drb_modified : pdu_session_modified.drb_modified_list_ng_ran) {
      if (!drb_modified.pdcp_sn_status_info.has_value()) {
        continue;
      }
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
  return true;
}
