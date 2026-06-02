// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_resume_routine.h"
#include "pdu_session_routine_helpers.h"
#include "ocudu/ran/cu_cp_types.h"

using namespace ocudu;
using namespace ocucp;
using namespace asn1::rrc_nr;

static bool
verify_rrc_resume_request(const rrc_resume_request& request, ue_manager& ue_mng, const ocudulog::basic_logger& logger)
{
  if (request.ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("UE index must not be invalid");
    return false;
  }

  if (!ue_mng.find_du_ue(request.ue_index)) {
    logger.warning("Can't find ue={}");
    return false;
  }

  return true;
}

ue_resume_routine::ue_resume_routine(const rrc_resume_request&              request_,
                                     const ue_configuration&                ue_cfg_,
                                     du_processor&                          du_proc_,
                                     cu_cp_ue_context_manipulation_handler& ue_context_handler_,
                                     e1ap_bearer_context_manager&           e1ap_bearer_ctxt_mng_,
                                     ue_manager&                            ue_mng_,
                                     cu_cp_location_manager_handler&        loc_mng_handler_,
                                     ocudulog::basic_logger&                logger_) :
  request(request_),
  ue_cfg(ue_cfg_),
  du_proc(du_proc_),
  ue_context_handler(ue_context_handler_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  ue_mng(ue_mng_),
  loc_mng_handler(loc_mng_handler_),
  logger(logger_)
{
}

void ue_resume_routine::operator()(coro_context<async_task<rrc_resume_request_response>>& ctx)
{
  CORO_BEGIN(ctx);

  {
    // Verify input.
    if (!verify_rrc_resume_request(request, ue_mng, logger)) {
      logger.warning("ue={}: \"{}\" - invalid input parameters", request.ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    // Retrieve source UE context.
    ue          = ue_mng.find_du_ue(request.ue_index);
    rrc_context = ue->get_rrc_ue()->get_transfer_context();
    next_config = to_config_update(rrc_context.up_ctx);
  }

  logger.debug("ue={}: \"{}\" started...", request.ue_index, name());

  // If resume cause is RNA update, the UE will be set to RRC inactive again.
  {
    if (request.cause == resume_cause_t::rna_upd) {
      logger.debug("ue={}: RRC Resume cause is RNA Update. Requesting UE Release to set UE to RRC Inactive",
                   request.ue_index);

      // Set UE as active.
      ue_mng.set_active(request.ue_index);

      // Set UE as inactive to allocate new I-RNTIs.
      i_rntis = ue_mng.set_inactive(request.ue_index);
      if (i_rntis.has_value()) {
        logger.debug("ue={}: Set UE as inactive with {} {}",
                     request.ue_index,
                     i_rntis.value().short_i_rnti,
                     i_rntis.value().full_i_rnti);

        CORO_AWAIT_VALUE(
            released_ue_index,
            du_proc.get_f1ap_handler().handle_ue_context_release_command(fill_du_ue_context_release_command()));

        if (released_ue_index != request.ue_index) {
          logger.warning("ue={}: \"{}\" failed to release UE context at DU", request.ue_index, name());
          CORO_EARLY_RETURN(response_msg);
        }

        // Start RNA update timer after successful transition to RRC Inactive state.
        {
          ue_context_handler.initialize_rna_update_timer(request.ue_index);
        }

        logger.debug("ue={}: \"{}\" finished successfully by setting UE to inactive", request.ue_index, name());
        response_msg.success = true;
        CORO_EARLY_RETURN(response_msg);
      }
    }
  }

  {
    // Prepare F1AP UE Context Setup Command and call F1AP notifier.
    if (!generate_ue_context_setup_request(ue_context_setup_request,
                                           ue->get_rrc_ue()->get_srbs(),
                                           rrc_context,
                                           ue->get_rrc_ue()->get_cell_group_config())) {
      logger.warning("ue={}: \"{}\" failed to generate UeContextSetupRequest", request.ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
    ue_context_setup_request.ue_index                   = request.ue_index;
    ue_context_setup_request.cu_to_du_rrc_info.meas_cfg = ue->get_rrc_ue()->get_packed_meas_config();
    ue_context_setup_request.serving_cell_mo            = ue->get_rrc_ue()->get_serving_cell_mo();

    CORO_AWAIT_VALUE(ue_context_setup_response,
                     du_proc.get_f1ap_handler().handle_ue_context_setup_request(ue_context_setup_request, rrc_context));

    // Handle UE Context Setup Response.
    if (!handle_ue_context_setup_response()) {
      logger.warning("ue={}: \"{}\" failed to create UE context at DU", request.ue_index, name());
      // Note: From this point the UE is removed and only the stored context can be accessed.
      CORO_EARLY_RETURN(response_msg);
    }

    // Store updated cell group config.
    ue->get_rrc_ue()->update_cell_group_config(ue_context_setup_response.du_to_cu_rrc_info.cell_group_cfg.copy());
  }

  // Update UE context with new C-RNTI.
  if (ue_context_setup_response.c_rnti.has_value()) {
    ue->get_ue_context().crnti = ue_context_setup_response.c_rnti.value();
    ue->get_rrc_ue()->update_c_rnti(ue_context_setup_response.c_rnti.value());
  } else {
    ue->get_ue_context().crnti = request.new_c_rnti;
    ue->get_rrc_ue()->update_c_rnti(request.new_c_rnti);
  }

  {
    // Prepare update for UP resource manager.
    up_config_update_result result;
    for (const auto& pdu_session_to_add : next_config.pdu_sessions_to_setup_list) {
      result.pdu_sessions_added_list.push_back(pdu_session_to_add.second);
    }
    ue->get_up_resource_manager().apply_config_update(result);
  }

  {
    // Prepare E1AP Bearer Context Modification Request and call E1AP notifier.
    bearer_context_modification_request.ue_index                     = request.ue_index;
    bearer_context_modification_request.bearer_context_status_change = e1ap_bearer_context_status_change::resume;

    CORO_AWAIT_VALUE(
        bearer_context_modification_response,
        e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse.
    if (!bearer_context_modification_response.success) {
      logger.warning("ue={}: \"{}\" failed to modify bearer at CU-UP", request.ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
  }

  {
    // Prepare RRC Resume and return it.
    // If default DRB is being setup, SRB2 needs to be setup as well.
    {
      if (!fill_rrc_resume_request_response(ue_context_setup_request.srbs_to_be_setup_list,
                                            next_config.pdu_sessions_to_setup_list,
                                            {} /* No DRB to be removed */,
                                            ue_context_setup_response.du_to_cu_rrc_info,
                                            ue->get_rrc_ue()->generate_meas_config(rrc_context.meas_cfg),
                                            false /* The UE reestablished SRBs after sending the resume request */,
                                            true /* Reestablish DRBs */,
                                            std::nullopt /* Selected algos */)) {
        logger.warning("ue={}: \"{}\" Failed to fill RrcReconfiguration", request.ue_index, name());
        CORO_EARLY_RETURN(response_msg);
      }
    }

    logger.debug("ue={}: \"{}\" finished successfully", request.ue_index, name());
    response_msg.success = true;
  }

  // Send a location report if needed.
  loc_mng_handler.handle_location_update(request.ue_index);

  CORO_RETURN(response_msg);
}

f1ap_ue_context_release_command ue_resume_routine::fill_du_ue_context_release_command()
{
  // Add all local cells to RAN area cells.
  // Note: Support for configurable ran area cells is left as future work.
  rrc_ue_release_context release_context = ue->get_rrc_ue()->get_rrc_ue_release_context(
      true,
      std::nullopt,
      rrc_inactivity_context{.i_rntis                    = i_rntis.value(),
                             .next_hop_chaining_count    = ue->get_security_manager().get_ncc(),
                             .ran_paging_cycle           = ue_cfg.ran_paging_cycle,
                             .ran_notification_area_info = du_proc.get_rrc_du_handler().get_ran_area_cells(),
                             .t380                       = ue_cfg.t380});

  // Prepare UE Context Release Command and call F1.
  f1ap_ue_context_release_command du_ue_context_release_command;
  du_ue_context_release_command.ue_index = request.ue_index;
  du_ue_context_release_command.cause    = f1ap_cause_radio_network_t::normal_release;
  du_ue_context_release_command.rrc_pdu  = release_context.rrc_pdu.copy();
  du_ue_context_release_command.srb_id   = release_context.srb_id;

  return du_ue_context_release_command;
}

bool ue_resume_routine::generate_ue_context_setup_request(f1ap_ue_context_setup_request&               setup_request,
                                                          const static_vector<srb_id_t, MAX_NOF_SRBS>& srbs,
                                                          const rrc_ue_transfer_context&               transfer_context,
                                                          byte_buffer& cell_group_config)
{
  setup_request.serv_cell_idx = 0; // TODO: Remove hardcoded value
  setup_request.sp_cell_id    = request.cgi;

  if (transfer_context.handover_preparation_info.empty()) {
    return false;
  }

  setup_request.cu_to_du_rrc_info.ue_cap_rat_container_list = transfer_context.ue_cap_rat_container_list.copy();
  // Add cell group config IE, so the DU will request RLC reestablishment from the UE.
  setup_request.cu_to_du_rrc_info.ie_exts.emplace();
  setup_request.cu_to_du_rrc_info.ie_exts->cell_group_cfg = cell_group_config.copy();

  for (const auto& srb_id : srbs) {
    f1ap_srb_to_setup srb_item;
    srb_item.srb_id = srb_id;
    setup_request.srbs_to_be_setup_list.push_back(srb_item);
  }

  for (const auto& pdu_session : next_config.pdu_sessions_to_setup_list) {
    for (const auto& drb : pdu_session.second.drb_to_add) {
      const up_drb_context& drb_context = drb.second;

      f1ap_drb_to_setup drb_item;
      drb_item.drb_id           = drb_context.drb_id;
      drb_item.qos_info.drb_qos = drb_context.qos_params;

      // Add each QoS flow including QoS.
      for (const auto& flow : drb_context.qos_flows) {
        flow_mapped_to_drb flow_item;
        flow_item.qos_flow_id               = flow.first;
        flow_item.qos_flow_level_qos_params = flow.second.qos_params;
        drb_item.qos_info.flows_mapped_to_drb_list.push_back(flow_item);
      }
      drb_item.uluptnl_info_list = drb_context.ul_up_tnl_info_to_be_setup_list;
      drb_item.mode              = drb_context.rlc_mod;
      drb_item.pdcp_sn_len       = drb_context.pdcp_cfg.tx.sn_size;

      setup_request.drbs_to_be_setup_list.push_back(drb_item);
    }
  }

  return true;
}

bool ue_resume_routine::handle_ue_context_setup_response()
{
  // Sanity checks.
  if (ue_context_setup_response.ue_index == cu_cp_ue_index_t::invalid) {
    logger.warning("Failed to create UE at the DU");
    return false;
  }

  if (!ue_context_setup_response.srbs_failed_to_be_setup_list.empty()) {
    logger.warning("Couldn't setup {} SRBs at DU", ue_context_setup_response.srbs_failed_to_be_setup_list.size());
    return false;
  }

  if (!ue_context_setup_response.drbs_failed_to_be_setup_list.empty()) {
    logger.warning("Couldn't setup {} DRBs at DU", ue_context_setup_response.drbs_failed_to_be_setup_list.size());
    return false;
  }

  if (ue_context_setup_response.c_rnti.has_value() && (*ue_context_setup_response.c_rnti != request.new_c_rnti)) {
    logger.warning(
        "Inconsistent C-RNTI in Initial UL RRC Message and UE Context Setup Response. init_c_rnti={} resp_c_rnti={}",
        request.new_c_rnti,
        ue_context_setup_response.c_rnti);
    // Intentionaly continue. Will update the C-RNTI with the latest one provided by the DU.
  }

  // Create bearer context mod request.
  if (!ue_context_setup_response.drbs_setup_list.empty()) {
    // Fill security info
    security::sec_as_config security_cfg = ue->get_security_manager().get_up_as_config();

    bearer_context_modification_request.security_info.emplace();
    bearer_context_modification_request.security_info->security_algorithm.ciphering_algo = security_cfg.cipher_algo;
    bearer_context_modification_request.security_info->security_algorithm.integrity_protection_algorithm =
        security_cfg.integ_algo;
    auto k_enc_buffer = byte_buffer::create(security_cfg.k_enc);
    if (not k_enc_buffer.has_value()) {
      logger.warning("Unable to allocate byte_buffer");
      return false;
    }
    bearer_context_modification_request.security_info->up_security_key.encryption_key = std::move(k_enc_buffer.value());
    if (security_cfg.k_int.has_value()) {
      auto k_int_buffer = byte_buffer::create(security_cfg.k_int.value());
      if (not k_int_buffer.has_value()) {
        logger.warning("Unable to allocate byte_buffer");
        return false;
      }
      bearer_context_modification_request.security_info->up_security_key.integrity_protection_key =
          std::move(k_int_buffer.value());
    }

    auto& context_mod_request = bearer_context_modification_request.ng_ran_bearer_context_mod_request.emplace();

    // Extract new DL tunnel information for CU-UP.
    for (const auto& pdu_session : next_config.pdu_sessions_to_setup_list) {
      // The modifications are only for this PDU session.
      e1ap_pdu_session_res_to_modify_item e1ap_mod_item;
      e1ap_mod_item.pdu_session_id = pdu_session.first;

      for (const auto& drb_item : pdu_session.second.drb_to_add) {
        auto drb_it = std::find_if(ue_context_setup_response.drbs_setup_list.begin(),
                                   ue_context_setup_response.drbs_setup_list.end(),
                                   [&drb_item](const auto& drb) { return drb.drb_id == drb_item.first; });
        ocudu_assert(drb_it != ue_context_setup_response.drbs_setup_list.end(),
                     "Couldn't find {} in UE context setup response",
                     drb_item.first);
        const auto& context_setup_drb_item = *drb_it;

        e1ap_drb_to_modify_item_ng_ran e1ap_drb_item;
        e1ap_drb_item.drb_id = drb_item.first;

        for (const auto& dl_up_tnl_info : context_setup_drb_item.dluptnl_info_list) {
          e1ap_up_params_item e1ap_dl_up_param;
          e1ap_dl_up_param.up_tnl_info   = dl_up_tnl_info;
          e1ap_dl_up_param.cell_group_id = 0;

          e1ap_drb_item.dl_up_params.push_back(e1ap_dl_up_param);
        }

        // Request PDCP reestablishment.
        const up_drb_context& drb_ctxt = ue->get_up_resource_manager().get_drb_context(drb_item.first);
        e1ap_drb_item.pdcp_cfg.emplace();
        fill_e1ap_drb_pdcp_config(e1ap_drb_item.pdcp_cfg.value(), drb_ctxt.pdcp_cfg);
        e1ap_drb_item.pdcp_cfg->pdcp_reest = true;

        e1ap_mod_item.drb_to_modify_list_ng_ran.emplace(e1ap_drb_item.drb_id, e1ap_drb_item);
      }

      context_mod_request.pdu_session_res_to_modify_list.emplace(e1ap_mod_item.pdu_session_id, e1ap_mod_item);
    }
  }

  return ue_context_setup_response.success;
}

bool ue_resume_routine::fill_rrc_resume_request_response(
    const std::vector<f1ap_srb_to_setup>&                            srbs_to_be_setup_mod_list,
    const std::map<pdu_session_id_t, up_pdu_session_context_update>& pdu_sessions,
    const std::vector<drb_id_t>&                                     drb_to_remove,
    const f1ap_du_to_cu_rrc_info&                                    du_to_cu_rrc_info,
    const std::optional<rrc_meas_cfg>&                               rrc_meas_cfg,
    bool                                                             reestablish_srbs,
    bool                                                             reestablish_drbs,
    std::optional<security::sec_selected_algos>                      selected_algos)
{
  rrc_radio_bearer_config radio_bearer_config;
  // If default DRB is being setup, SRB2 needs to be setup as well.
  if (!srbs_to_be_setup_mod_list.empty()) {
    for (const f1ap_srb_to_setup& srb_to_add_mod : srbs_to_be_setup_mod_list) {
      rrc_srb_to_add_mod srb = {};
      srb.srb_id             = srb_to_add_mod.srb_id;
      if (reestablish_srbs) {
        srb.reestablish_pdcp_present = true;
      }
      radio_bearer_config.srb_to_add_mod_list.emplace(srb_to_add_mod.srb_id, srb);
    }
  }

  // Verify DU container content.
  if (!du_to_cu_rrc_info.cell_group_cfg.empty()) {
    if (!verify_and_log_cell_group_config(du_to_cu_rrc_info.cell_group_cfg, logger)) {
      logger.warning("Failed to verify cellGroupConfig");
      return false;
    }
    // Set masterCellGroupConfig as received by DU.
    response_msg.master_cell_group = du_to_cu_rrc_info.cell_group_cfg.copy();
  }

  for (const auto& pdu_session_to_add_mod : pdu_sessions) {
    // Fill radio bearer config.
    for (const auto& drb_to_add : pdu_session_to_add_mod.second.drb_to_add) {
      rrc_drb_to_add_mod drb_to_add_mod;
      drb_to_add_mod.drb_id = drb_to_add.first;
      if (reestablish_drbs) {
        drb_to_add_mod.reestablish_pdcp_present = true;
      } else {
        drb_to_add_mod.pdcp_cfg = drb_to_add.second.pdcp_cfg;

        // Fill CN association and SDAP config.
        rrc_cn_assoc cn_assoc;
        cn_assoc.sdap_cfg       = drb_to_add.second.sdap_cfg;
        drb_to_add_mod.cn_assoc = cn_assoc;
      }

      radio_bearer_config.drb_to_add_mod_list.emplace(drb_to_add.first, drb_to_add_mod);
    }

    for (const auto& drb_to_modify : pdu_session_to_add_mod.second.drb_to_modify) {
      rrc_drb_to_add_mod drb_to_add_mod;
      drb_to_add_mod.drb_id = drb_to_modify.first;
      if (reestablish_drbs) {
        drb_to_add_mod.reestablish_pdcp_present = true;
      } else {
        drb_to_add_mod.pdcp_cfg = drb_to_modify.second.pdcp_cfg;

        // Fill CN association and SDAP config.
        rrc_cn_assoc cn_assoc;
        cn_assoc.sdap_cfg       = drb_to_modify.second.sdap_cfg;
        drb_to_add_mod.cn_assoc = cn_assoc;
      }

      radio_bearer_config.drb_to_add_mod_list.emplace(drb_to_modify.first, drb_to_add_mod);
    }

    // Remove DRB from a PDU session (PDU session itself still exists with out DRBs).
    for (const auto& drb_id : pdu_session_to_add_mod.second.drb_to_remove) {
      radio_bearer_config.drb_to_release_list.push_back(drb_id);
    }
  }

  // Remove DRB (if not already) that are not associated with any PDU session anymore.
  for (const auto& drb_id : drb_to_remove) {
    if (std::any_of(radio_bearer_config.drb_to_release_list.begin(),
                    radio_bearer_config.drb_to_release_list.end(),
                    [drb_id](const auto& item) { return item == drb_id; })) {
      // The DRB is already set to be removed.
      continue;
    }

    radio_bearer_config.drb_to_release_list.push_back(drb_id);
  }

  // If selected security algos, fill securityConfig
  if (selected_algos) {
    radio_bearer_config.security_cfg.emplace();
    radio_bearer_config.security_cfg->security_algorithm_cfg.emplace();
    radio_bearer_config.security_cfg->security_algorithm_cfg->ciphering_algorithm      = selected_algos->cipher_algo;
    radio_bearer_config.security_cfg->security_algorithm_cfg->integrity_prot_algorithm = selected_algos->integ_algo;
  }

  if (radio_bearer_config.contains_values()) {
    // Fill radio bearer config.
    response_msg.radio_bearer_cfg = radio_bearer_config;
  }

  // Fill measurement config.
  response_msg.meas_cfg = rrc_meas_cfg;

  // Fill meas gap config.
  if (!du_to_cu_rrc_info.meas_gap_cfg.empty()) {
    response_msg.meas_gap_cfg = du_to_cu_rrc_info.meas_gap_cfg.copy();
  }

  return true;
}
