// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

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

  du_ue_index_t       test_ue_index = to_du_ue_index(0);
  gnb_cu_ue_f1ap_id_t cu_ue_id      = gnb_cu_ue_f1ap_id_t{0};
  gnb_du_ue_f1ap_id_t du_ue_id      = gnb_du_ue_f1ap_id_t{0};
};

TEST_F(f1ap_du_e_cid_measurement_procedure_test, when_e_cid_measurement_is_requested_then_failure_is_sent)
{
  static constexpr uint16_t lmf_ue_meas_id = 3;
  static constexpr uint16_t ran_ue_meas_id = 4;

  this->f1ap->handle_message(
      test_helpers::generate_e_cid_measurement_initiation_request(cu_ue_id, du_ue_id, lmf_ue_meas_id, ran_ue_meas_id));

  // The gNB-CU gets an answer without waiting for its procedure timeout.
  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(tx_msg.has_value());
  ASSERT_EQ(tx_msg->pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(tx_msg->pdu.unsuccessful_outcome().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::e_c_id_meas_initiation_fail);

  const auto& fail = tx_msg->pdu.unsuccessful_outcome().value.e_c_id_meas_initiation_fail();
  ASSERT_EQ(fail->gnb_cu_ue_f1ap_id, to_underlying(cu_ue_id));
  ASSERT_EQ(fail->gnb_du_ue_f1ap_id, to_underlying(du_ue_id));
  ASSERT_EQ(fail->lmf_ue_meas_id, lmf_ue_meas_id);
  ASSERT_EQ(fail->ran_ue_meas_id, ran_ue_meas_id);
  ASSERT_EQ(fail->cause.type().value, asn1::f1ap::cause_c::types_opts::radio_network);
  ASSERT_EQ(fail->cause.radio_network().value, asn1::f1ap::cause_radio_network_opts::meas_not_supported_for_the_obj);
}
