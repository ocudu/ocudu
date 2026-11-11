// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "lib/f1ap/du/ue_context/f1c_du_bearer_impl.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/common.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;
using namespace asn1::f1ap;

TEST_F(f1ap_du_test, when_sdu_is_received_then_sdu_is_forwarded_to_tx_pdu_notifier_as_ul_rrc_message_transfer)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  ue_test_context* ue  = run_f1ap_ue_create(to_du_ue_index(0));
  f1ap_message     msg = test_helpers::generate_ue_context_setup_request(
      gnb_cu_ue_f1ap_id_t{0}, gnb_du_ue_f1ap_id_t{0}, 1, {}, config_helpers::make_default_du_cell_config().nr_cgi);
  run_ue_context_setup_procedure(ue->ue_index, msg);
  this->f1c_gw.clear_tx_pdus();

  std::vector<uint8_t> bytes = test_rng::vector_of_uniform_ints<uint8_t>(test_rng::uniform_int<unsigned>(1, 4000));
  byte_buffer          sdu   = byte_buffer::create(bytes).value();
  ue->f1c_bearers[1].bearer->handle_sdu(byte_buffer_chain::create(sdu.copy()).value());

  ASSERT_EQ(this->f1c_gw.last_tx_pdu().pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->f1c_gw.last_tx_pdu().pdu.init_msg().proc_code, ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER);
  const ul_rrc_msg_transfer_s& ul_rrc_msg = this->f1c_gw.last_tx_pdu().pdu.init_msg().value.ul_rrc_msg_transfer();
  ASSERT_EQ(ul_rrc_msg->srb_id, 1);
  ASSERT_EQ(ul_rrc_msg->rrc_container, sdu);
}

TEST_F(f1ap_du_test, redcap_identity_is_encoded_in_initial_ul_rrc_transfer)
{
  run_f1_setup_procedure();
  ue_test_context* ue = run_f1ap_ue_create(to_du_ue_index(0), true);
  ASSERT_NE(ue, nullptr);
  byte_buffer sdu = byte_buffer::create({1, 2, 3, 4, 5, 6}).value();
  ue->f1c_bearers[0].bearer->handle_sdu(byte_buffer_chain::create(sdu.copy()).value());
  ctrl_worker.run_pending_tasks();

  byte_buffer   packed;
  asn1::bit_ref writer{packed};
  ASSERT_EQ(asn1::pack(writer, f1c_gw.last_tx_pdu().pdu), asn1::OCUDUASN_SUCCESS);
  asn1::cbit_ref reader{packed};
  f1ap_pdu_c     decoded;
  ASSERT_EQ(asn1::unpack(decoded, reader), asn1::OCUDUASN_SUCCESS);
  const auto& initial = decoded.init_msg().value.init_ul_rrc_msg_transfer();
  ASSERT_TRUE(initial->nr_red_cap_ue_ind_present);
  EXPECT_EQ(initial->nr_red_cap_ue_ind, nr_red_cap_ue_ind_opts::true_value);
  EXPECT_EQ(initial->rrc_container, sdu);
}

TEST_F(f1ap_du_test, ordinary_initial_ul_rrc_transfer_omits_redcap_identity)
{
  run_f1_setup_procedure();
  ue_test_context* ue = run_f1ap_ue_create(to_du_ue_index(0));
  ASSERT_NE(ue, nullptr);
  byte_buffer sdu = byte_buffer::create({1, 2, 3, 4, 5, 6}).value();
  ue->f1c_bearers[0].bearer->handle_sdu(byte_buffer_chain::create(std::move(sdu)).value());
  ctrl_worker.run_pending_tasks();
  EXPECT_FALSE(f1c_gw.last_tx_pdu().pdu.init_msg().value.init_ul_rrc_msg_transfer()->nr_red_cap_ue_ind_present);
}
