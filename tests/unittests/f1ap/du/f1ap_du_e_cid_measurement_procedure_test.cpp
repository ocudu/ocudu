// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

namespace {

constexpr uint16_t test_azimuth_aoa    = 1234;
constexpr uint16_t test_zenith_aoa     = 567;
constexpr uint16_t test_lmf_ue_meas_id = 3;
constexpr uint16_t test_ran_ue_meas_id = 4;

using e_cid_quantity = asn1::f1ap::e_c_id_meas_quantities_value_opts::options;

} // namespace

class f1ap_du_e_cid_measurement_procedure_test : public f1ap_du_test
{
protected:
  f1ap_du_e_cid_measurement_procedure_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();
    run_f1ap_ue_create(test_ue_index);
    f1ap_message msg = test_helpers::generate_ue_context_setup_request(
        cu_ue_id, du_ue_id, 1, {}, config_helpers::make_default_du_cell_config().nr_cgi);
    run_ue_context_setup_procedure(test_ue_index, msg);

    this->f1c_gw.clear_tx_pdus();
  }

  /// Programs the DU to report a successful E-CID measurement with a UL angle of arrival.
  void du_reports_aoa(std::optional<uint16_t> zenith_aoa = test_zenith_aoa)
  {
    du_e_cid_meas_response& resp = this->f1ap_du_cfg_handler.next_e_cid_meas_response;
    resp.success                 = true;
    resp.ul_aoa_results.push_back(pos_meas_result_ul_aoa{.azimuth_aoa = test_azimuth_aoa, .zenith_aoa = zenith_aoa});
  }

  f1ap_message make_request(const std::vector<e_cid_quantity>&                      quantities,
                            asn1::f1ap::e_c_id_report_characteristics_opts::options report_characteristics =
                                asn1::f1ap::e_c_id_report_characteristics_opts::options::on_demand)
  {
    return test_helpers::generate_e_cid_measurement_initiation_request(
        cu_ue_id, du_ue_id, quantities, report_characteristics, test_lmf_ue_meas_id, test_ran_ue_meas_id);
  }

  du_ue_index_t       test_ue_index = to_du_ue_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id      = gnb_cu_ue_f1ap_id_t{0};
  gnb_du_ue_f1ap_id_t du_ue_id      = gnb_du_ue_f1ap_id_t{0};
};

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_valid_on_demand_request_is_received_then_du_is_notified)
{
  du_reports_aoa();

  this->f1ap->handle_message(make_request({e_cid_quantity::angle_of_arrival_nr}));

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_e_cid_meas_request.has_value());
  const auto& du_req = this->f1ap_du_cfg_handler.last_e_cid_meas_request.value();
  ASSERT_EQ(du_req.ue_index, test_ue_index);
  ASSERT_EQ(du_req.quantities.size(), 1);
  ASSERT_EQ(du_req.quantities[0], e_cid_meas_quantity::nr_angle_of_arrival);
}

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_nr_aoa_is_requested_then_response_contains_nr_aoa)
{
  du_reports_aoa();

  this->f1ap->handle_message(make_request({e_cid_quantity::angle_of_arrival_nr}));

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_e_cid_measurement_initiation_response(tx_msg.value()));

  const auto& resp = tx_msg.value().pdu.successful_outcome().value.e_c_id_meas_initiation_resp();
  ASSERT_EQ(resp->gnb_cu_ue_f1ap_id, to_underlying(cu_ue_id));
  ASSERT_EQ(resp->gnb_du_ue_f1ap_id, to_underlying(du_ue_id));
  ASSERT_EQ(resp->lmf_ue_meas_id, test_lmf_ue_meas_id);
  ASSERT_EQ(resp->ran_ue_meas_id, test_ran_ue_meas_id);

  ASSERT_TRUE(resp->e_c_id_meas_result_present);
  ASSERT_EQ(resp->e_c_id_meas_result.measured_results_list.size(), 1);
  const auto& result = resp->e_c_id_meas_result.measured_results_list[0].e_c_id_measured_results_value;
  ASSERT_EQ(result.type().value, asn1::f1ap::e_c_id_measured_results_value_c::types_opts::value_angleof_arrival_nr);
  ASSERT_EQ(result.value_angleof_arrival_nr().azimuth_ao_a, test_azimuth_aoa);
  ASSERT_TRUE(result.value_angleof_arrival_nr().zenith_ao_a_present);
  ASSERT_EQ(result.value_angleof_arrival_nr().zenith_ao_a, test_zenith_aoa);
}

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_du_reports_no_zenith_angle_then_response_omits_it)
{
  du_reports_aoa(std::nullopt);

  this->f1ap->handle_message(make_request({e_cid_quantity::angle_of_arrival_nr}));

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  const auto& resp   = tx_msg.value().pdu.successful_outcome().value.e_c_id_meas_initiation_resp();
  const auto& result = resp->e_c_id_meas_result.measured_results_list[0].e_c_id_measured_results_value;
  ASSERT_FALSE(result.value_angleof_arrival_nr().zenith_ao_a_present);
}

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_periodic_is_requested_then_failure_is_sent)
{
  du_reports_aoa();

  this->f1ap->handle_message(make_request({e_cid_quantity::angle_of_arrival_nr},
                                          asn1::f1ap::e_c_id_report_characteristics_opts::options::periodic));

  // The DU rejects the request without asking for a measurement.
  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_e_cid_meas_request.has_value());

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_e_cid_measurement_initiation_failure(tx_msg.value()));
}

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_timing_advance_is_requested_then_failure_is_sent)
{
  du_reports_aoa();

  // The gNB-DU must initiate all requested quantities, or none, as per TS 38.473 section 8.13.12.3.
  this->f1ap->handle_message(make_request({e_cid_quantity::angle_of_arrival_nr, e_cid_quantity::timing_advance_nr}));

  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_e_cid_meas_request.has_value());

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_e_cid_measurement_initiation_failure(tx_msg.value()));
}

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_measurement_quantities_list_is_empty_then_failure_is_sent)
{
  du_reports_aoa();

  this->f1ap->handle_message(make_request({}));

  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_e_cid_meas_request.has_value());

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_e_cid_measurement_initiation_failure(tx_msg.value()));
}

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_du_cannot_measure_then_failure_is_sent)
{
  // The DU reports a failed measurement.
  this->f1ap_du_cfg_handler.next_e_cid_meas_response.success = false;

  this->f1ap->handle_message(make_request({e_cid_quantity::angle_of_arrival_nr}));

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_e_cid_meas_request.has_value());

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_e_cid_measurement_initiation_failure(tx_msg.value()));

  const auto& fail = tx_msg.value().pdu.unsuccessful_outcome().value.e_c_id_meas_initiation_fail();
  ASSERT_EQ(fail->lmf_ue_meas_id, test_lmf_ue_meas_id);
  ASSERT_EQ(fail->ran_ue_meas_id, test_ran_ue_meas_id);
}

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_ue_does_not_exist_then_failure_is_sent)
{
  du_reports_aoa();

  // Use a gNB-DU UE F1AP ID that no UE context uses.
  f1ap_message req = test_helpers::generate_e_cid_measurement_initiation_request(
      cu_ue_id,
      gnb_du_ue_f1ap_id_t{99},
      {e_cid_quantity::angle_of_arrival_nr},
      asn1::f1ap::e_c_id_report_characteristics_opts::options::on_demand,
      test_lmf_ue_meas_id,
      test_ran_ue_meas_id);
  this->f1ap->handle_message(req);

  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_e_cid_meas_request.has_value());

  // The CU-CP gets an answer instead of waiting for its own procedure timeout.
  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_e_cid_measurement_initiation_failure(tx_msg.value()));
}

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_only_the_default_quantity_is_requested_then_response_is_sent)
{
  this->f1ap_du_cfg_handler.next_e_cid_meas_response.success = true;

  this->f1ap->handle_message(make_request({e_cid_quantity::default_value}));

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_e_cid_meas_request.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_e_cid_meas_request->quantities[0], e_cid_meas_quantity::default_quantity);

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_e_cid_measurement_initiation_response(tx_msg.value()));
}
