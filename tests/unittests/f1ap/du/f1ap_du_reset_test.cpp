// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "lib/f1ap/du/procedures/f1ap_du_reset_procedure.h"
#include "test_doubles/f1ap/f1ap_test_message_validators.h"
#include "test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

class f1_reset_test : public f1ap_du_test
{
public:
  f1_reset_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();

    this->f1c_gw.clear_tx_pdus();
  }
};

TEST_F(f1_reset_test, when_f1_reset_is_received_but_no_ue_is_created_then_no_request_is_forwarded_to_du)
{
  auto req = test_helpers::generate_f1ap_reset_message();
  this->f1ap->handle_message(req);

  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_ues_to_reset.has_value());

  ASSERT_TRUE(test_helpers::is_valid_f1_reset_ack(req, this->f1c_gw.last_tx_pdu()));
}

TEST_F(f1_reset_test, when_f1_reset_is_received_then_reset_request_is_forwarded_to_du)
{
  du_ue_index_t test_ue_index = to_du_ue_index(0);
  this->run_f1ap_ue_create(test_ue_index);

  auto req = test_helpers::generate_f1ap_reset_message();
  this->f1ap->handle_message(req);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ues_to_reset.has_value());
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ues_to_reset.value().empty());

  ASSERT_TRUE(test_helpers::is_valid_f1_reset_ack(req, this->f1c_gw.last_tx_pdu()));
}

TEST_F(f1_reset_test, when_f1_reset_is_received_for_a_specific_ue_then_reset_request_is_forwarded_to_du_with_ue)
{
  du_ue_index_t test_ue_index = to_du_ue_index(0);
  this->run_f1ap_ue_create(test_ue_index);

  auto req = test_helpers::generate_f1ap_reset_message({{gnb_du_ue_f1ap_id_t{0}, std::nullopt}});
  this->f1ap->handle_message(req);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ues_to_reset.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_ues_to_reset.value().size(), 1);
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_ues_to_reset.value()[0], test_ue_index);

  ASSERT_TRUE(test_helpers::is_valid_f1_reset_ack(req, this->f1c_gw.last_tx_pdu()));
}

/// Verifies that unsupported reset choices produce an Error Indication without resetting UEs.
TEST_F(f1_reset_test, invalid_reset_type_returns_error_indication_without_reset)
{
  for (auto reset_type : {asn1::f1ap::reset_type_c::types::nulltype, asn1::f1ap::reset_type_c::types::choice_ext}) {
    auto req = test_helpers::generate_f1ap_reset_message();
    req.pdu.init_msg().value.reset()->reset_type.set(reset_type);
    req.pdu.init_msg().value.reset()->transaction_id = 23;
    f1c_gw.clear_tx_pdus();

    f1ap->handle_message(req);

    ASSERT_FALSE(f1ap_du_cfg_handler.last_ues_to_reset.has_value());
    ASSERT_TRUE(f1c_gw.tx_pdus_sent());
    ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
    ASSERT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.type().value,
              asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::error_ind);
    byte_buffer   encoded;
    asn1::bit_ref writer(encoded);
    ASSERT_EQ(asn1::pack(writer, f1c_gw.last_tx_pdu().pdu), asn1::OCUDUASN_SUCCESS);
    asn1::cbit_ref         reader(encoded);
    asn1::f1ap::f1ap_pdu_c decoded;
    ASSERT_EQ(asn1::unpack(decoded, reader), asn1::OCUDUASN_SUCCESS);
    const auto& error = *decoded.init_msg().value.error_ind();
    EXPECT_EQ(error.transaction_id, req.pdu.init_msg().value.reset()->transaction_id);
    ASSERT_TRUE(error.cause_present);
    ASSERT_EQ(error.cause.type().value, asn1::f1ap::cause_c::types_opts::protocol);
    EXPECT_EQ(error.cause.protocol().value, asn1::f1ap::cause_protocol_opts::abstract_syntax_error_reject);
  }
}
