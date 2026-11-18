// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_to_fapi_fastpath_translator.h"
#include "pdu_translators/helpers.h"
#include "ocudu/fapi/p7/messages/dl_tti_request.h"
#include "ocudu/fapi/p7/messages/tx_data_request.h"
#include "ocudu/fapi/p7/messages/ul_dci_request.h"
#include "ocudu/fapi/p7/messages/ul_tti_request.h"
#include "ocudu/fapi/p7/p7_last_request_notifier.h"
#include "ocudu/fapi/p7/p7_requests_gateway.h"
#include "ocudu/fapi_adaptor/precoding_codebook_generator.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

namespace {

/// Spy implementation of a P7 requests gateway, that keeps the last message of each request.
class p7_requests_gateway_spy : public fapi::p7_requests_gateway
{
public:
  void send_dl_tti_request(const fapi::dl_tti_request& msg) override { dl_tti_msg = msg; }
  void send_ul_tti_request(const fapi::ul_tti_request& msg) override { ul_tti_msg = msg; }
  void send_ul_dci_request(const fapi::ul_dci_request& msg) override { ul_dci_msg = msg; }
  void send_tx_data_request(const fapi::tx_data_request& msg) override { tx_data_msg = msg; }

  std::optional<fapi::dl_tti_request>  dl_tti_msg;
  std::optional<fapi::ul_tti_request>  ul_tti_msg;
  std::optional<fapi::ul_dci_request>  ul_dci_msg;
  std::optional<fapi::tx_data_request> tx_data_msg;
};

class slot_last_message_notifier_spy : public fapi::p7_last_request_notifier
{
public:
  void on_last_message(slot_point slot) override { notified_slot = slot; }

  std::optional<slot_point> notified_slot;
};

class mac_to_fapi_translator_fixture : public ::testing::Test
{
  using precoding_tools =
      std::pair<std::unique_ptr<precoding_codebook_mapper>, std::unique_ptr<precoding_codebook_repository>>;
  using uci_part2_tools =
      std::pair<std::unique_ptr<uci_part2_correspondence_mapper>, std::unique_ptr<uci_part2_correspondence_repository>>;

protected:
  static constexpr unsigned nof_prbs  = 51U;
  static constexpr unsigned sector_id = 1U;

  mac_to_fapi_translator_fixture() :
    translator(mac_to_fapi_fastpath_translator_config{.cell_nof_prbs = nof_prbs, .sector_id = sector_id},
               mac_to_fapi_fastpath_translator_dependencies{.p7_gateway           = gateway_spy,
                                                            .p7_last_req_notifier = notifier_spy,
                                                            .pm_mapper            = std::move(pm_tools.first),
                                                            .part2_mapper         = std::move(uci_tools.first),
                                                            .fapi_logger = ocudulog::fetch_basic_logger("FAPI")})
  {
  }

  /// Returns a data result with one PDU of the given codewords for each kind of PDSCH grant, in the order SIB, RAR, UE
  /// and paging.
  static mac_dl_data_result_test_helper make_data_result(span<const unsigned> si_codewords,
                                                         span<const unsigned> rar_codewords,
                                                         span<const unsigned> ue_codewords,
                                                         span<const unsigned> paging_codewords)
  {
    mac_dl_data_result_test_helper helper;
    helper.data        = std::make_unique<std::array<uint8_t, 1>>();
    helper.result.slot = slot_point(subcarrier_spacing::kHz30, 1, 1);

    auto add_pdus = [&helper](auto& pdus, span<const unsigned> codewords) {
      for (unsigned cw : codewords) {
        pdus.emplace_back(cw, shared_transport_block(*helper.data));
      }
    };
    add_pdus(helper.result.si_pdus, si_codewords);
    add_pdus(helper.result.rar_pdus, rar_codewords);
    add_pdus(helper.result.ue_pdus, ue_codewords);
    add_pdus(helper.result.paging_pdus, paging_codewords);
    return helper;
  }

  p7_requests_gateway_spy        gateway_spy;
  slot_last_message_notifier_spy notifier_spy;
  precoding_tools pm_tools  = generate_precoding_codebooks(pmi_codebook_one_port{}, antenna_topology::one_port, 0);
  uci_part2_tools uci_tools = generate_uci_part2_correspondence(1);
  mac_to_fapi_fastpath_translator translator;
};

} // namespace

TEST_F(mac_to_fapi_translator_fixture, valid_dl_sched_results_generate_correct_dl_tti_request)
{
  mac_dl_sched_result_test_helper result_test = build_valid_mac_dl_sched_result();
  translator.on_new_downlink_scheduler_results(result_test.result);

  // Four PDCCH, two SSB and one PDSCH PDUs.
  ASSERT_TRUE(gateway_spy.dl_tti_msg.has_value());
  const fapi::dl_tti_request& msg = *gateway_spy.dl_tti_msg;
  EXPECT_EQ(4U, msg.pdcch_pdus.size());
  EXPECT_EQ(2U, msg.ssb_pdus.size());
  EXPECT_EQ(1U, msg.pdsch_pdus.size());
  EXPECT_TRUE(msg.csi_rs_pdus.empty());
  EXPECT_TRUE(msg.prs_pdus.empty());
}

TEST_F(mac_to_fapi_translator_fixture, valid_ul_sched_results_generate_correct_ul_tti_request)
{
  mac_ul_sched_result_test_helper result_test = build_valid_mac_ul_sched_result();
  translator.on_new_uplink_scheduler_results(result_test.result);

  // One PRACH, one PUSCH and two PUCCH PDUs.
  ASSERT_TRUE(gateway_spy.ul_tti_msg.has_value());
  const fapi::ul_tti_request& msg = *gateway_spy.ul_tti_msg;
  EXPECT_EQ(1U, msg.prach_pdus.size());
  EXPECT_EQ(1U, msg.pusch_pdus.size());
  EXPECT_EQ(2U, msg.pucch_pdus.size());
  EXPECT_TRUE(msg.srs_pdus.empty());
}

TEST_F(mac_to_fapi_translator_fixture, valid_dl_data_results_generate_correct_tx_data_request)
{
  mac_dl_sched_result_test_helper result_test = build_valid_mac_dl_sched_result();
  translator.on_new_downlink_scheduler_results(result_test.result);
  mac_dl_data_result_test_helper data_result = build_valid_mac_data_result();
  translator.on_new_downlink_data(data_result.result);

  ASSERT_TRUE(gateway_spy.tx_data_msg.has_value());
  EXPECT_EQ(1U, gateway_spy.tx_data_msg->pdus.size());
}

TEST_F(mac_to_fapi_translator_fixture, valid_dl_sched_results_generate_correct_ul_dci_request)
{
  mac_dl_sched_result_test_helper result_test = build_valid_mac_dl_sched_result();
  translator.on_new_downlink_scheduler_results(result_test.result);

  ASSERT_TRUE(gateway_spy.ul_dci_msg.has_value());
  EXPECT_EQ(1U, gateway_spy.ul_dci_msg->pdus.size());
}

TEST_F(mac_to_fapi_translator_fixture, last_message_is_notified)
{
  slot_point slot(1, 1, 1);

  ASSERT_FALSE(notifier_spy.notified_slot.has_value());
  translator.on_cell_results_completion(slot);
  EXPECT_EQ(slot, notifier_spy.notified_slot);
}

TEST_F(mac_to_fapi_translator_fixture, dl_tti_message_with_all_pdus_passes)
{
  mac_dl_sched_result_test_helper result_test = build_valid_mac_dl_sched_result_with_all_supported_pdus();
  translator.on_new_downlink_scheduler_results(result_test.result);

  ASSERT_TRUE(gateway_spy.ul_dci_msg.has_value());
  EXPECT_EQ(MAX_UL_PDCCH_PDUS_PER_SLOT, gateway_spy.ul_dci_msg->pdus.size());
  ASSERT_TRUE(gateway_spy.dl_tti_msg.has_value());
  const fapi::dl_tti_request& msg = *gateway_spy.dl_tti_msg;
  EXPECT_EQ(MAX_DL_PDCCH_PDUS_PER_SLOT, msg.pdcch_pdus.size());
  EXPECT_EQ(MAX_SSB_PER_SLOT, msg.ssb_pdus.size());
  EXPECT_EQ(MAX_CSI_RS_PDUS_PER_SLOT, msg.csi_rs_pdus.size());
  EXPECT_EQ(MAX_PRS_PDUS_PER_SLOT, msg.prs_pdus.size());
  EXPECT_EQ(MAX_SI_PDUS_PER_SLOT + MAX_RAR_PDUS_PER_SLOT + MAX_UE_PDUS_PER_SLOT + MAX_PAGING_PDUS_PER_SLOT,
            msg.pdsch_pdus.size());
}

TEST_F(mac_to_fapi_translator_fixture, tx_data_message_with_all_pdus_passes)
{
  mac_dl_sched_result_test_helper result_test = build_valid_mac_dl_sched_result_with_all_supported_pdus();
  translator.on_new_downlink_scheduler_results(result_test.result);
  mac_dl_data_result_test_helper data_result = build_valid_mac_data_result_with_all_supported_pdu();
  translator.on_new_downlink_data(data_result.result);

  ASSERT_TRUE(gateway_spy.tx_data_msg.has_value());
  EXPECT_EQ(MAX_SI_PDUS_PER_SLOT + MAX_RAR_PDUS_PER_SLOT + MAX_UE_PDUS_PER_SLOT + MAX_PAGING_PDUS_PER_SLOT,
            gateway_spy.tx_data_msg->pdus.size());
}

TEST_F(mac_to_fapi_translator_fixture, ul_tti_message_with_all_pdus_passes)
{
  mac_ul_sched_result_test_helper result_test = build_valid_mac_ul_sched_result_with_all_supported_pdus();
  translator.on_new_uplink_scheduler_results(result_test.result);

  ASSERT_TRUE(gateway_spy.ul_tti_msg.has_value());
  const fapi::ul_tti_request& msg = *gateway_spy.ul_tti_msg;
  EXPECT_EQ(MAX_PRACH_OCCASIONS_PER_SLOT, msg.prach_pdus.size());
  EXPECT_EQ(MAX_PUSCH_PDUS_PER_SLOT, msg.pusch_pdus.size());
  EXPECT_EQ(MAX_PUCCH_PDUS_PER_SLOT, msg.pucch_pdus.size());
  EXPECT_EQ(MAX_SRS_PDUS_PER_SLOT, msg.srs_pdus.size());
}

TEST_F(mac_to_fapi_translator_fixture, scheduler_results_slot_is_forwarded_in_every_request)
{
  mac_dl_sched_result_test_helper dl_result_test = build_valid_mac_dl_sched_result();
  translator.on_new_downlink_scheduler_results(dl_result_test.result);
  mac_dl_data_result_test_helper data_result_test = build_valid_mac_data_result();
  translator.on_new_downlink_data(data_result_test.result);
  mac_ul_sched_result_test_helper ul_result_test = build_valid_mac_ul_sched_result();
  translator.on_new_uplink_scheduler_results(ul_result_test.result);

  ASSERT_TRUE(gateway_spy.dl_tti_msg.has_value());
  ASSERT_TRUE(gateway_spy.ul_dci_msg.has_value());
  ASSERT_TRUE(gateway_spy.tx_data_msg.has_value());
  ASSERT_TRUE(gateway_spy.ul_tti_msg.has_value());
  EXPECT_EQ(dl_result_test.result.slot, gateway_spy.dl_tti_msg->slot);
  EXPECT_EQ(dl_result_test.result.slot, gateway_spy.ul_dci_msg->slot);
  EXPECT_EQ(data_result_test.result.slot, gateway_spy.tx_data_msg->slot);
  EXPECT_EQ(ul_result_test.result.slot, gateway_spy.ul_tti_msg->slot);
}

TEST_F(mac_to_fapi_translator_fixture, ul_dci_request_is_not_sent_when_there_are_no_ul_pdcch_pdus)
{
  mac_dl_sched_result_test_helper result_test;
  result_test.result.slot   = slot_point(subcarrier_spacing::kHz30, 1, 1);
  result_test.result.dl_res = result_test.sched_result;

  translator.on_new_downlink_scheduler_results(result_test.result);

  ASSERT_TRUE(gateway_spy.dl_tti_msg.has_value());
  EXPECT_TRUE(gateway_spy.dl_tti_msg->empty());
  EXPECT_FALSE(gateway_spy.ul_dci_msg.has_value());
}

/// Returns the number of CSI-RS PDUs for rate matching of each PDSCH PDU of the given DL_TTI.request.
static std::vector<unsigned> get_nof_csi_pdus_for_rm(const fapi::dl_tti_request& msg)
{
  std::vector<unsigned> nof_csi_pdus;
  for (const auto& pdsch : msg.pdsch_pdus) {
    nof_csi_pdus.push_back(pdsch.nof_csi_pdus_for_rm);
  }
  return nof_csi_pdus;
}

TEST_F(mac_to_fapi_translator_fixture, pdsch_pdus_carry_the_number_of_csi_rs_pdus_of_the_slot)
{
  mac_dl_sched_result_test_helper with_csi_rs = build_valid_mac_dl_sched_result_with_all_supported_pdus();
  translator.on_new_downlink_scheduler_results(with_csi_rs.result);

  ASSERT_TRUE(gateway_spy.dl_tti_msg.has_value());
  std::vector<unsigned> nof_csi_pdus = get_nof_csi_pdus_for_rm(*gateway_spy.dl_tti_msg);
  EXPECT_FALSE(nof_csi_pdus.empty());
  EXPECT_EQ(std::vector<unsigned>(nof_csi_pdus.size(), MAX_CSI_RS_PDUS_PER_SLOT), nof_csi_pdus);
}

TEST_F(mac_to_fapi_translator_fixture, pdsch_pdus_carry_no_csi_rs_pdus_when_the_slot_has_none)
{
  mac_dl_sched_result_test_helper without_csi_rs = build_valid_mac_dl_sched_result();
  translator.on_new_downlink_scheduler_results(without_csi_rs.result);

  ASSERT_TRUE(gateway_spy.dl_tti_msg.has_value());
  std::vector<unsigned> nof_csi_pdus = get_nof_csi_pdus_for_rm(*gateway_spy.dl_tti_msg);
  EXPECT_FALSE(nof_csi_pdus.empty());
  EXPECT_EQ(std::vector<unsigned>(nof_csi_pdus.size(), 0U), nof_csi_pdus);
}

TEST_F(mac_to_fapi_translator_fixture, tx_data_pdus_are_indexed_in_the_order_of_the_dl_tti_pdsch_pdus)
{
  // One grant of each kind, except for two UE grants, all of them with a single codeword.
  static constexpr std::array<unsigned, 1> one_grant  = {0};
  static constexpr std::array<unsigned, 2> two_grants = {0, 0};

  mac_dl_data_result_test_helper data_result = make_data_result(one_grant, one_grant, two_grants, one_grant);
  translator.on_new_downlink_data(data_result.result);

  ASSERT_TRUE(gateway_spy.tx_data_msg.has_value());
  const auto& pdus = gateway_spy.tx_data_msg->pdus;
  ASSERT_EQ(5U, pdus.size());
  for (unsigned i = 0, e = pdus.size(); i != e; ++i) {
    SCOPED_TRACE(i);
    EXPECT_EQ(i, pdus[i].pdu_index);
    EXPECT_EQ(0U, pdus[i].cw_index);
  }
}

TEST_F(mac_to_fapi_translator_fixture, tx_data_pdus_of_the_same_pdsch_pdu_share_its_index)
{
  // One SIB, then a UE grant with two codewords, a UE grant with one codeword and a paging grant.
  static constexpr std::array<unsigned, 1> one_cw    = {0};
  static constexpr std::array<unsigned, 3> ue_grants = {0, 1, 0};

  mac_dl_data_result_test_helper data_result = make_data_result(one_cw, {}, ue_grants, one_cw);
  translator.on_new_downlink_data(data_result.result);

  ASSERT_TRUE(gateway_spy.tx_data_msg.has_value());
  const auto& pdus = gateway_spy.tx_data_msg->pdus;
  ASSERT_EQ(5U, pdus.size());
  static constexpr std::array<std::pair<unsigned, unsigned>, 5> expected_index_and_codeword = {
      {{0, 0}, {1, 0}, {1, 1}, {2, 0}, {3, 0}}};
  for (unsigned i = 0, e = pdus.size(); i != e; ++i) {
    SCOPED_TRACE(i);
    EXPECT_EQ(expected_index_and_codeword[i].first, pdus[i].pdu_index);
    EXPECT_EQ(expected_index_and_codeword[i].second, pdus[i].cw_index);
  }
}

TEST_F(mac_to_fapi_translator_fixture, results_are_dropped_while_the_translator_is_stopped_and_sent_again_once_started)
{
  mac_dl_sched_result_test_helper dl_result_test   = build_valid_mac_dl_sched_result();
  mac_dl_data_result_test_helper  data_result_test = build_valid_mac_data_result();
  mac_ul_sched_result_test_helper ul_result_test   = build_valid_mac_ul_sched_result();

  auto send_results = [&]() {
    translator.on_new_downlink_scheduler_results(dl_result_test.result);
    translator.on_new_downlink_data(data_result_test.result);
    translator.on_new_uplink_scheduler_results(ul_result_test.result);
  };

  translator.stop();
  send_results();

  EXPECT_FALSE(gateway_spy.dl_tti_msg.has_value());
  EXPECT_FALSE(gateway_spy.ul_dci_msg.has_value());
  EXPECT_FALSE(gateway_spy.tx_data_msg.has_value());
  EXPECT_FALSE(gateway_spy.ul_tti_msg.has_value());

  translator.start();
  send_results();

  EXPECT_TRUE(gateway_spy.dl_tti_msg.has_value());
  EXPECT_TRUE(gateway_spy.tx_data_msg.has_value());
  EXPECT_TRUE(gateway_spy.ul_tti_msg.has_value());

  // The translator holds a stop token until the cell results are completed.
  translator.on_cell_results_completion(dl_result_test.result.slot);
}
