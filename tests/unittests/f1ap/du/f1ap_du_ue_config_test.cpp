// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace odu;

class f1ap_du_ue_management_tester : public f1ap_du_test
{
protected:
  dummy_f1c_rx_sdu_notifier srb2_rx_sdu_notifier;
  dummy_f1u_rx_sdu_notifier drb1_rx_sdu_notifier;

  f1ap_ue_configuration_request create_ue_config_update_request(du_ue_index_t ue_index)
  {
    f1ap_ue_configuration_request req;
    req.ue_index = ue_index;
    // > SRB2
    req.f1c_bearers_to_add.emplace_back();
    req.f1c_bearers_to_add[0].srb_id          = srb_id_t::srb2;
    req.f1c_bearers_to_add[0].rx_sdu_notifier = &srb2_rx_sdu_notifier;
    return req;
  }
};

TEST_F(f1ap_du_ue_management_tester, f1ap_ue_config_update_returns_valid_response)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  // Run Test Preamble.
  run_f1_setup_procedure();
  run_f1ap_ue_create(ue_index);

  // Process F1AP UE configuration.
  f1ap_ue_configuration_request  req  = create_ue_config_update_request(ue_index);
  f1ap_ue_configuration_response resp = f1ap->handle_ue_configuration_request(req);
  ASSERT_EQ(resp.f1c_bearers_added.size(), 1);
  ASSERT_EQ(resp.f1c_bearers_added[0].srb_id, req.f1c_bearers_to_add[0].srb_id);
  ASSERT_NE(resp.f1c_bearers_added[0].bearer, nullptr);
}

TEST_F(f1ap_du_ue_management_tester, f1ap_created_bearers_forward_messages_to_notifiers)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  // Run Test Preamble.
  run_f1_setup_procedure();
  run_f1ap_ue_create(ue_index);

  // Process F1AP UE configuration.
  f1ap_ue_configuration_request  req  = create_ue_config_update_request(ue_index);
  f1ap_ue_configuration_response resp = f1ap->handle_ue_configuration_request(req);

  // Send DL data through created F1-C bearer.
  byte_buffer dl_srb_buf =
      byte_buffer::create(test_rng::vector_of_uniform_ints<uint8_t>(test_rng::uniform_int<unsigned>(3, 100))).value();
  ASSERT_TRUE(this->srb2_rx_sdu_notifier.last_pdu.empty());
  f1ap->handle_message(test_helpers::generate_dl_rrc_message_transfer(
      int_to_gnb_du_ue_f1ap_id(0), int_to_gnb_cu_ue_f1ap_id(0), srb_id_t::srb2, dl_srb_buf.copy()));
  ASSERT_EQ(dl_srb_buf, this->srb2_rx_sdu_notifier.last_pdu);

  // Send UL data through created F1-C bearer.
  byte_buffer ul_srb_buf =
      byte_buffer::create(test_rng::vector_of_uniform_ints<uint8_t>(test_rng::uniform_int<unsigned>(3, 100))).value();
  resp.f1c_bearers_added[0].bearer->handle_sdu(byte_buffer_chain::create(ul_srb_buf.copy()).value());
  const auto& ul_f1ap_msg = this->f1c_gw.last_tx_pdu().pdu.init_msg().value.ul_rrc_msg_transfer();
  ASSERT_EQ(ul_f1ap_msg->rrc_container, ul_srb_buf);
  ASSERT_EQ((srb_id_t)ul_f1ap_msg->srb_id, srb_id_t::srb2);
}

TEST_F(f1ap_du_ue_management_tester, f1ap_created_bearers_do_not_forward_invalid_dl_messages_to_notifiers)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  // Run Test Preamble.
  run_f1_setup_procedure();
  run_f1ap_ue_create(ue_index);

  // Process F1AP UE configuration.
  f1ap_ue_configuration_request  req  = create_ue_config_update_request(ue_index);
  f1ap_ue_configuration_response resp = f1ap->handle_ue_configuration_request(req);

  // Send DL data through created F1-C bearer.
  byte_buffer dl_srb_buf =
      byte_buffer::create(test_rng::vector_of_uniform_ints<uint8_t>(test_rng::uniform_int<unsigned>(1, 2))).value();
  ASSERT_TRUE(this->srb2_rx_sdu_notifier.last_pdu.empty());
  f1ap->handle_message(test_helpers::generate_dl_rrc_message_transfer(
      int_to_gnb_du_ue_f1ap_id(0), int_to_gnb_cu_ue_f1ap_id(0), srb_id_t::srb2, dl_srb_buf.copy()));
  ASSERT_TRUE(this->srb2_rx_sdu_notifier.last_pdu.empty());
}
