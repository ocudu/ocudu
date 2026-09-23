// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/rrc/rrc_packed_test_messages.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class cu_cp_paging_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_paging_test() : cu_cp_test_environment(cu_cp_test_env_params{})
  {
    // Run NG setup to completion.
    run_ng_setup();
  }

  [[nodiscard]] unsigned connect_du()
  {
    // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure).
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    return ret.value();
  }

  [[nodiscard]] unsigned setup_du(const f1ap_message& f1_setup_request)
  {
    // Setup DU.
    unsigned tmp_du_idx = connect_du();

    get_du(tmp_du_idx).push_ul_pdu(f1_setup_request);
    EXPECT_TRUE(this->wait_for_f1ap_tx_pdu(tmp_du_idx, f1ap_pdu));
    return tmp_du_idx;
  }

  [[nodiscard]] bool send_ngap_paging(unsigned du_idx_, const ngap_message& paging_msg)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx_).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");

    get_amf().push_tx_pdu(paging_msg);
    return true;
  }

  [[nodiscard]] bool send_minimal_ngap_paging_and_await_f1ap_paging(unsigned du_idx_)
  {
    // Inject NGAP Paging and wait for F1AP Paging.
    if (!send_ngap_paging(du_idx_, generate_valid_minimal_paging_message())) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx_, f1ap_pdu), "Failed to receive F1AP Paging");
    report_fatal_error_if_not(test_helpers::is_valid_paging(f1ap_pdu), "Invalid F1AP Paging");
    report_fatal_error_if_not(is_valid_minimal_paging_result(f1ap_pdu), "Invalid minimal F1AP Paging");
    return true;
  }

  [[nodiscard]] bool send_ngap_paging_and_await_f1ap_paging(unsigned du_idx_)
  {
    // Inject NGAP Paging and wait for F1AP Paging.
    if (!send_ngap_paging(du_idx_, generate_valid_paging_message())) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx_, f1ap_pdu), "Failed to receive F1AP Paging");
    report_fatal_error_if_not(test_helpers::is_valid_paging(f1ap_pdu), "Invalid F1AP Paging");
    report_fatal_error_if_not(is_valid_paging_result(f1ap_pdu), "Invalid F1AP Paging");
    return true;
  }

  [[nodiscard]] bool is_valid_minimal_paging_result(const f1ap_message& msg)
  {
    const auto& paging_msg = msg.pdu.init_msg().value.paging();

    // Check UE ID idx value.
    if (paging_msg->ue_id_idx_value.idx_len10().to_number() != (279089024671 % 1024)) {
      test_logger.error("UE ID idx value mismatch {} != {}",
                        paging_msg->ue_id_idx_value.idx_len10().to_number(),
                        (279089024671 % 1024));
      return false;
    }

    // Check paging ID.
    if (paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number() != 279089024671) {
      test_logger.error("Paging ID mismatch {} != {}",
                        paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number(),
                        279089024671);
      return false;
    }

    // Check paging cell list.
    if (paging_msg->paging_cell_list.size() != 1) {
      test_logger.error("Paging cell list size mismatch {} != {}", paging_msg->paging_cell_list.size(), 1);
      return false;
    }

    const auto&      paging_cell_item = paging_msg->paging_cell_list[0].value().paging_cell_item();
    nr_cell_identity nci              = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
    if (paging_cell_item.nr_cgi.nr_cell_id.to_number() != nci.value()) {
      test_logger.error("NR CGI NCI mismatch {} != {}", paging_cell_item.nr_cgi.nr_cell_id.to_number(), nci);
      return false;
    }
    if (paging_cell_item.nr_cgi.plmn_id.to_string() != "00f110") {
      test_logger.error("NR CGI PLMN mismatch {} != 00f110", paging_cell_item.nr_cgi.plmn_id.to_string());
      return false;
    }

    return true;
  }

  [[nodiscard]] bool is_valid_paging_result(const f1ap_message& msg)
  {
    if (!is_valid_minimal_paging_result(msg)) {
      return false;
    }

    const auto& paging_msg = msg.pdu.init_msg().value.paging();

    // Check paging DRX.
    if (!paging_msg->paging_drx_present) {
      return false;
    }
    if (paging_msg->paging_drx.to_number() != 64) {
      test_logger.error("Paging DRX mismatch {} != {}", paging_msg->paging_drx.to_number(), 64);
      return false;
    }

    // Check paging prio.
    if (!paging_msg->paging_prio_present) {
      return false;
    }
    if (paging_msg->paging_prio.to_number() != 5) {
      test_logger.error("Paging prio mismatch {} != {}", paging_msg->paging_prio.to_number(), 5);
      return false;
    }

    // Check paging origin.
    if (!paging_msg->paging_origin_present) {
      return false;
    }
    if ((std::string)paging_msg->paging_origin.to_string() != "non-3gpp") {
      test_logger.error("Paging origin mismatch {} != non-3gpp", paging_msg->paging_origin.to_string());
      return false;
    }

    return true;
  }

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
};

TEST_F(cu_cp_paging_test, when_du_connection_not_finished_then_paging_is_not_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure).
  unsigned du_idx = connect_du();

  // Inject NGAP Paging with only mandatory values.
  ASSERT_TRUE(send_ngap_paging(du_idx, generate_valid_minimal_paging_message()));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the DU.
  ASSERT_FALSE(this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_no_du_for_tac_exists_then_paging_is_not_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject NGAP Paging with unknown TAC.
  ngap_message paging_msg = generate_valid_minimal_paging_message();
  paging_msg.pdu.init_msg().value.paging()->tai_list_for_paging[0].tai.tac.from_number(8);
  ASSERT_TRUE(send_ngap_paging(du_idx, paging_msg));

  // Make sure the paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the DU.
  ASSERT_FALSE(this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_paged_tac_is_a_secondary_broadcast_tac_then_paging_is_sent_to_du)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-3");

  // Cell broadcasts TACs 7, 8 and 9 in trackingAreaList, TS 38.331. F1AP carries only the primary TAC, so the CU-CP
  // recovers the list from the SIB1 the DU provides.
  static const std::array<tac_t, 3> broadcast_tacs = {7, 8, 9};
  unsigned                          du_idx         = setup_du(test_helpers::generate_f1_setup_request(
      int_to_gnb_du_id(0x11),
      {test_helpers::served_cell_item_info{
                                           .tac = broadcast_tacs[0],
                                           .sib1_str = test_helpers::create_sib1_hex_string(plmn_identity::test_value(), broadcast_tacs)}}));

  // Page a TAC the cell broadcasts but which is not its primary TAC. Matching only the primary would drop this.
  ngap_message paging_msg = generate_valid_minimal_paging_message();
  paging_msg.pdu.init_msg().value.paging()->tai_list_for_paging[0].tai.tac.from_number(9);
  ASSERT_TRUE(send_ngap_paging(du_idx, paging_msg));

  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu)) << "F1AP Paging should have been sent to the DU";
  ASSERT_TRUE(test_helpers::is_valid_paging(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_paged_tac_is_outside_the_broadcast_tac_list_then_paging_is_not_sent_to_du)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-3");

  static const std::array<tac_t, 3> broadcast_tacs = {7, 8, 9};
  unsigned                          du_idx         = setup_du(test_helpers::generate_f1_setup_request(
      int_to_gnb_du_id(0x11),
      {test_helpers::served_cell_item_info{
                                           .tac = broadcast_tacs[0],
                                           .sib1_str = test_helpers::create_sib1_hex_string(plmn_identity::test_value(), broadcast_tacs)}}));

  // A TAC outside the broadcast list must still be rejected.
  ngap_message paging_msg = generate_valid_minimal_paging_message();
  paging_msg.pdu.init_msg().value.paging()->tai_list_for_paging[0].tai.tac.from_number(10);
  ASSERT_TRUE(send_ngap_paging(du_idx, paging_msg));

  ASSERT_FALSE(this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_assist_data_for_paging_for_unknown_tac_is_included_then_paging_is_not_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject NGAP Paging with unknown TAC but assist data for paging.
  ngap_message paging_msg = generate_valid_paging_message();
  paging_msg.pdu.init_msg().value.paging()->tai_list_for_paging[0].tai.tac.from_number(8);
  ASSERT_TRUE(send_ngap_paging(du_idx, paging_msg));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the DU.
  ASSERT_FALSE(this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_invalid_paging_message_received_then_paging_is_not_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject invalid NGAP Paging.
  ASSERT_TRUE(send_ngap_paging(du_idx, generate_invalid_paging_message()));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the DU.
  ASSERT_FALSE(this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_valid_paging_message_received_then_paging_is_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject NGAP Paging with only mandatory values and await F1AP Paging.
  ASSERT_TRUE(send_minimal_ngap_paging_and_await_f1ap_paging(du_idx));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";
}

TEST_F(cu_cp_paging_test, when_valid_paging_message_received_then_paging_is_only_sent_to_du_with_matching_tac)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Connect second DU and run F1Setup.
  unsigned du_idx2 = setup_du(test_helpers::generate_f1_setup_request(
      int_to_gnb_du_id(0x12), {{.nci = nr_cell_identity::create(6577).value(), .pci = 1, .tac = 8}}));

  // Inject NGAP Paging with only mandatory values and await F1AP Paging.
  ASSERT_TRUE(send_minimal_ngap_paging_and_await_f1ap_paging(du_idx));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the second DU.
  ASSERT_FALSE(this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_valid_paging_message_received_then_paging_is_only_sent_to_du_with_matching_nci)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Connect second DU and run F1Setup.
  unsigned du_idx2 = setup_du(test_helpers::generate_f1_setup_request(
      int_to_gnb_du_id(0x12), {{.nci = nr_cell_identity::create(6577).value(), .pci = 1, .tac = 7}}));

  // Connect third DU and run F1Setup.
  unsigned du_idx3 = setup_du(test_helpers::generate_f1_setup_request(
      int_to_gnb_du_id(0x13), {{.nci = nr_cell_identity::create(6578).value(), .pci = 2, .tac = 7}}));
  // Drop the second DU connection to test that paging is not sent to disconnected DUs.
  ASSERT_TRUE(drop_du_connection(du_idx2));

  // Inject NGAP Paging with only mandatory values and await F1AP Paging.
  ASSERT_TRUE(send_minimal_ngap_paging_and_await_f1ap_paging(du_idx));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the third DU.
  ASSERT_FALSE(this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_valid_paging_message_with_optional_values_received_then_paging_is_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject NGAP Paging with optional values and await F1AP Paging.
  ASSERT_TRUE(send_ngap_paging_and_await_f1ap_paging(du_idx));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";
}

/// Covers paging the cells the core names by a Mapped Cell ID, TS 38.300 sec. 16.14.5.
class cu_cp_paging_mapped_cell_id_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  static constexpr uint64_t uu_nci        = 0x66c000; // gnb_id 411, bit length 22, cell 0.
  static constexpr uint64_t second_uu_nci = 0x66c001;
  static constexpr uint64_t mapped_nci    = 0x66c0ff;
  /// A cell of the same tracking area that no Mapped Cell ID names, so that it is paged for its TAC alone.
  static constexpr uint64_t unmapped_uu_nci = 0x66c002;
  /// Tracking area the paging message pages.
  static constexpr tac_t paged_tac = 7;

  cu_cp_paging_mapped_cell_id_test() : cu_cp_test_environment(make_params()) { run_ng_setup(); }

protected:
  /// Both cells report the same Mapped Cell ID, so that one identity names the area the two of them cover.
  static cu_cp_test_env_params make_params()
  {
    cu_cp_test_env_params params{};
    for (uint64_t nci : {uu_nci, second_uu_nci}) {
      ntn_location_area area;
      area.tac        = 7;
      area.mapped_nci = nr_cell_identity::create(mapped_nci).value();
      area.lat_min    = 50.0;
      area.lat_max    = 52.0;
      area.lon_min    = 14.0;
      area.lon_max    = 17.0;

      ntn_cell_location_mapping cell_mapping{nr_cell_identity::create(nci).value(), ntn_location_mapping{}};
      cell_mapping.mapping.location_areas.push_back(area);
      params.ntn_location_mappings.push_back(cell_mapping);
    }
    return params;
  }

  /// Brings up a gNB-DU serving \c cells, each a Uu Cell ID with the tracking area it broadcasts.
  void connect_du_serving(const std::vector<std::pair<uint64_t, tac_t>>& cells)
  {
    std::vector<test_helpers::served_cell_item_info> served_cells;
    unsigned                                         pci = 0;
    for (const auto& [nci, tac] : cells) {
      served_cells.push_back(
          {.nci = nr_cell_identity::create(nci).value(), .pci = static_cast<pci_t>(pci++), .tac = tac});
    }

    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x11), served_cells));
    EXPECT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  }

  /// Brings up a gNB-DU serving \c ncis, all in the tracking area that the paging message pages.
  void connect_du_serving(std::initializer_list<uint64_t> ncis)
  {
    std::vector<std::pair<uint64_t, tac_t>> cells;
    cells.reserve(ncis.size());
    for (uint64_t nci : ncis) {
      cells.emplace_back(nci, paged_tac);
    }
    connect_du_serving(cells);
  }

  /// A paging message whose recommended cells are named by \c ncis, the identities the core knows them by.
  static ngap_message paging_recommending(std::initializer_list<uint64_t> ncis)
  {
    ngap_message msg  = generate_valid_paging_message();
    auto&        list = msg.pdu.init_msg()
                     .value.paging()
                     ->assist_data_for_paging.assist_data_for_recommended_cells.recommended_cells_for_paging
                     .recommended_cell_list;
    list.resize(0);
    for (uint64_t nci : ncis) {
      asn1::ngap::recommended_cell_item_s item;
      auto&                               nr_cgi = item.ngran_cgi.set_nr_cgi();
      nr_cgi.plmn_id.from_string("00f110");
      nr_cgi.nr_cell_id.from_number(nci);
      list.push_back(item);
    }
    return msg;
  }

  /// The cells the gNB-DU is asked to page in, in the order it is asked.
  std::vector<uint64_t> paged_cells() const
  {
    std::vector<uint64_t> ncis;
    for (const auto& item : f1ap_pdu.pdu.init_msg().value.paging()->paging_cell_list) {
      ncis.push_back(item.value().paging_cell_item().nr_cgi.nr_cell_id.to_number());
    }
    return ncis;
  }

  unsigned     du_idx = 0;
  f1ap_message f1ap_pdu;
};

TEST_F(cu_cp_paging_mapped_cell_id_test, a_cell_recommended_by_its_mapped_cell_id_is_paged_by_its_uu_cell_id)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2", "CU-NTN-LOC-3");

  // The core names the cell by the identity the gNB reported for it, TS 38.300 sec. 16.14.5, while the gNB-DU pages
  // the cells it knows by their Uu Cell ID, TS 38.473 sec. 8.7.1.2.
  connect_du_serving({uu_nci});

  get_amf().push_tx_pdu(paging_recommending({mapped_nci}));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  EXPECT_EQ(paged_cells(), std::vector<uint64_t>{uu_nci}) << "the Mapped Cell ID names no cell of this gNB-DU";
}

TEST_F(cu_cp_paging_mapped_cell_id_test, a_mapped_cell_id_covering_two_cells_pages_both)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2", "CU-NTN-LOC-3");

  // A Mapped Cell ID names a geographical area, and TS 38.300 sec. 16.14.5 leaves the mapping to configuration, so
  // more than one cell may cover it. Paging only the first would leave the rest of the area unpaged.
  connect_du_serving({uu_nci, second_uu_nci});

  get_amf().push_tx_pdu(paging_recommending({mapped_nci}));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  EXPECT_EQ(paged_cells(), (std::vector<uint64_t>{uu_nci, second_uu_nci}));
}

TEST_F(cu_cp_paging_mapped_cell_id_test, a_cell_recommended_by_both_identities_is_paged_once)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2", "CU-NTN-LOC-3");

  // The core may name one cell by its Uu Cell ID and by the Mapped Cell ID of an area it covers. Both resolve to the
  // same cell, which must not be paged twice.
  connect_du_serving({uu_nci});

  get_amf().push_tx_pdu(paging_recommending({uu_nci, mapped_nci}));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  EXPECT_EQ(paged_cells(), std::vector<uint64_t>{uu_nci}) << "the cell is named twice, so it must not be paged twice";
}

TEST_F(cu_cp_paging_mapped_cell_id_test, a_recommended_cell_is_paged_before_the_rest_of_the_tracking_area)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2", "CU-NTN-LOC-3");

  // Every served cell of a paged tracking area is paged whether or not the core recommended it, so what a
  // recommendation decides is the order, TS 38.413 sec. 9.3.1.70 making it assistance data rather than a selection.
  connect_du_serving({unmapped_uu_nci, uu_nci});

  get_amf().push_tx_pdu(paging_recommending({mapped_nci}));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  EXPECT_EQ(paged_cells(), (std::vector<uint64_t>{uu_nci, unmapped_uu_nci}))
      << "no Mapped Cell ID names unmapped_uu_nci, so it is paged for its TAC alone and last";
}

TEST_F(cu_cp_paging_mapped_cell_id_test, a_mapped_cell_id_does_not_page_a_cell_outside_the_paged_tracking_area)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2", "CU-NTN-LOC-3");

  // The cells one Mapped Cell ID covers need not broadcast the same TAC: TS 38.300 sec. 16.14.3.1 does not
  // synchronise the TAC in system information with the illumination on ground. The paged tracking area still decides.
  connect_du_serving({{uu_nci, paged_tac}, {second_uu_nci, 9}});

  get_amf().push_tx_pdu(paging_recommending({mapped_nci}));

  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  EXPECT_EQ(paged_cells(), std::vector<uint64_t>{uu_nci})
      << "second_uu_nci covers the Mapped Cell ID, but broadcasts a TAC the paging does not name";
}
