// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_e_cid_measurement_initiation_procedure.h"
#include "../../asn1_helpers.h"
#include "../ue_context/f1ap_du_ue.h"
#include "../ue_context/f1ap_du_ue_manager.h"
#include "proc_logger.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_ies.h"
#include "ocudu/f1ap/f1ap_message.h"

using namespace ocudu;
using namespace odu;

f1ap_du_e_cid_measurement_initiation_procedure::f1ap_du_e_cid_measurement_initiation_procedure(
    const asn1::f1ap::e_c_id_meas_initiation_request_s& msg_,
    f1ap_du_positioning_handler&                        du_mng_,
    f1ap_du_ue_manager&                                 ues_) :
  msg(msg_),
  du_mng(du_mng_),
  ues(ues_),
  du_ue_id(int_to_gnb_du_ue_f1ap_id(msg_->gnb_du_ue_f1ap_id)),
  ue_ctxt(ues_.find(int_to_gnb_du_ue_f1ap_id(msg_->gnb_du_ue_f1ap_id))->context),
  logger(ocudulog::fetch_basic_logger("DU-F1"))
{
}

void f1ap_du_e_cid_measurement_initiation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("{}: Procedure started...", f1ap_log_prefix{ue_ctxt, name()});

  if (not read_request()) {
    send_failure();
    CORO_EARLY_RETURN();
  }

  CORO_AWAIT_VALUE(du_result, request_e_cid_measurement());

  if (ues.find(du_ue_id) == nullptr) {
    logger.info("{}: Stopping procedure. Cause: UE was removed while the measurement was running",
                f1ap_log_prefix{ue_ctxt, name()});
    CORO_EARLY_RETURN();
  }

  if (not du_result.success) {
    send_failure();
    CORO_EARLY_RETURN();
  }

  send_response();

  CORO_RETURN();
}

bool f1ap_du_e_cid_measurement_initiation_procedure::read_request()
{
  using namespace asn1::f1ap;

  if (msg->e_c_id_report_characteristics.value != e_c_id_report_characteristics_opts::on_demand) {
    logger.warning("{}: Periodic E-CID measurement not supported", f1ap_log_prefix{ue_ctxt, name()});
    return false;
  }

  if (msg->e_c_id_meas_quantities.size() == 0) {
    logger.warning("{}: E-CID measurement quantities list is empty", f1ap_log_prefix{ue_ctxt, name()});
    return false;
  }

  // The gNB-DU must initiate all requested quantities, or none of them, as per TS 38.473 section 8.13.12.3.
  for (const auto& item : msg->e_c_id_meas_quantities) {
    const auto& quantity = item->e_c_id_meas_quantities_item().e_c_id_meas_quantities_value;
    switch (quantity.value) {
      case e_c_id_meas_quantities_value_opts::default_value:
        quantities.push_back(e_cid_meas_quantity::default_quantity);
        break;
      case e_c_id_meas_quantities_value_opts::angle_of_arrival_nr:
        quantities.push_back(e_cid_meas_quantity::nr_angle_of_arrival);
        break;
      default:
        logger.warning("{}: E-CID measurement quantity \"{}\" not supported",
                       f1ap_log_prefix{ue_ctxt, name()},
                       quantity.to_string());
        return false;
    }
  }

  return true;
}

async_task<du_e_cid_meas_response> f1ap_du_e_cid_measurement_initiation_procedure::request_e_cid_measurement()
{
  du_e_cid_meas_request du_req;
  du_req.ue_index   = ue_ctxt.ue_index;
  du_req.quantities = quantities;

  return du_mng.request_e_cid_measurement(du_req);
}

void f1ap_du_e_cid_measurement_initiation_procedure::send_response() const
{
  using namespace asn1::f1ap;
  f1ap_message f1ap_msg;

  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_E_C_ID_MEAS_INITIATION);
  e_c_id_meas_initiation_resp_s& resp = f1ap_msg.pdu.successful_outcome().value.e_c_id_meas_initiation_resp();

  resp->gnb_cu_ue_f1ap_id = msg->gnb_cu_ue_f1ap_id;
  resp->gnb_du_ue_f1ap_id = msg->gnb_du_ue_f1ap_id;
  resp->lmf_ue_meas_id    = msg->lmf_ue_meas_id;
  resp->ran_ue_meas_id    = msg->ran_ue_meas_id;

  resp->e_c_id_meas_result_present = true;
  if (du_result.geo_coords.has_value()) {
    resp->e_c_id_meas_result.geographical_coordinates_present = true;
    resp->e_c_id_meas_result.geographical_coordinates = geographical_coordinates_to_asn1(du_result.geo_coords.value());
  }
  resp->e_c_id_meas_result.measured_results_list.resize(du_result.ul_aoa_results.size());
  for (unsigned i = 0, e = du_result.ul_aoa_results.size(); i != e; ++i) {
    const pos_meas_result_ul_aoa&   aoa  = du_result.ul_aoa_results[i];
    e_c_id_measured_results_item_s& item = resp->e_c_id_meas_result.measured_results_list[i];

    ul_ao_a_s& asn1_aoa   = item.e_c_id_measured_results_value.set_value_angleof_arrival_nr();
    asn1_aoa.azimuth_ao_a = aoa.azimuth_aoa;
    if (aoa.zenith_aoa.has_value()) {
      asn1_aoa.zenith_ao_a_present = true;
      asn1_aoa.zenith_ao_a         = aoa.zenith_aoa.value();
    }
  }

  ues.find(du_ue_id)->f1ap_msg_notifier.on_new_message(f1ap_msg);

  logger.info("{}: Procedure finished successfully.", f1ap_log_prefix{ue_ctxt, name()});
}

void f1ap_du_e_cid_measurement_initiation_procedure::send_failure() const
{
  using namespace asn1::f1ap;
  f1ap_message f1ap_msg;

  f1ap_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_E_C_ID_MEAS_INITIATION);
  e_c_id_meas_initiation_fail_s& fail = f1ap_msg.pdu.unsuccessful_outcome().value.e_c_id_meas_initiation_fail();

  fail->gnb_cu_ue_f1ap_id      = msg->gnb_cu_ue_f1ap_id;
  fail->gnb_du_ue_f1ap_id      = msg->gnb_du_ue_f1ap_id;
  fail->lmf_ue_meas_id         = msg->lmf_ue_meas_id;
  fail->ran_ue_meas_id         = msg->ran_ue_meas_id;
  fail->cause.set_misc().value = cause_misc_opts::unspecified;

  ues.find(du_ue_id)->f1ap_msg_notifier.on_new_message(f1ap_msg);

  logger.info("{}: Procedure failed.", f1ap_log_prefix{ue_ctxt, name()});
}
