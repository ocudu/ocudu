// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_resume_procedure.h"
#include "ue/rrc_asn1_converters.h"
#include "ue/rrc_asn1_helpers.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/asn1/rrc_nr/nr_ue_variables.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/security/integrity.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

rrc_resume_procedure::rrc_resume_procedure(const asn1::rrc_nr::rrc_resume_request_s& request_,
                                           rrc_ue_context_t&                         context_,
                                           rnti_t                                    new_c_rnti_,
                                           rrc_ue_msg4_proc_notifier&                rrc_ue_resume_notifier_,
                                           rrc_ue_context_update_notifier&           cu_cp_notifier_,
                                           rrc_ue_cu_cp_ue_notifier&                 cu_cp_ue_notifier_,
                                           rrc_ue_event_notifier&                    metrics_notifier_,
                                           rrc_ue_event_manager&                     event_mng_,
                                           rrc_ue_logger&                            logger_) :
  resume_request(request_),
  context(context_),
  new_c_rnti(new_c_rnti_),
  rrc_ue_resume_notifier(rrc_ue_resume_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  cu_cp_ue_notifier(cu_cp_ue_notifier_),
  metrics_notifier(metrics_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
  procedure_timeout = context.cell.timers.t311 + context.cfg.rrc_procedure_guard_time_ms;
}

void rrc_resume_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_info("\"{}\" started...", name());

  // Verify security context.
  if (not verify_and_update_security_context()) {
    // Request UE release due to invalid security context.
    logger.log_warning("Rejecting RRC Resume UE. Cause: UE does not have valid security context");
    logger.log_info("\"{}\" failed. Requesting UE context release", name());
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
    logger.log_info("\"{}\" failed. Requesting UE context release", name());
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

    for (auto& nas_pdu : context.pending_dl_nas_transport_messages) {
      // If there is a pending DL NAS Transport message, send it to the UE now that it is resumed.
      logger.log_debug("Sending pending DL NAS Transport message to UE after successful resume");
      send_pending_dl_nas(nas_pdu);
    }
    context.pending_dl_nas_transport_messages.clear();

    logger.log_info("\"{}\" finished successfully", name());

  } else {
    logger.log_warning("\"{}\" timed out after {}ms", name(), procedure_timeout.count());
    logger.log_info("\"{}\" failed", name());
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

bool rrc_resume_procedure::verify_and_update_security_context()
{
  bool valid = false;

  // Get RX resume MAC.
  security::sec_short_mac_i resume_mac     = {};
  uint16_t                  resume_mac_int = htons(resume_request.rrc_resume_request.resume_mac_i.to_number());
  std::memcpy(resume_mac.data(), &resume_mac_int, 2);

  // Get packed varResumeMAC-Input.
  asn1::rrc_nr::var_resume_mac_input_s var_resume_mac_input = {};
  var_resume_mac_input.source_pci                           = context.cell.pci;
  var_resume_mac_input.target_cell_id.from_number(context.cell.cgi.nci.value());
  var_resume_mac_input.source_c_rnti        = to_value(context.c_rnti);
  byte_buffer   var_resume_mac_input_packed = {};
  asn1::bit_ref bref(var_resume_mac_input_packed);
  var_resume_mac_input.pack(bref);

  logger.log_debug(var_resume_mac_input_packed.begin(),
                   var_resume_mac_input_packed.end(),
                   "Packed varResumeMAC-Input. Source pci={}, target cell-id=0x{:x}, source c-rnti={}",
                   var_resume_mac_input.source_pci,
                   var_resume_mac_input.target_cell_id.to_number(),
                   to_rnti(var_resume_mac_input.source_c_rnti));

  // Verify ResumeMAC-I.
  security::security_context sec_context = cu_cp_ue_notifier.get_security_context();
  if (sec_context.sel_algos.algos_selected) {
    security::sec_as_config source_as_config = sec_context.get_as_config(security::sec_domain::rrc);
    valid = security::verify_short_mac(resume_mac, var_resume_mac_input_packed, source_as_config);
    logger.log_debug("Received RRC resume request. resume_mac_valid={}", valid);
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
  // Reestablish SRBs.
  security::sec_128_as_config sec_cfg = cu_cp_ue_notifier.get_rrc_128_as_config();
  for (auto& [srb_id, srb_ctxt] : context.srbs) {
    srb_ctxt.reestablish(sec_cfg);
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

  if (context.srbs.find(srb_id_t::srb2) != context.srbs.end()) {
    rrc_ue_resume_notifier.on_new_dl_dcch(srb_id_t::srb2, dl_dcch_msg);
  } else {
    rrc_ue_resume_notifier.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
  }
}
