// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fapi_to_mac_indications_fastpath_translator.h"
#include "ocudu/fapi/p7/messages/crc_indication.h"
#include "ocudu/fapi/p7/messages/rach_indication.h"
#include "ocudu/fapi/p7/messages/rx_data_indication.h"
#include "ocudu/fapi/p7/messages/srs_indication.h"
#include "ocudu/fapi/p7/messages/uci_indication.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <array>
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;
using namespace fapi_adaptor;

using detection_status = uci_pusch_or_pucch_f2_3_4_detection_status;

/// Slot of the indications built by the tests.
static const slot_point test_slot = slot_point(subcarrier_spacing::kHz30, 1, 3);
/// RNTI of the indications built by the tests.
static constexpr rnti_t test_rnti = to_rnti(0x4601);

/// Conversion factors of the FAPI power units built by the tests. They are distinct and non-zero so that using the
/// wrong conversion (normalized dB, dBm) is detected.
static constexpr float test_dbfs_to_dbm_factor = 20.F;
static constexpr float test_db_to_dbfs_factor  = -3.F;

/// Builds a FAPI power unit from the given normalized value, using the test conversion factors.
static fapi::fapi_power_unit make_power(float value_dB)
{
  return fapi::fapi_power_unit(value_dB, test_dbfs_to_dbm_factor, test_db_to_dbfs_factor);
}

/// Returns the power in dBFS that the translator is expected to report for the given normalized value.
static float to_expected_dBFS(float value_dB)
{
  return value_dB + test_db_to_dbfs_factor;
}

/// Returns true when both bitsets have the same size and the same bits set.
template <typename BitsetA, typename BitsetB>
static bool have_same_bits(const BitsetA& a, const BitsetB& b)
{
  if (a.size() != b.size()) {
    return false;
  }
  for (unsigned i = 0, e = a.size(); i != e; ++i) {
    if (a.test(i) != b.test(i)) {
      return false;
    }
  }
  return true;
}

/// Builds a payload with a recognizable bit pattern.
static uci_payload_type make_payload(unsigned nof_bits)
{
  uci_payload_type payload(nof_bits);
  for (unsigned i = 0; i < nof_bits; i += 3) {
    payload.set(i);
  }
  return payload;
}

/// Builds a HARQ-ACK report with the given detection status and a payload of the expected length.
static fapi::uci_harq_pdu make_harq(detection_status status, unsigned nof_bits)
{
  return fapi::uci_harq_pdu{
      .detection_status = status, .expected_bit_length = units::bits(nof_bits), .payload = make_payload(nof_bits)};
}

/// Builds a HARQ-ACK report that has no payload, as the PHY reports an undetected one.
static fapi::uci_harq_pdu make_undetected_harq(unsigned expected_nof_bits)
{
  return fapi::uci_harq_pdu{
      .detection_status = detection_status::dtx, .expected_bit_length = units::bits(expected_nof_bits), .payload = {}};
}

/// Builds a CSI Part 1 report with the given detection status and a payload of the expected length.
static fapi::uci_csi_part1 make_csi_part1(detection_status status, unsigned nof_bits)
{
  return fapi::uci_csi_part1{
      .detection_status = status, .expected_bit_length = units::bits(nof_bits), .payload = make_payload(nof_bits)};
}

/// Builds a CSI Part 2 report with the given detection status and a payload of the expected length.
static fapi::uci_csi_part2 make_csi_part2(detection_status status, unsigned nof_bits)
{
  return fapi::uci_csi_part2{
      .detection_status = status, .expected_bit_length = units::bits(nof_bits), .payload = make_payload(nof_bits)};
}

/// Builds a 2x2 SRS channel matrix with distinct coefficients.
static srs_channel_matrix make_channel_matrix()
{
  return srs_channel_matrix({cf_t{1.F, 0.F}, cf_t{0.F, 1.F}, cf_t{-1.F, 0.F}, cf_t{0.F, -1.F}}, 2, 2);
}

/// Builds a RACH.indication whose PRACH occasion is at the given slot index and that carries no preambles.
static fapi::rach_indication build_rach_indication(unsigned slot_index)
{
  return fapi::rach_indication{.slot = slot_point(subcarrier_spacing::kHz30, 1, 0),
                               .pdu  = fapi::rach_indication_pdu{.handle       = 0,
                                                                 .symbol_index = 4,
                                                                 .slot_index   = static_cast<uint8_t>(slot_index),
                                                                 .ra_index     = 1,
                                                                 .avg_rssi     = std::nullopt,
                                                                 .avg_snr_dB   = std::nullopt,
                                                                 .preambles    = {}}};
}

/// Builds a preamble with the given index and timing advance.
static fapi::rach_indication_pdu_preamble build_preamble(unsigned index, std::optional<phy_time_unit> timing_advance)
{
  return fapi::rach_indication_pdu_preamble{.preamble_index        = static_cast<uint8_t>(index),
                                            .timing_advance_offset = timing_advance,
                                            .preamble_pwr          = make_power(-10.F),
                                            .preamble_snr_dB       = std::nullopt};
}

namespace {

class mac_cell_rach_handler_spy : public mac_cell_rach_handler
{
public:
  void handle_rach_indication(const mac_rach_indication& rach_ind) override
  {
    last_ind = rach_ind;
    ++nof_indications;
  }

  std::optional<rnti_t>
  handle_msga_ccch_sdu(rnti_t ra_rnti, uint8_t rapid, slot_point sl_rx, const ue_con_res_id_t& con_res_id) override
  {
    return std::nullopt;
  }

  std::optional<ue_con_res_id_t> resolve_msga_con_res_id(rnti_t tc_rnti) override { return std::nullopt; }

  mac_rach_indication last_ind;
  unsigned            nof_indications = 0;
};

class mac_cell_control_information_handler_spy : public mac_cell_control_information_handler
{
public:
  void handle_crc(const mac_crc_indication_message& msg) override { last_crc_ind = msg; }
  void handle_uci(const mac_uci_indication_message& msg) override { last_uci_ind = msg; }
  void handle_srs(const mac_srs_indication_message& msg) override { last_srs_ind = msg; }

  std::optional<mac_crc_indication_message> last_crc_ind;
  std::optional<mac_uci_indication_message> last_uci_ind;
  std::optional<mac_srs_indication_message> last_srs_ind;
};

class mac_pdu_handler_spy : public mac_pdu_handler
{
public:
  void handle_rx_data_indication(mac_rx_data_indication pdu) override
  {
    last_ind = std::move(pdu);
    ++nof_indications;
  }

  std::optional<mac_rx_data_indication> last_ind;
  unsigned                              nof_indications = 0;
};

/// Fixture that connects the translator to a spy of every MAC handler.
class fapi_to_mac_indications_fixture : public ::testing::Test
{
protected:
  static constexpr unsigned sector_id = 2;

  fapi_to_mac_indications_fixture()
  {
    translator.set_cell_rach_handler(rach_handler);
    translator.set_cell_pdu_handler(pdu_handler);
    translator.set_cell_crc_handler(control_handler);
  }

  mac_cell_rach_handler_spy                   rach_handler;
  mac_pdu_handler_spy                         pdu_handler;
  mac_cell_control_information_handler_spy    control_handler;
  fapi_to_mac_indications_fastpath_translator translator{sector_id, ocudulog::fetch_basic_logger("FAPI")};
};

} // namespace

// RACH.indication.

namespace {

class mac_rach_indication_fixture : public fapi_to_mac_indications_fixture, public ::testing::WithParamInterface<float>
{
protected:
  /// Sends a RACH.indication with the given powers and checks the indication received by the MAC.
  void test_pdu(float rssi_dB = 14.F, float power_dB = 13.F)
  {
    fapi::rach_indication msg = build_rach_indication(slot_index);
    msg.pdu.ra_index          = freq_index;
    msg.pdu.avg_rssi          = make_power(rssi_dB);
    fapi::rach_indication_pdu_preamble& preamble =
        msg.pdu.preambles.emplace_back(build_preamble(preamble_index, time_advance));
    preamble.preamble_pwr = make_power(power_dB);

    translator.on_rach_indication(msg);

    ASSERT_EQ(1U, rach_handler.nof_indications);
    ASSERT_EQ(1U, rach_handler.last_ind.occasions.size());
    const mac_rach_indication::rach_occasion& occ = rach_handler.last_ind.occasions.front();
    EXPECT_EQ(msg.pdu.symbol_index, occ.start_symbol);
    EXPECT_EQ(slot_index, occ.slot_index);
    EXPECT_EQ(freq_index, occ.frequency_index);
    ASSERT_TRUE(occ.rssi_dBFS.has_value());
    EXPECT_FLOAT_EQ(to_expected_dBFS(rssi_dB), *occ.rssi_dBFS);

    ASSERT_EQ(1U, occ.preambles.size());
    const mac_rach_indication::rach_preamble& mac_preamble = occ.preambles.front();
    EXPECT_EQ(preamble_index, mac_preamble.index);
    ASSERT_TRUE(mac_preamble.pwr_dBFS.has_value());
    EXPECT_FLOAT_EQ(to_expected_dBFS(power_dB), *mac_preamble.pwr_dBFS);
    EXPECT_EQ(time_advance, mac_preamble.time_advance);
  }

  static constexpr unsigned slot_index     = 3;
  static constexpr unsigned freq_index     = 5;
  static constexpr unsigned preamble_index = 7;
  const phy_time_unit       time_advance   = phy_time_unit::from_units_of_Tc(16);
};

} // namespace

TEST_P(mac_rach_indication_fixture, power_values_are_converted_to_dbfs)
{
  test_pdu(GetParam(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(PowerValues, mac_rach_indication_fixture, testing::Values(-140.F, -20.F, 0.F, 30.F, 63.F));

TEST_F(mac_rach_indication_fixture, rach_indication_is_converted_correctly)
{
  test_pdu();
}

TEST_F(fapi_to_mac_indications_fixture, last_valid_occasion_slot_index_is_forwarded)
{
  static constexpr unsigned max_occasion_slot_index = 79;

  fapi::rach_indication msg = build_rach_indication(max_occasion_slot_index);
  msg.pdu.preambles.push_back(build_preamble(3, phy_time_unit::from_units_of_Tc(16)));
  translator.on_rach_indication(msg);

  ASSERT_EQ(1U, rach_handler.nof_indications);
  EXPECT_EQ(max_occasion_slot_index, rach_handler.last_ind.occasions.front().slot_index);
}

TEST_F(fapi_to_mac_indications_fixture, occasion_slot_index_above_the_maximum_is_discarded)
{
  static constexpr unsigned first_invalid_slot_index = 80;

  fapi::rach_indication msg = build_rach_indication(first_invalid_slot_index);
  msg.pdu.preambles.push_back(build_preamble(3, phy_time_unit::from_units_of_Tc(16)));
  translator.on_rach_indication(msg);

  EXPECT_EQ(0U, rach_handler.nof_indications);
}

TEST_F(fapi_to_mac_indications_fixture, preamble_without_timing_advance_is_ignored_and_the_others_are_kept)
{
  fapi::rach_indication msg = build_rach_indication(3);
  msg.pdu.preambles.push_back(build_preamble(10, std::nullopt));
  msg.pdu.preambles.push_back(build_preamble(11, phy_time_unit::from_units_of_Tc(32)));
  translator.on_rach_indication(msg);

  ASSERT_EQ(1U, rach_handler.nof_indications);
  const auto& preambles = rach_handler.last_ind.occasions.front().preambles;
  ASSERT_EQ(1U, preambles.size());
  EXPECT_EQ(11U, preambles.front().index);
  EXPECT_EQ(phy_time_unit::from_units_of_Tc(32), preambles.front().time_advance);
}

TEST_F(fapi_to_mac_indications_fixture, occasion_without_any_valid_preamble_is_discarded)
{
  // None of the preambles carries a timing advance, so none can be reported to the MAC.
  fapi::rach_indication msg = build_rach_indication(3);
  msg.pdu.preambles.push_back(build_preamble(10, std::nullopt));
  msg.pdu.preambles.push_back(build_preamble(11, std::nullopt));
  translator.on_rach_indication(msg);

  EXPECT_EQ(0U, rach_handler.nof_indications);
}

// Rx_Data.indication.

TEST_F(fapi_to_mac_indications_fixture, transport_block_is_forwarded_to_the_pdu_handler)
{
  std::array<uint8_t, 4> transport_block{0xde, 0xad, 0xbe, 0xef};

  fapi::rx_data_indication msg{
      .slot = test_slot,
      .pdu  = fapi::rx_data_indication_pdu{
           .handle = 0, .rnti = test_rnti, .rapid = 5, .harq_id = to_harq_id(3), .transport_block = transport_block}};

  translator.on_rx_data_indication(msg);

  ASSERT_TRUE(pdu_handler.last_ind.has_value());
  EXPECT_EQ(msg.slot, pdu_handler.last_ind->sl_rx);
  EXPECT_EQ(to_du_cell_index(sector_id), pdu_handler.last_ind->cell_index);
  ASSERT_EQ(1U, pdu_handler.last_ind->pdus.size());
  const mac_rx_pdu& pdu = pdu_handler.last_ind->pdus.front();
  EXPECT_EQ(msg.pdu.rnti, pdu.rnti);
  EXPECT_EQ(static_cast<unsigned>(msg.pdu.harq_id), static_cast<unsigned>(pdu.harq_id));
  EXPECT_EQ(msg.pdu.rapid, pdu.rapid);
  EXPECT_EQ(std::vector<uint8_t>(transport_block.begin(), transport_block.end()),
            std::vector<uint8_t>(pdu.pdu.begin(), pdu.pdu.end()));
}

TEST_F(fapi_to_mac_indications_fixture, empty_transport_block_is_not_forwarded)
{
  // A PDU that was not successfully decoded has a zero length transport block.
  fapi::rx_data_indication msg{
      .slot = test_slot,
      .pdu  = fapi::rx_data_indication_pdu{
           .handle = 0, .rnti = test_rnti, .rapid = std::nullopt, .harq_id = {}, .transport_block = {}}};

  translator.on_rx_data_indication(msg);

  EXPECT_EQ(0U, pdu_handler.nof_indications);
}

// CRC.indication.

TEST_F(fapi_to_mac_indications_fixture, crc_pdu_is_forwarded_with_the_measurements_in_dbfs)
{
  static constexpr float sinr_dB = 12.5F;
  static constexpr float rsrp_dB = -50.F;

  fapi::crc_indication msg{.slot = test_slot,
                           .pdu  = fapi::crc_ind_pdu{.handle                = 0,
                                                     .rnti                  = test_rnti,
                                                     .rapid                 = 7,
                                                     .harq_id               = to_harq_id(2),
                                                     .tb_crc_status_ok      = true,
                                                     .ul_sinr_metric_dB     = sinr_dB,
                                                     .timing_advance_offset = phy_time_unit::from_units_of_Tc(28),
                                                     .rssi                  = std::nullopt,
                                                     .rsrp                  = make_power(rsrp_dB)}};

  translator.on_crc_indication(msg);

  ASSERT_TRUE(control_handler.last_crc_ind.has_value());
  EXPECT_EQ(msg.slot, control_handler.last_crc_ind->sl_rx);
  ASSERT_EQ(1U, control_handler.last_crc_ind->crcs.size());
  const mac_crc_pdu& pdu = control_handler.last_crc_ind->crcs.front();
  EXPECT_EQ(msg.pdu.rnti, pdu.rnti);
  EXPECT_EQ(static_cast<unsigned>(msg.pdu.harq_id), static_cast<unsigned>(pdu.harq_id));
  EXPECT_EQ(msg.pdu.rapid, pdu.rapid);
  EXPECT_TRUE(pdu.tb_crc_success);
  EXPECT_EQ(msg.pdu.timing_advance_offset, pdu.time_advance_offset);
  ASSERT_TRUE(pdu.ul_sinr_dB.has_value());
  EXPECT_FLOAT_EQ(sinr_dB, *pdu.ul_sinr_dB);
  ASSERT_TRUE(pdu.ul_rsrp_dBFS.has_value());
  EXPECT_FLOAT_EQ(to_expected_dBFS(rsrp_dB), *pdu.ul_rsrp_dBFS);
}

TEST_F(fapi_to_mac_indications_fixture, failed_crc_without_measurements_is_forwarded_without_them)
{
  fapi::crc_indication msg{.slot = test_slot,
                           .pdu  = fapi::crc_ind_pdu{.handle                = 0,
                                                     .rnti                  = test_rnti,
                                                     .rapid                 = std::nullopt,
                                                     .harq_id               = to_harq_id(2),
                                                     .tb_crc_status_ok      = false,
                                                     .ul_sinr_metric_dB     = std::nullopt,
                                                     .timing_advance_offset = std::nullopt,
                                                     .rssi                  = std::nullopt,
                                                     .rsrp                  = std::nullopt}};

  translator.on_crc_indication(msg);

  ASSERT_TRUE(control_handler.last_crc_ind.has_value());
  ASSERT_EQ(1U, control_handler.last_crc_ind->crcs.size());
  const mac_crc_pdu& pdu = control_handler.last_crc_ind->crcs.front();
  EXPECT_FALSE(pdu.tb_crc_success);
  EXPECT_FALSE(pdu.rapid.has_value());
  EXPECT_FALSE(pdu.ul_sinr_dB.has_value());
  EXPECT_FALSE(pdu.ul_rsrp_dBFS.has_value());
  EXPECT_FALSE(pdu.time_advance_offset.has_value());
}

// UCI.indication.

namespace {

class mac_uci_indication_fixture : public fapi_to_mac_indications_fixture
{
protected:
  /// Sends the given PDU in a UCI.indication and returns true when exactly one UCI PDU reached the MAC.
  template <typename FapiUciPdu>
  bool send(const FapiUciPdu& fapi_pdu)
  {
    fapi::uci_indication msg{.slot = test_slot, .pdu = fapi_pdu};
    translator.on_uci_indication(msg);
    return control_handler.last_uci_ind.has_value() && control_handler.last_uci_ind->ucis.size() == 1;
  }

  /// Returns the only UCI PDU received by the MAC.
  const mac_uci_pdu& received_pdu() const { return control_handler.last_uci_ind->ucis.front(); }

  /// Returns the only UCI PDU received by the MAC as the given type, or nullptr when it is of another type.
  template <typename MacUciPdu>
  const MacUciPdu* received_pdu_as() const
  {
    return std::get_if<MacUciPdu>(&received_pdu().pdu);
  }

  /// Checks that the MAC PDU carries the measurements of the FAPI PDU, which were built with the given powers.
  template <typename FapiUciPdu, typename MacUciPdu>
  static void expect_measurements(const FapiUciPdu& fapi_pdu, const MacUciPdu& mac_pdu, float rssi_dB, float rsrp_dB)
  {
    ASSERT_TRUE(mac_pdu.ul_sinr_dB.has_value());
    EXPECT_FLOAT_EQ(*fapi_pdu.ul_sinr_metric_dB, *mac_pdu.ul_sinr_dB);
    EXPECT_EQ(fapi_pdu.timing_advance_offset, mac_pdu.time_advance_offset);
    ASSERT_TRUE(mac_pdu.rssi_dBFS.has_value());
    EXPECT_FLOAT_EQ(to_expected_dBFS(rssi_dB), *mac_pdu.rssi_dBFS);
    ASSERT_TRUE(mac_pdu.rsrp_dBFS.has_value());
    EXPECT_FLOAT_EQ(to_expected_dBFS(rsrp_dB), *mac_pdu.rsrp_dBFS);
  }

  /// Checks that the MAC PDU carries no measurement.
  template <typename MacUciPdu>
  static void expect_no_measurements(const MacUciPdu& mac_pdu)
  {
    EXPECT_FALSE(mac_pdu.ul_sinr_dB.has_value());
    EXPECT_FALSE(mac_pdu.time_advance_offset.has_value());
    EXPECT_FALSE(mac_pdu.rssi_dBFS.has_value());
    EXPECT_FALSE(mac_pdu.rsrp_dBFS.has_value());
  }
};

} // namespace

TEST_F(mac_uci_indication_fixture, pusch_uci_with_harq_and_csi_is_converted)
{
  static constexpr float rssi_dB = -40.F;
  static constexpr float rsrp_dB = -45.F;

  fapi::uci_pusch_pdu fapi_pdu{.handle                = 0,
                               .rnti                  = test_rnti,
                               .ul_sinr_metric_dB     = 9.5F,
                               .timing_advance_offset = phy_time_unit::from_units_of_Tc(56),
                               .rssi                  = make_power(rssi_dB),
                               .rsrp                  = make_power(rsrp_dB),
                               .harq                  = make_harq(detection_status::crc_pass, 4),
                               .csi_part1             = make_csi_part1(detection_status::crc_pass, 8),
                               .csi_part2             = make_csi_part2(detection_status::no_dtx, 10)};

  ASSERT_TRUE(send(fapi_pdu));

  EXPECT_EQ(test_slot, control_handler.last_uci_ind->sl_rx);
  EXPECT_EQ(fapi_pdu.rnti, received_pdu().rnti);
  const auto* pusch = received_pdu_as<mac_uci_pdu::pusch_type>();
  ASSERT_NE(pusch, nullptr);
  expect_measurements(fapi_pdu, *pusch, rssi_dB, rsrp_dB);

  ASSERT_TRUE(pusch->harq_info.has_value());
  EXPECT_TRUE(pusch->harq_info->is_valid);
  EXPECT_TRUE(have_same_bits(fapi_pdu.harq->payload, pusch->harq_info->payload));
  ASSERT_TRUE(pusch->csi_part1_info.has_value());
  EXPECT_TRUE(pusch->csi_part1_info->is_valid);
  EXPECT_TRUE(have_same_bits(fapi_pdu.csi_part1->payload, pusch->csi_part1_info->payload));
  ASSERT_TRUE(pusch->csi_part2_info.has_value());
  EXPECT_TRUE(pusch->csi_part2_info->is_valid);
  EXPECT_TRUE(have_same_bits(fapi_pdu.csi_part2->payload, pusch->csi_part2_info->payload));
}

TEST_F(mac_uci_indication_fixture, pusch_uci_without_harq_payload_is_reported_as_undetected)
{
  static constexpr unsigned expected_nof_bits = 4;

  fapi::uci_pusch_pdu fapi_pdu{.handle                = 0,
                               .rnti                  = test_rnti,
                               .ul_sinr_metric_dB     = std::nullopt,
                               .timing_advance_offset = std::nullopt,
                               .rssi                  = std::nullopt,
                               .rsrp                  = std::nullopt,
                               .harq                  = make_undetected_harq(expected_nof_bits),
                               .csi_part1             = std::nullopt,
                               .csi_part2             = std::nullopt};

  ASSERT_TRUE(send(fapi_pdu));

  const auto* pusch = received_pdu_as<mac_uci_pdu::pusch_type>();
  ASSERT_NE(pusch, nullptr);
  ASSERT_TRUE(pusch->harq_info.has_value());
  EXPECT_FALSE(pusch->harq_info->is_valid);
  EXPECT_EQ(expected_nof_bits, pusch->harq_info->payload.size());
  EXPECT_FALSE(pusch->csi_part1_info.has_value());
  EXPECT_FALSE(pusch->csi_part2_info.has_value());
}

TEST_F(mac_uci_indication_fixture, pusch_uci_without_reports_or_measurements_forwards_nothing_optional)
{
  fapi::uci_pusch_pdu fapi_pdu{.handle                = 0,
                               .rnti                  = test_rnti,
                               .ul_sinr_metric_dB     = std::nullopt,
                               .timing_advance_offset = std::nullopt,
                               .rssi                  = std::nullopt,
                               .rsrp                  = std::nullopt,
                               .harq                  = std::nullopt,
                               .csi_part1             = std::nullopt,
                               .csi_part2             = std::nullopt};

  ASSERT_TRUE(send(fapi_pdu));

  const auto* pusch = received_pdu_as<mac_uci_pdu::pusch_type>();
  ASSERT_NE(pusch, nullptr);
  expect_no_measurements(*pusch);
  EXPECT_FALSE(pusch->harq_info.has_value());
  EXPECT_FALSE(pusch->csi_part1_info.has_value());
  EXPECT_FALSE(pusch->csi_part2_info.has_value());
}

namespace {

/// CSI detection status and whether the MAC must consider the report valid for it.
struct csi_detection_param {
  detection_status status;
  bool             expected_valid;
};

class mac_uci_csi_detection_fixture : public mac_uci_indication_fixture,
                                      public ::testing::WithParamInterface<csi_detection_param>
{};

} // namespace

TEST_P(mac_uci_csi_detection_fixture, csi_reports_are_valid_only_when_detected)
{
  fapi::uci_pusch_pdu fapi_pdu{.handle                = 0,
                               .rnti                  = test_rnti,
                               .ul_sinr_metric_dB     = std::nullopt,
                               .timing_advance_offset = std::nullopt,
                               .rssi                  = std::nullopt,
                               .rsrp                  = std::nullopt,
                               .harq                  = std::nullopt,
                               .csi_part1             = make_csi_part1(GetParam().status, 8),
                               .csi_part2             = make_csi_part2(GetParam().status, 8)};

  ASSERT_TRUE(send(fapi_pdu));

  const auto* pusch = received_pdu_as<mac_uci_pdu::pusch_type>();
  ASSERT_NE(pusch, nullptr);
  ASSERT_TRUE(pusch->csi_part1_info.has_value());
  ASSERT_TRUE(pusch->csi_part2_info.has_value());
  EXPECT_EQ(GetParam().expected_valid, pusch->csi_part1_info->is_valid);
  EXPECT_EQ(GetParam().expected_valid, pusch->csi_part2_info->is_valid);
}

INSTANTIATE_TEST_SUITE_P(
    detection_statuses,
    mac_uci_csi_detection_fixture,
    ::testing::Values(csi_detection_param{.status = detection_status::crc_pass, .expected_valid = true},
                      csi_detection_param{.status = detection_status::no_dtx, .expected_valid = true},
                      csi_detection_param{.status = detection_status::crc_failure, .expected_valid = false},
                      csi_detection_param{.status = detection_status::dtx, .expected_valid = false},
                      csi_detection_param{.status = detection_status::dtx_not_checked, .expected_valid = false}),
    [](const ::testing::TestParamInfo<csi_detection_param>& param_info) {
      return fmt::format("status_{}", static_cast<unsigned>(param_info.param.status));
    });

TEST_F(mac_uci_indication_fixture, pucch_format_0_1_uci_with_sr_and_harq_is_converted)
{
  static constexpr float rssi_dB = -41.F;
  static constexpr float rsrp_dB = -46.F;

  fapi::uci_harq_format_0_1 harq{.harq_values = {}};
  harq.harq_values.push_back(uci_pucch_f0_or_f1_harq_values::ack);
  harq.harq_values.push_back(uci_pucch_f0_or_f1_harq_values::nack);

  fapi::uci_pucch_pdu_format_0_1 fapi_pdu{.handle            = 0,
                                          .rnti              = test_rnti,
                                          .pucch_format      = fapi::uci_pucch_pdu_format_0_1::format_type::format_1,
                                          .ul_sinr_metric_dB = 7.5F,
                                          .timing_advance_offset = phy_time_unit::from_units_of_Tc(84),
                                          .rssi                  = make_power(rssi_dB),
                                          .rsrp                  = make_power(rsrp_dB),
                                          .sr                    = fapi::sr_pdu_format_0_1{.sr_detected = true},
                                          .harq                  = harq};

  ASSERT_TRUE(send(fapi_pdu));

  EXPECT_EQ(fapi_pdu.rnti, received_pdu().rnti);
  const auto* pucch = received_pdu_as<mac_uci_pdu::pucch_f0_or_f1_type>();
  ASSERT_NE(pucch, nullptr);
  expect_measurements(fapi_pdu, *pucch, rssi_dB, rsrp_dB);

  ASSERT_TRUE(pucch->sr_info.has_value());
  EXPECT_TRUE(pucch->sr_info->detected);
  ASSERT_TRUE(pucch->harq_info.has_value());
  ASSERT_EQ(2U, pucch->harq_info->harqs.size());
  EXPECT_EQ(uci_pucch_f0_or_f1_harq_values::ack, pucch->harq_info->harqs[0]);
  EXPECT_EQ(uci_pucch_f0_or_f1_harq_values::nack, pucch->harq_info->harqs[1]);
}

TEST_F(mac_uci_indication_fixture, pucch_format_0_1_uci_without_sr_or_harq_forwards_nothing_optional)
{
  fapi::uci_pucch_pdu_format_0_1 fapi_pdu{.handle            = 0,
                                          .rnti              = test_rnti,
                                          .pucch_format      = fapi::uci_pucch_pdu_format_0_1::format_type::format_0,
                                          .ul_sinr_metric_dB = std::nullopt,
                                          .timing_advance_offset = std::nullopt,
                                          .rssi                  = std::nullopt,
                                          .rsrp                  = std::nullopt,
                                          .sr                    = std::nullopt,
                                          .harq                  = std::nullopt};

  ASSERT_TRUE(send(fapi_pdu));

  const auto* pucch = received_pdu_as<mac_uci_pdu::pucch_f0_or_f1_type>();
  ASSERT_NE(pucch, nullptr);
  expect_no_measurements(*pucch);
  EXPECT_FALSE(pucch->sr_info.has_value());
  EXPECT_FALSE(pucch->harq_info.has_value());
}

TEST_F(mac_uci_indication_fixture, pucch_format_0_1_sr_not_detected_is_forwarded_as_not_detected)
{
  fapi::uci_pucch_pdu_format_0_1 fapi_pdu{.handle            = 0,
                                          .rnti              = test_rnti,
                                          .pucch_format      = fapi::uci_pucch_pdu_format_0_1::format_type::format_1,
                                          .ul_sinr_metric_dB = std::nullopt,
                                          .timing_advance_offset = std::nullopt,
                                          .rssi                  = std::nullopt,
                                          .rsrp                  = std::nullopt,
                                          .sr                    = fapi::sr_pdu_format_0_1{.sr_detected = false},
                                          .harq                  = std::nullopt};

  ASSERT_TRUE(send(fapi_pdu));

  const auto* pucch = received_pdu_as<mac_uci_pdu::pucch_f0_or_f1_type>();
  ASSERT_NE(pucch, nullptr);
  ASSERT_TRUE(pucch->sr_info.has_value());
  EXPECT_FALSE(pucch->sr_info->detected);
}

TEST_F(mac_uci_indication_fixture, pucch_format_2_3_4_uci_with_all_reports_is_converted)
{
  static constexpr float rssi_dB = -42.F;
  static constexpr float rsrp_dB = -47.F;

  bounded_bitset<4> sr_payload(4);
  sr_payload.set(1);

  fapi::uci_pucch_pdu_format_2_3_4 fapi_pdu{.handle       = 0,
                                            .rnti         = test_rnti,
                                            .pucch_format = fapi::uci_pucch_pdu_format_2_3_4::format_type::format_3,
                                            .ul_sinr_metric_dB     = 6.5F,
                                            .timing_advance_offset = phy_time_unit::from_units_of_Tc(112),
                                            .rssi                  = make_power(rssi_dB),
                                            .rsrp                  = make_power(rsrp_dB),
                                            .sr        = fapi::sr_pdu_format_2_3_4{.sr_payload = sr_payload},
                                            .harq      = make_harq(detection_status::crc_pass, 4),
                                            .csi_part1 = make_csi_part1(detection_status::crc_pass, 8),
                                            .csi_part2 = make_csi_part2(detection_status::crc_failure, 10)};

  ASSERT_TRUE(send(fapi_pdu));

  EXPECT_EQ(fapi_pdu.rnti, received_pdu().rnti);
  const auto* pucch = received_pdu_as<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>();
  ASSERT_NE(pucch, nullptr);
  expect_measurements(fapi_pdu, *pucch, rssi_dB, rsrp_dB);

  ASSERT_TRUE(pucch->sr_info.has_value());
  EXPECT_EQ(fapi_pdu.sr->sr_payload, *pucch->sr_info);
  ASSERT_TRUE(pucch->harq_info.has_value());
  EXPECT_TRUE(pucch->harq_info->is_valid);
  EXPECT_TRUE(have_same_bits(fapi_pdu.harq->payload, pucch->harq_info->payload));
  ASSERT_TRUE(pucch->csi_part1_info.has_value());
  EXPECT_TRUE(pucch->csi_part1_info->is_valid);
  EXPECT_TRUE(have_same_bits(fapi_pdu.csi_part1->payload, pucch->csi_part1_info->payload));
  ASSERT_TRUE(pucch->csi_part2_info.has_value());
  EXPECT_FALSE(pucch->csi_part2_info->is_valid);
}

TEST_F(mac_uci_indication_fixture, pucch_format_2_3_4_uci_without_harq_payload_is_reported_as_undetected)
{
  static constexpr unsigned expected_nof_bits = 6;

  fapi::uci_pucch_pdu_format_2_3_4 fapi_pdu{.handle       = 0,
                                            .rnti         = test_rnti,
                                            .pucch_format = fapi::uci_pucch_pdu_format_2_3_4::format_type::format_2,
                                            .ul_sinr_metric_dB     = std::nullopt,
                                            .timing_advance_offset = std::nullopt,
                                            .rssi                  = std::nullopt,
                                            .rsrp                  = std::nullopt,
                                            .sr                    = std::nullopt,
                                            .harq                  = make_undetected_harq(expected_nof_bits),
                                            .csi_part1             = std::nullopt,
                                            .csi_part2             = std::nullopt};

  ASSERT_TRUE(send(fapi_pdu));

  const auto* pucch = received_pdu_as<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>();
  ASSERT_NE(pucch, nullptr);
  ASSERT_TRUE(pucch->harq_info.has_value());
  EXPECT_FALSE(pucch->harq_info->is_valid);
  EXPECT_EQ(expected_nof_bits, pucch->harq_info->payload.size());
  EXPECT_FALSE(pucch->sr_info.has_value());
  EXPECT_FALSE(pucch->csi_part1_info.has_value());
  EXPECT_FALSE(pucch->csi_part2_info.has_value());
}

TEST_F(mac_uci_indication_fixture, uci_indication_without_a_uci_pdu_is_not_forwarded)
{
  fapi::uci_indication msg{.slot = test_slot, .pdu = {}};

  translator.on_uci_indication(msg);

  EXPECT_FALSE(control_handler.last_uci_ind.has_value());
}

// SRS.indication.

TEST_F(fapi_to_mac_indications_fixture, positioning_report_with_aoa_converts_correctly)
{
  static constexpr float rsrp_dB = -50.F;

  fapi::srs_indication fapi_msg{
      .slot = test_slot,
      .pdu  = fapi::srs_indication_pdu{
           .handle                = 0,
           .rnti                  = test_rnti,
           .timing_advance_offset = std::nullopt,
           .matrix                = std::nullopt,
           .positioning           = fapi::srs_positioning_report{.ul_relative_toa = phy_time_unit::from_units_of_Tc(28),
                                                                 .rsrp            = make_power(rsrp_dB),
                                                                 .azimuth_aoa_deg = -45.F,
                                                                 .zenith_aoa_deg  = 90.5F}}};
  translator.on_srs_indication(fapi_msg);

  ASSERT_TRUE(control_handler.last_srs_ind.has_value()) << "No SRS indication was forwarded to MAC";
  EXPECT_EQ(fapi_msg.slot, control_handler.last_srs_ind->sl_rx);
  ASSERT_EQ(1, control_handler.last_srs_ind->srss.size());
  const mac_srs_pdu& pdu = control_handler.last_srs_ind->srss.front();
  EXPECT_EQ(fapi_msg.pdu.rnti, pdu.rnti);

  const auto* report = std::get_if<mac_srs_pdu::positioning_report>(&pdu.report);
  ASSERT_NE(nullptr, report) << "The SRS report is not a positioning report";
  EXPECT_EQ(fapi_msg.pdu.positioning->ul_relative_toa, report->ul_rtoa);
  ASSERT_TRUE(report->ul_rsrp_dBFS.has_value());
  EXPECT_FLOAT_EQ(to_expected_dBFS(rsrp_dB), *report->ul_rsrp_dBFS);
  EXPECT_EQ(fapi_msg.pdu.positioning->azimuth_aoa_deg, report->azimuth_aoa);
  EXPECT_EQ(fapi_msg.pdu.positioning->zenith_aoa_deg, report->zenith_aoa);
}

TEST_F(fapi_to_mac_indications_fixture, normalized_channel_iq_matrix_is_forwarded)
{
  srs_channel_matrix   matrix = make_channel_matrix();
  fapi::srs_indication fapi_msg{
      .slot = test_slot,
      .pdu  = fapi::srs_indication_pdu{.handle                = 0,
                                       .rnti                  = test_rnti,
                                       .timing_advance_offset = phy_time_unit::from_units_of_Tc(140),
                                       .matrix                = matrix,
                                       .positioning           = std::nullopt}};

  translator.on_srs_indication(fapi_msg);

  ASSERT_TRUE(control_handler.last_srs_ind.has_value());
  ASSERT_EQ(1U, control_handler.last_srs_ind->srss.size());
  const mac_srs_pdu& pdu = control_handler.last_srs_ind->srss.front();
  EXPECT_EQ(fapi_msg.pdu.rnti, pdu.rnti);
  EXPECT_EQ(fapi_msg.pdu.timing_advance_offset, pdu.time_advance_offset);
  const auto* report = std::get_if<mac_srs_pdu::normalized_channel_iq_matrix>(&pdu.report);
  ASSERT_NE(report, nullptr) << "The SRS report is not a normalized channel IQ matrix";
  const srs_channel_matrix& forwarded = report->channel_matrix;
  ASSERT_EQ(matrix.get_nof_rx_ports(), forwarded.get_nof_rx_ports());
  ASSERT_EQ(matrix.get_nof_tx_ports(), forwarded.get_nof_tx_ports());
  for (unsigned i_rx = 0, nof_rx = matrix.get_nof_rx_ports(); i_rx != nof_rx; ++i_rx) {
    for (unsigned i_tx = 0, nof_tx = matrix.get_nof_tx_ports(); i_tx != nof_tx; ++i_tx) {
      EXPECT_EQ(matrix.get_coefficient(i_rx, i_tx), forwarded.get_coefficient(i_rx, i_tx))
          << "rx port " << i_rx << ", tx port " << i_tx;
    }
  }
}

TEST_F(fapi_to_mac_indications_fixture, matrix_and_positioning_reports_are_forwarded_as_separate_pdus)
{
  fapi::srs_indication fapi_msg{
      .slot = test_slot,
      .pdu  = fapi::srs_indication_pdu{
           .handle                = 0,
           .rnti                  = test_rnti,
           .timing_advance_offset = phy_time_unit::from_units_of_Tc(140),
           .matrix                = make_channel_matrix(),
           .positioning           = fapi::srs_positioning_report{.ul_relative_toa = phy_time_unit::from_units_of_Tc(28),
                                                                 .rsrp            = make_power(-50.F),
                                                                 .azimuth_aoa_deg = -45.F,
                                                                 .zenith_aoa_deg  = std::nullopt}}};

  translator.on_srs_indication(fapi_msg);

  ASSERT_TRUE(control_handler.last_srs_ind.has_value());
  ASSERT_EQ(2U, control_handler.last_srs_ind->srss.size());
  unsigned nof_matrix_reports      = 0;
  unsigned nof_positioning_reports = 0;
  for (const mac_srs_pdu& pdu : control_handler.last_srs_ind->srss) {
    EXPECT_EQ(fapi_msg.pdu.rnti, pdu.rnti);
    EXPECT_EQ(fapi_msg.pdu.timing_advance_offset, pdu.time_advance_offset);
    nof_matrix_reports += std::holds_alternative<mac_srs_pdu::normalized_channel_iq_matrix>(pdu.report) ? 1 : 0;
    nof_positioning_reports += std::holds_alternative<mac_srs_pdu::positioning_report>(pdu.report) ? 1 : 0;
  }
  EXPECT_EQ(1U, nof_matrix_reports);
  EXPECT_EQ(1U, nof_positioning_reports);
}

TEST_F(fapi_to_mac_indications_fixture, srs_pdu_without_any_report_is_not_forwarded)
{
  fapi::srs_indication fapi_msg{
      .slot = test_slot,
      .pdu  = fapi::srs_indication_pdu{.handle                = 0,
                                       .rnti                  = test_rnti,
                                       .timing_advance_offset = phy_time_unit::from_units_of_Tc(140),
                                       .matrix                = std::nullopt,
                                       .positioning           = std::nullopt}};

  translator.on_srs_indication(fapi_msg);

  EXPECT_FALSE(control_handler.last_srs_ind.has_value());
}
