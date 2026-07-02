// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_source_handover_preparation_procedure.h"
#include "../xnap_asn1_converters.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/xnap/common.h"
#include "ocudu/asn1/xnap/xnap_ies.h"
#include "ocudu/cu_cp/cu_cp_cho_types.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/security/security_asn1_utils.h"
#include "ocudu/xnap/xnap_message.h"
#include "ocudu/xnap/xnap_types.h"

using namespace ocudu;
using namespace ocucp;
using namespace asn1::xnap;

xnap_source_handover_preparation_procedure::xnap_source_handover_preparation_procedure(
    const xnap_handover_request& request_,
    xnap_ue_context_list&        ue_ctxt_list_,
    xnap_message_notifier&       xnc_notifier_,
    xnap_cu_cp_notifier&         cu_cp_notifier_,
    timer_factory                timers) :
  request(request_),
  ue_ctxt_list(ue_ctxt_list_),
  xnc_notifier(xnc_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  txn_reloc_prep_timer(timers.create_timer())
{
}

void xnap_source_handover_preparation_procedure::operator()(
    coro_context<async_task<xnap_handover_preparation_response>>& ctx)
{
  CORO_BEGIN(ctx);

  ue_ctxt = ue_ctxt_list.find(request.ue_index);
  if (ue_ctxt == nullptr) {
    ocudulog::fetch_basic_logger("XNAP").error(
        "ue={}: \"{}\" failed. Cause: UE context not found", request.ue_index, name());
    CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
  }

  ue_ctxt->logger.log_info("\"{}\" started...", name());

  if (ue_ctxt->ue_ids.local_xnap_ue_id == local_xnap_ue_id_t::invalid) {
    ue_ctxt->logger.log_error("\"{}\" failed. Cause: Invalid LOCAL XNAP UE ID", name());
    CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
  }

  if (request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list.empty()) {
    ue_ctxt->logger.log_error("\"{}\" failed. Cause: PDU session list is empty", name());
    CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
  }

  // Subscribe to respective publisher to receive HANDOVER REQUEST ACK/HANDOVER PREPARATION FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt->xn_handover_outcome, txn_reloc_prep_ms);

  // Send Handover Request to XN-C peer.
  if (!send_handover_request()) {
    ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Could not send Handover Request", name());
    CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
  }

  CORO_AWAIT(transaction_sink);

  if (!transaction_sink.successful()) {
    if (transaction_sink.timeout_expired()) {
      ue_ctxt->logger.log_warning(
          "\"{}\" failed. Cause: Timeout receiving Handover Request ACK/Handover Preparation Failure after {}ms",
          name(),
          txn_reloc_prep_ms.count());
      // Initialize Handover Cancellation procedure.
      if (!send_handover_cancel()) {
        ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Could not send Handover Cancel", name());
        CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
      }

      CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
    }

    if (transaction_sink.failed()) {
      ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Received Handover Preparation Failure", name());
      CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
    }

    // Neither a HandoverPreparationFailure nor a timeout, e.g. the transaction was cancelled because XNAP is
    // stopping.
    ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Transaction cancelled", name());
    CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
  }

  // Set Target XNAP UE ID.
  ue_ctxt_list.update_peer_xnap_ue_id(
      ue_ctxt->ue_ids.local_xnap_ue_id,
      uint_to_peer_xnap_ue_id(transaction_sink.response()->target_ng_ra_nnode_ue_xn_ap_id));

  if (!request.is_conditional_handover) {
    // Immediate HO: forward RRC Handover Command to DU Processor.
    CORO_AWAIT_VALUE(
        rrc_reconfig_success,
        cu_cp_notifier.on_new_rrc_handover_command(
            request.ue_index, transaction_sink.response()->target2_source_ng_ra_nnode_transp_container.copy()));
    if (!rrc_reconfig_success) {
      ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Received invalid Handover Command", name());
      CORO_EARLY_RETURN(xnap_handover_preparation_response{});
    }

    // Forward procedure result to DU manager.
    response.success = true;
  } else {
    // CHO: return the pre-packed RRC bytes and peer UE ID to the coordinator.
    // Execution is deferred until the UE satisfies the CHO conditions.
    auto packed_rrc = transaction_sink.response()->target2_source_ng_ra_nnode_transp_container.copy();
    if (packed_rrc.empty()) {
      ue_ctxt->logger.log_warning("\"{}\" failed. Cause: Empty RRC container in HandoverRequest Ack (CHO)", name());
      CORO_EARLY_RETURN(xnap_handover_preparation_response{});
    }

    response.success          = true;
    response.packed_rrc_recfg = std::move(packed_rrc);
    response.peer_xnap_ue_id  = uint_to_peer_xnap_ue_id(transaction_sink.response()->target_ng_ra_nnode_ue_xn_ap_id);
  }

  ue_ctxt->logger.log_info("\"{}\" finished successfully", name());

  CORO_RETURN(response);
}

bool xnap_source_handover_preparation_procedure::send_handover_request()
{
  xnap_message msg = {};
  // Set XNAP PDU contents.
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_HO_PREP);
  ho_request_s& ho_request = msg.pdu.init_msg().value.ho_request();

  // Fill XNAP UE ID.
  // This is sent from the source to the target, so the source UE ID is the local XNAP UE ID.
  ho_request->source_ng_ra_nnode_ue_xn_ap_id = local_xnap_ue_id_to_uint(ue_ctxt->ue_ids.local_xnap_ue_id);

  // Fill cause.
  ho_request->cause.set_radio_network() = cause_radio_network_layer_opts::ho_desirable_for_radio_reasons;

  // Fill target cell global ID.
  ho_request->target_cell_global_id.set_nr() = cgi_to_asn1(request.nr_cgi);

  // Fill GUAMI.
  ho_request->guami = guami_to_asn1(request.guami);

  // Fill Context information.
  auto& asn1_ue_context_info = ho_request->ue_context_info_ho_request;
  // > Fill NG-C UE associated signalling reference.
  asn1_ue_context_info.ng_c_ue_ref = request.ue_context_info_ho_request.amf_ue_id;
  // > Fill AMF address.
  asn1_ue_context_info.cp_tnl_info_source.set_endpoint_ip_address();
  tla_to_asn1_bitstring(asn1_ue_context_info.cp_tnl_info_source.endpoint_ip_address(),
                        request.ue_context_info_ho_request.amf_addr);
  // > Fill UE security capabilities.
  auto& sec_cap = asn1_ue_context_info.ue_security_cap;
  sec_cap.nr_encyption_algorithms =
      security::supported_algorithms_to_asn1(request.ue_context_info_ho_request.security_context.supported_enc_algos);
  sec_cap.nr_integrity_protection_algorithms =
      security::supported_algorithms_to_asn1(request.ue_context_info_ho_request.security_context.supported_int_algos);
  // > Fill AS security information.
  asn1_ue_context_info.security_info.key_ng_ran_star =
      security::key_to_asn1(request.ue_context_info_ho_request.security_context.k);
  asn1_ue_context_info.security_info.ncc = request.ue_context_info_ho_request.security_context.ncc;
  // > Fill UE aggregated maximum bit rate.
  asn1_ue_context_info.ue_ambr.dl_ue_ambr = request.ue_context_info_ho_request.ue_ambr.dl;
  asn1_ue_context_info.ue_ambr.ul_ue_ambr = request.ue_context_info_ho_request.ue_ambr.ul;
  // > Fill PDU session resource setup list.
  fill_asn1_pdu_session_res_list(asn1_ue_context_info.pdu_session_res_to_be_setup_list);
  // > Fill RRC container (containing HandoverPreparationInformation).
  asn1_ue_context_info.rrc_context = request.ue_context_info_ho_request.rrc_handover_preparation_information.copy();
  // > Fill location reporting information.
  if (request.ue_context_info_ho_request.location_report_info.has_value()) {
    asn1_ue_context_info.location_report_info_present = true;
    asn1_ue_context_info.location_report_info =
        location_report_info_to_asn1(request.ue_context_info_ho_request.location_report_info.value());
  }

  if (request.is_conditional_handover) {
    // Set CHO indication: this is a conditional handover preparation, not an immediate handover.
    ho_request->ch_oinfo_req_present           = true;
    ho_request->ch_oinfo_req.cho_trigger.value = ch_otrigger_opts::cho_initiation;
    // Inform the target how long to keep the prepared UE context active.
    if (request.cho_timeout.count() > 0) {
      const auto duration = std::min(request.cho_timeout, cho_window_duration_max);
      if (duration.count() > 0) {
        ho_request->ch_oinfo_req.ie_exts_present                              = true;
        ho_request->ch_oinfo_req.ie_exts.cho_time_based_info_present          = true;
        ho_request->ch_oinfo_req.ie_exts.cho_time_based_info.cho_ho_win_start = 0;
        ho_request->ch_oinfo_req.ie_exts.cho_time_based_info.cho_ho_win_dur =
            static_cast<uint16_t>(duration.count() / cho_window_duration_step.count());
      }
    }
  }

  // Fill UE history info.
  asn1::xnap::last_visited_cell_item_c last_visited_cell;
  // TODO: Add real data.
  expected<byte_buffer> last_visited_cell_information = make_byte_buffer("0000f11000066c0000800000");
  if (!last_visited_cell_information.has_value()) {
    ue_ctxt->logger.log_warning("Failed to encode last visited cell information");
  }
  last_visited_cell.set_ng_ran_cell() = last_visited_cell_information.value().copy();
  ho_request->ue_history_info.push_back(last_visited_cell);

  // Forward message to XN-C peer.
  if (!xnc_notifier.on_new_message(msg)) {
    ue_ctxt->logger.log_warning("XN-C notifier is not set. Cannot send Handover Request");
    return false;
  }

  // TODO: Notify the CU-CP about the transmission of a handover request.

  return true;
}

bool xnap_source_handover_preparation_procedure::send_handover_cancel()
{
  xnap_message msg = {};
  // Set XNAP PDU contents.
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_XNAP_ID_HO_CANCEL);
  ho_cancel_s& ho_cancel = msg.pdu.init_msg().value.ho_cancel();

  // This is sent from the source to the target, so the source UE ID is the local XNAP UE ID.
  ho_cancel->source_ng_ra_nnode_ue_xn_ap_id = local_xnap_ue_id_to_uint(ue_ctxt->ue_ids.local_xnap_ue_id);

  ho_cancel->cause.set_radio_network() = cause_radio_network_layer_opts::txn_relo_cprep_expiry;

  // Forward message to XN-C peer.
  if (!xnc_notifier.on_new_message(msg)) {
    ue_ctxt->logger.log_warning("XN-C notifier is not set. Cannot send Handover Cancel");
    return false;
  }

  return true;
}

void xnap_source_handover_preparation_procedure::fill_asn1_pdu_session_res_list(
    pdu_session_res_to_be_setup_list_l& pdu_session_res_list)
{
  for (const auto& pdu_session_item : request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list) {
    pdu_session_res_to_be_setup_item_s asn1_pdu_session_item;
    asn1_pdu_session_item.pdu_session_id = pdu_session_id_to_uint(pdu_session_item.pdu_session_id);

    // Fill S-NSSAI.
    asn1_pdu_session_item.s_nssai = s_nssai_to_asn1(pdu_session_item.s_nssai);
    // Fill UL NGU TNL at UPF.
    asn1_pdu_session_item.ul_ng_u_tnl_at_up_f.set_gtp_tunnel();
    asn1_pdu_session_item.ul_ng_u_tnl_at_up_f.gtp_tunnel().gtp_teid.from_number(
        pdu_session_item.ul_ngu_up_tnl_info.gtp_teid.value());
    tla_to_asn1_bitstring(asn1_pdu_session_item.ul_ng_u_tnl_at_up_f.gtp_tunnel().tnl_address,
                          pdu_session_item.ul_ngu_up_tnl_info.tp_address);
    // Fill PDU session type.
    asn1_pdu_session_item.pdu_session_type = pdu_session_type_to_asn1(pdu_session_item.pdu_session_type);

    // Fill QoS flow setup request items.
    for (const auto& qos_flow : pdu_session_item.qos_flow_setup_request_items) {
      qos_flows_to_be_setup_item_s qos_flow_setup_item = {};
      // Set QFI.
      qos_flow_setup_item.qfi = qos_flow_id_to_uint(qos_flow.qos_flow_id);
      // Fill QoS flow level QoS parameters.
      qos_flow_setup_item.qos_flow_level_qos_params =
          qos_flow_level_qos_parameters_to_asn1(qos_flow.qos_flow_level_qos_params);
      asn1_pdu_session_item.qos_flows_to_be_setup_list.push_back(qos_flow_setup_item);
    }

    pdu_session_res_list.push_back(asn1_pdu_session_item);
  }
}
