// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "retrieved_context_path_switch_routine.h"
#include "retrieved_context_helpers.h"

using namespace ocudu;
using namespace ocucp;

retrieved_context_path_switch_routine::retrieved_context_path_switch_routine(
    cu_cp_ue&                        ue_,
    const cu_cp_path_switch_request& path_switch_request_,
    e1ap_bearer_context_manager&     e1ap_bearer_ctxt_mng_,
    ngap_interface&                  ngap_,
    xnap_interface*                  xnap_,
    ocudulog::basic_logger&          logger_) :
  ue(ue_),
  path_switch_request(path_switch_request_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  ngap(ngap_),
  xnap(xnap_),
  logger(logger_),
  ue_index(ue_.get_ue_index())
{
}

void retrieved_context_path_switch_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("ue={}: \"{}\" started...", ue_index, name());

  // Ask the AMF to move the DL path to this node (TS 38.413 section 8.4.4).
  CORO_AWAIT_VALUE(path_switch_response,
                   ngap.get_ngap_control_message_handler().handle_path_switch_request_required(path_switch_request));

  if (std::holds_alternative<cu_cp_path_switch_request_failure>(path_switch_response)) {
    logger.warning("ue={}: \"{}\" failed. Cause: Path Switch Request rejected by AMF", ue_index, name());
    ue_context_release_request = cu_cp_ue_context_release_request{.ue_index = ue_index,
                                                                  .pdu_session_res_list_cxt_rel_req =
                                                                      ue.get_up_resource_manager().get_pdu_sessions(),
                                                                  .cause = ngap_cause_radio_network_t::unspecified};
    CORO_AWAIT(ngap.handle_ue_context_release_request(ue_context_release_request));
    CORO_EARLY_RETURN(false);
  }

  // Inform the CU-UP of the new UL NG-U tunnel endpoints and of any PDU session the AMF released.
  fill_retrieved_context_tunnel_update_request(
      tunnel_context_mod_request, ue_index, std::get<cu_cp_path_switch_request_ack>(path_switch_response));
  if (!tunnel_context_mod_request.ng_ran_bearer_context_mod_request->pdu_session_res_to_modify_list.empty() ||
      !tunnel_context_mod_request.ng_ran_bearer_context_mod_request->pdu_session_res_to_rem_list.empty()) {
    CORO_AWAIT(e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(tunnel_context_mod_request));
  }

  // The AMF associates the UE with this node now, so the reporting the peer asked for can be run from here.
  start_retrieved_context_location_reporting(ue, ngap.get_ngap_location_reporting_handler(), logger);

  // The path is switched, so the release tells the peer the retrieval was carried through and its context can go.
  if (xnap == nullptr || !xnap->handle_ue_context_release_required(ue_index)) {
    // The UE is served here either way, and the peer drops the context once its own supervision expires.
    logger.warning("ue={}: Could not release the UE context at the peer", ue_index);
  }

  logger.info("ue={}: \"{}\" finished successfully", ue_index, name());

  CORO_RETURN(true);
}
