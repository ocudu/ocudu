// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "retrieved_context_resume_routine.h"
#include "../pdu_session_routine_helpers.h"
#include "mobility_helpers.h"
#include "retrieved_context_helpers.h"

using namespace ocudu;
using namespace ocucp;

retrieved_context_resume_routine::retrieved_context_resume_routine(
    const rrc_resume_request&    request_,
    cu_cp_ue&                    ue_,
    e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng_,
    du_processor&                du_proc_,
    const ue_configuration&      ue_cfg_,
    const security_indication_t& default_security_indication_,
    ocudulog::basic_logger&      logger_) :
  request(request_),
  ue(ue_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  du_proc(du_proc_),
  ue_cfg(ue_cfg_),
  default_security_indication(default_security_indication_),
  logger(logger_),
  ue_index(ue_.get_ue_index())
{
}

void retrieved_context_resume_routine::operator()(coro_context<async_task<rrc_resume_request_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("ue={}: \"{}\" started...", ue_index, name());

  {
    const cu_cp_ue_context_retrieval_context& retrieval_context = ue.get_context_retrieval_context().value();

    // The DRB numbering of the old NG-RAN node is recovered from the AS-Config inside the RRC container. The UE
    // resumes the DRBs it had at that node, so keeping that numbering is what makes its stored configuration match.
    up_old_drb_association old_drb_association;
    merge_old_drb_association_from_as_config(old_drb_association, retrieval_context.rrc_context, logger);

    next_config = ue.get_up_resource_manager().calculate_update(retrieval_context.pdu_session_res_to_be_setup_list,
                                                                old_drb_association);
  }

  // Establish the bearers at the CU-UP from the retrieved PDU session list.
  {
    if (!fill_retrieved_context_bearer_setup_request(
            bearer_context_setup_request, ue, next_config, ue_cfg, default_security_indication, logger)) {
      logger.warning("ue={}: \"{}\" failed. Cause: Could not fill context at CU-UP", ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    CORO_AWAIT_VALUE(bearer_context_setup_response,
                     e1ap_bearer_ctxt_mng.handle_bearer_context_setup_request(bearer_context_setup_request));

    if (!bearer_context_setup_response.success || !fill_ue_context_setup_request()) {
      logger.warning("ue={}: \"{}\" failed. Cause: Could not setup bearer at CU-UP", ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
  }

  // Establish the UE context at the DU, which holds the random access this UE just performed.
  {
    CORO_AWAIT_VALUE(
        ue_context_setup_response,
        du_proc.get_f1ap_handler().handle_ue_context_setup_request(ue_context_setup_request, std::nullopt));

    if (!ue_context_setup_response.success || !ue_context_setup_response.srbs_failed_to_be_setup_list.empty() ||
        !ue_context_setup_response.drbs_failed_to_be_setup_list.empty()) {
      logger.warning("ue={}: \"{}\" failed. Cause: Could not setup UE context at DU", ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }

    ue.get_rrc_ue()->update_cell_group_config(ue_context_setup_response.du_to_cu_rrc_info.cell_group_cfg.copy());
  }

  // Inform the CU-UP of the DL F1-U tunnels the DU allocated.
  {
    if (!update_setup_list_with_ue_ctxt_setup_response(
            bearer_context_mod_request, ue_context_setup_response.drbs_setup_list, next_config, logger)) {
      logger.warning("ue={}: \"{}\" failed. Cause: Could not handle UE context setup response", ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
    bearer_context_mod_request.ue_index = ue_index;

    CORO_AWAIT_VALUE(bearer_context_mod_response,
                     e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_mod_request));

    if (!bearer_context_mod_response.success) {
      logger.warning("ue={}: \"{}\" failed. Cause: Could not modify bearer at CU-UP", ue_index, name());
      CORO_EARLY_RETURN(response_msg);
    }
  }

  // The bearers exist on both sides, so the UE can be told about them in the RRCResume.
  if (!fill_rrc_resume_response()) {
    logger.warning("ue={}: \"{}\" failed. Cause: Could not fill the RRC Resume", ue_index, name());
    CORO_EARLY_RETURN(response_msg);
  }

  // The UE is served from here, so commit the configuration. The DL path is moved once the UE confirms the
  // RRCResume, so the request is left ready for that step.
  apply_retrieved_context_up_config_update(ue, next_config);
  ue.get_context_retrieval_context()->path_switch_request =
      fill_retrieved_context_path_switch_request(ue, bearer_context_setup_response);

  logger.info("ue={}: \"{}\" finished successfully", ue_index, name());

  response_msg.success = true;
  CORO_RETURN(response_msg);
}

bool retrieved_context_resume_routine::fill_ue_context_setup_request()
{
  const cu_cp_ue_context_retrieval_context& retrieval_context = ue.get_context_retrieval_context().value();

  ue_context_setup_request.ue_index        = ue_index;
  ue_context_setup_request.serv_cell_idx   = 0; // TODO: Remove hardcoded value
  ue_context_setup_request.sp_cell_id      = request.cgi;
  ue_context_setup_request.serving_cell_mo = ue.get_rrc_ue()->get_serving_cell_mo();

  // The capabilities are the ones the peer reported, packed inside the retrieved RRC container.
  ue_context_setup_request.cu_to_du_rrc_info.ue_cap_rat_container_list =
      ue.get_rrc_ue()->get_packed_ue_capability_rat_container_list();
  ue_context_setup_request.cu_to_du_rrc_info.meas_cfg = ue.get_rrc_ue()->get_packed_meas_config();

  // SRB1 carries the RRCResume itself, SRB2 is resumed together with the DRBs.
  for (srb_id_t srb_id : {srb_id_t::srb1, srb_id_t::srb2}) {
    f1ap_srb_to_setup srb_item;
    srb_item.srb_id = srb_id;
    ue_context_setup_request.srbs_to_be_setup_list.push_back(srb_item);
  }

  return update_setup_list_with_bearer_ctxt_setup_response(ue_context_setup_request.drbs_to_be_setup_list,
                                                           next_config,
                                                           retrieval_context.pdu_session_res_to_be_setup_list,
                                                           bearer_context_setup_response,
                                                           ue.get_up_resource_manager(),
                                                           default_security_indication,
                                                           logger);
}

bool retrieved_context_resume_routine::fill_rrc_resume_response()
{
  // The UE re-established PDCP for SRB1 itself when it sent the RRCResumeRequest (TS 38.331 section 5.3.13.3), so the
  // SRBs are not marked for PDCP re-establishment. The DRBs are set up with their full configuration, since the UE
  // releases the ones it stored while suspended (TS 38.331 section 5.3.5.11).
  if (!fill_rrc_resume_request_response(response_msg,
                                        ue_context_setup_request.srbs_to_be_setup_list,
                                        next_config.pdu_sessions_to_setup_list,
                                        {} /* No DRB to be removed */,
                                        ue_context_setup_response.du_to_cu_rrc_info,
                                        ue.get_rrc_ue()->generate_meas_config(),
                                        false /* The UE reestablished SRBs after sending the resume request */,
                                        false /* Set the DRBs up anew */,
                                        std::nullopt /* Selected algos */,
                                        logger)) {
    return false;
  }

  // The configuration the UE stored while suspended is the one of the peer that held its context, so this node cannot
  // signal a delta on top of it (TS 38.331 section 5.3.13.4).
  response_msg.full_cfg = true;

  return true;
}
