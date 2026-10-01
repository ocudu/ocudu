// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "lib/f1ap/asn1_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

class f1ap_du_gnbcu_config_update_test : public f1ap_du_test
{
public:
  f1ap_du_gnbcu_config_update_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();
    this->f1c_gw.clear_tx_pdus();
  }

  const f1_setup_request_message f1setupreq = generate_f1_setup_request_message();
};

TEST_F(f1ap_du_gnbcu_config_update_test, when_gnbcu_config_request_is_received_then_du_receives_cells_to_activate)
{
  // DU receives GNB-CU Config Update Request.
  std::vector<nr_cell_global_id_t> cgis = {
      nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(0).value()},
      nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(1).value()}};
  f1ap_message msg = test_helpers::generate_gnb_cu_configuration_update_request(0, cgis);
  this->f1ap->handle_message(msg);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_cu_upd_req.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_cu_upd_req.value().cells_to_activate.size(), 2);
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_cu_upd_req.value().cells_to_activate[0].cgi, cgis[0]);
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_cu_upd_req.value().cells_to_activate[1].cgi, cgis[1]);

  ASSERT_TRUE(test_helpers::is_gnb_cu_config_update_acknowledge_valid(this->f1c_gw.last_tx_pdu(), msg));
  ASSERT_FALSE(this->f1c_gw.last_tx_pdu()
                   .pdu.successful_outcome()
                   .value.gnb_cu_cfg_upd_ack()
                   ->cells_failed_to_be_activ_list_present);
}

TEST_F(f1ap_du_gnbcu_config_update_test, when_gnbcu_config_request_contains_cells_to_bar_then_du_receives_them)
{
  // DU receives GNB-CU Config Update Request carrying the Cells to be Barred List (one barred, one unbarred).
  std::vector<f1ap_cell_to_bar> cells_to_bar = {
      {nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(0).value()}, true},
      {nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(1).value()}, false}};
  f1ap_message msg = test_helpers::generate_gnb_cu_configuration_update_request(0, {}, {}, cells_to_bar);
  this->f1ap->handle_message(msg);

  // The decoded request carries the cells to bar with their CGIs and barred values.
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_cu_upd_req.has_value());
  const auto& du_req = this->f1ap_du_cfg_handler.last_cu_upd_req.value();
  ASSERT_EQ(du_req.cells_to_bar.size(), 2);
  ASSERT_EQ(du_req.cells_to_bar[0].cgi, cells_to_bar[0].cgi);
  ASSERT_TRUE(du_req.cells_to_bar[0].barred);
  ASSERT_EQ(du_req.cells_to_bar[1].cgi, cells_to_bar[1].cgi);
  ASSERT_FALSE(du_req.cells_to_bar[1].barred);

  // The DU acknowledges the update.
  ASSERT_TRUE(test_helpers::is_gnb_cu_config_update_acknowledge_valid(this->f1c_gw.last_tx_pdu(), msg));
}

TEST_F(f1ap_du_gnbcu_config_update_test, when_du_fails_to_start_cell_then_du_adds_cell_in_failed_to_activate_list)
{
  std::vector<nr_cell_global_id_t> cgis = {
      nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(0).value()},
      nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(1).value()}};
  this->f1ap_du_cfg_handler.next_cu_upd_resp.cells_failed_to_activate.push_back(cgis[0]);

  // DU receives GNB-CU Config Update Request.
  f1ap_message msg = test_helpers::generate_gnb_cu_configuration_update_request(0, cgis);
  this->f1ap->handle_message(msg);
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_cu_upd_req.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_cu_upd_req.value().cells_to_activate.size(), 2);

  // DU sends gNB-CU Config Update Acknowledge.
  ASSERT_TRUE(test_helpers::is_gnb_cu_config_update_acknowledge_valid(this->f1c_gw.last_tx_pdu(), msg));
  auto& upd_ack = *this->f1c_gw.last_tx_pdu().pdu.successful_outcome().value.gnb_cu_cfg_upd_ack();
  ASSERT_TRUE(upd_ack.cells_failed_to_be_activ_list_present);
  ASSERT_EQ(upd_ack.cells_failed_to_be_activ_list.size(), 1);
  ASSERT_EQ(cgi_from_asn1(upd_ack.cells_failed_to_be_activ_list[0]->cells_failed_to_be_activ_list_item().nr_cgi),
            cgis[0]);
}

TEST_F(f1ap_du_gnbcu_config_update_test, when_du_fails_to_start_all_cells_then_du_sends_cu_update_failure)
{
  std::vector<nr_cell_global_id_t> cgis = {
      nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(0).value()},
      nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(1).value()}};
  this->f1ap_du_cfg_handler.next_cu_upd_resp.cells_failed_to_activate.push_back(cgis[0]);
  this->f1ap_du_cfg_handler.next_cu_upd_resp.cells_failed_to_activate.push_back(cgis[1]);

  // DU receives GNB-CU Config Update Request.
  f1ap_message msg = test_helpers::generate_gnb_cu_configuration_update_request(0, cgis);
  this->f1ap->handle_message(msg);
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_cu_upd_req.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_cu_upd_req.value().cells_to_activate.size(), 2);

  // DU sends gNB-CU Config Update Failure.
  ASSERT_TRUE(test_helpers::is_gnb_cu_config_update_failure_valid(this->f1c_gw.last_tx_pdu(), msg));
}
