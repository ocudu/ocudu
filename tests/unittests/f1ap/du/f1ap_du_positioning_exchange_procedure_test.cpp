// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

class f1ap_du_positioning_exchange_procedure_test : public f1ap_du_test
{
protected:
  f1ap_du_positioning_exchange_procedure_test()
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

TEST_F(f1ap_du_positioning_exchange_procedure_test, when_request_is_received_then_du_is_notified)
{
  f1ap_message req =
      test_helpers::generate_positioning_information_request(gnb_du_ue_f1ap_id_t{0}, gnb_cu_ue_f1ap_id_t{0});

  this->f1ap->handle_message(req);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_positioning_info_request.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_positioning_info_request.value().ue_index, to_du_ue_index(0));
}

TEST_F(f1ap_du_positioning_exchange_procedure_test, when_positioning_info_exchange_succeeds_then_response_is_sent_to_cu)
{
  f1ap_message req =
      test_helpers::generate_positioning_information_request(gnb_du_ue_f1ap_id_t{0}, gnb_cu_ue_f1ap_id_t{0});

  auto& du_resp = this->f1ap_du_cfg_handler.next_positioning_info_response;
  du_resp.srs_carriers.resize(1);
  du_resp.srs_carriers[0].pci = pci_t{0};
  du_resp.srs_carriers[0].ul_ch_bw_per_scs_list.resize(1);
  du_resp.srs_carriers[0].ul_ch_bw_per_scs_list[0].scs               = subcarrier_spacing::kHz15;
  du_resp.srs_carriers[0].ul_ch_bw_per_scs_list[0].carrier_bandwidth = 5;

  this->f1ap->handle_message(req);

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(test_helpers::is_valid_positioning_information_response(tx_msg.value()));
  auto& resp = tx_msg.value().pdu.successful_outcome().value.positioning_info_resp();
  ASSERT_EQ(resp->srs_configuration.srs_carrier_list.size(), 1);
  ASSERT_EQ(resp->srs_configuration.srs_carrier_list[0].pci, 0);
  ASSERT_EQ(resp->srs_configuration.srs_carrier_list[0].ul_ch_bw_per_scs_list.size(), 1);
  ASSERT_EQ(resp->srs_configuration.srs_carrier_list[0].ul_ch_bw_per_scs_list[0].carrier_bw, 5);
}
