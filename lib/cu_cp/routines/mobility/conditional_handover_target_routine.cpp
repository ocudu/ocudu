// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "conditional_handover_target_routine.h"
#include "../../cu_up_processor/cu_up_processor_repository.h"
#include "../../du_processor/du_processor_repository.h"

using namespace ocudu;
using namespace ocudu::ocucp;

conditional_handover_target_routine::conditional_handover_target_routine(
    const cu_cp_cho_target_request&        request_,
    ue_manager&                            ue_mng_,
    du_processor_repository&               du_db_,
    cu_up_processor_repository&            cu_up_db_,
    cu_cp_ue_context_manipulation_handler& cu_cp_handler_,
    cu_cp_ue_context_release_handler&      ue_context_release_handler_,
    mobility_manager&                      mobility_mng_,
    ocudulog::basic_logger&                logger_) :
  request(request_),
  ue_mng(ue_mng_),
  du_db(du_db_),
  cu_up_db(cu_up_db_),
  cu_cp_handler(cu_cp_handler_),
  ue_context_release_handler(ue_context_release_handler_),
  mobility_mng(mobility_mng_),
  logger(logger_)
{
}

void conditional_handover_target_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (ue_mng.find_du_ue(request.target_ue_index) == nullptr) {
    logger.warning("CHO target_ue={} got removed", request.target_ue_index);
    CORO_EARLY_RETURN();
  }
  target_ue = ue_mng.find_du_ue(request.target_ue_index);

  logger.debug(
      "target_ue={} source_ue={}: \"{}\" started. Waiting for RRCReconfigurationComplete with transaction_id={}",
      request.target_ue_index,
      request.source_ue_index,
      name(),
      request.transaction_id);

  // Wait for RRCReconfigurationComplete on this target UE.
  // The UE sends this after evaluating the CHO condition and attaching to this cell.
  CORO_AWAIT_VALUE(reconf_result,
                   target_ue->get_rrc_ue()->handle_handover_reconfiguration_complete_expected(
                       request.transaction_id, request.timeout, /*release_on_cancel=*/false));

  if (!reconf_result) {
    logger.debug("target_ue={}: \"{}\" did not receive RRCReconfigurationComplete. CHO to this target failed/canceled",
                 request.target_ue_index,
                 name());
    release_cmd                      = {};
    release_cmd.ue_index             = request.target_ue_index;
    release_cmd.cause                = ngap_cause_radio_network_t::unspecified;
    release_cmd.requires_rrc_message = false;
    CORO_AWAIT_VALUE(release_complete, ue_context_release_handler.handle_ue_context_release_command(release_cmd));
    CORO_EARLY_RETURN();
  }

  logger.debug("target_ue={} source_ue={}: \"{}\" observed RRCReconfigurationComplete",
               request.target_ue_index,
               request.source_ue_index,
               name());

  // Transfer NGAP/E1AP UE context from source to target.
  cu_cp_handler.handle_handover_ue_context_push(request.source_ue_index, request.target_ue_index);

  target_ue = ue_mng.find_du_ue(request.target_ue_index);
  if (target_ue == nullptr) {
    logger.warning("target_ue={}: UE no longer exists after context push", request.target_ue_index);
    CORO_EARLY_RETURN();
  }

  // Activate target user plane with updated UP security keys.
  bearer_ctx_mod_request = request.bearer_context_mod_request;
  if (!fill_bearer_context_security_info(bearer_ctx_mod_request,
                                         target_ue->get_security_manager().get_up_as_config())) {
    CORO_EARLY_RETURN();
  }
  bearer_ctx_mod_request.ue_index = request.target_ue_index;

  CORO_AWAIT_VALUE(bearer_ctx_mod_response,
                   cu_up_db.find_cu_up_processor(target_ue->get_cu_up_index())
                       ->get_e1ap_bearer_context_manager()
                       .handle_bearer_context_modification_request(bearer_ctx_mod_request));
  if (!bearer_ctx_mod_response.success) {
    logger.warning("target_ue={}: CU-UP bearer context modification failed", request.target_ue_index);
    CORO_EARLY_RETURN();
  }

  // Notify target DU that RRC reconfiguration is complete (flushes F1-U UL buffers).
  target_du_context_mod_request.ue_index               = request.target_ue_index;
  target_du_context_mod_request.rrc_recfg_complete_ind = f1ap_rrc_recfg_complete_ind::true_value;
  CORO_AWAIT(du_db.get_du_processor(target_ue->get_du_index())
                 .get_f1ap_handler()
                 .handle_ue_context_modification_request(target_du_context_mod_request));

  // Release source UE context. Must happen after context push so NGAP/E1AP state has been transferred.
  schedule_source_release_on_source_task_sched(request.source_ue_index);

  // Clear winning target UE's CHO context.
  target_ue = ue_mng.find_du_ue(request.target_ue_index);
  if (target_ue != nullptr) {
    target_ue->get_cho_context().reset();
  }

  mobility_mng.get_metrics_handler().aggregate_successful_handover_execution();

  logger.info(
      "target_ue={} source_ue={}: CHO inter-DU completion finalized", request.target_ue_index, request.source_ue_index);

  mobility_mng.trigger_auto_conditional_handover(request.target_ue_index);

  CORO_RETURN();
}

bool conditional_handover_target_routine::fill_bearer_context_security_info(
    e1ap_bearer_context_modification_request& bearer_mod_request,
    const security::sec_as_config&            sec_cfg)
{
  bearer_mod_request.security_info.emplace();
  bearer_mod_request.security_info->security_algorithm.ciphering_algo                 = sec_cfg.cipher_algo;
  bearer_mod_request.security_info->security_algorithm.integrity_protection_algorithm = sec_cfg.integ_algo;

  auto k_enc = byte_buffer::create(sec_cfg.k_enc);
  if (!k_enc.has_value()) {
    logger.warning("source_ue={}: Failed to allocate UP security encryption key", request.source_ue_index);
    return false;
  }
  bearer_mod_request.security_info->up_security_key.encryption_key = std::move(k_enc.value());

  if (sec_cfg.k_int.has_value()) {
    auto k_int = byte_buffer::create(sec_cfg.k_int.value());
    if (!k_int.has_value()) {
      logger.warning("source_ue={}: Failed to allocate UP security integrity key", request.source_ue_index);
      return false;
    }
    bearer_mod_request.security_info->up_security_key.integrity_protection_key = std::move(k_int.value());
  }

  return true;
}

void conditional_handover_target_routine::schedule_source_release_on_source_task_sched(cu_cp_ue_index_t source_ue_index)
{
  auto* src_ue = ue_mng.find_du_ue(source_ue_index);
  if (src_ue == nullptr) {
    logger.warning("target_ue={}: source_ue={} already removed", request.target_ue_index, source_ue_index);
    return;
  }
  // Note: Cannot capture [this] as routine may be destroyed before the lambda runs on the source UE's scheduler.
  cu_cp_ue_context_release_command cmd = {};
  cmd.ue_index                         = source_ue_index;
  cmd.cause                            = ngap_cause_radio_network_t::unspecified;
  cmd.requires_rrc_message             = false;
  src_ue->get_task_sched().schedule_async_task(
      launch_async([&handler = ue_context_release_handler, cmd](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        CORO_AWAIT(handler.handle_ue_context_release_command(cmd));
        CORO_RETURN();
      }));
  logger.debug("target_ue={}: scheduled source_ue={} context release", request.target_ue_index, source_ue_index);
}
