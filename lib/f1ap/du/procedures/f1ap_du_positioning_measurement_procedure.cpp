// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_positioning_measurement_procedure.h"
#include "../../asn1_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/f1ap/f1ap_message_notifier.h"

using namespace ocudu;
using namespace odu;
using namespace asn1::f1ap;

static odu::pos_meas_type asn1_pos_meas_type_to_du_pos_meas_type(pos_meas_type_opts::options asn1_type)
{
  switch (asn1_type) {
    case pos_meas_type_opts::gnb_rx_tx:
      return odu::pos_meas_type::gnb_rx_tx;
    case pos_meas_type_opts::ul_srs_rsrp:
      return odu::pos_meas_type::ul_srs_rsrp;
    case pos_meas_type_opts::ul_aoa:
      return odu::pos_meas_type::ul_aoa;
    case pos_meas_type_opts::ul_rtoa:
      return odu::pos_meas_type::ul_rtoa;
    case pos_meas_type_opts::multiple_ul_aoa:
      return odu::pos_meas_type::multiple_ul_aoa;
    case pos_meas_type_opts::ul_srs_rsrpp:
      return odu::pos_meas_type::ul_srs_rsrpp;
    default:
      // Retun UL-RTOA as default type, we'll fail at the caller.
      return odu::pos_meas_type::ul_rtoa;
  }
}

f1ap_du_positioning_measurement_procedure::f1ap_du_positioning_measurement_procedure(
    const positioning_meas_request_s& msg_,
    f1ap_du_positioning_handler&      du_mng_,
    f1ap_message_notifier&            cu_notifier_) :
  msg(msg_), du_mng(du_mng_), cu_notifier(cu_notifier_), logger(ocudulog::fetch_basic_logger("DU-F1"))
{
}

void f1ap_du_positioning_measurement_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not validate_request()) {
    send_failure();
    CORO_EARLY_RETURN();
  }

  CORO_AWAIT_VALUE(du_result, request_positioning_measurement());

  // Respond back to CU-CP.
  send_response();

  CORO_RETURN();
}

bool f1ap_du_positioning_measurement_procedure::validate_request() const
{
  // Although the TRP Measurement Request List is not mandatory (TS 38.473, Section 9.2.12.3), the DU carries out
  // measurement for the TRPs indicated in the list (TS 38.473, Section 8.13.3.2); if no TRP is indicated, we can abort
  // the measurement.
  // TODO: evaluate whether we can measure all TRPs when the TRP Measurement Request List is empty.
  if (msg->trp_meas_request_list.size() == 0U) {
    logger.warning("TRP Measurement Request List is empty: no measurement will be carried out");
    return false;
  }

  if (msg->pos_report_characteristics.value != pos_report_characteristics_opts::ondemand) {
    logger.warning("Periodic positioning measurement not supported");
    return false;
  }

  if (msg->pos_meas_quantities.size() == 0) {
    logger.warning("Positioning Measurement Req: positioning measurement quantities list is empty");
    return false;
  }

  // At least 1 of the elements must be UL-RTOA, UL SRS-RSRP or UL-AoA.
  if (not std::any_of(msg->pos_meas_quantities.begin(), msg->pos_meas_quantities.end(), [](const auto& quantity) {
        return quantity.pos_meas_type.value == pos_meas_type_opts::ul_rtoa or
               quantity.pos_meas_type.value == pos_meas_type_opts::ul_srs_rsrp or
               quantity.pos_meas_type.value == pos_meas_type_opts::ul_aoa;
      })) {
    logger.warning(
        "Positioning Measurement Req.: no UL-RTOA, UL SRS RSRP or UL-AoA pos. measurement type found in the list");
    return false;
  }

  // Quantity type-specific checks.
  for (const auto& quant : msg->pos_meas_quantities) {
    if (quant.pos_meas_type.value == pos_meas_type_opts::ul_rtoa) {
      if (not quant.timing_report_granularity_factor_present) {
        logger.warning("Granularity factor not present");
      }
    } else if (quant.pos_meas_type.value == pos_meas_type_opts::ul_srs_rsrp) {
      // Placeholder for UL SRS RSRP measurement specifc type checks.
    } else if (quant.pos_meas_type.value == pos_meas_type_opts::ul_aoa) {
      // Placeholder for UL-AoA measurement specific type checks.
    } else {
      logger.warning("Only UL-RTOA, UL SRS RSRP and UL-AoA positioning measurement types are supported. Other "
                     "measurement types will be ignored");
    }
  }

  if (not msg->srs_configuration_present) {
    logger.warning("SRS Configuration not present");
    return false;
  }

  if (msg->sfn_initisation_time_present or msg->slot_num_present) {
    logger.warning(
        "System Frame Number and Slot Number are not supported in Positioning Measurement Request and will be ignored");
  }

  // TODO: Review this assumption once the F1AP we defined which elements of Positioning measurement request are
  //       mandatory.
  const bool srs_not_configured =
      msg->srs_configuration.srs_carrier_list.size() == 0U or
      msg->srs_configuration.srs_carrier_list[0U].active_ul_bwp.srs_cfg.srs_res_set_list.size() != 1U or
      msg->srs_configuration.srs_carrier_list[0U].active_ul_bwp.srs_cfg.srs_res_list.size() != 1U;
  if (srs_not_configured) {
    logger.warning("SRS Configuration missing, or configured but with several SRS resources or sets");
    return false;
  }

  // At this point, we know only 1 SRS set and 1 SRS resource are configured.
  const bool non_periodic_srs =
      msg->srs_configuration.srs_carrier_list[0U]
              .active_ul_bwp.srs_cfg.srs_res_set_list[0U]
              .res_set_type.type()
              .value != asn1::f1ap::res_set_type_c::types_opts::periodic or
      msg->srs_configuration.srs_carrier_list[0U].active_ul_bwp.srs_cfg.srs_res_list[0U].res_type.type().value !=
          asn1::f1ap::res_type_c::types_opts::periodic;
  if (non_periodic_srs) {
    logger.warning("Only periodic SRS supported for positioning measurement");
    return false;
  }

  return true;
}

async_task<du_positioning_meas_response> f1ap_du_positioning_measurement_procedure::request_positioning_measurement()
{
  du_positioning_meas_request du_req;

  // TRP Measurement Request List.
  du_req.trp_meas_req_list.resize(msg->trp_meas_request_list.size());
  for (unsigned i = 0, e = du_req.trp_meas_req_list.size(); i != e; ++i) {
    du_req.trp_meas_req_list[i].trp_id = uint_to_trp_id(msg->trp_meas_request_list[i].trp_id);
    if (msg->trp_meas_request_list[i].ie_exts_present and msg->trp_meas_request_list[i].ie_exts.nr_cgi_present) {
      du_req.trp_meas_req_list[i].cgi.emplace();
      du_req.trp_meas_req_list[i].cgi.value().plmn_id =
          plmn_identity::from_bytes(msg->trp_meas_request_list[i].ie_exts.nr_cgi.plmn_id.to_bytes()).value();
      du_req.trp_meas_req_list[i].cgi.value().nci =
          nr_cell_identity::create(msg->trp_meas_request_list[i].ie_exts.nr_cgi.nr_cell_id.to_number()).value();
    }
  }

  // Positioning measurement quantities.
  du_req.pos_meas_quants.resize(msg->pos_meas_quantities.size());
  for (unsigned i = 0, e = du_req.pos_meas_quants.size(); i != e; ++i) {
    // At this point, the requested positioning measurement types have been validated already.
    du_req.pos_meas_quants[i].meas_type =
        asn1_pos_meas_type_to_du_pos_meas_type(msg->pos_meas_quantities[i].pos_meas_type.value);
    if (msg->pos_meas_quantities[i].timing_report_granularity_factor_present) {
      du_req.pos_meas_quants[i].granularity_factor = msg->pos_meas_quantities[i].timing_report_granularity_factor;
    }
  }

  // SRS Configuration.
  if (msg->srs_configuration_present) {
    du_req.srs_carriers = from_asn1(msg->srs_configuration);
  }

  return du_mng.request_positioning_measurement(du_req);
}

void f1ap_du_positioning_measurement_procedure::send_response() const
{
  f1ap_message resp_msg;

  resp_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_POSITIONING_MEAS_EXCHANGE);
  auto& resp = resp_msg.pdu.successful_outcome().value.positioning_meas_resp();

  resp->transaction_id = msg->transaction_id;
  resp->lmf_meas_id    = msg->lmf_meas_id;
  resp->ran_meas_id    = msg->ran_meas_id;

  resp->pos_meas_result_list_present = not du_result.pos_meas_list.empty();
  resp->pos_meas_result_list.resize(du_result.pos_meas_list.size());
  for (unsigned i = 0, e = du_result.pos_meas_list.size(); i != e; ++i) {
    pos_meas_result_list_item_s& asn1_meas = resp->pos_meas_result_list[i];
    const pos_meas_result&       meas      = du_result.pos_meas_list[i];

    asn1_meas.trp_id = trp_id_to_uint(meas.trp_id);

    asn1_meas.pos_meas_result.resize(meas.results.size());
    for (unsigned j = 0, je = meas.results.size(); j != je; ++j) {
      const auto&             du_meas_result   = meas.results[j];
      pos_meas_result_item_s& asn1_meas_result = asn1_meas.pos_meas_result[j];

      // Measurement Result Value.
      if (std::holds_alternative<pos_meas_result_ul_rtoa>(du_meas_result)) {
        // UL-RTOA measurement result.
        const auto& du_rtoa_result = std::get<pos_meas_result_ul_rtoa>(du_meas_result);
        auto&       rtoa_item      = asn1_meas_result.measured_results_value.set_ul_rtoa().ul_rtoa_meas_item;
        switch (du_rtoa_result.granularity) {
          case 0:
            rtoa_item.set_k0() = du_rtoa_result.ul_rtoa;
            break;
          case 1:
            rtoa_item.set_k1() = du_rtoa_result.ul_rtoa;
            break;
          case 2:
            rtoa_item.set_k2() = du_rtoa_result.ul_rtoa;
            break;
          case 3:
            rtoa_item.set_k3() = du_rtoa_result.ul_rtoa;
            break;
          case 4:
            rtoa_item.set_k4() = du_rtoa_result.ul_rtoa;
            break;
          case 5:
            rtoa_item.set_k5() = du_rtoa_result.ul_rtoa;
            break;
          default:
            report_fatal_error("Invalid k value");
        }
      } else if (std::holds_alternative<pos_meas_result_ul_rsrp>(du_meas_result)) {
        // UL-RSRP measurement result.
        const auto& du_rsrp_result = std::get<pos_meas_result_ul_rsrp>(du_meas_result);
        auto&       rsrp_item      = asn1_meas_result.measured_results_value.set_ul_srs_rsrp();
        rsrp_item                  = du_rsrp_result.ul_rsrp;
      } else {
        // UL-AoA measurement result.
        const auto& du_aoa_result = std::get<pos_meas_result_ul_aoa>(du_meas_result);
        auto&       aoa_item      = asn1_meas_result.measured_results_value.set_ul_angle_of_arrival();
        aoa_item.azimuth_ao_a     = du_aoa_result.azimuth_aoa;
        if (du_aoa_result.zenith_aoa.has_value()) {
          aoa_item.zenith_ao_a_present = true;
          aoa_item.zenith_ao_a         = du_aoa_result.zenith_aoa.value();
        }
      }

      // Slot and time stamp.
      // NOTE: use the same slot/time-stamp for all results the Positioning Measurement Result.
      asn1_meas_result.time_stamp.sys_frame_num = meas.sl_rx.sfn();
      switch (meas.sl_rx.scs()) {
        case ocudu::subcarrier_spacing::kHz15: // SCS 15 kHz
          asn1_meas_result.time_stamp.slot_idx.set(time_stamp_slot_idx_c::types::scs_15);
          asn1_meas_result.time_stamp.slot_idx.set_scs_15() = meas.sl_rx.slot_index();
          break;
        case ocudu::subcarrier_spacing::kHz30: // SCS 30 kHz
          asn1_meas_result.time_stamp.slot_idx.set(time_stamp_slot_idx_c::types::scs_30);
          asn1_meas_result.time_stamp.slot_idx.set_scs_30() = meas.sl_rx.slot_index();
          break;
        case ocudu::subcarrier_spacing::kHz60: // SCS 60 kHz
          asn1_meas_result.time_stamp.slot_idx.set(time_stamp_slot_idx_c::types::scs_60);
          asn1_meas_result.time_stamp.slot_idx.set_scs_60() = meas.sl_rx.slot_index();
          break;
        case ocudu::subcarrier_spacing::kHz120: // SCS 120 kHz
          asn1_meas_result.time_stamp.slot_idx.set(time_stamp_slot_idx_c::types::scs_120);
          asn1_meas_result.time_stamp.slot_idx.set_scs_120() = meas.sl_rx.slot_index();
          break;
        default:
          logger.error("Time-stamp only supports 15kHz, 30kHz, 60kHz, and 120kHz SCS");
      }
    }
  }

  // Send response to CU-CP.
  cu_notifier.on_new_message(resp_msg);
}

void f1ap_du_positioning_measurement_procedure::send_failure() const
{
  f1ap_message resp_msg;
  resp_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_POSITIONING_MEAS_EXCHANGE);
  auto& fail = resp_msg.pdu.unsuccessful_outcome().value.positioning_meas_fail();

  fail->transaction_id         = msg->transaction_id;
  fail->lmf_meas_id            = msg->lmf_meas_id;
  fail->ran_meas_id            = msg->ran_meas_id;
  fail->cause.set_misc().value = cause_misc_opts::unspecified;

  // Send response to CU-CP.
  cu_notifier.on_new_message(resp_msg);
}
