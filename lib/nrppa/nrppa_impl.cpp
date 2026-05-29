// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "nrppa_impl.h"
#include "nrppa_asn1_helpers.h"
#include "nrppa_helper.h"
#include "procedures/e_cid_measurement_initiation_procedure.h"
#include "procedures/e_cid_measurement_termination_procedure.h"
#include "procedures/measurement_procedure.h"
#include "procedures/positioning_activation_procedure.h"
#include "procedures/positioning_information_exchange_procedure.h"
#include "procedures/trp_information_exchange_procedure.h"
#include "ocudu/asn1/nrppa/common.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/coroutine.h"
#include "fmt/format.h"
#include <variant>

using namespace ocudu;
using namespace asn1::nrppa;
using namespace ocucp;

static inline void create_plmn_tac_map(const std::vector<cu_cp_configuration::ngap_config>& ngaps,
                                       std::map<plmn_identity, tac_t>&                      plmn_tac_map)
{
  for (const auto& ngap : ngaps) {
    for (const auto& supported_ta : ngap.supported_tas) {
      for (const auto& plmn : supported_ta.plmn_list) {
        plmn_tac_map.emplace(plmn.plmn_id, supported_ta.tac);
      }
    }
  }
}

nrppa_impl::nrppa_impl(const cu_cp_configuration& cfg,
                       nrppa_cu_cp_notifier&      cu_cp_notifier_,
                       async_task_scheduler&      common_task_sched_) :
  logger(ocudulog::fetch_basic_logger("NRPPA")),
  ue_ctxt_list(logger),
  du_ctxt_list(logger),
  meas_ctxt_list(logger),
  cu_cp_cfg(cfg),
  cu_cp_notifier(cu_cp_notifier_),
  common_task_sched(common_task_sched_)
{
  create_plmn_tac_map(cfg.ngap.ngaps, plmn_to_tac);
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
nrppa_impl::~nrppa_impl() {}

void nrppa_impl::remove_ue_context(cu_cp_ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.debug("ue={}: UE context not found", ue_index);
    return;
  }

  ue_ctxt_list.remove_ue_context(ue_index);
}

void nrppa_impl::initialize_meas_report_timer(cu_cp_ue_index_t ue_index, std::chrono::milliseconds meas_periodicity_ms)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: UE context not found", ue_index);
    return;
  }

  nrppa_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  if (ue_ctxt.meas_report_timer.is_running()) {
    ue_ctxt.logger.log_warning("Measurement report timer already running");
    return;
  }

  // Start timer.
  ue_ctxt.meas_report_timer.set(meas_periodicity_ms,
                                [this, ue_index](timer_id_t /*tid*/) { on_meas_report_timer_expired(ue_index); });
  ue_ctxt.meas_report_timer.run();
}

void nrppa_impl::handle_e_cid_meas_result(cu_cp_ue_index_t ue_index, const nrppa_e_cid_meas_result& result)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: Dropping E-CID measurement result. UE context doesn't exists", ue_index);
    return;
  }
  auto& ue_ctxt = ue_ctxt_list[ue_index];

  // Fill measurement report.
  asn1::nrppa::nr_ppa_pdu_c nrppa_msg;
  nrppa_msg.set_init_msg();
  nrppa_msg.init_msg().load_info_obj(ASN1_NRPPA_ID_E_C_ID_MEAS_REPORT);

  auto& asn1_report               = nrppa_msg.init_msg().value.e_c_id_meas_report();
  asn1_report->lmf_ue_meas_id     = lmf_ue_meas_id_to_uint(ue_ctxt.ue_ids.lmf_ue_meas_id);
  asn1_report->ran_ue_meas_id     = ran_ue_meas_id_to_uint(ue_ctxt.ue_ids.ran_ue_meas_id);
  asn1_report->e_c_id_meas_result = e_cid_meas_result_to_asn1(result);

  byte_buffer nrppa_pdu = pack_into_pdu(nrppa_msg, "E-CID measurement report");

  // Log Tx message.
  log_nrppa_message(logger, Tx, nrppa_pdu, nrppa_msg);
  cu_cp_notifier.on_ul_nrppa_pdu(nrppa_pdu, ue_index);
}

void nrppa_impl::on_meas_report_timer_expired(cu_cp_ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: UE context not found", ue_index);
    return;
  }

  nrppa_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  auto* ue = ue_ctxt.get_cu_cp_ue();
  ocudu_assert(ue != nullptr,
               "ue={} ran_ue={} lmf_ue={}: UE for UE context doesn't exist",
               fmt::underlying(ue_ctxt.ue_ids.ue_index),
               fmt::underlying(ue_ctxt.ue_ids.ran_ue_meas_id),
               fmt::underlying(ue_ctxt.ue_ids.lmf_ue_meas_id));

  // Restart timer.
  ue_ctxt.meas_report_timer.set(ue_ctxt.meas_periodicity_ms.value(),
                                [this, ue_index](timer_id_t /*tid*/) { on_meas_report_timer_expired(ue_index); });
  ue_ctxt.meas_report_timer.run();

  std::optional<cell_measurement_positioning_info>& ue_measurement_results = ue->on_measurement_results_required();
  if (!ue_measurement_results.has_value()) {
    logger.warning("ue={}: No measurement results available", ue_index);
    return;
  }

  if (ue_measurement_results.value().cell_measurements.empty()) {
    logger.warning("ue={}: No measurement results available", ue_index);
    return;
  }

  plmn_identity plmn = ue_measurement_results.value().serving_cell_id.plmn_id;
  if (plmn_to_tac.find(plmn) == plmn_to_tac.end()) {
    logger.warning("ue={}: TAC for PLMN={} not found", ue_index, plmn);
    return;
  }

  // Fill measurement result.
  auto meas_result = fill_e_cid_measurement_result(
      ue_ctxt.ue_ids.ue_index, ue_measurement_results.value(), ue_ctxt.meas_quantities, plmn_to_tac.at(plmn));
  if (!meas_result.has_value()) {
    logger.warning("{}", meas_result.error());
    return;
  }

  handle_e_cid_meas_result(ue_index, meas_result.value());
}

void nrppa_impl::handle_new_nrppa_pdu(const byte_buffer&                                nrppa_pdu,
                                      std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t> ue_or_amf_index)
{
  // Parse NRPPa-PDU.
  asn1::nrppa::nr_ppa_pdu_c nrppa_msg;
  asn1::cbit_ref            bref(nrppa_pdu);
  if (nrppa_msg.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
    logger.error(nrppa_pdu.begin(), nrppa_pdu.end(), "Failed to unpack NRPPa-PDU");
  }

  // Log Rx message.
  log_nrppa_message(logger, Rx, nrppa_pdu, nrppa_msg);

  switch (nrppa_msg.type().value) {
    case nr_ppa_pdu_c::types_opts::init_msg:
      handle_initiating_message(nrppa_msg.init_msg(), ue_or_amf_index);
      break;
    case nr_ppa_pdu_c::types_opts::successful_outcome:
      handle_successful_outcome(nrppa_msg.successful_outcome());
      break;
    case nr_ppa_pdu_c::types_opts::unsuccessful_outcome:
      handle_unsuccessful_outcome(nrppa_msg.unsuccessful_outcome());
      break;
    default:
      logger.error("Invalid PDU type");
      break;
  }
}

void nrppa_impl::handle_initiating_message(const init_msg_s&                                 msg,
                                           std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t> ue_or_amf_index)
{
  switch (msg.value.type().value) {
    case nr_ppa_elem_procs_o::init_msg_c::types_opts::e_c_id_meas_initiation_request:
      handle_e_cid_meas_initiation_request(msg.value.e_c_id_meas_initiation_request(),
                                           std::get<cu_cp_ue_index_t>(ue_or_amf_index),
                                           msg.nrppatransaction_id);
      break;
    case nr_ppa_elem_procs_o::init_msg_c::types_opts::e_c_id_meas_termination_cmd:
      handle_e_cid_meas_termination_command(msg.value.e_c_id_meas_termination_cmd(),
                                            std::get<cu_cp_ue_index_t>(ue_or_amf_index));
      break;
    case nr_ppa_elem_procs_o::init_msg_c::types_opts::trp_info_request:
      handle_trp_information_request(
          msg.value.trp_info_request(), std::get<cu_cp_amf_index_t>(ue_or_amf_index), msg.nrppatransaction_id);
      break;
    case nr_ppa_elem_procs_o::init_msg_c::types_opts::positioning_info_request:
      handle_positioning_information_request(
          msg.value.positioning_info_request(), std::get<cu_cp_ue_index_t>(ue_or_amf_index), msg.nrppatransaction_id);
      break;
    case nr_ppa_elem_procs_o::init_msg_c::types_opts::positioning_activation_request:
      handle_positioning_activation_request(msg.value.positioning_activation_request(),
                                            std::get<cu_cp_ue_index_t>(ue_or_amf_index),
                                            msg.nrppatransaction_id);
      break;
    case nr_ppa_elem_procs_o::init_msg_c::types_opts::meas_request:
      handle_measurement_request(
          msg.value.meas_request(), std::get<cu_cp_amf_index_t>(ue_or_amf_index), msg.nrppatransaction_id);
      break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
      break;
  }
}

void nrppa_impl::handle_e_cid_meas_initiation_request(const asn1::nrppa::e_c_id_meas_initiation_request_s& msg,
                                                      cu_cp_ue_index_t                                     ue_index,
                                                      uint16_t transaction_id)
{
  // If it doesn't exist, create NRPPA UE.
  if (!ue_ctxt_list.contains(ue_index)) {
    // Allocate RAN UE MEAS ID.
    auto ret = ue_ctxt_list.allocate_ran_ue_meas_id();
    if (!ret.has_value()) {
      logger.error("ue={}: Couldn't allocate ran ue meas id. Cause: {}", ue_index, ret.error());

      // Create failure response.
      asn1::nrppa::nr_ppa_pdu_c asn1_fail;
      asn1_fail.set_unsuccessful_outcome().load_info_obj(ASN1_NRPPA_ID_E_C_ID_MEAS_INITIATION);
      asn1_fail.unsuccessful_outcome().nrppatransaction_id = transaction_id;
      asn1::nrppa::e_c_id_meas_initiation_fail_s& meas_init_fail =
          asn1_fail.unsuccessful_outcome().value.e_c_id_meas_initiation_fail();
      meas_init_fail->lmf_ue_meas_id = msg->lmf_ue_meas_id;
      meas_init_fail->cause          = cause_to_asn1(nrppa_cause_protocol_t::msg_not_compatible_with_receiver_state);
      byte_buffer ul_nrppa_pdu       = pack_into_pdu(asn1_fail, "ECIDMeasInitiationFailure");

      // Log Tx message.
      log_nrppa_message(logger, Tx, ul_nrppa_pdu, asn1_fail);

      // Send response to CU-CP.
      cu_cp_notifier.on_ul_nrppa_pdu(ul_nrppa_pdu, ue_index);

      return;
    }

    // Create UE context and store it.
    nrppa_cu_cp_ue_notifier* ue_notifier = cu_cp_notifier.on_new_nrppa_ue(ue_index);
    ue_ctxt_list.add_ue(ue_index,
                        ret.value(),
                        uint_to_lmf_ue_meas_id(msg->lmf_ue_meas_id),
                        *ue_notifier,
                        *cu_cp_cfg.services.timers,
                        *cu_cp_cfg.services.cu_cp_executor);
  }

  nrppa_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  auto* ue = ue_ctxt.get_cu_cp_ue();
  ocudu_assert(ue != nullptr,
               "ue={} ran_ue={} lmf_ue={}: UE for UE context doesn't exist",
               fmt::underlying(ue_ctxt.ue_ids.ue_index),
               fmt::underlying(ue_ctxt.ue_ids.ran_ue_meas_id),
               fmt::underlying(ue_ctxt.ue_ids.lmf_ue_meas_id));

  logger.debug("Handling E-CID measurement initiation request");

  nrppa_e_cid_meas_initiation_request request;
  fill_nrppa_e_cid_meas_initiation_request(request, msg);

  ue->schedule_async_task(launch_async<e_cid_measurement_initiation_procedure>(
      ue_index, request, transaction_id, ue_ctxt_list, cu_cp_notifier, plmn_to_tac, *this, logger));
}

void nrppa_impl::handle_e_cid_meas_termination_command(const asn1::nrppa::e_c_id_meas_termination_cmd_s& msg,
                                                       cu_cp_ue_index_t                                  ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: Dropping E-CID Measurement Termination Command. UE context doesn't exists", ue_index);
    return;
  }

  nrppa_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  ue_ctxt.logger.log_debug("Handling E-CID measurement termination command");

  auto* ue = ue_ctxt.get_cu_cp_ue();
  ocudu_assert(ue != nullptr,
               "ue={} ran_ue={} lmf_ue={}: UE for UE context doesn't exist",
               fmt::underlying(ue_ctxt.ue_ids.ue_index),
               fmt::underlying(ue_ctxt.ue_ids.ran_ue_meas_id),
               fmt::underlying(ue_ctxt.ue_ids.lmf_ue_meas_id));

  ue->schedule_async_task(launch_async<e_cid_measurement_termination_procedure>(ue_index, ue_ctxt_list, logger));
}

void nrppa_impl::handle_trp_information_request(const asn1::nrppa::trp_info_request_s& msg,
                                                cu_cp_amf_index_t                      amf_index,
                                                uint16_t                               transaction_id)
{
  logger.debug("Handling TRP Information Request");

  trp_information_request_t request;
  fill_trp_information_request(request, msg);

  common_task_sched.schedule(launch_async<trp_information_exchange_procedure>(
      amf_index, request, transaction_id, cu_cp_notifier, trp_id_to_du_idx, du_ctxt_list, logger));
}

void nrppa_impl::handle_positioning_information_request(const asn1::nrppa::positioning_info_request_s& msg,
                                                        cu_cp_ue_index_t                               ue_index,
                                                        uint16_t                                       transaction_id)
{
  logger.debug("Handling positioning information request");

  nrppa_cu_cp_ue_notifier* ue_notifier = cu_cp_notifier.on_new_nrppa_ue(ue_index);
  cu_cp_du_index_t         du_index    = ue_notifier->get_du_index();

  positioning_information_request_t request;
  request.ue_index = ue_index;
  fill_positioning_information_request(request, msg);

  ue_notifier->schedule_async_task(launch_async<positioning_information_exchange_procedure>(
      request, transaction_id, du_index, du_ctxt_list, cu_cp_notifier, logger));
}

void nrppa_impl::handle_positioning_activation_request(const asn1::nrppa::positioning_activation_request_s& msg,
                                                       cu_cp_ue_index_t                                     ue_index,
                                                       uint16_t transaction_id)
{
  logger.debug("Handling positioning activation request");

  nrppa_cu_cp_ue_notifier* ue_notifier = cu_cp_notifier.on_new_nrppa_ue(ue_index);
  cu_cp_du_index_t         du_index    = ue_notifier->get_du_index();

  positioning_activation_request_t request;
  request.ue_index = ue_index;
  fill_positioning_activation_request(request, msg);

  ue_notifier->schedule_async_task(launch_async<positioning_activation_procedure>(
      request, transaction_id, du_index, du_ctxt_list, cu_cp_notifier, logger));
}

void nrppa_impl::handle_measurement_request(const asn1::nrppa::meas_request_s& msg,
                                            cu_cp_amf_index_t                  amf_index,
                                            uint16_t                           transaction_id)
{
  logger.debug("Handling Measurement Request");

  measurement_request_t request;
  fill_measurement_request(request, msg);

  common_task_sched.schedule(launch_async<measurement_procedure>(
      amf_index, request, transaction_id, trp_id_to_du_idx, meas_ctxt_list, du_ctxt_list, cu_cp_notifier, logger));
}

void nrppa_impl::handle_successful_outcome(const successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    default:
      logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void nrppa_impl::handle_unsuccessful_outcome(const unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}
