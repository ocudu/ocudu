// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_positioning_activation_procedure.h"
#include "../../f1ap_asn1_utils.h"
#include "asn1_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_ies.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/cause/common.h"
#include "ocudu/ran/cause/f1ap_cause_converters.h"
#include "ocudu/ran/positioning/positioning_activation.h"
#include "ocudu/support/async/coroutine.h"
#include <variant>

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::f1ap;

/// \brief Convert the positioning activation request from common type to ASN.1.
/// \param[out] asn1_request The ASN.1 struct to store the result.
/// \param[in] request The common type positioning activation request.
static bool fill_asn1_positioning_activation_request(asn1::f1ap::positioning_activation_request_s& asn1_request,
                                                     const positioning_activation_request_t&       request);

/// \brief Convert the positioning activation response from ASN.1 to common type.
/// \param[in] asn1_response The ASN.1 type positioning activation response.
/// \returns the common type struct.
static positioning_activation_response_t
fill_positioning_activation_response(const asn1::f1ap::positioning_activation_resp_s& asn1_response);

/// \brief Convert the positioning activation failure from ASN.1 to common type.
/// \param[in] asn1_fail The ASN.1 type positioning activation failure.
/// \returns the common type struct.
static positioning_activation_failure_t
fill_positioning_activation_failure(const asn1::f1ap::positioning_activation_fail_s& asn1_fail);

f1ap_positioning_activation_procedure::f1ap_positioning_activation_procedure(
    const f1ap_configuration&               f1ap_cfg_,
    const positioning_activation_request_t& request_,
    f1ap_ue_context&                        ue_ctxt_,
    f1ap_message_notifier&                  f1ap_notif_,
    ocudulog::basic_logger&                 logger_) :
  f1ap_cfg(f1ap_cfg_), request(request_), ue_ctxt(ue_ctxt_), f1ap_notifier(f1ap_notif_), logger(logger_)
{
}

void f1ap_positioning_activation_procedure::operator()(
    coro_context<async_task<expected<positioning_activation_response_t, positioning_activation_failure_t>>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("{}: Procedure started...", f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()});

  // Subscribe to respective publisher to receive POSITIONING ACTIVATION RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt.ev_mng.positioning_activation_outcome, f1ap_cfg.proc_timeout);

  // Send command to DU.
  if (!send_positioning_activation_request()) {
    logger.warning("{}: Procedure failed. Cause: Positioning Activation Request misses mandatory fields",
                   f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()});
    CORO_EARLY_RETURN(make_unexpected(positioning_activation_failure_t{
        f1ap_to_nrppa_cause(cause_protocol_t::abstract_syntax_error_falsely_constructed_msg)}));
  }

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_positioning_activation_result());
}

bool f1ap_positioning_activation_procedure::send_positioning_activation_request()
{
  ocudu_sanity_check(ue_ctxt.ue_ids.du_ue_f1ap_id && ue_ctxt.ue_ids.du_ue_f1ap_id != gnb_du_ue_f1ap_id_t::invalid,
                     "Invalid gNB-DU-UE-F1AP-Id");

  // Pack message into PDU
  f1ap_message f1ap_pos_info_request_msg;
  f1ap_pos_info_request_msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_POSITIONING_ACTIVATION);
  positioning_activation_request_s& pos_info_req =
      f1ap_pos_info_request_msg.pdu.init_msg().value.positioning_activation_request();
  pos_info_req->gnb_du_ue_f1ap_id = to_underlying(*ue_ctxt.ue_ids.du_ue_f1ap_id);
  pos_info_req->gnb_cu_ue_f1ap_id = to_underlying(ue_ctxt.ue_ids.cu_ue_f1ap_id);

  if (!fill_asn1_positioning_activation_request(pos_info_req, request)) {
    return false;
  }

  // send positioning activation request message
  f1ap_notifier.on_new_message(f1ap_pos_info_request_msg);

  return true;
}

expected<positioning_activation_response_t, positioning_activation_failure_t>
f1ap_positioning_activation_procedure::create_positioning_activation_result()
{
  expected<positioning_activation_response_t, positioning_activation_failure_t> res;

  auto logger_prefix = f1ap_ue_log_prefix{ue_ctxt.ue_ids, name()};

  if (transaction_sink.successful()) {
    const asn1::f1ap::positioning_activation_resp_s& resp = transaction_sink.response();

    logger.info("{}: Procedure finished successfully", logger_prefix);

    res = fill_positioning_activation_response(resp);

  } else if (transaction_sink.failed()) {
    const asn1::f1ap::positioning_activation_fail_s& fail = transaction_sink.failure();

    logger.warning("{}: Procedure failed. Cause: {}", logger_prefix, get_cause_str(fail->cause));

    res = make_unexpected(fill_positioning_activation_failure(fail));
  } else {
    logger.warning(
        "{}: Procedure failed. Cause: Timeout reached for PositioningInformationResponse reception (timeout={}ms)",
        logger_prefix,
        f1ap_cfg.proc_timeout.count());

    res = make_unexpected(positioning_activation_failure_t{f1ap_to_nrppa_cause(cause_misc_t::unspecified)});
  }

  return res;
}

static inline asn1::f1ap::ref_sig_c ref_sig_to_asn1(const spatial_relation_info_t::reference_signal& ref_sig)
{
  asn1::f1ap::ref_sig_c asn1_ref_sig;

  if (std::holds_alternative<nzp_csi_rs_res_id_t>(ref_sig)) {
    asn1_ref_sig.set_nzp_csi_rs() = std::get<nzp_csi_rs_res_id_t>(ref_sig);
  } else if (std::holds_alternative<srs_config::srs_res_id>(ref_sig)) {
    ssb_t              ssb      = std::get<ssb_t>(ref_sig);
    asn1::f1ap::ssb_s& asn1_ssb = asn1_ref_sig.set_ssb();
    asn1_ssb.pci_nr             = ssb.pci_nr;
    if (ssb.ssb_idx.has_value()) {
      asn1_ssb.ssb_idx_present = true;
      asn1_ssb.ssb_idx         = ssb.ssb_idx.value();
    }
  } else if (std::holds_alternative<srs_config::srs_res_id>(ref_sig)) {
    asn1_ref_sig.set_srs() = std::get<srs_config::srs_res_id>(ref_sig);
  } else if (std::holds_alternative<srs_config::srs_pos_res_id>(ref_sig)) {
    asn1_ref_sig.set_positioning_srs() = std::get<srs_config::srs_pos_res_id>(ref_sig);
  } else {
    dl_prs_t              dl_prs      = std::get<dl_prs_t>(ref_sig);
    asn1::f1ap::dl_prs_s& asn1_dl_prs = asn1_ref_sig.set_dl_prs();
    asn1_dl_prs.prsid                 = dl_prs.prs_id;
    asn1_dl_prs.dl_prs_res_set_id     = dl_prs.dl_prs_res_set_id;
    if (dl_prs.dl_prs_res_id.has_value()) {
      asn1_dl_prs.dl_prs_res_id_present = true;
      asn1_dl_prs.dl_prs_res_id         = dl_prs.dl_prs_res_id.value();
    }
  }

  return asn1_ref_sig;
}

static bool fill_asn1_positioning_activation_request(asn1::f1ap::positioning_activation_request_s& asn1_request,
                                                     const positioning_activation_request_t&       request)
{
  // Fill SRS type.
  if (std::holds_alternative<semipersistent_srs_t>(request.srs_type)) {
    semipersistent_srs_t              srs_type      = std::get<semipersistent_srs_t>(request.srs_type);
    asn1::f1ap::semipersistent_srs_s& asn1_srs_type = asn1_request->srs_type.set_semipersistent_srs();
    asn1_srs_type.srs_res_set_id                    = srs_type.srs_res_set_id;
    if (!srs_type.srs_spatial_relation_per_srs_res.has_value()) {
      return false;
    }

    for (const auto& ref_sig : srs_type.srs_spatial_relation_per_srs_res->reference_signals) {
      asn1_srs_type.ie_exts_present                                  = true;
      asn1_srs_type.ie_exts.srs_spatial_relation_per_srs_res_present = true;
      spatial_relation_per_srs_res_item_s item;
      item.ref_sig = ref_sig_to_asn1(ref_sig);
      asn1_srs_type.ie_exts.srs_spatial_relation_per_srs_res.spatial_relation_per_srs_res_list.push_back(item);
    }

  } else {
    aperiodic_srs_t              srs_type      = std::get<aperiodic_srs_t>(request.srs_type);
    asn1::f1ap::aperiodic_srs_s& asn1_srs_type = asn1_request->srs_type.set_aperiodic_srs();
    if (srs_type.aperiodic) {
      asn1_srs_type.aperiodic = asn1::f1ap::aperiodic_srs_s::aperiodic_opts::options::true_value;
    }
    if (srs_type.srs_res_trigger.has_value()) {
      for (const auto& srs_res_trigger_item : srs_type.srs_res_trigger->aperiodic_srs_res_trigger_list) {
        asn1_srs_type.srs_res_trigger.aperiodic_srs_res_trigger_list.push_back(srs_res_trigger_item);
      }
    }
  }

  // Fill activation time.
  if (request.activation_time.has_value()) {
    asn1_request->activation_time_present = true;
    asn1_request->activation_time.from_number(request.activation_time.value());
  }

  return true;
}

static positioning_activation_response_t
fill_positioning_activation_response(const asn1::f1ap::positioning_activation_resp_s& asn1_resp)
{
  positioning_activation_response_t resp;

  if (asn1_resp->crit_diagnostics_present) {
    // TODO: Add crit diagnostics
  }

  if (asn1_resp->sys_frame_num_present) {
    resp.sys_frame_num = asn1_resp->sys_frame_num;
  }

  if (asn1_resp->slot_num_present) {
    resp.slot_num = asn1_resp->slot_num;
  }

  return resp;
}

static positioning_activation_failure_t
fill_positioning_activation_failure(const asn1::f1ap::positioning_activation_fail_s& asn1_fail)
{
  positioning_activation_failure_t fail;
  fail.cause = f1ap_to_nrppa_cause(asn1_to_cause(asn1_fail->cause));
  if (asn1_fail->crit_diagnostics_present) {
    // TODO: Add crit diagnostics
  }

  return fail;
}
