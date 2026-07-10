// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e_cid_measurement_initiation_procedure.h"
#include "../nrppa_asn1_converters.h"
#include "../nrppa_asn1_helpers.h"
#include "../nrppa_helper.h"
#include "ocudu/asn1/nrppa/common.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

e_cid_measurement_initiation_procedure::e_cid_measurement_initiation_procedure(
    cu_cp_ue_index_t                           ue_index_,
    const nrppa_e_cid_meas_initiation_request& request_,
    uint16_t                                   transaction_id_,
    nrppa_ue_context_list&                     ue_ctxt_list_,
    nrppa_cu_cp_notifier&                      cu_cp_notifier_,
    const std::map<plmn_identity, unsigned>&   plmn_to_tac_,
    nrppa_impl&                                parent_,
    ocudulog::basic_logger&                    logger_) :
  ue_index(ue_index_),
  e_cid_meas_init_request(request_),
  transaction_id(transaction_id_),
  ue_ctxt_list(ue_ctxt_list_),
  cu_cp_notifier(cu_cp_notifier_),
  plmn_to_tac(plmn_to_tac_),
  parent(parent_),
  logger(logger_)
{
}

void e_cid_measurement_initiation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (!ue_ctxt_list.contains(ue_index)) {
    logger.info("ue={}: Stopping \"{}\". UE was already removed", ue_index, name());
    CORO_EARLY_RETURN();
  }

  ue_ctxt = &ue_ctxt_list[ue_index];

  ue_ctxt->logger.log_info("\"{}\" started...", name());

  if (e_cid_meas_init_request.report_characteristics == report_characteristics_t::periodic) {
    is_on_demand_measurement = false;

    // Check if periodic measurements are already configured.
    if (ue_ctxt->meas_report_timer.is_running()) {
      ue_ctxt->logger.log_warning("Stopping \"{}\". Periodic measurements are already configured", name());

      // Send E-CID measurement initiation failure to CU-CP.
      send_ul_nrppa_pdu(
          logger,
          cu_cp_notifier,
          create_e_cid_measurement_initiation_failure(nrppa_cause_protocol_t::msg_not_compatible_with_receiver_state),
          "ECIDMeasInitiationResponse",
          "ECIDMeasInitiationFailure",
          ue_ctxt->ue_ids.ue_index);

      CORO_EARLY_RETURN();
    }

    // Reject the request if periodic reporting was requested without the mandatory Measurement Periodicity IE.
    if (!e_cid_meas_init_request.meas_periodicity.has_value()) {
      ue_ctxt->logger.log_warning("Stopping \"{}\". Periodic reporting requested without Measurement Periodicity",
                                  name());

      // Send E-CID measurement initiation failure to CU-CP.
      send_ul_nrppa_pdu(logger,
                        cu_cp_notifier,
                        create_e_cid_measurement_initiation_failure(nrppa_cause_protocol_t::semantic_error),
                        "ECIDMeasInitiationResponse",
                        "ECIDMeasInitiationFailure",
                        ue_ctxt->ue_ids.ue_index);

      CORO_EARLY_RETURN();
    }

    // Setup periodic measurement.
    setup_periodic_measurement();

  } else {
    is_on_demand_measurement = true;

    // Get measurement results.
    get_measurement_result();
  }

  // Pack E-CID Measurement Initiation Response and forward to CU-CP.
  handle_procedure_outcome(is_on_demand_measurement);

  CORO_RETURN();
}

static std::chrono::milliseconds get_meas_period_ms(const nrppa_e_cid_meas_initiation_request& request)
{
  std::chrono::milliseconds meas_period_ms;

  // Convert measurement periodicity to milliseconds.
  if (request.meas_periodicity.value() == meas_periodicity_t::min1 ||
      request.meas_periodicity.value() == meas_periodicity_t::min6 ||
      request.meas_periodicity.value() == meas_periodicity_t::min12 ||
      request.meas_periodicity.value() == meas_periodicity_t::min30 ||
      request.meas_periodicity.value() == meas_periodicity_t::min60) {
    meas_period_ms = std::chrono::milliseconds{static_cast<uint32_t>(request.meas_periodicity.value()) * 60000U};

  } else {
    meas_period_ms = std::chrono::milliseconds{static_cast<uint32_t>(request.meas_periodicity.value())};
  }

  return meas_period_ms;
}

void e_cid_measurement_initiation_procedure::setup_periodic_measurement()
{
  ue_ctxt->meas_quantities     = e_cid_meas_init_request.meas_quantities;
  ue_ctxt->meas_periodicity_ms = get_meas_period_ms(e_cid_meas_init_request);

  ue_ctxt->logger.log_debug("Setting measurement report timer to {}ms", ue_ctxt->meas_periodicity_ms.value().count());

  // Start timer for periodic reporting.
  parent.initialize_meas_report_timer(ue_ctxt->ue_ids.ue_index, ue_ctxt->meas_periodicity_ms.value());
}

void e_cid_measurement_initiation_procedure::get_measurement_result()
{
  ocudu_assert(ue_ctxt->get_cu_cp_ue() != nullptr,
               "ue={} ran_ue={} lmf_ue={}: UE for UE context doesn't exist",
               fmt::underlying(ue_ctxt->ue_ids.ue_index),
               fmt::underlying(ue_ctxt->ue_ids.ran_ue_meas_id),
               fmt::underlying(ue_ctxt->ue_ids.lmf_ue_meas_id));

  std::optional<cell_measurement_positioning_info>& ue_measurement_results =
      ue_ctxt->get_cu_cp_ue()->on_measurement_results_required();

  // Perform sanity checks.
  nrppa_cause_t failure_cause = nrppa_cause_radio_network_t::requested_item_temporarily_not_available;
  if (!ue_measurement_results.has_value() or ue_measurement_results.value().cell_measurements.empty()) {
    ue_ctxt->logger.log_warning("ue={}: No measurement results available", fmt::underlying(ue_ctxt->ue_ids.ue_index));
    e_cid_meas_results = make_unexpected(failure_cause);
    return;
  }

  plmn_identity plmn = ue_measurement_results.value().serving_cell_id.plmn_id;
  if (plmn_to_tac.find(plmn) == plmn_to_tac.end()) {
    ue_ctxt->logger.log_warning("ue={}: TAC for PLMN={} not found", ue_ctxt->ue_ids.ue_index, plmn);
    e_cid_meas_results = make_unexpected(failure_cause);
    return;
  }

  auto result = fill_e_cid_measurement_result(ue_ctxt->ue_ids.ue_index,
                                              ue_measurement_results.value(),
                                              e_cid_meas_init_request.meas_quantities,
                                              plmn_to_tac.at(plmn));
  if (!result.has_value()) {
    ue_ctxt->logger.log_warning("{}", result.error());
    failure_cause      = nrppa_cause_radio_network_t::requested_item_not_supported;
    e_cid_meas_results = make_unexpected(failure_cause);
    return;
  }

  e_cid_meas_results = result.value();
}

void e_cid_measurement_initiation_procedure::handle_procedure_outcome(bool on_demand)
{
  // If we are not in on-demand mode, we must have a measurement result.
  if (on_demand && !e_cid_meas_results.has_value()) {
    e_cid_meas_outcome = create_e_cid_measurement_initiation_failure(e_cid_meas_results.error());
    ue_ctxt->logger.log_warning("\"{}\" failed", name());
  } else {
    e_cid_meas_outcome = create_e_cid_measurement_initiation_response(on_demand);
    ue_ctxt->logger.log_info("\"{}\" finished successfully", name());
  }

  // Send response to CU-CP.
  send_ul_nrppa_pdu(logger,
                    cu_cp_notifier,
                    e_cid_meas_outcome,
                    "ECIDMeasInitiationResponse",
                    "ECIDMeasInitiationFailure",
                    ue_ctxt->ue_ids.ue_index);
}

asn1::nrppa::nr_ppa_pdu_c
e_cid_measurement_initiation_procedure::create_e_cid_measurement_initiation_failure(nrppa_cause_t cause)
{
  asn1::nrppa::nr_ppa_pdu_c asn1_fail;

  asn1_fail.set_unsuccessful_outcome().load_info_obj(ASN1_NRPPA_ID_E_C_ID_MEAS_INITIATION);
  asn1_fail.unsuccessful_outcome().nrppatransaction_id = transaction_id;
  asn1::nrppa::e_c_id_meas_initiation_fail_s& meas_init_fail =
      asn1_fail.unsuccessful_outcome().value.e_c_id_meas_initiation_fail();

  meas_init_fail->lmf_ue_meas_id = lmf_ue_meas_id_to_uint(ue_ctxt->ue_ids.lmf_ue_meas_id);
  meas_init_fail->cause          = cause_to_asn1(cause);

  return asn1_fail;
}

asn1::nrppa::nr_ppa_pdu_c
e_cid_measurement_initiation_procedure::create_e_cid_measurement_initiation_response(bool on_demand)
{
  asn1::nrppa::nr_ppa_pdu_c asn1_resp;

  asn1_resp.set_successful_outcome().load_info_obj(ASN1_NRPPA_ID_E_C_ID_MEAS_INITIATION);
  asn1_resp.successful_outcome().nrppatransaction_id = transaction_id;

  asn1::nrppa::e_c_id_meas_initiation_resp_s& meas_init_resp =
      asn1_resp.successful_outcome().value.e_c_id_meas_initiation_resp();

  meas_init_resp->lmf_ue_meas_id = lmf_ue_meas_id_to_uint(ue_ctxt->ue_ids.lmf_ue_meas_id);
  meas_init_resp->ran_ue_meas_id = ran_ue_meas_id_to_uint(ue_ctxt->ue_ids.ran_ue_meas_id);

  if (on_demand) {
    meas_init_resp->e_c_id_meas_result_present = true;
    meas_init_resp->e_c_id_meas_result         = e_cid_meas_result_to_asn1(e_cid_meas_results.value());
  }

  return asn1_resp;
}
