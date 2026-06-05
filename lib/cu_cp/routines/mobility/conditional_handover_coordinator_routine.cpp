// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "conditional_handover_coordinator_routine.h"
#include "../../du_processor/du_processor_repository.h"
#include "../../mobility_manager/mobility_manager_helpers.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "conditional_handover_reconfiguration_routine.h"
#include "intra_cu_handover_routine.h"
#include "ocudu/xnap/xnap_handover.h"

using namespace ocudu;
using namespace ocudu::ocucp;

conditional_handover_coordinator_routine::conditional_handover_coordinator_routine(
    const cu_cp_intra_cu_cho_request& request_,
    du_processor_repository&          du_db_,
    cu_cp_impl_interface&             cu_cp_handler_,
    ue_manager&                       ue_mng_,
    mobility_manager&                 mobility_mng_,
    ngap_repository&                  ngap_db_,
    xnap_repository*                  xnap_db_,
    ocudulog::basic_logger&           logger_) :
  request(request_),
  du_db(du_db_),
  cu_cp_handler(cu_cp_handler_),
  ue_mng(ue_mng_),
  mobility_mng(mobility_mng_),
  ngap_db(ngap_db_),
  xnap_db(xnap_db_),
  logger(logger_)
{
}

std::vector<async_task<cu_cp_cho_candidate>> conditional_handover_coordinator_routine::build_intra_cu_prep_tasks()
{
  std::vector<async_task<cu_cp_cho_candidate>> tasks;

  for (size_t i = 0; i < request.targets.size(); ++i) {
    const auto& target = request.targets[i];
    if (target.xnc_index.has_value()) {
      continue;
    }

    cu_cp_intra_cu_handover_request req;
    req.source_ue_index = request.source_ue_index;
    req.target_du_index = target.du_index;
    req.target_pci      = target.pci;
    req.cgi             = target.cgi;
    req.cho_preparation.emplace();
    req.cho_preparation->cond_recfg_id = static_cast<cond_recfg_id_t>(i + 1);

    const cu_cp_du_index_t target_du = target.du_index;
    byte_buffer            sib1 = du_db.get_du_processor(target_du).get_mobility_handler().get_packed_sib1(target.cgi);

    cu_cp_cho_candidate cand = {};
    cand.cond_recfg_id       = static_cast<cond_recfg_id_t>(i + 1);
    cand.target_pci          = target.pci;
    cand.target_cgi          = target.cgi;

    tasks.push_back(launch_async([cand, req, sib1 = std::move(sib1), this, target_du](
                                     coro_context<async_task<cu_cp_cho_candidate>>& ctx) mutable {
      cu_cp_intra_cu_handover_response ho_result;
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(
          ho_result,
          launch_async<intra_cu_handover_routine>(req,
                                                  sib1,
                                                  du_db.get_du_processor(request.source_du_index).get_f1ap_handler(),
                                                  du_db.get_du_processor(target_du).get_f1ap_handler(),
                                                  cu_cp_handler,
                                                  ue_mng,
                                                  mobility_mng,
                                                  logger));
      if (ho_result.success && ho_result.cho_preparation_result.has_value()) {
        cand.target_ue_index             = ho_result.cho_preparation_result->target_ue_index;
        cand.prepared_rrc_recfg          = std::move(ho_result.cho_preparation_result->packed_rrc_recfg);
        cand.rrc_reconfig_transaction_id = ho_result.cho_preparation_result->transaction_id;
        cand.bearer_context_mod_request.ng_ran_bearer_context_mod_request =
            std::move(ho_result.cho_preparation_result->ng_ran_bearer_context_mod_request);
      }
      CORO_RETURN(cand);
    }));
  }

  return tasks;
}

std::vector<async_task<cu_cp_cho_candidate>> conditional_handover_coordinator_routine::build_inter_cu_prep_tasks()
{
  std::vector<async_task<cu_cp_cho_candidate>> tasks;

  if (xnap_db == nullptr) {
    return tasks;
  }

  cu_cp_ue* ue = ue_mng.find_du_ue(request.source_ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: CHO inter-CU preparation skipped: source UE not found", request.source_ue_index);
    return tasks;
  }

  ngap_interface* ngap = ngap_db.find_ngap(ue->get_ue_context().plmn);
  if (ngap == nullptr) {
    logger.warning("ue={}: CHO inter-CU preparation skipped: NGAP not found", request.source_ue_index);
    return tasks;
  }

  const ngap_context_t&  ngap_ctxt = ngap->get_ngap_context();
  std::optional<guami_t> served_guami;
  for (const auto& guami : ngap_ctxt.served_guami_list) {
    if (guami.plmn == ue->get_ue_context().plmn) {
      served_guami = guami;
      break;
    }
  }
  if (!served_guami.has_value()) {
    logger.warning("ue={}: CHO inter-CU preparation skipped: GUAMI not found for plmn={}",
                   request.source_ue_index,
                   ue->get_ue_context().plmn);
    return tasks;
  }

  amf_ue_id_t source_amf_ue_id = ngap->get_amf_ue_id(request.source_ue_index);
  if (source_amf_ue_id == amf_ue_id_t::invalid) {
    logger.warning("ue={}: CHO inter-CU preparation skipped: invalid AMF UE ID", request.source_ue_index);
    return tasks;
  }

  for (size_t i = 0; i < request.targets.size(); ++i) {
    const auto& target = request.targets[i];
    if (!target.xnc_index.has_value()) {
      continue;
    }

    xnap_interface* xnap = xnap_db->find_xnap(*target.xnc_index);
    if (xnap == nullptr) {
      logger.warning("ue={}: CHO inter-CU candidate skipped: XNAP not found for xnc_index={}",
                     request.source_ue_index,
                     *target.xnc_index);
      continue;
    }

    xnap_handover_request ho_request = generate_xnap_handover_request(
        request.source_ue_index,
        target.cgi,
        served_guami.value(),
        source_amf_ue_id,
        ue->get_ue_ambr(),
        ue->get_security_manager().get_security_context(),
        ue->get_up_resource_manager().get_pdu_sessions_map(),
        ue->get_rrc_ue()->get_rrc_ue_control_message_handler().get_packed_handover_preparation_message(),
        ue->get_location_manager().get_location_reporting_request());

    ho_request.is_conditional_handover = true;
    ho_request.cho_timeout             = request.timeout;

    // Pre-populate candidate with input fields; response fields filled in by the wrapper below.
    cu_cp_cho_candidate cand = {};
    cand.cond_recfg_id       = static_cast<cond_recfg_id_t>(i + 1);
    cand.target_pci          = target.pci;
    cand.target_cgi          = target.cgi;
    cand.xnc_index           = target.xnc_index;

    tasks.push_back(launch_async([cand, xnap, ho_request, resp = xnap_handover_preparation_response{}](
                                     coro_context<async_task<cu_cp_cho_candidate>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(resp, xnap->handle_handover_request_required(ho_request));
      if (resp.success) {
        cand.prepared_rrc_recfg          = std::move(resp.packed_rrc_recfg);
        cand.rrc_reconfig_transaction_id = resp.rrc_transaction_id;
        cand.peer_xnap_ue_id             = resp.peer_xnap_ue_id;
      }
      CORO_RETURN(cand);
    }));
  }

  return tasks;
}

std::vector<async_task<cu_cp_cho_candidate>> conditional_handover_coordinator_routine::build_all_prep_tasks()
{
  std::vector<async_task<cu_cp_cho_candidate>> all_tasks = build_intra_cu_prep_tasks();
  std::vector<async_task<cu_cp_cho_candidate>> inter_cu  = build_inter_cu_prep_tasks();
  all_tasks.insert(all_tasks.end(), std::make_move_iterator(inter_cu.begin()), std::make_move_iterator(inter_cu.end()));
  return all_tasks;
}

void conditional_handover_coordinator_routine::operator()(coro_context<async_task<cu_cp_intra_cu_cho_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" started...", request.source_ue_index, name());

  if (request.source_ue_index == cu_cp_ue_index_t::invalid || request.source_du_index == cu_cp_du_index_t::invalid ||
      request.targets.empty()) {
    logger.warning("CHO coordinator request is invalid");
    CORO_EARLY_RETURN(response);
  }

  source_ue = ue_mng.find_du_ue(request.source_ue_index);
  if (source_ue == nullptr || !source_ue->get_cho_context().has_value()) {
    logger.warning("ue={}: CHO coordinator failed. Source UE/CHO context missing", request.source_ue_index);
    CORO_EARLY_RETURN(response);
  }

  // Pre-start: verify CHO measurement config can be generated before preparing any targets.
  {
    rrc_ue_transfer_context source_rrc_context = source_ue->get_rrc_ue()->get_transfer_context();
    std::vector<pci_t>      candidate_target_pcis;
    for (const auto& target : request.targets) {
      candidate_target_pcis.push_back(target.pci);
    }
    if (!source_ue->get_rrc_ue()
             ->generate_meas_config(source_rrc_context.meas_cfg, true, candidate_target_pcis)
             .has_value()) {
      logger.warning("ue={}: CHO aborted. No conditional trigger configs match UE capabilities",
                     request.source_ue_index);
      CORO_EARLY_RETURN(response);
    }
  }

  // Phase 1: CHO Preparation — launch all intra-CU and inter-CU candidates in parallel.
  source_ue->get_cho_context()->state = cu_cp_ue_cho_context::state_t::targets_preparation;
  CORO_AWAIT_VALUE(all_candidates, when_all(build_all_prep_tasks()));

  // No need to re-fetch source UE here: this routine runs on the source UE's
  // per-UE fifo_task_scheduler, which serializes same-UE procedures and
  // prevents concurrent UE release/procedure execution for this UE.
  ocudu_assert(source_ue != nullptr, "ue={}: Unexpected null source UE after CHO preparation", request.source_ue_index);

  // Process all preparation responses uniformly.
  for (auto& candidate : all_candidates) {
    const bool success = candidate.is_inter_cu() ? candidate.peer_xnap_ue_id != peer_xnap_ue_id_t::invalid
                                                 : candidate.target_ue_index != cu_cp_ue_index_t::invalid;

    if (!success) {
      logger.warning(
          "ue={}: CHO candidate preparation failed for target_pci={}", request.source_ue_index, candidate.target_pci);
      continue;
    }

    if (!candidate.is_inter_cu() && candidate.target_ue_index != request.source_ue_index) {
      // Set source_ue_idx on intra-CU target UE so it can be found during CHO completion.
      auto* target_ue_ptr = ue_mng.find_du_ue(candidate.target_ue_index);
      if (target_ue_ptr != nullptr) {
        target_ue_ptr->get_cho_context().emplace();
        target_ue_ptr->get_cho_context()->role            = cu_cp_ue_cho_context::role_t::target;
        target_ue_ptr->get_cho_context()->source_ue_index = request.source_ue_index;
      }
    }

    if (source_ue->get_cho_context().has_value()) {
      source_ue->get_cho_context()->candidates.push_back(std::move(candidate));
    }
  }

  // Finalize CHO preparation state on source UE.
  source_ue = ue_mng.find_du_ue(request.source_ue_index);
  if (source_ue == nullptr || !source_ue->get_cho_context().has_value() ||
      source_ue->get_cho_context()->candidates.empty()) {
    logger.warning("ue={}: CHO coordinator failed. No prepared candidates", request.source_ue_index);
    CORO_EARLY_RETURN(response);
  }

  // Phase 2: CHO Execution.
  source_ue->get_cho_context()->state    = cu_cp_ue_cho_context::state_t::rrc_reconfiguration;
  cho_reconfig_request.source_ue_index   = request.source_ue_index;
  cho_reconfig_request.timeout           = request.timeout;
  cho_reconfig_request.t1_thres_override = request.t1_thres_override;
  CORO_AWAIT_VALUE(cho_reconfig_result,
                   launch_async<conditional_handover_reconfiguration_routine>(
                       cho_reconfig_request,
                       *source_ue,
                       du_db.get_du_processor(request.source_du_index).get_f1ap_handler(),
                       cu_cp_handler,
                       cu_cp_handler,
                       ue_mng,
                       mobility_mng,
                       logger));
  if (!cho_reconfig_result) {
    logger.warning("ue={}: CHO coordinator failed. Reconfiguration phase failed", request.source_ue_index);
    CORO_EARLY_RETURN(response);
  }

  // Start cancellation timer. Fires conditional_handover_cancellation_routine if UE never executes CHO.
  cu_cp_handler.initialize_cho_execution_timer(request.source_ue_index, request.timeout);

  // Phase 3: CHO completion is handled asynchronously by conditional_handover_source_routine after Access Success,
  // or by inter_cu_cho_source_completion_routine after HandoverSuccess from the winning target CU-CP.

  logger.debug("ue={}: \"{}\" finished successfully", source_ue->get_ue_index(), name());
  response.success = true;
  CORO_RETURN(response);
}
