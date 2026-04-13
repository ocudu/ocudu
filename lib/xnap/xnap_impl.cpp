// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_impl.h"
#include "log_helpers.h"
#include "procedures/sn_status_transfer_asn1_helpers.h"
#include "procedures/xn_handover_asn1_helpers.h"
#include "procedures/xn_setup_asn1_helpers.h"
#include "procedures/xn_setup_procedure.h"
#include "procedures/xn_setup_procedure_asn1_helpers.h"
#include "procedures/xnap_sn_status_transfer_procedure.h"
#include "procedures/xnap_source_handover_preparation_procedure.h"
#include "procedures/xnap_target_handover_preparation_procedure.h"
#include "xnap_asn1_converters.h"
#include "xnap_asn1_utils.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/asn1/xnap/xnap.h"
#include "ocudu/asn1/xnap/xnap_ies.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/xnap/xnap_message.h"
#include "ocudu/xnap/xnap_types.h"

using namespace ocudu;
using namespace asn1::xnap;
using namespace ocucp;

xnap_impl::xnap_impl(xnc_peer_index_t          xnc_index_,
                     const xnap_configuration& xnap_cfg_,
                     xnap_cu_cp_notifier&      cu_cp_notifier_,
                     timer_manager&            timers_,
                     task_executor&            ctrl_exec_) :
  logger(ocudulog::fetch_basic_logger("XNAP")),
  ue_ctxt_list(timers_, ctrl_exec_, logger),
  xnc_index(xnc_index_),
  xnap_cfg(xnap_cfg_),
  cu_cp_notifier(cu_cp_notifier_),
  timers(timers_),
  ctrl_exec(ctrl_exec_),
  xn_setup_outcome(timer_factory{timers, ctrl_exec})
{
}

async_task<void> xnap_impl::stop()
{
  // Stop XN setup procedure if in progress.
  xn_setup_outcome.stop();
  return launch_no_op_task();
}

void xnap_impl::handle_message(const xnap_message& msg)
{
  // Run XNAP protocols in Control executor.
  if (not ctrl_exec.execute([this, msg]() {
        log_xnap_pdu(logger, logger.debug.enabled(), true, msg.pdu);
        switch (msg.pdu.type().value) {
          case xn_ap_pdu_c::types_opts::init_msg:
            handle_initiating_message(msg.pdu.init_msg());
            break;
          case xn_ap_pdu_c::types_opts::successful_outcome:
            handle_successful_outcome(msg.pdu.successful_outcome());
            break;
          case xn_ap_pdu_c::types_opts::unsuccessful_outcome:
            handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
            break;
          default:
            logger.error("Invalid PDU type");
            break;
        }
      })) {
    logger.error("Discarding Rx XNAP PDU. Cause: task queue is full");
  }
}

void xnap_impl::remove_ue_context(cu_cp_ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.debug("ue={}: UE context not found", ue_index);
    return;
  }

  ue_ctxt_list.remove_ue_context(ue_index);
}

void xnap_impl::handle_initiating_message(const init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case xnap_elem_procs_o::init_msg_c::types_opts::xn_setup_request:
      handle_xn_setup_request(msg.value.xn_setup_request());
      break;
    case xnap_elem_procs_o::init_msg_c::types_opts::ho_request:
      handle_handover_request(msg.value.ho_request());
      break;
    case xnap_elem_procs_o::init_msg_c::types_opts::ho_cancel:
      handle_handover_cancel(msg.value.ho_cancel());
      break;
    case xnap_elem_procs_o::init_msg_c::types_opts::sn_status_transfer:
      handle_sn_status_transfer(msg.value.sn_status_transfer());
      break;
    case xnap_elem_procs_o::init_msg_c::types_opts::ue_context_release:
      handle_ue_context_release(msg.value.ue_context_release());
      break;
    case xnap_elem_procs_o::init_msg_c::types_opts::ho_success:
      handle_handover_success(msg.value.ho_success());
      break;
    case xnap_elem_procs_o::init_msg_c::types_opts::conditional_ho_cancel:
      handle_conditional_ho_cancel(msg.value.conditional_ho_cancel());
      break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void xnap_impl::handle_successful_outcome(const successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case xnap_elem_procs_o::successful_outcome_c::types_opts::xn_setup_resp: {
      xn_setup_outcome.set(outcome.value.xn_setup_resp());
    } break;
    case xnap_elem_procs_o::successful_outcome_c::types_opts::ho_request_ack: {
      if (auto* ue_ctxt = asn1_utils::get_ue_ctxt_in_ue_assoc_msg(outcome, ue_ctxt_list, logger)) {
        ue_ctxt->xn_handover_outcome.set(outcome.value.ho_request_ack());
      }
    } break;
    default:
      logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void xnap_impl::handle_unsuccessful_outcome(const unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case xnap_elem_procs_o::unsuccessful_outcome_c::types_opts::xn_setup_fail: {
      xn_setup_outcome.set(outcome.value.xn_setup_fail());
    } break;
    case xnap_elem_procs_o::unsuccessful_outcome_c::types_opts::ho_prep_fail: {
      if (auto* ue_ctxt = asn1_utils::get_ue_ctxt_in_ue_assoc_msg(outcome, ue_ctxt_list, logger)) {
        ue_ctxt->xn_handover_outcome.set(outcome.value.ho_prep_fail());
      }
    } break;
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

async_task<bool> xnap_impl::handle_xn_setup_request_required()
{
  return launch_async<xn_setup_procedure>(
      xnap_cfg, peer_ctxt, tx_notifier, xn_setup_outcome, timer_factory{timers, ctrl_exec}, logger);
}

void xnap_impl::handle_xn_setup_request(const xn_setup_request_s& request)
{
  xnap_message xn_setup_result;

  // Message content validation.
  auto msgerr = validate_xn_setup_request_response(request);
  if (not msgerr.has_value()) {
    logger.warning("Rejecting XN Setup Request. Cause: {}", msgerr.error().second);
    xn_setup_result = generate_asn1_xn_setup_failure(asn1_to_cause(msgerr.error().first));
  } else {
    // Store peer context information.
    peer_ctxt = create_peer_xnap_context(request);
    // Generate XN Setup Response.
    xn_setup_result = generate_asn1_xn_setup_response(xnap_cfg);
  }

  // Transmit XN Setup Response/Failure.
  if (not tx_notifier.on_new_message(xn_setup_result)) {
    logger.error("Failed to send XN Setup Response. Cause: no SCTP association available");
  }
}

void xnap_impl::handle_handover_request(const asn1::xnap::ho_request_s& msg)
{
  // Add lambda that generates and transmits Handover Preparation Failure message.
  auto send_handover_failure = [this](uint64_t peer_xnap_ue_id, xnap_cause_t cause) {
    xnap_message xnap_msg;
    xnap_msg.pdu.set_unsuccessful_outcome();
    xnap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_XNAP_ID_HO_PREP);
    auto& ho_fail = xnap_msg.pdu.unsuccessful_outcome().value.ho_prep_fail();
    // This is sent from the target to the source, so the peer XNAP UE ID is the source UE ID.
    ho_fail->source_ng_ra_nnode_ue_xn_ap_id = peer_xnap_ue_id;
    ho_fail->cause                          = cause_to_asn1(cause);

    if (!tx_notifier.on_new_message(xnap_msg)) {
      logger.warning("XN-C association is not set. Cannot send HandoverFailure");
      return;
    }
    logger.warning("Sending HandoverFailure");
  };

  // Convert Handover Request to common type.
  xnap_handover_request ho_request;
  if (!asn1_to_handover_request(ho_request, msg)) {
    logger.info("Received invalid HandoverRequest");
    send_handover_failure(msg->source_ng_ra_nnode_ue_xn_ap_id,
                          cause_protocol_t::abstract_syntax_error_falsely_constructed_msg);

    return;
  }

  logger.info("HandoverRequest - extracted target cell. plmn={}, target cell_id=0x{:x}",
              ho_request.nr_cgi.plmn_id,
              ho_request.nr_cgi.nci);

  // Create UE in target cell.
  ho_request.ue_index = cu_cp_notifier.request_new_ue_index_allocation(ho_request.nr_cgi, ho_request.guami.plmn);
  if (ho_request.ue_index == cu_cp_ue_index_t::invalid) {
    logger.debug("Couldn't allocate UE index for handover target cell");
    send_handover_failure(msg->source_ng_ra_nnode_ue_xn_ap_id, xnap_cause_misc_t::not_enough_user_plane_processing_res);
    return;
  }

  // Inititialize security context of target UE.
  if (!cu_cp_notifier.on_handover_request_received(
          ho_request.ue_index, ho_request.guami.plmn, ho_request.ue_context_info_ho_request.security_context)) {
    logger.debug("Failed to initialize security context for UE index {}. Rejecting handover request",
                 ho_request.ue_index);
    send_handover_failure(msg->source_ng_ra_nnode_ue_xn_ap_id, xnap_cause_misc_t::not_enough_user_plane_processing_res);
    return;
  }

  if (!cu_cp_notifier.schedule_async_task(ho_request.ue_index,
                                          launch_async<xnap_target_handover_preparation_procedure>(
                                              ho_request,
                                              xnc_index,
                                              uint_to_peer_xnap_ue_id(msg->source_ng_ra_nnode_ue_xn_ap_id),
                                              ue_ctxt_list,
                                              cu_cp_notifier,
                                              tx_notifier,
                                              logger))) {
    logger.debug("Couldn't schedule targer handover preparation procedure");
    send_handover_failure(msg->source_ng_ra_nnode_ue_xn_ap_id, xnap_cause_misc_t::not_enough_user_plane_processing_res);
    return;
  }
}

void xnap_impl::handle_handover_cancel(const asn1::xnap::ho_cancel_s& msg)
{
  // This is sent from the source to the target, so the source XNAP UE ID is the peer UE ID.
  peer_xnap_ue_id_t peer_xnap_ue_id = uint_to_peer_xnap_ue_id(msg->source_ng_ra_nnode_ue_xn_ap_id);
  if (!ue_ctxt_list.contains(peer_xnap_ue_id)) {
    logger.info("Received HandoverCancel for unknown UE. peer_xnap_ue_id={}", msg->source_ng_ra_nnode_ue_xn_ap_id);
    return;
  }

  cu_cp_ue_index_t ue_index = ue_ctxt_list[peer_xnap_ue_id].ue_ids.ue_index;

  // Request CU-CP to release the UE context.
  cu_cp_notifier.on_handover_cancel_received(ue_index);

  // Remove local UE context.
  ue_ctxt_list.remove_ue_context(ue_index);
}

void xnap_impl::handle_sn_status_transfer(const asn1::xnap::sn_status_transfer_s& msg)
{
  // This is sent from the source to the target, so the source XNAP UE ID is the peer UE ID and the target XNAP UE ID is
  // the local UE ID.
  peer_xnap_ue_id_t peer_xnap_ue_id = uint_to_peer_xnap_ue_id(msg->source_ng_ra_nnode_ue_xn_ap_id);

  if (!ue_ctxt_list.contains(peer_xnap_ue_id)) {
    logger.warning("peer_xnap_ue={} local_xnap_ue={}: Dropping SNStatusTransfer. UE context does not exist",
                   msg->source_ng_ra_nnode_ue_xn_ap_id,
                   msg->target_ng_ra_nnode_ue_xn_ap_id);
    return;
  }

  xnap_ue_context& ue_ctxt = ue_ctxt_list[peer_xnap_ue_id];

  ue_ctxt.sn_status_transfer_outcome.set(msg);
}

void xnap_impl::handle_ue_context_release(const asn1::xnap::ue_context_release_s& msg)
{
  // This is sent from the target to the source, so the source XNAP UE ID is the local UE ID and the target XNAP UE ID
  // is the peer UE ID.
  local_xnap_ue_id_t local_xnap_ue_id = uint_to_local_xnap_ue_id(msg->source_ng_ra_nnode_ue_xn_ap_id);

  if (!ue_ctxt_list.contains(local_xnap_ue_id)) {
    logger.warning("local_xnap_ue={} peer_xnap_ue={}: Dropping UEContextRelease. UE context does not exist",
                   msg->source_ng_ra_nnode_ue_xn_ap_id,
                   msg->target_ng_ra_nnode_ue_xn_ap_id);
    return;
  }

  xnap_ue_context& ue_ctxt = ue_ctxt_list[local_xnap_ue_id];

  // Request CU-CP to release the UE context.
  cu_cp_notifier.on_ue_context_release_received(ue_ctxt.ue_ids.ue_index);
}

async_task<xnap_handover_preparation_response>
xnap_impl::handle_handover_request_required(const xnap_handover_request& request)
{
  if (!ue_ctxt_list.contains(request.ue_index)) {
    // Allocate new local XNAP UE context if it doesn't exist.
    local_xnap_ue_id_t local_xnap_ue_id = ue_ctxt_list.allocate_local_xnap_ue_id();
    if (local_xnap_ue_id == local_xnap_ue_id_t::invalid) {
      logger.error("Failed to allocate XNAP UE ID for ue={}. Cannot transmit HandoverPreparationRequest",
                   request.ue_index);
      return launch_no_op_task(xnap_handover_preparation_response{false});
    }
    ue_ctxt_list.add_ue(request.ue_index, local_xnap_ue_id);
  }

  ue_ctxt_list[request.ue_index].logger.log_debug("Starting HO source preparation");

  return launch_async<xnap_source_handover_preparation_procedure>(
      request, ue_ctxt_list, tx_notifier, cu_cp_notifier, timer_factory{timers, ctrl_exec});
}

void xnap_impl::handle_cho_cancel_required(cu_cp_ue_index_t ue_index, const nr_cell_global_id_t& target_cgi)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: Cannot send HandoverCancel (CHO non-winner): UE context not found", ue_index);
    return;
  }

  xnap_ue_context&  ue_ctxt    = ue_ctxt_list[ue_index];
  peer_xnap_ue_id_t peer_ue_id = ue_ctxt.ue_ids.peer_xnap_ue_id;

  if (peer_ue_id == peer_xnap_ue_id_t::invalid) {
    ue_ctxt.logger.log_warning("HandoverCancel (CHO non-winner) skipped: peer XNAP UE ID is invalid");
  } else {
    // TS 38.423 Section 8.2.3: source sends HANDOVER CANCEL to release a non-winning prepared context at the target.
    // Include TargetCellsToCancel IE to identify the specific candidate cell being cancelled.
    xnap_message msg = {};
    msg.pdu.set_init_msg();
    msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_HO_CANCEL);
    ho_cancel_s& ho_cancel                    = msg.pdu.init_msg().value.ho_cancel();
    ho_cancel->source_ng_ra_nnode_ue_xn_ap_id = local_xnap_ue_id_to_uint(ue_ctxt.ue_ids.local_xnap_ue_id);
    ho_cancel->cause.set_radio_network()      = cause_radio_network_layer_opts::proc_cancelled;
    if (ue_ctxt.ue_ids.peer_xnap_ue_id != peer_xnap_ue_id_t::invalid) {
      ho_cancel->target_ng_ra_nnode_ue_xn_ap_id_present = true;
      ho_cancel->target_ng_ra_nnode_ue_xn_ap_id         = peer_xnap_ue_id_to_uint(ue_ctxt.ue_ids.peer_xnap_ue_id);
    }
    ho_cancel->target_cells_to_cancel_present = true;
    asn1::xnap::target_cell_list_item_s cell_item;
    cell_item.target_cell.set_nr() = cgi_to_asn1(target_cgi);
    ho_cancel->target_cells_to_cancel.push_back(cell_item);

    if (!tx_notifier.on_new_message(msg)) {
      ue_ctxt.logger.log_warning("HandoverCancel (CHO non-winner): XN-C notifier not set, message not sent");
    } else {
      ue_ctxt.logger.log_debug("HandoverCancel sent to release non-winning CHO target");
    }
  }

  // Release the local XNAP UE context for this candidate.
  ue_ctxt_list.remove_ue_context(ue_index);
}

void xnap_impl::handle_handover_success_required(cu_cp_ue_index_t ue_index, const nr_cell_global_id_t& cgi)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: Cannot send HandoverSuccess: UE context not found", ue_index);
    return;
  }

  xnap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  xnap_message xnap_msg = {};
  xnap_msg.pdu.set_init_msg();
  xnap_msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_HO_SUCCESS);

  ho_success_s& ho_success = xnap_msg.pdu.init_msg().value.ho_success();
  // HandoverSuccess: target -> source. source_id is the peer (source) XNAP UE ID; target_id is the local XNAP UE ID.
  ho_success->source_ng_ra_nnode_ue_xn_ap_id           = peer_xnap_ue_id_to_uint(ue_ctxt.ue_ids.peer_xnap_ue_id);
  ho_success->target_ng_ra_nnode_ue_xn_ap_id           = local_xnap_ue_id_to_uint(ue_ctxt.ue_ids.local_xnap_ue_id);
  ho_success->requested_target_cell_global_id.set_nr() = cgi_to_asn1(cgi);

  if (!tx_notifier.on_new_message(xnap_msg)) {
    ue_ctxt.logger.log_warning("XN-C association is not set. Cannot send HandoverSuccess");
    return;
  }
}

void xnap_impl::handle_sn_status_transfer_required(const cu_cp_status_transfer& sn_status_transfer)
{
  const cu_cp_ue_index_t ue_index = sn_status_transfer.ue_index;
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: Cannot transmit SNStatusTransfer. UE context does not exist", ue_index);
    return;
  }

  xnap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  xnap_message xnap_msg = {};
  xnap_msg.pdu.set_init_msg();
  xnap_msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_S_N_STATUS_TRANSFER);

  sn_status_transfer_s& asn1_sn_status = xnap_msg.pdu.init_msg().value.sn_status_transfer();
  // This is sent from the source to the target, so the source XNAP UE ID is the local UE ID and the target XNAP UE ID
  // is the peer UE ID.
  asn1_sn_status->source_ng_ra_nnode_ue_xn_ap_id = local_xnap_ue_id_to_uint(ue_ctxt.ue_ids.local_xnap_ue_id);
  asn1_sn_status->target_ng_ra_nnode_ue_xn_ap_id = peer_xnap_ue_id_to_uint(ue_ctxt.ue_ids.peer_xnap_ue_id);

  sn_status_transfer_to_asn1(asn1_sn_status, sn_status_transfer.drbs_subject_to_status_transfer_list);

  // Forward message to XN-C peer CU-CP.
  if (!tx_notifier.on_new_message(xnap_msg)) {
    ue_ctxt.logger.log_warning("XN-C association is not set. Cannot send SNStatusTransfer");
    return;
  }
}

async_task<expected<cu_cp_status_transfer>> xnap_impl::handle_sn_status_transfer_expected(cu_cp_ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: Cannot await SNStatusTransfer. UE context does not exist", ue_index);
    expected<cu_cp_status_transfer> ret = make_unexpected(default_error_t{});
    return launch_no_op_task(ret);
  }

  xnap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];
  return launch_async<xnap_sn_status_transfer_procedure>(
      xnap_cfg.procedure_timeout, ue_ctxt.sn_status_transfer_outcome, ue_ctxt.logger);
}

void xnap_impl::handle_handover_success(const asn1::xnap::ho_success_s& msg)
{
  // HandoverSuccess is sent from the target to the source. From the source's perspective,
  // Source NG-RAN node UE XnAP ID is our own (local) XNAP UE ID set in the HandoverRequest.
  local_xnap_ue_id_t local_xnap_ue_id = uint_to_local_xnap_ue_id(msg->source_ng_ra_nnode_ue_xn_ap_id);
  if (!ue_ctxt_list.contains(local_xnap_ue_id)) {
    logger.warning("Received HandoverSuccess for unknown local_xnap_ue_id={}", local_xnap_ue_id);
    return;
  }

  cu_cp_ue_index_t ue_index = ue_ctxt_list[local_xnap_ue_id].ue_ids.ue_index;

  // Target NG-RAN node UE XnAP ID is the target's local XNAP UE ID, stored at the source as peer_xnap_ue_id.
  peer_xnap_ue_id_t winner_peer_id = uint_to_peer_xnap_ue_id(msg->target_ng_ra_nnode_ue_xn_ap_id);

  // Notify CU-CP: source must now send SN Status Transfer and release UE context.
  cu_cp_notifier.on_handover_success_received(ue_index, winner_peer_id);
}

void xnap_impl::handle_conditional_ho_cancel(const asn1::xnap::conditional_ho_cancel_s& msg)
{
  // ConditionalHandoverCancel is sent from TARGET to SOURCE (TS 38.423 Section 8.2.9).
  // The target self-cancels a prepared CHO context it no longer wants to hold.
  // Source NG-RAN node UE XnAP ID is our local XNAP UE ID (we allocated it when initiating HO).
  // Target NG-RAN node UE XnAP ID is the cancelling target's XNAP UE ID (our peer_xnap_ue_id).
  local_xnap_ue_id_t local_id = uint_to_local_xnap_ue_id(msg->source_ng_ra_nnode_ue_xn_ap_id);
  if (!ue_ctxt_list.contains(local_id)) {
    logger.info("Received ConditionalHandoverCancel for unknown local_xnap_ue_id={}",
                msg->source_ng_ra_nnode_ue_xn_ap_id);
    return;
  }

  cu_cp_ue_index_t ue_index = ue_ctxt_list[local_id].ue_ids.ue_index;

  // Release the XNAP UE context for this preparation link.
  ue_ctxt_list.remove_ue_context(ue_index);
}

bool xnap_impl::handle_ue_context_release_required(cu_cp_ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: Cannot transmit UEContextReleaseRequest. UE context does not exist", ue_index);
    return false;
  }

  xnap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  xnap_message xnap_msg = {};
  xnap_msg.pdu.set_init_msg();
  xnap_msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_U_E_CONTEXT_RELEASE);

  ue_context_release_s& ue_ctxt_release = xnap_msg.pdu.init_msg().value.ue_context_release();
  // This is sent from the target to the source, so the local XNAP UE ID is the target and the peer XNAP UE ID is the
  // source.
  ue_ctxt_release->source_ng_ra_nnode_ue_xn_ap_id = peer_xnap_ue_id_to_uint(ue_ctxt.ue_ids.peer_xnap_ue_id);
  ue_ctxt_release->target_ng_ra_nnode_ue_xn_ap_id = local_xnap_ue_id_to_uint(ue_ctxt.ue_ids.local_xnap_ue_id);

  // Forward message to XN-C peer CU-CP.
  if (!tx_notifier.on_new_message(xnap_msg)) {
    ue_ctxt.logger.log_warning("XN-C association is not set. Cannot send UEContextReleaseRequest");
    return false;
  }

  // Remove UE context locally as well, as no further messages for this UE are expected.
  ue_ctxt_list.remove_ue_context(ue_index);

  return true;
}
