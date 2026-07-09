// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

class f1ap_du_trp_information_exchange_procedure_test : public f1ap_du_test
{
protected:
  f1ap_du_trp_information_exchange_procedure_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();
    this->f1c_gw.clear_tx_pdus();
  }
};

TEST_F(f1ap_du_trp_information_exchange_procedure_test, when_trp_info_exchange_succeeds_then_response_is_sent_to_cu)
{
  f1ap_message req = test_helpers::generate_trp_information_request();

  this->f1ap->handle_message(req);

  auto tx_msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(test_helpers::is_valid_f1ap_trp_information_response(tx_msg.value()));
  auto& resp = tx_msg.value().pdu.successful_outcome().value.trp_info_resp();
  ASSERT_EQ(resp->trp_info_list_trp_resp.size(), 4);

  // Check the geographical coordinates in the response.
  const auto& trp_info_type_resp_list =
      resp->trp_info_list_trp_resp[3].value().trp_info_item().trp_info.trp_info_type_resp_list;
  ASSERT_EQ(trp_info_type_resp_list.size(), 1);
  const auto& geo_coords_item = trp_info_type_resp_list[0];
  ASSERT_EQ(geo_coords_item.type(), asn1::f1ap::trp_info_type_resp_item_c::types_opts::geographical_coordinates);
  const auto& asn1_geo_coords = geo_coords_item.geographical_coordinates();
  ASSERT_EQ(asn1_geo_coords.trp_position_definition_type.type(),
            asn1::f1ap::trp_position_definition_type_c::types_opts::direct);
  const auto& asn1_direct = asn1_geo_coords.trp_position_definition_type.direct();
  ASSERT_EQ(asn1_direct.accuracy.type(), asn1::f1ap::trp_position_direct_accuracy_c::types_opts::trp_position);
  const auto& asn1_position = asn1_direct.accuracy.trp_position();
  ASSERT_EQ(asn1_position.latitude_sign, asn1::f1ap::access_point_position_s::latitude_sign_opts::north);
  ASSERT_EQ(asn1_position.latitude, 6791812);
  ASSERT_EQ(asn1_position.longitude, 1288490);
  ASSERT_EQ(asn1_position.altitude, 35);
  ASSERT_EQ(asn1_position.confidence, 90);
}
