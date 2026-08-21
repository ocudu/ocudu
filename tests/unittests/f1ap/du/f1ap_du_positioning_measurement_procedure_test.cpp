// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
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

/// Logs the JSON representation of an F1AP PDU, to ease debugging of positioning measurement test failures.
void log_f1ap_pdu(ocudulog::basic_logger& logger, const char* label, const f1ap_message& msg)
{
  asn1::json_writer js;
  msg.pdu.to_json(js);
  logger.info("{}: {}", label, js.to_string());
}

} // namespace

class f1ap_du_positioning_measurement_procedure_test : public f1ap_du_test
{
protected:
  f1ap_du_positioning_measurement_procedure_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();
    run_f1ap_ue_create(test_ue_index);
    f1ap_message msg = test_helpers::generate_ue_context_setup_request(
        gnb_cu_ue_f1ap_id_t{0}, gnb_du_ue_f1ap_id_t{0}, 1, {}, config_helpers::make_default_du_cell_config().nr_cgi);
    run_ue_context_setup_procedure(test_ue_index, msg);

    this->f1c_gw.clear_tx_pdus();
  }

  du_ue_index_t test_ue_index = to_du_ue_index(0);
};

TEST_F(f1ap_du_positioning_measurement_procedure_test, when_valid_request_is_received_then_du_is_notified)
{
  f1ap_message req =
      test_helpers::generate_positioning_measurement_request({trp_id_t::min}, lmf_meas_id_t::min, ran_meas_id_t::min);
  log_f1ap_pdu(test_logger, "Positioning Measurement Request", req);

  this->f1ap->handle_message(req);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_positioning_meas_request.has_value());
}

TEST_F(f1ap_du_positioning_measurement_procedure_test, when_invalid_request_is_received_then_failure_is_sent_to_cu)
{
  f1ap_message req =
      test_helpers::generate_positioning_measurement_request({trp_id_t::min},
                                                             lmf_meas_id_t::min,
                                                             ran_meas_id_t::min,
                                                             {asn1::f1ap::pos_meas_type_opts::options::ul_rtoa,
                                                              asn1::f1ap::pos_meas_type_opts::options::ul_srs_rsrp,
                                                              asn1::f1ap::pos_meas_type_opts::options::ul_aoa});

  // Remove SRS Configuration from the request to simulate an invalid request.
  req.pdu.init_msg().value.positioning_meas_request()->srs_configuration_present = false;
  log_f1ap_pdu(test_logger, "Positioning Measurement Request", req);

  this->f1ap->handle_message(req);

  // DU is not notified about the invalid request.
  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_positioning_meas_request.has_value());

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  log_f1ap_pdu(test_logger, "Positioning Measurement Failure", tx_msg.value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_positioning_measurement_failure(tx_msg.value()));
}

TEST_F(f1ap_du_positioning_measurement_procedure_test,
       when_positioning_measurement_succeeds_then_response_is_sent_to_cu)
{
  f1ap_message req =
      test_helpers::generate_positioning_measurement_request({trp_id_t::min}, lmf_meas_id_t::min, ran_meas_id_t::min);
  log_f1ap_pdu(test_logger, "Positioning Measurement Request", req);

  this->f1ap->handle_message(req);

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  log_f1ap_pdu(test_logger, "Positioning Measurement Response", tx_msg.value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_positioning_measurement_response(tx_msg.value()));
}

TEST_F(f1ap_du_positioning_measurement_procedure_test, when_ul_aoa_is_requested_then_response_contains_ul_aoa)
{
  static constexpr uint16_t azimuth_aoa = 1234;
  static constexpr uint16_t zenith_aoa  = 567;

  // Program the DU to report a UL-AoA measurement for the requested TRP.
  du_positioning_meas_response& meas_resp  = this->f1ap_du_cfg_handler.next_positioning_meas_response;
  pos_meas_result&              trp_result = meas_resp.pos_meas_list.emplace_back();
  trp_result.trp_id                        = trp_id_t::min;
  trp_result.sl_rx                         = slot_point{subcarrier_spacing::kHz15, 0, 0};
  trp_result.results.push_back(pos_meas_result_ul_aoa{.azimuth_aoa = azimuth_aoa, .zenith_aoa = zenith_aoa});

  f1ap_message req = test_helpers::generate_positioning_measurement_request(
      {trp_id_t::min}, lmf_meas_id_t::min, ran_meas_id_t::min, {asn1::f1ap::pos_meas_type_opts::options::ul_aoa});
  log_f1ap_pdu(test_logger, "Positioning Measurement Request", req);

  this->f1ap->handle_message(req);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_positioning_meas_request.has_value());

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  log_f1ap_pdu(test_logger, "Positioning Measurement Response", tx_msg.value());
  ASSERT_TRUE(test_helpers::is_valid_f1ap_positioning_measurement_response(tx_msg.value()));

  const auto& resp = tx_msg.value().pdu.successful_outcome().value.positioning_meas_resp();
  ASSERT_TRUE(resp->pos_meas_result_list_present);
  ASSERT_EQ(resp->pos_meas_result_list.size(), 1U);
  const auto& pos_meas_result = resp->pos_meas_result_list[0].pos_meas_result;
  ASSERT_EQ(pos_meas_result.size(), 1U);
  ASSERT_EQ(pos_meas_result[0].measured_results_value.type(),
            asn1::f1ap::measured_results_value_c::types_opts::ul_angle_of_arrival);
  const auto& asn1_aoa = pos_meas_result[0].measured_results_value.ul_angle_of_arrival();
  ASSERT_EQ(asn1_aoa.azimuth_ao_a, azimuth_aoa);
  ASSERT_TRUE(asn1_aoa.zenith_ao_a_present);
  ASSERT_EQ(asn1_aoa.zenith_ao_a, zenith_aoa);
}
