// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_resume_procedure.h"
#include "rrc_setup_procedure.h"
#include "ue/rrc_asn1_converters.h"
#include "ue/rrc_asn1_helpers.h"
#include "ue/rrc_ue_security_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

rrc_resume_procedure::rrc_resume_procedure(const asn1::rrc_nr::rrc_resume_request_s& request_,
                                           rrc_ue_context_t&                         context_,
                                           rnti_t                                    new_c_rnti_,
                                           const byte_buffer&                        du_to_cu_container_,
                                           rrc_ue_msg4_proc_notifier&                rrc_ue_resume_notifier_,
                                           rrc_ue_setup_proc_notifier&               rrc_ue_setup_notifier_,
                                           rrc_ue_control_message_handler&           srb_notifier_,
                                           rrc_ue_context_update_notifier&           cu_cp_notifier_,
                                           rrc_ue_cu_cp_ue_notifier&                 cu_cp_ue_notifier_,
                                           rrc_ue_event_notifier&                    metrics_notifier_,
                                           rrc_ue_ngap_notifier&                     ngap_notifier_,
                                           rrc_ue_event_manager&                     event_mng_,
                                           rrc_ue_logger&                            logger_,
                                           bool                                      context_retrieval_required_) :
  resume_request(request_),
  context(context_),
  new_c_rnti(new_c_rnti_),
  du_to_cu_container(du_to_cu_container_),
  rrc_ue_resume_notifier(rrc_ue_resume_notifier_),
  rrc_ue_setup_notifier(rrc_ue_setup_notifier_),
  srb_notifier(srb_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  cu_cp_ue_notifier(cu_cp_ue_notifier_),
  metrics_notifier(metrics_notifier_),
  ngap_notifier(ngap_notifier_),
  event_mng(event_mng_),
  logger(logger_),
  context_retrieval_required(context_retrieval_required_)
{
  // The UE supervises the resume with T319, which it started when it sent the RRCResumeRequest and stops on Msg4
  // (TS 38.331 section 5.3.13.2), so waiting for the RRCResumeComplete is guarded against T319. T319 comes from SIB1,
  // so fall back to T311 if the cell did not report it.
  const std::chrono::milliseconds ue_timeout =
      context.cell.timers.t319.count() > 0 ? context.cell.timers.t319 : context.cell.timers.t311;
  procedure_timeout = ue_timeout + context.cfg.rrc_procedure_guard_time_ms;
}

void rrc_resume_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_info("\"{}\" started...", name());

  if (context_retrieval_required) {
    // The context is retrieved from the peer that allocated the I-RNTI (TS 38.423 section 8.2.4) before the UE is
    // answered, because the RRCResume is integrity protected with keys derived from the retrieved KgNB*. The peer
    // verifies the ResumeMAC-I on the way, as it holds the keys the UE computed it with.
    CORO_AWAIT_VALUE(context_retrieval_response,
                     cu_cp_notifier.on_ue_context_retrieval_required(make_context_retrieval_request()));

    if (not context_retrieval_response.success or not transfer_retrieved_context_and_update_keys()) {
      logger.log_info("\"{}\" failed. Cause: UE context could not be retrieved from a peer. Falling back to RRC Setup",
                      name());
      CORO_AWAIT(handle_rrc_resume_fallback());
      CORO_EARLY_RETURN();
    }
  } else if (not verify_and_update_security_context()) {
    // Request UE release due to invalid security context.
    logger.log_warning("\"{}\" failed. Requesting UE context release", name());
    CORO_AWAIT(handle_rrc_resume_failure());
    CORO_EARLY_RETURN();
  }

  // Notify the CU-CP about the resume request.
  request.ue_index   = context.ue_index;
  request.cgi        = context.cell.cgi;
  request.new_c_rnti = new_c_rnti;
  request.cause      = asn1_to_resume_cause(resume_request.rrc_resume_request.resume_cause);
  CORO_AWAIT_VALUE(rrc_resume_context, cu_cp_notifier.on_rrc_resume_request(request));

  if (!rrc_resume_context.success) {
    logger.log_warning("\"{}\" failed. Requesting UE context release", name());
    CORO_AWAIT(handle_rrc_resume_failure());
    CORO_EARLY_RETURN();
  }

  if (request.cause == resume_cause_t::rna_upd) {
    logger.log_debug(
        "ue={}: \"{}\" finished successfully by setting UE to inactive for RNA update", context.ue_index, name());
    CORO_EARLY_RETURN();
  }

  // Accept RRC Resume Request by sending RRC Resume.
  // Note: From this point we should guarantee that a Resume will be performed.

  if (context_retrieval_required) {
    // The UE arrives at an RRC UE this node created for the RRCResumeRequest, so the bearers the RRCResume carries
    // are created here.
    create_retrieved_context_srbs();
  }

  // Create new transaction for RRC Resume.
  transaction = event_mng.transactions.create_transaction(procedure_timeout);

  // Send RRC Resume to UE.
  send_rrc_resume();

  // Await UE response.
  CORO_AWAIT(transaction);

  if (transaction.has_response()) {
    context.state = rrc_state::connected;

    // Notify metrics about successful RRC connection resume.
    metrics_notifier.on_successful_rrc_connection_resume(
        asn1_to_resume_cause(resume_request.rrc_resume_request.resume_cause));

    if (context_retrieval_required) {
      // The UE is served here now, so the DL path is moved over and the peer drops its context
      // (TS 38.300 section 9.2.2.4.1, steps 7 to 9).
      CORO_AWAIT_VALUE(path_switch_success, cu_cp_notifier.on_retrieved_context_path_switch_required());
      if (not path_switch_success) {
        logger.log_warning("\"{}\" failed. Requesting UE context release", name());
        ue_context_release_request.ue_index = context.ue_index;
        ue_context_release_request.cause    = ngap_cause_radio_network_t::unspecified;
        CORO_AWAIT(cu_cp_notifier.on_ue_release_required(ue_context_release_request));
        CORO_EARLY_RETURN();
      }
    }

    for (auto& nas_pdu : context.pending_dl_nas_transport_messages) {
      // If there is a pending DL NAS Transport message, send it to the UE now that it is resumed.
      logger.log_debug("Sending pending DL NAS Transport message to UE after successful resume");
      send_pending_dl_nas(nas_pdu);
    }
    context.pending_dl_nas_transport_messages.clear();

    logger.log_info("\"{}\" finished successfully", name());

  } else {
    logger.log_warning("\"{}\" failed. Cause: Timeout after {}ms", name(), procedure_timeout.count());
  }

  CORO_RETURN();
}

async_task<void> rrc_resume_procedure::handle_rrc_resume_failure()
{
  context.connection_cause = asn1_resume_cause_to_establishment_cause(resume_request.rrc_resume_request.resume_cause);

  // Notify metrics about RRC connection resume followed by network release.
  metrics_notifier.on_rrc_connection_resume_followed_by_network_release(
      asn1_to_resume_cause(resume_request.rrc_resume_request.resume_cause));

  // Request UE Release.
  return launch_async([this](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Reject RRC Resume Request by sending RRC Setup.
    CORO_AWAIT(cu_cp_notifier.on_ue_release_required(
        {.ue_index = context.ue_index, .cause = ngap_cause_radio_network_t::unspecified}));
    CORO_RETURN();
  });
}

rrc_ue_context_retrieval_request rrc_resume_procedure::make_context_retrieval_request() const
{
  rrc_ue_context_retrieval_request retrieval_request{
      .ue_id = rrc_ue_context_retrieval_id_for_resume{.i_rnti = context.remote_resume_context->rrc_resume_id.value(),
                                                      .allocated_c_rnti = new_c_rnti,
                                                      .access_pci       = context.cell.pci},
      .mac_i = to_short_mac_i(resume_request.rrc_resume_request.resume_mac_i.to_number()),
      .target_nci = context.cell.cgi.nci};

  // T319 is the timer the retrieval runs inside: the UE started it when it sent the RRCResumeRequest
  // (TS 38.331 section 5.3.13.2) and stops it only once it receives a Msg4, be it the RRCResume (section 5.3.13.4) or
  // the RRC Setup fallback (section 5.3.3.4). T319 therefore has to cover the whole exchange, not just the retrieval,
  // so only part of it can be spent waiting for the peer; the rest is needed to get Msg4 to the UE before T319 expires.
  // Half is used, mirroring what the reestablishment does with T301. T319 comes from SIB1, so keep the default guard if
  // the cell did not report it.
  if (context.cell.timers.t319.count() > 0) {
    retrieval_request.max_response_time = context.cell.timers.t319 / 2;
  }

  return retrieval_request;
}

bool rrc_resume_procedure::transfer_retrieved_context_and_update_keys()
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

  // SRB1 carries the RRCResume, so it is created before Msg4 is sent.
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index = context.ue_index;
  srb1_msg.srb_id   = srb_id_t::srb1;
  srb_notifier.create_srb(srb1_msg);
  rrc_ue_resume_notifier.on_new_as_security_context(/* security_mode_active */ true);
  rrc_ue_resume_notifier.on_as_security_activated();

  return true;
}

async_task<void> rrc_resume_procedure::handle_rrc_resume_fallback()
{
  context.connection_cause = asn1_resume_cause_to_establishment_cause(resume_request.rrc_resume_request.resume_cause);

  // Notify metrics about the attempted RRC connection resume followed by RRC Setup.
  metrics_notifier.on_attempted_rrc_connection_resume_followed_by_rrc_setup(
      asn1_to_resume_cause(resume_request.rrc_resume_request.resume_cause));

  return launch_async([this](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT(launch_async<rrc_setup_procedure>(context,
                                                 du_to_cu_container,
                                                 rrc_ue_setup_notifier,
                                                 srb_notifier,
                                                 cu_cp_notifier,
                                                 metrics_notifier,
                                                 ngap_notifier,
                                                 event_mng,
                                                 logger,
                                                 false,
                                                 true));
    CORO_RETURN();
  });
}

bool rrc_resume_procedure::verify_and_update_security_context()
{
  // A local resume reuses the AS keys the UE was suspended with. Without a security context there is nothing to verify
  // the ResumeMAC-I against and nothing to derive the refreshed keys from, so the resume is rejected before the key
  // derivation below runs on unselected algorithms.
  if (not cu_cp_ue_notifier.get_security_context().sel_algos.algos_selected) {
    logger.log_warning("Invalid resume request. Cause: UE has no AS security context");
    return false;
  }

  // The ResumeMAC-I is computed with the AS keys of the cell the UE was suspended in, which for a local resume is a
  // cell of this node.
  const bool valid = verify_resume_mac_i(to_short_mac_i(resume_request.rrc_resume_request.resume_mac_i.to_number()),
                                         context.cell.pci,
                                         context.c_rnti,
                                         context.cell.cgi.nci,
                                         cu_cp_ue_notifier.get_security_context(),
                                         logger);
  if (not valid) {
    logger.log_warning("Invalid ResumeMAC-I in resume request. Source pci={}, target cell-id={}, source c-rnti={}",
                       context.cell.pci,
                       context.cell.cgi.nci,
                       context.c_rnti);
  }

  // Update the security keys and reestablish the SRBs. This must be done directly after validating the RRC Resume
  // Request. Even if it fails, the security context must be updated, as the UE did this after sending the RRC Resume
  // Request.
  update_security_keys();
  reestablish_srbs();

  return valid;
}

void rrc_resume_procedure::update_security_keys()
{
  // Update security keys.
  // freq_and_timing must be present, otherwise the RRC UE would've never been created.
  uint32_t ssb_arfcn = context.cfg.meas_timings.begin()->freq_and_timing.value().carrier_freq;
  cu_cp_ue_notifier.perform_horizontal_key_derivation(context.cell.pci, ssb_arfcn);
  logger.log_debug("Refreshed keys horizontally. pci={} ssb-arfcn_f_ref={}", context.cell.pci, ssb_arfcn);
}

void rrc_resume_procedure::reestablish_srbs()
{
  security::sec_128_as_config sec_cfg = cu_cp_ue_notifier.get_rrc_128_as_config();
  for (srb_id_t srb_id : context.pdcp_manager.get_srb_ids()) {
    context.get_pdcp_notifier(srb_id)->reestablish(sec_cfg);
  }
}

void rrc_resume_procedure::create_retrieved_context_srbs()
{
  if (not rrc_resume_context.radio_bearer_cfg.has_value()) {
    return;
  }

  for (const rrc_srb_to_add_mod& srb : rrc_resume_context.radio_bearer_cfg.value().srb_to_add_mod_list) {
    // SRB1 carries the RRCResume itself and was created when the retrieved context was taken over.
    if (srb.srb_id == srb_id_t::srb1) {
      continue;
    }

    srb_creation_message srb_msg{};
    srb_msg.ue_index = context.ue_index;
    srb_msg.srb_id   = srb.srb_id;
    srb_notifier.create_srb(srb_msg);
  }
}

void rrc_resume_procedure::send_rrc_resume()
{
  asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_resume();

  fill_asn1_rrc_resume_msg(dl_dcch_msg.msg.c1().rrc_resume(), transaction.id(), rrc_resume_context);

  rrc_ue_resume_notifier.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
}

void rrc_resume_procedure::send_pending_dl_nas(byte_buffer& nas_pdu)
{
  asn1::rrc_nr::dl_dcch_msg_s           dl_dcch_msg;
  asn1::rrc_nr::dl_info_transfer_ies_s& dl_info_transfer =
      dl_dcch_msg.msg.set_c1().set_dl_info_transfer().crit_exts.set_dl_info_transfer();
  dl_info_transfer.ded_nas_msg = nas_pdu.copy();

  if (context.pdcp_manager.has_srb(srb_id_t::srb2)) {
    rrc_ue_resume_notifier.on_new_dl_dcch(srb_id_t::srb2, dl_dcch_msg);
  } else {
    rrc_ue_resume_notifier.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
  }
}
