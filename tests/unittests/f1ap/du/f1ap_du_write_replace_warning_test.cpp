// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "lib/f1ap/asn1_helpers.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;
using namespace asn1::f1ap;

namespace {

f1ap_message generate_write_replace_warning_request(uint8_t                                         sib_type,
                                                    const std::vector<uint8_t>&                     sib_bytes,
                                                    uint32_t                                        repeat_period,
                                                    uint32_t                                        nof_broadcasts,
                                                    std::optional<std::vector<nr_cell_global_id_t>> cells)
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_WRITE_REPLACE_WARNING);
  write_replace_warning_request_s& req = msg.pdu.init_msg().value.write_replace_warning_request();
  req->transaction_id                  = 0;
  req->pws_sys_info.sib_type           = sib_type;
  req->pws_sys_info.sib_msg.from_bytes(sib_bytes);
  req->repeat_period           = repeat_period;
  req->numof_broadcast_request = nof_broadcasts;

  if (cells.has_value()) {
    req->cells_to_be_broadcast_list_present = true;
    for (const auto& cgi : *cells) {
      asn1::protocol_ie_single_container_s<cells_to_be_broadcast_list_item_ies_o> item;
      item->cells_to_be_broadcast_item().nr_cgi = cgi_to_asn1(cgi);
      req->cells_to_be_broadcast_list.push_back(item);
    }
  }

  return msg;
}

} // namespace

class f1ap_du_write_replace_warning_test : public f1ap_du_test
{
protected:
  f1ap_du_write_replace_warning_test()
  {
    run_f1_setup_procedure();
    this->f1c_gw.clear_tx_pdus();
  }

  const nr_cell_global_id_t served_cell_cgi = config_helpers::make_default_du_cell_config().nr_cgi;
};

TEST_F(f1ap_du_write_replace_warning_test, when_cells_list_is_absent_then_all_served_cells_are_targeted)
{
  pws_handler.cells_to_accept = {du_cell_index_t::MIN_DU_CELL_INDEX};

  f1ap_message msg = generate_write_replace_warning_request(6, {0x1, 0x2, 0x3}, 60, 4, std::nullopt);
  this->f1ap->handle_message(msg);

  ASSERT_TRUE(this->pws_handler.last_pws_info.has_value());
  const auto& info = this->pws_handler.last_pws_info.value();
  ASSERT_EQ(info.sib_type, 6);
  ASSERT_EQ(info.repeat_period, std::chrono::seconds{60});
  ASSERT_EQ(info.nof_broadcasts_requested, 4U);
  ASSERT_EQ(info.cells.size(), 1U);
  ASSERT_EQ(info.cells[0], du_cell_index_t::MIN_DU_CELL_INDEX);

  ASSERT_EQ(this->f1c_gw.last_tx_pdu().pdu.type().value, f1ap_pdu_c::types_opts::successful_outcome);
  const auto& resp = this->f1c_gw.last_tx_pdu().pdu.successful_outcome().value.write_replace_warning_resp();
  ASSERT_TRUE(resp->cells_broadcast_completed_list_present);
  ASSERT_EQ(resp->cells_broadcast_completed_list.size(), 1U);
  auto reported_cgi = cgi_from_asn1(resp->cells_broadcast_completed_list[0]->cells_broadcast_completed_item().nr_cgi);
  ASSERT_TRUE(reported_cgi.has_value());
  ASSERT_EQ(reported_cgi.value(), served_cell_cgi);
}

TEST_F(f1ap_du_write_replace_warning_test, when_cell_list_has_unknown_cgi_then_it_is_omitted)
{
  nr_cell_global_id_t unknown_cgi{plmn_identity::test_value(), nr_cell_identity::create(123456).value()};

  f1ap_message msg =
      generate_write_replace_warning_request(8, {0x1, 0x2, 0x3}, 0, 1, std::vector<nr_cell_global_id_t>{unknown_cgi});
  this->f1ap->handle_message(msg);

  ASSERT_TRUE(this->pws_handler.last_pws_info.has_value());
  ASSERT_TRUE(this->pws_handler.last_pws_info.value().cells.empty());

  const auto& resp = this->f1c_gw.last_tx_pdu().pdu.successful_outcome().value.write_replace_warning_resp();
  ASSERT_FALSE(resp->cells_broadcast_completed_list_present);
}

TEST_F(f1ap_du_write_replace_warning_test, when_matching_cgi_provided_then_it_is_resolved_and_forwarded)
{
  pws_handler.cells_to_accept = {du_cell_index_t::MIN_DU_CELL_INDEX};

  f1ap_message msg =
      generate_write_replace_warning_request(7, {0x1}, 30, 2, std::vector<nr_cell_global_id_t>{served_cell_cgi});
  this->f1ap->handle_message(msg);

  ASSERT_TRUE(this->pws_handler.last_pws_info.has_value());
  ASSERT_EQ(this->pws_handler.last_pws_info.value().cells.size(), 1U);
  ASSERT_EQ(this->pws_handler.last_pws_info.value().cells[0], du_cell_index_t::MIN_DU_CELL_INDEX);
}
