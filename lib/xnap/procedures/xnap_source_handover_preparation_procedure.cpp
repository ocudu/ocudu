// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
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
    xnap_ue_context&             ue_ctxt_,
    ho_prep_outcome_t&           ho_outcome_,
    xnap_ue_context_list&        ue_ctxt_list_,
    xnap_message_notifier&       xnc_notifier_,
    xnap_cu_cp_notifier&         cu_cp_notifier_,
    timer_factory                timers) :
  request(request_),
  ue_ctxt_list(ue_ctxt_list_),
  xnc_notifier(xnc_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  txn_reloc_prep_timer(timers.create_timer()),
  ho_outcome(ho_outcome_),
  ue_ids(ue_ctxt_.ue_ids),
  logger(ue_ctxt_.logger)
{
}

void xnap_source_handover_preparation_procedure::operator()(
    coro_context<async_task<xnap_handover_preparation_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_info("\"{}\" started...", name());

  if (ue_ids.local_xnap_ue_id == local_xnap_ue_id_t::invalid) {
    logger.log_error("\"{}\" failed. Cause: Invalid LOCAL XNAP UE ID", name());
    discard_cho_cell_prep();
    CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
  }

  if (request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list.empty()) {
    logger.log_error("\"{}\" failed. Cause: PDU session list is empty", name());
    discard_cho_cell_prep();
    CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
  }

  // Subscribe to respective publisher to receive HANDOVER REQUEST ACK/HANDOVER PREPARATION FAILURE message. This is
  // the last use of ho_outcome: resolving the preparation drops the event source, and the subscription is all this
  // procedure needs from it afterwards.
  transaction_sink.subscribe_to(ho_outcome, txn_reloc_prep_ms);

  // Send Handover Request to Xn-C peer.
  if (!send_handover_request()) {
    logger.log_warning("\"{}\" failed. Cause: Could not send Handover Request", name());
    discard_cho_cell_prep();
    CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
  }

  CORO_AWAIT(transaction_sink);

  if (!transaction_sink.successful()) {
    if (transaction_sink.timeout_expired()) {
      logger.log_warning(
          "\"{}\" failed. Cause: Timeout receiving Handover Request ACK/Handover Preparation Failure after {}ms",
          name(),
          txn_reloc_prep_ms.count());
      discard_cho_cell_prep();
      // Initialize Handover Cancellation procedure.
      if (!send_handover_cancel()) {
        logger.log_warning("\"{}\" failed. Cause: Could not send Handover Cancel", name());
        CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
      }

      CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
    }

    if (transaction_sink.failed()) {
      logger.log_warning("\"{}\" failed. Cause: Received Handover Preparation Failure", name());
      discard_cho_cell_prep();
      CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
    }

    // Neither a HandoverPreparationFailure nor a timeout, e.g. the transaction was cancelled because XNAP is
    // stopping. The UE context is being torn down, so do not touch it here.
    logger.log_warning("\"{}\" failed. Cause: Transaction cancelled", name());
    CORO_EARLY_RETURN(xnap_handover_preparation_response{false});
  }

  peer_xnap_ue_id = uint_to_peer_xnap_ue_id(transaction_sink.response()->target_ng_ra_nnode_ue_xn_ap_id);

  if (!request.is_conditional_handover) {
    // Set Target XNAP UE ID.
    ue_ctxt_list.update_peer_xnap_ue_id(ue_ids.local_xnap_ue_id, peer_xnap_ue_id);

    // Immediate HO: forward RRC Handover Command to DU Processor.
    ho_command.ue_index      = request.ue_index;
    ho_command.rrc_container = transaction_sink.response()->target2_source_ng_ra_nnode_transp_container.copy();
    // Report the forwarding tunnels the target allocated, so that the source CU-UP can be pointed at them
    // (TS 38.423 section 9.2.1.19).
    for (const auto& asn1_admitted_item : transaction_sink.response()->pdu_session_res_admitted_list) {
      if (not asn1_admitted_item.pdu_session_res_admitted_info.data_forwarding_info_from_target_present) {
        continue;
      }
      ho_command.data_forwarding_info_from_target.emplace(
          uint_to_pdu_session_id(asn1_admitted_item.pdu_session_id),
          asn1_to_data_forwarding_info_from_target(
              asn1_admitted_item.pdu_session_res_admitted_info.data_forwarding_info_from_target));
    }
    CORO_AWAIT_VALUE(rrc_reconfig_success, cu_cp_notifier.on_new_rrc_handover_command(std::move(ho_command)));
    if (!rrc_reconfig_success) {
      logger.log_warning("\"{}\" failed. Cause: Received invalid Handover Command", name());
      CORO_EARLY_RETURN(xnap_handover_preparation_response{});
    }

    // Forward procedure result to DU manager.
    response.success = true;
  } else {
    // CHO: return the pre-packed RRC bytes and peer UE ID to the coordinator.
    // Execution is deferred until the UE satisfies the CHO conditions.
    // TODO: Carry the data forwarding tunnels of this candidate to the CHO execution, so that the source CU-UP can be
    // pointed at the tunnels of the winning target (TS 38.423 section 9.2.1.19).
    auto packed_rrc = transaction_sink.response()->target2_source_ng_ra_nnode_transp_container.copy();
    if (packed_rrc.empty()) {
      logger.log_warning("\"{}\" failed. Cause: Empty RRC container in HandoverRequest Ack (CHO)", name());
      discard_cho_cell_prep();
      CORO_EARLY_RETURN(xnap_handover_preparation_response{});
    }

    // Record the candidate so that it can later be cancelled, or reported as the winner, by target cell.
    ue_ctxt_list.mark_cho_prepared(ue_ids.local_xnap_ue_id, request.nr_cgi, peer_xnap_ue_id);

    response.success          = true;
    response.packed_rrc_recfg = std::move(packed_rrc);
    response.peer_xnap_ue_id  = peer_xnap_ue_id;
  }

  logger.log_info("\"{}\" finished successfully", name());

  CORO_RETURN(response);
}

void xnap_source_handover_preparation_procedure::discard_cho_cell_prep()
{
  if (request.is_conditional_handover) {
    ue_ctxt_list.remove_cho_cell_prep(ue_ids.local_xnap_ue_id, request.nr_cgi);
  }
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
  ho_request->source_ng_ra_nnode_ue_xn_ap_id = to_underlying(ue_ids.local_xnap_ue_id);

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
    logger.log_warning("Failed to encode last visited cell information");
  }
  last_visited_cell.set_ng_ran_cell() = last_visited_cell_information.value().copy();
  ho_request->ue_history_info.push_back(last_visited_cell);

  // Forward message to Xn-C peer.
  if (!xnc_notifier.on_new_message(msg)) {
    logger.log_warning("Cannot send Handover Request");
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
  ho_cancel->source_ng_ra_nnode_ue_xn_ap_id = to_underlying(ue_ids.local_xnap_ue_id);

  ho_cancel->cause.set_radio_network() = cause_radio_network_layer_opts::txn_relo_cprep_expiry;

  if (request.is_conditional_handover) {
    // TS 38.423 Section 8.2.3.2: the Target Cells to Cancel IE scopes the cancellation to the named candidates.
    // Without it the target releases the whole UE-associated signalling connection, taking the sibling CHO
    // candidates that share this Source NG-RAN node UE XnAP ID with it.
    ho_cancel->target_cells_to_cancel_present = true;
    target_cell_list_item_s cell_item;
    cell_item.target_cell.set_nr() = cgi_to_asn1(request.nr_cgi);
    ho_cancel->target_cells_to_cancel.push_back(cell_item);
  }

  // Forward message to Xn-C peer.
  if (!xnc_notifier.on_new_message(msg)) {
    logger.log_warning("Cannot send Handover Cancel");
    return false;
  }

  return true;
}

void xnap_source_handover_preparation_procedure::fill_asn1_pdu_session_res_list(
    pdu_session_res_to_be_setup_list_l& pdu_session_res_list)
{
  for (const auto& pdu_session_item : request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list) {
    pdu_session_res_to_be_setup_item_s asn1_pdu_session_item;
    asn1_pdu_session_item.pdu_session_id = to_underlying(pdu_session_item.pdu_session_id);

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
      qos_flow_setup_item.qfi = to_underlying(qos_flow.qos_flow_id);
      // Fill QoS flow level QoS parameters.
      qos_flow_setup_item.qos_flow_level_qos_params =
          qos_flow_level_qos_parameters_to_asn1(qos_flow.qos_flow_level_qos_params);
      asn1_pdu_session_item.qos_flows_to_be_setup_list.push_back(qos_flow_setup_item);
    }

    // Fill the Data Forwarding and Offloading Info from source NG-RAN node IE (TS 38.423 Section 9.2.1.17). The IE
    // mandates a non-empty QoS Flows To Be Forwarded List, so it is only sent once a flow is proposed.
    if (pdu_session_item.data_forwarding_info_from_source.has_value()) {
      const cu_cp_data_forwarding_info_from_source& forwarding_info =
          pdu_session_item.data_forwarding_info_from_source.value();
      for (const auto& qos_flow_to_be_forwarded : forwarding_info.qos_flows_to_be_forwarded) {
        if (not qos_flow_to_be_forwarded.dl_forwarding.value_or(false)) {
          continue;
        }
        qos_f_lows_to_be_forwarded_item_s asn1_qos_flow_to_be_forwarded;
        asn1_qos_flow_to_be_forwarded.qos_flow_id       = to_underlying(qos_flow_to_be_forwarded.qos_flow_id);
        asn1_qos_flow_to_be_forwarded.dl_dataforwarding = dl_forwarding_opts::dl_forwarding_proposed;
        // The UL Forwarding IE is mandatory and shall be ignored by the target.
        asn1_qos_flow_to_be_forwarded.ul_dataforwarding = ul_forwarding_opts::ul_forwarding_proposed;
        asn1_pdu_session_item.dataforwardinginfofrom_source.qos_flows_to_be_forwarded.push_back(
            asn1_qos_flow_to_be_forwarded);
        asn1_pdu_session_item.dataforwardinginfofrom_source_present = true;
      }
    }

    if (asn1_pdu_session_item.dataforwardinginfofrom_source_present) {
      for (const auto& drbs_to_qos_flows_map_item :
           pdu_session_item.data_forwarding_info_from_source->drbs_to_qos_flows_map_list) {
        drb_to_qos_flow_map_item_s asn1_drb_item;
        asn1_drb_item.drb_id = to_underlying(drbs_to_qos_flows_map_item.drb_id);
        for (const auto& assoc_qos_flow : drbs_to_qos_flows_map_item.associated_qos_flow_list) {
          qos_flow_item_s asn1_qos_flow_item;
          asn1_qos_flow_item.qfi = to_underlying(assoc_qos_flow.qos_flow_id);
          if (assoc_qos_flow.qos_flow_map_ind.has_value()) {
            asn1_qos_flow_item.qos_flow_map_ind_present = true;
            asn1_qos_flow_item.qos_flow_map_ind =
                assoc_qos_flow.qos_flow_map_ind.value() == cu_cp_qos_flow_map_indication::ul
                    ? qos_flow_map_ind_opts::options::ul
                    : qos_flow_map_ind_opts::options::dl;
          }
          asn1_drb_item.qos_flows_list.push_back(asn1_qos_flow_item);
        }
        asn1_pdu_session_item.dataforwardinginfofrom_source.source_drb_to_qos_flow_map.push_back(asn1_drb_item);
      }
    }

    pdu_session_res_list.push_back(asn1_pdu_session_item);
  }
}
