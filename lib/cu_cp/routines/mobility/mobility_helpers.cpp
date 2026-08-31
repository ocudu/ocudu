// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mobility_helpers.h"
#include "../pdu_session_routine_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"

using namespace ocudu;
using namespace ocudu::ocucp;

void ocudu::ocucp::merge_old_drb_association_from_as_config(up_old_drb_association& old_drb_association,
                                                            const byte_buffer& rrc_handover_preparation_information,
                                                            const ocudulog::basic_logger& logger)
{
  if (rrc_handover_preparation_information.empty()) {
    logger.debug("No RRC container received. Cannot derive the source's DRB-to-QoS-flow mapping from AS-Config");
    return;
  }

  asn1::rrc_nr::ho_prep_info_s ho_prep_info;
  asn1::cbit_ref bref({rrc_handover_preparation_information.begin(), rrc_handover_preparation_information.end()});
  if (ho_prep_info.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
    logger.warning("Couldn't unpack HandoverPreparationInformation");
    return;
  }
  if (ho_prep_info.crit_exts.type() != asn1::rrc_nr::ho_prep_info_s::crit_exts_c_::types::c1 or
      ho_prep_info.crit_exts.c1().type() != asn1::rrc_nr::ho_prep_info_s::crit_exts_c_::c1_c_::types::ho_prep_info) {
    logger.warning("Unsupported HandoverPreparationInformation critical extension");
    return;
  }

  const auto& ies = ho_prep_info.crit_exts.c1().ho_prep_info();
  if (!ies.source_cfg_present) {
    // The source may report the mapping through the XnAP-native IE instead, so this is not an error.
    logger.debug("AS-Config not present in HandoverPreparationInformation");
    return;
  }

  asn1::rrc_nr::rrc_recfg_s source_recfg;
  asn1::cbit_ref            recfg_bref({ies.source_cfg.rrc_recfg.begin(), ies.source_cfg.rrc_recfg.end()});
  if (source_recfg.unpack(recfg_bref) != asn1::OCUDUASN_SUCCESS) {
    logger.warning("Couldn't unpack RRCReconfiguration in AS-Config");
    return;
  }
  if (source_recfg.crit_exts.type() != asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types::rrc_recfg) {
    logger.warning("Unsupported RRCReconfiguration critical extension in AS-Config");
    return;
  }

  const auto& recfg_ies = source_recfg.crit_exts.rrc_recfg();
  if (!recfg_ies.radio_bearer_cfg_present) {
    logger.debug("No radio bearer configuration in AS-Config");
    return;
  }

  for (const auto& drb : recfg_ies.radio_bearer_cfg.drb_to_add_mod_list) {
    if (!drb.cn_assoc_present ||
        drb.cn_assoc.type() != asn1::rrc_nr::drb_to_add_mod_s::cn_assoc_c_::types_opts::sdap_cfg) {
      continue;
    }
    const auto&      sdap_cfg = drb.cn_assoc.sdap_cfg();
    pdu_session_id_t psi      = uint_to_pdu_session_id(sdap_cfg.pdu_session);
    drb_id_t         drb_id   = uint_to_drb_id(drb.drb_id);
    for (uint8_t qfi : sdap_cfg.mapped_qos_flows_to_add) {
      old_drb_association[psi][uint_to_qos_flow_id(qfi)] = drb_id;
    }
  }
}

bool ocudu::ocucp::handle_context_setup_response(
    cu_cp_intra_cu_handover_response&         response_msg,
    e1ap_bearer_context_modification_request& bearer_context_modification_request,
    const f1ap_ue_context_setup_response&     target_ue_context_setup_response,
    up_config_update&                         next_config,
    const ocudulog::basic_logger&             logger,
    bool                                      reestablish_pdcp)
{
  // Sanity checks.
  if (target_ue_context_setup_response.ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Failed to create UE at the target DU");
    return false;
  }

  if (!target_ue_context_setup_response.srbs_failed_to_be_setup_list.empty()) {
    logger.warning("Couldn't setup {} SRBs at target DU",
                   target_ue_context_setup_response.srbs_failed_to_be_setup_list.size());
    return false;
  }

  if (!target_ue_context_setup_response.drbs_failed_to_be_setup_list.empty()) {
    logger.warning("Couldn't setup {} DRBs at target DU",
                   target_ue_context_setup_response.drbs_failed_to_be_setup_list.size());
    return false;
  }

  if (!target_ue_context_setup_response.c_rnti.has_value()) {
    logger.warning("No C-RNTI present in UE context setup");
    return false;
  }

  // Create bearer context mod request.
  if (!target_ue_context_setup_response.drbs_setup_list.empty()) {
    auto& context_mod_request = bearer_context_modification_request.ng_ran_bearer_context_mod_request.emplace();

    // Extract new DL tunnel information for CU-UP.
    for (const auto& pdu_session : next_config.pdu_sessions_to_setup_list) {
      // The modifications are only for this PDU session.
      e1ap_pdu_session_res_to_modify_item e1ap_mod_item;
      e1ap_mod_item.pdu_session_id = pdu_session.first;

      for (const auto& drb_item : pdu_session.second.drb_to_add) {
        auto drb_it = std::find_if(target_ue_context_setup_response.drbs_setup_list.begin(),
                                   target_ue_context_setup_response.drbs_setup_list.end(),
                                   [&drb_item](const auto& drb) { return drb.drb_id == drb_item.first; });
        ocudu_assert(drb_it != target_ue_context_setup_response.drbs_setup_list.end(),
                     "Couldn't find {} in UE context setup response",
                     drb_item.first);
        const auto& context_setup_drb_item = *drb_it;

        e1ap_drb_to_modify_item_ng_ran e1ap_drb_item;
        e1ap_drb_item.drb_id = drb_item.first;

        for (const auto& dl_up_tnl_info : context_setup_drb_item.dluptnl_info_list) {
          e1ap_up_params_item e1ap_dl_up_param;
          e1ap_dl_up_param.up_tnl_info   = dl_up_tnl_info;
          e1ap_dl_up_param.cell_group_id = 0; // TODO: Remove hardcoded value

          e1ap_drb_item.dl_up_params.push_back(e1ap_dl_up_param);
        }

        if (reestablish_pdcp) {
          // Reestablish PDCP.
          e1ap_drb_item.pdcp_cfg.emplace();
          fill_e1ap_drb_pdcp_config(e1ap_drb_item.pdcp_cfg.value(), drb_item.second.pdcp_cfg);
          e1ap_drb_item.pdcp_cfg->pdcp_reest = true;
        }

        e1ap_mod_item.drb_to_modify_list_ng_ran.emplace(e1ap_drb_item.drb_id, e1ap_drb_item);
      }

      context_mod_request.pdu_session_res_to_modify_list.emplace(e1ap_mod_item.pdu_session_id, e1ap_mod_item);
    }
  }

  return target_ue_context_setup_response.success;
}

bool ocudu::ocucp::handle_bearer_context_modification_response(
    cu_cp_intra_cu_handover_response&                response_msg,
    f1ap_ue_context_modification_request&            source_ue_context_mod_request,
    const e1ap_bearer_context_modification_response& bearer_context_modification_response,
    up_config_update&                                next_config,
    const ocudulog::basic_logger&                    logger)

{
  // TOOD: Add proper handling.
  return bearer_context_modification_response.success;
}

unsigned ocudu::ocucp::cancel_cho_candidates(cu_cp_ue&                          source_ue,
                                             ue_manager&                        ue_mng,
                                             xnap_repository*                   xnap_db,
                                             cu_cp_ue_index_t                   winner_ue_index,
                                             std::optional<nr_cell_global_id_t> winner_cgi)
{
  unsigned cancelled = 0;
  auto&    cho_ctx   = source_ue.get_cho_context();
  if (!cho_ctx.has_value()) {
    return 0;
  }
  const cu_cp_ue_index_t source_ue_index = source_ue.get_ue_index();

  // TS 38.423 Section 8.2.8.2: a Handover Success implicitly cancels every other CHO preparation accepted for this UE
  // on the same UE-associated signalling connection, and Sections 8.2.3.2 and 8.2.9.2 identify such a connection by
  // the Source *and* Target NG-RAN node UE XnAP IDs. Candidates on the winner's connection are therefore skipped
  // below, while every other one is cancelled explicitly - the winning node alone is not the criterion, since a peer
  // that answers each candidate with a Target NG-RAN node UE XnAP ID of its own puts them on separate connections.
  std::optional<xnc_peer_index_t> winner_xnc_index;
  peer_xnap_ue_id_t               winner_peer_xnap_ue_id = peer_xnap_ue_id_t::invalid;
  if (winner_cgi.has_value()) {
    for (const auto& candidate : cho_ctx->candidates) {
      if (candidate.target_cgi == *winner_cgi) {
        winner_xnc_index       = candidate.xnc_index;
        winner_peer_xnap_ue_id = candidate.peer_xnap_ue_id;
        break;
      }
    }
  }

  for (const auto& candidate : cho_ctx->candidates) {
    if (candidate.is_inter_cu()) {
      if (winner_cgi.has_value() && candidate.target_cgi == *winner_cgi) {
        continue;
      }
      if (winner_peer_xnap_ue_id != peer_xnap_ue_id_t::invalid && candidate.xnc_index == winner_xnc_index &&
          candidate.peer_xnap_ue_id == winner_peer_xnap_ue_id) {
        continue;
      }
      if (xnap_db != nullptr && candidate.xnc_index.has_value()) {
        xnap_interface* xnap = xnap_db->find_xnap(*candidate.xnc_index);
        if (xnap != nullptr) {
          xnap->handle_cho_cancel_required(source_ue_index, candidate.target_cgi);
          ++cancelled;
        }
      }
    } else {
      if (candidate.target_ue_index == cu_cp_ue_index_t::invalid || candidate.target_ue_index == source_ue_index ||
          candidate.target_ue_index == winner_ue_index) {
        continue;
      }
      auto* cand_ue = ue_mng.find_du_ue(candidate.target_ue_index);
      if (cand_ue == nullptr) {
        continue;
      }
      cand_ue->get_rrc_ue()->cancel_handover_reconfiguration_transaction(
          static_cast<uint8_t>(candidate.rrc_reconfig_transaction_id));
      ++cancelled;
    }
  }
  return cancelled;
}
