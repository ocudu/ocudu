// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_reestablishment_procedure.h"
#include "rrc_setup_procedure.h"
#include "ue/rrc_asn1_converters.h"
#include "ue/rrc_ue_helpers.h"
#include "ue/rrc_ue_security_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/ran/cu_cp_types.h"

using namespace ocudu;
using namespace ocudu::ocucp;

rrc_reestablishment_procedure::rrc_reestablishment_procedure(const asn1::rrc_nr::rrc_reest_request_s& request_,
                                                             rrc_ue_context_t&                        context_,
                                                             const byte_buffer&              du_to_cu_container_,
                                                             rrc_ue_setup_proc_notifier&     rrc_ue_setup_notifier_,
                                                             rrc_ue_msg4_proc_notifier&      rrc_ue_reest_notifier_,
                                                             rrc_ue_control_message_handler& srb_notifier_,
                                                             rrc_ue_context_update_notifier& cu_cp_notifier_,
                                                             rrc_ue_cu_cp_ue_notifier&       cu_cp_ue_notifier_,
                                                             rrc_ue_event_notifier&          metrics_notifier_,
                                                             rrc_ue_ngap_notifier&           ngap_notifier_,
                                                             rrc_ue_event_manager&           event_mng_,
                                                             rrc_ue_logger&                  logger_) :
  reestablishment_request(request_),
  context(context_),
  du_to_cu_container(du_to_cu_container_),
  rrc_ue_setup_notifier(rrc_ue_setup_notifier_),
  rrc_ue_reest_notifier(rrc_ue_reest_notifier_),
  srb_notifier(srb_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  cu_cp_ue_notifier(cu_cp_ue_notifier_),
  metrics_notifier(metrics_notifier_),
  ngap_notifier(ngap_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
  procedure_timeout = context.cell.timers.t311 + context.cfg.rrc_procedure_guard_time_ms;
}

void rrc_reestablishment_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_info("\"{}\" for old c-rnti={}, pci={} started...",
                  name(),
                  to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti),
                  reestablishment_request.rrc_reest_request.ue_id.pci);

  // Verify if we are in conditions for a Reestablishment, or should opt for RRC Setup as fallback.
  local_outcome = is_local_reestablishment_accepted();

  if (local_outcome == local_reestablishment_outcome::context_not_found) {
    // No local UE context matches, but the UE may have come from a peer NG-RAN node that still holds it. Retrieve it
    // over Xn (TS 38.423 section 8.2.4) before answering the UE, as the RRCReestablishment is integrity protected with
    // keys derived from the retrieved KgNB*.
    CORO_AWAIT_VALUE(context_retrieval_response,
                     cu_cp_notifier.on_ue_context_retrieval_required(make_context_retrieval_request()));
  }

  if (local_outcome != local_reestablishment_outcome::accepted and not is_context_retrieved()) {
    if (local_outcome == local_reestablishment_outcome::context_not_found) {
      log_rejected_reestablishment("Old UE context not found locally and could not be retrieved from a peer");
    }
    CORO_AWAIT(handle_rrc_reestablishment_fallback());
    logger.log_info("\"{}\" finished successfully", name());
    CORO_EARLY_RETURN();
  }

  if (not is_context_retrieved()) {
    // Transfer old UE context to new UE context. If it fails, resort to fallback.
    CORO_AWAIT_VALUE(context_transfer_success, cu_cp_notifier.on_ue_transfer_required(old_ue_reest_context.ue_index));
    if (not context_transfer_success) {
      CORO_AWAIT(handle_rrc_reestablishment_fallback());
      logger.log_info("\"{}\" for old_ue={} finished successfully", name(), old_ue_reest_context.ue_index);
      CORO_EARLY_RETURN();
    }
  }

  // Accept RRC Reestablishment Request by sending RRC Reestablishment.
  // Note: From this point we should guarantee that a Reestablishment will be performed.

  // Transfer reestablishment context and update security keys.
  if (is_context_retrieved()) {
    if (not transfer_retrieved_context_and_update_keys()) {
      CORO_AWAIT(handle_rrc_reestablishment_fallback());
      logger.log_info("\"{}\" finished successfully", name());
      CORO_EARLY_RETURN();
    }
  } else {
    transfer_reestablishment_context_and_update_keys();
  }

  // Create SRB1.
  create_srb1();

  // Create new transaction for RRC Reestablishment.
  transaction = event_mng.transactions.create_transaction(procedure_timeout);

  // Send RRC Reestablishment to UE.
  send_rrc_reestablishment();

  // Enable ciphering.
  // Note: Ciphering needs to be enabled after transmitting the RRC Reestablishment message, as the
  // ReestablishmentComplete will be ciphered.
  enable_srb1_ciphering();

  // Await UE response.
  CORO_AWAIT(transaction);

  if (transaction.has_response()) {
    context.state = rrc_state::connected;

    // Notify metrics about successful RRC connection reestablishment.
    metrics_notifier.on_successful_rrc_connection_reestablishment();
    metrics_notifier.on_new_rrc_connection();

    // Notify DU Processor to start a Reestablishment Context Modification Routine.
    CORO_AWAIT_VALUE(context_modification_success,
                     cu_cp_notifier.on_rrc_reestablishment_context_modification_required());

    // Trigger UE context release at AMF in case of failure.
    if (not context_modification_success) {
      logger.log_info(
          "\"{}\" for old_ue={} failed. Requesting UE context release", name(), old_ue_reest_context.ue_index);
      // Release the old UE.
      ue_context_release_request.ue_index = context.ue_index;
      ue_context_release_request.cause    = ngap_cause_radio_network_t::unspecified;
      CORO_AWAIT(cu_cp_notifier.on_ue_release_required(ue_context_release_request));
    } else {
      logger.log_info("\"{}\" for old_ue={} finished successfully", name(), old_ue_reest_context.ue_index);
    }

  } else {
    logger.log_warning("\"{}\" for old_ue={} failed. Cause: timed out after {}ms",
                       name(),
                       old_ue_reest_context.ue_index,
                       procedure_timeout.count());
  }

  // Notify CU-CP to remove the old UE. A retrieved context has no local old UE; the peer's context is released over Xn
  // once the path has been switched.
  if (not is_context_retrieved()) {
    cu_cp_notifier.on_rrc_reestablishment_complete(old_ue_reest_context.ue_index);
  }

  // Note: From this point the UE is removed and only the stored context can be accessed.

  CORO_RETURN();
}

async_task<void> rrc_reestablishment_procedure::handle_rrc_reestablishment_fallback()
{
  context.connection_cause = establishment_cause_t::mt_access;

  return launch_async([this](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Reject RRC Reestablishment Request by sending RRC Setup.
    CORO_AWAIT(launch_async<rrc_setup_procedure>(context,
                                                 du_to_cu_container,
                                                 rrc_ue_setup_notifier,
                                                 srb_notifier,
                                                 cu_cp_notifier,
                                                 metrics_notifier,
                                                 ngap_notifier,
                                                 event_mng,
                                                 logger,
                                                 true));

    if (old_ue_reest_context.ue_index != cu_cp_ue_index_t::invalid and !old_ue_reest_context.old_ue_fully_attached) {
      // The UE exists but still has not established an SRB2 and DRB. Request the release of the old UE.
      logger.log_debug("old_ue={} was not fully attached yet. Requesting UE context release",
                       old_ue_reest_context.ue_index);
      ue_context_release_request.ue_index = old_ue_reest_context.ue_index;
      ue_context_release_request.cause    = ngap_cause_radio_network_t::unspecified;
      cu_cp_notifier.on_rrc_reestablishment_failure(ue_context_release_request);
    }

    CORO_RETURN();
  });
}

rrc_reestablishment_procedure::local_reestablishment_outcome
rrc_reestablishment_procedure::is_local_reestablishment_accepted()
{
  // Notify the CU-CP about the reestablishment. This will return the old RRC UE context if it exists.
  // Note that this needs to be run before any other sanity check, as it will also cancel an possibly ongoing handover
  // transaction for the old UE.
  old_ue_reest_context =
      cu_cp_notifier.on_rrc_reestablishment_request(reestablishment_request.rrc_reest_request.ue_id.pci,
                                                    to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti));

  if (context.cfg.force_reestablishment_fallback) {
    log_rejected_reestablishment("RRC Reestablishments were disabled by the app configuration");
    return local_reestablishment_outcome::rejected;
  }

  if (reestablishment_request.rrc_reest_request.reest_cause.value == asn1::rrc_nr::reest_cause_opts::recfg_fail) {
    log_rejected_reestablishment("Cannot recover from failed RRC Reconfiguration for old UE");
    return local_reestablishment_outcome::rejected;
  }

  // Check if an old UE context with matching C-RNTI, PCI exists. If not, a peer NG-RAN node may still hold it.
  if (old_ue_reest_context.ue_index == cu_cp_ue_index_t::invalid) {
    return local_reestablishment_outcome::context_not_found;
  }

  if (old_ue_reest_context.reestablishment_ongoing) {
    log_rejected_reestablishment("Old UE is already in reestablishment procedure");
    return local_reestablishment_outcome::rejected;
  }

  // Check if the old UE completed the SRB2 and DRB establishment.
  if (not old_ue_reest_context.old_ue_fully_attached) {
    log_rejected_reestablishment("Old UE bearers were not fully established");
    return local_reestablishment_outcome::rejected;
  }

  // Verify security context.
  if (not verify_security_context()) {
    return local_reestablishment_outcome::rejected;
  }

  return local_reestablishment_outcome::accepted;
}

rrc_ue_context_retrieval_request rrc_reestablishment_procedure::make_context_retrieval_request() const
{
  rrc_ue_context_retrieval_request request{
      .ue_id = rrc_ue_context_retrieval_id_for_reest{.old_pci    = reestablishment_request.rrc_reest_request.ue_id.pci,
                                                     .old_c_rnti = to_rnti(
                                                         reestablishment_request.rrc_reest_request.ue_id.c_rnti)},
      .mac_i = to_short_mac_i(reestablishment_request.rrc_reest_request.ue_id.short_mac_i.to_number()),
      .target_nci = context.cell.cgi.nci};

  // T301 is the timer the retrieval runs inside: the UE stopped T311 and started T301 when it sent the
  // RRCReestablishmentRequest (TS 38.331 section 5.3.7.3), and stops T301 only once it receives a Msg4, be it the
  // RRCReestablishment (section 5.3.7.5) or the RRC Setup fallback (section 5.3.3.4). T301 therefore has to cover the
  // whole exchange, not just the retrieval, so only part of it can be spent waiting for the peer; the rest is needed
  // to get Msg4 to the UE before T301 expires and the UE goes to idle. Half is used, so that the shortest T301
  // (100 ms) gives up early enough for the fallback to still reach the UE. T301 comes from SIB1, so keep the default
  // guard if the cell did not report it.
  if (context.cell.timers.t301.count() > 0) {
    request.max_response_time = context.cell.timers.t301 / 2;
  }

  return request;
}

bool rrc_reestablishment_procedure::verify_security_context()
{
  if (!old_ue_reest_context.sec_context.sel_algos.algos_selected) {
    log_rejected_reestablishment("Old UE does not have valid security context");
    return false;
  }

  return ocucp::verify_short_mac_i(
      to_short_mac_i(reestablishment_request.rrc_reest_request.ue_id.short_mac_i.to_number()),
      reestablishment_request.rrc_reest_request.ue_id.pci,
      to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti),
      context.cell.cgi.nci,
      old_ue_reest_context.sec_context,
      logger);
}

void rrc_reestablishment_procedure::transfer_reestablishment_context_and_update_keys()
{
  // Store capabilities if available.
  if (old_ue_reest_context.capabilities_list.has_value()) {
    context.capabilities_list = ue_cap_rat_container_list_to_asn1(old_ue_reest_context.capabilities_list.value());

    // Store parsed capabilities.
    std::optional<rrc_ue_capabilities_t> caps = get_capabilities(context.capabilities_list.value(), logger);
    if (caps.has_value()) {
      context.capabilities = caps.value();
    }
  }

  // Transfer UP context from old UE.
  cu_cp_notifier.on_up_context_setup_required(old_ue_reest_context.up_ctx);

  // Update security keys.
  // freq_and_timing must be present, otherwise the RRC UE would've never been created.
  uint32_t ssb_arfcn = context.cfg.meas_timings.begin()->freq_and_timing.value().carrier_freq;
  cu_cp_ue_notifier.update_security_context(old_ue_reest_context.sec_context);
  cu_cp_ue_notifier.perform_horizontal_key_derivation(context.cell.pci, ssb_arfcn);
  logger.log_debug("Refreshed keys horizontally. pci={} ssb-arfcn_f_ref={}", context.cell.pci, ssb_arfcn);
}

bool rrc_reestablishment_procedure::transfer_retrieved_context_and_update_keys()
{
  // Store the UE capabilities and the AS configuration the UE had at the peer, which the peer transfers packed inside
  // the RRC container.
  if (not context_retrieval_response.rrc_context.empty()) {
    if (not srb_notifier.handle_rrc_handover_preparation_info(context_retrieval_response.rrc_context.copy())) {
      logger.log_warning("Couldn't store the UE capabilities transferred by the peer");
    }
  }

  // Take over the keys as the peer derived them: it derived KgNB* for this node's cell when it answered the
  // retrieval (TS 33.501 section 6.11), which is the key the UE computed as well. The algorithms the UE protects with
  // come from the AS-Config in the same container.
  if (not cu_cp_ue_notifier.init_retrieved_security_context(
          context_retrieval_response.sec_context,
          get_as_security_algorithms(context_retrieval_response.rrc_context, logger))) {
    logger.log_warning("Could not initialize the security context retrieved from the peer");
    return false;
  }
  logger.log_debug("Initialized the security context retrieved from the peer. pci={}", context.cell.pci);
  return true;
}

void rrc_reestablishment_procedure::create_srb1()
{
  // Create SRB1.
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index     = context.ue_index;
  srb1_msg.old_ue_index = old_ue_reest_context.ue_index;
  srb1_msg.srb_id       = srb_id_t::srb1;
  srb1_msg.pdcp_cfg     = {}; // TODO: Get SRB1 PDCP config of the old UE context.
  srb_notifier.create_srb(srb1_msg);

  // Activate SRB1 PDCP security.
  rrc_ue_reest_notifier.on_new_as_security_context(/* security_mode_active */ true);
  rrc_ue_reest_notifier.on_as_security_activated();
}

void rrc_reestablishment_procedure::send_rrc_reestablishment()
{
  asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_reest();
  asn1::rrc_nr::rrc_reest_s& rrc_reest = dl_dcch_msg.msg.c1().rrc_reest();
  rrc_reest.rrc_transaction_id         = transaction.id();
  rrc_reest.crit_exts.set_rrc_reest();

  rrc_ue_reest_notifier.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
}

void rrc_reestablishment_procedure::enable_srb1_ciphering()
{
  if (!context.pdcp_manager.has_srb(srb_id_t::srb1)) {
    logger.log_error("Could not enable ciphering for SRB1. Cause: SRB1 not found in the UE context");
    return;
  }

  security::sec_128_as_config sec_cfg = cu_cp_ue_notifier.get_rrc_128_as_config();
  rrc_ue_pdcp_notifier&       srb1    = *context.get_pdcp_notifier(srb_id_t::srb1);
  srb1.enable_rx_security(security::integrity_enabled::on, security::ciphering_enabled::on, sec_cfg);
  srb1.enable_tx_security(security::integrity_enabled::on, security::ciphering_enabled::on, sec_cfg);
}

void rrc_reestablishment_procedure::log_rejected_reestablishment(const char* cause_str)
{
  logger.log_info(
      "Rejecting RRC Reestablishment to old UE c-rnti={}, pci={}. Cause: {}. Fallback to RRC Setup Procedure...",
      to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti),
      reestablishment_request.rrc_reest_request.ue_id.pci,
      cause_str);
}
