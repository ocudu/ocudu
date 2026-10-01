// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "retrieved_context_setup_routine.h"
#include "../pdu_session_routine_helpers.h"
#include "mobility_helpers.h"
#include "retrieved_context_helpers.h"
#include "retrieved_context_path_switch_routine.h"

using namespace ocudu;
using namespace ocucp;

retrieved_context_setup_routine::retrieved_context_setup_routine(cu_cp_ue&                    ue_,
                                                                 e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng_,
                                                                 f1ap_ue_context_manager&     f1ap_ue_ctxt_mng_,
                                                                 ngap_interface&              ngap_,
                                                                 xnap_interface*              xnap_,
                                                                 const ue_configuration&      ue_cfg_,
                                                                 const security_indication_t& default_sec_ind_,
                                                                 ocudulog::basic_logger&      logger_) :
  ue(ue_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  f1ap_ue_ctxt_mng(f1ap_ue_ctxt_mng_),
  ngap(ngap_),
  xnap(xnap_),
  ue_cfg(ue_cfg_),
  default_security_indication(default_sec_ind_),
  logger(logger_),
  ue_index(ue_.get_ue_index())
{
}

void retrieved_context_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("ue={}: \"{}\" started...", ue_index, name());

  {
    const cu_cp_ue_context_retrieval_context& retrieval_context = ue.get_context_retrieval_context().value();

    // The DRB numbering of the old NG-RAN node is recovered from the AS-Config inside the RRC container.
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
      CORO_EARLY_RETURN(false);
    }

    CORO_AWAIT_VALUE(bearer_context_setup_response,
                     e1ap_bearer_ctxt_mng.handle_bearer_context_setup_request(bearer_context_setup_request));

    if (!bearer_context_setup_response.success || !fill_ue_context_modification_request()) {
      logger.warning("ue={}: \"{}\" failed. Cause: Could not setup bearer at CU-UP", ue_index, name());
      CORO_EARLY_RETURN(false);
    }
  }

  // Add SRB2 and the DRBs to the UE context the DU created for this UE on Msg3.
  {
    CORO_AWAIT_VALUE(ue_context_mod_response,
                     f1ap_ue_ctxt_mng.handle_ue_context_modification_request(ue_context_mod_request));

    if (!ue_context_mod_response.success || !ue_context_mod_response.drbs_failed_to_be_setup_list.empty()) {
      logger.warning("ue={}: \"{}\" failed. Cause: Could not modify UE context at DU", ue_index, name());
      CORO_EARLY_RETURN(false);
    }

    ue.get_rrc_ue()->update_cell_group_config(ue_context_mod_response.du_to_cu_rrc_info.cell_group_cfg.copy());
  }

  // Inform the CU-UP of the DL F1-U tunnels the DU allocated.
  {
    if (!update_setup_list_with_ue_ctxt_setup_response(
            bearer_context_mod_request, ue_context_mod_response.drbs_setup_list, next_config, logger)) {
      logger.warning(
          "ue={}: \"{}\" failed. Cause: Could not handle UE context modification response", ue_index, name());
      CORO_EARLY_RETURN(false);
    }
    bearer_context_mod_request.ue_index = ue_index;

    CORO_AWAIT_VALUE(bearer_context_mod_response,
                     e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_mod_request));

    if (!bearer_context_mod_response.success) {
      logger.warning("ue={}: \"{}\" failed. Cause: Could not modify bearer at CU-UP", ue_index, name());
      CORO_EARLY_RETURN(false);
    }
  }

  // The bearers exist on both sides now, so the UE can be reconfigured with SRB2 and its DRBs.
  {
    if (!fill_rrc_reconfiguration_args()) {
      logger.warning("ue={}: \"{}\" failed. Cause: Could not fill RRC Reconfiguration", ue_index, name());
      CORO_EARLY_RETURN(false);
    }

    CORO_AWAIT_VALUE(rrc_reconfig_result, ue.get_rrc_ue()->handle_rrc_reconfiguration_request(rrc_reconfig_args));
    if (!rrc_reconfig_result) {
      logger.warning("ue={}: \"{}\" failed. Cause: RRC reconfiguration failed", ue_index, name());
      CORO_EARLY_RETURN(false);
    }
  }

  // The UE is fully served from here, so commit the new configuration before switching the path.
  apply_retrieved_context_up_config_update(ue, next_config);

  // The UE confirmed the RRC Reconfiguration, so the user plane can be moved over and the peer's context released.
  CORO_AWAIT_VALUE(path_switch_success,
                   launch_async<retrieved_context_path_switch_routine>(
                       ue,
                       fill_retrieved_context_path_switch_request(ue, bearer_context_setup_response),
                       e1ap_bearer_ctxt_mng,
                       ngap,
                       xnap,
                       logger));
  if (!path_switch_success) {
    logger.warning("ue={}: \"{}\" failed. Cause: Could not switch the path to this node", ue_index, name());
    CORO_EARLY_RETURN(false);
  }

  logger.info("ue={}: \"{}\" finished successfully", ue_index, name());

  CORO_RETURN(true);
}

bool retrieved_context_setup_routine::fill_ue_context_modification_request()
{
  const cu_cp_ue_context_retrieval_context& retrieval_context = ue.get_context_retrieval_context().value();

  ue_context_mod_request.ue_index = ue_index;

  // The UE holds the configuration of the peer that served it, which this node cannot signal a delta on top of, so
  // the DU is asked for a full configuration (TS 38.401 section 8.4.1.1).
  ue_context_mod_request.full_cfg = true;

  // SRB1 was re-established when the RRCReestablishment was sent, so SRB2 is added here.
  f1ap_srb_to_setup srb2;
  srb2.srb_id = srb_id_t::srb2;
  ue_context_mod_request.srbs_to_be_setup_mod_list.push_back(srb2);

  return update_setup_list_with_bearer_ctxt_setup_response(ue_context_mod_request.drbs_to_be_setup_mod_list,
                                                           next_config,
                                                           retrieval_context.pdu_session_res_to_be_setup_list,
                                                           bearer_context_setup_response,
                                                           ue.get_up_resource_manager(),
                                                           default_security_indication,
                                                           logger);
}

bool retrieved_context_setup_routine::fill_rrc_reconfiguration_args()
{
  // The bearers are set up with their full configuration, since the UE releases the ones it holds from the peer
  // (TS 38.331 section 5.3.5.11).
  if (!fill_rrc_reconfig_args(rrc_reconfig_args,
                              ue_context_mod_request.srbs_to_be_setup_mod_list,
                              next_config.pdu_sessions_to_setup_list,
                              {} /* No DRB to be removed */,
                              ue_context_mod_response.du_to_cu_rrc_info,
                              {} /* No NAS PDUs required */,
                              ue.get_rrc_ue()->generate_meas_config(),
                              false /* Set the SRBs up anew */,
                              false /* Set the DRBs up anew */,
                              // The keys were taken over as the peer derived them, so the UE must not derive again.
                              std::nullopt /* don't update keys */,
                              {},
                              std::nullopt,
                              logger)) {
    return false;
  }

  // The configuration the UE holds is the one of the peer, so this node cannot signal a delta on top of it
  // (TS 38.331 section 5.3.5.11).
  rrc_reconfig_args.non_crit_ext.value().full_cfg_present = true;

  return true;
}
