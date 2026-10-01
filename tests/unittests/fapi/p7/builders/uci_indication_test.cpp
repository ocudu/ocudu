// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/p7/builders/uci_indication_builder.h"
#include "fmt/chrono.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(uci_indication_builder, valid_pusch_pdu_metrics_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pusch_pdu         pdu;
    uci_pusch_pdu_builder builder(pdu);

    std::optional<float>           ul_sinr_metric;
    std::optional<phy_time_unit>   timing_advance_offset;
    std::optional<fapi_power_unit> rssi;
    std::optional<fapi_power_unit> rsrp;

    if (i) {
      ul_sinr_metric.emplace(-30.1F);
      timing_advance_offset.emplace(phy_time_unit::from_seconds(10));
      rssi.emplace(fapi_power_unit{-59.3F, 0, 0});
      rsrp.emplace(fapi_power_unit{-36.2F, 0, 0});
    }

    builder.set_metrics_parameters(ul_sinr_metric, timing_advance_offset, rssi, rsrp);

    ASSERT_EQ(ul_sinr_metric, pdu.ul_sinr_metric_dB);
    ASSERT_EQ(timing_advance_offset, pdu.timing_advance_offset);
    ASSERT_EQ(rssi, pdu.rssi);
    ASSERT_EQ(rsrp, pdu.rsrp);
  }
}

TEST(uci_indication_builder, valid_pusch_pdu_harq_passes)
{
  uci_pusch_pdu         pdu;
  uci_pusch_pdu_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(5);
  units::bits                                bit_length(1023);
  uci_payload_type                           payload(bit_length.value());

  builder.set_harq_parameters(status, bit_length, payload);

  const auto& harq = pdu.harq;
  ASSERT_TRUE(harq.has_value());
  ASSERT_EQ(status, harq->detection_status);
  ASSERT_EQ(bit_length, harq->expected_bit_length);
  ASSERT_EQ(payload, harq->payload);
}

TEST(uci_indication_builder, valid_pusch_pdu_csi_part1_passes)
{
  uci_pusch_pdu         pdu;
  uci_pusch_pdu_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(1);
  units::bits                                bit_length(129);
  uci_payload_type                           payload(bit_length.value());

  builder.set_csi_part1_parameters(status, bit_length, payload);

  const auto& csi = pdu.csi_part1;
  ASSERT_TRUE(csi.has_value());
  ASSERT_EQ(status, csi->detection_status);
  ASSERT_EQ(bit_length, csi->expected_bit_length);
  ASSERT_EQ(payload, csi->payload);
}

TEST(uci_indication_builder, valid_pusch_pdu_csi_part2_passes)
{
  uci_pusch_pdu         pdu;
  uci_pusch_pdu_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(3);
  units::bits                                bit_length(98);
  uci_payload_type                           payload(bit_length.value());

  builder.set_csi_part2_parameters(status, bit_length, payload);

  const auto& csi = pdu.csi_part2;
  ASSERT_TRUE(csi.has_value());
  ASSERT_EQ(status, csi->detection_status);
  ASSERT_EQ(bit_length, csi->expected_bit_length);
  ASSERT_EQ(payload, csi->payload);
}

TEST(uci_indication_builder, valid_pucch_f01_pdu_metrics_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pucch_pdu_format_0_1         pdu;
    uci_pucch_pdu_format_0_1_builder builder(pdu);

    std::optional<float>           ul_sinr_metric;
    std::optional<phy_time_unit>   timing_advance_offset;
    std::optional<fapi_power_unit> rssi;
    std::optional<fapi_power_unit> rsrp;

    if (i) {
      ul_sinr_metric.emplace(-30.1F);
      timing_advance_offset.emplace(phy_time_unit::from_seconds(10));
      rssi.emplace(fapi_power_unit{-59.3F, 0, 0});
      rsrp.emplace(fapi_power_unit{-36.2F, 0, 0});
    }

    builder.set_time_advance(timing_advance_offset).set_metrics_parameters(ul_sinr_metric, rssi, rsrp);

    ASSERT_EQ(ul_sinr_metric, pdu.ul_sinr_metric_dB);
    ASSERT_EQ(timing_advance_offset, pdu.timing_advance_offset);
    ASSERT_EQ(rssi, pdu.rssi);
    ASSERT_EQ(rsrp, pdu.rsrp);
  }
}

TEST(uci_indication_builder, valid_pucch_f01_pdu_sr_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pucch_pdu_format_0_1         pdu;
    uci_pucch_pdu_format_0_1_builder builder(pdu);

    builder.set_sr_parameters(i);

    const auto& sr_pdu = pdu.sr;
    ASSERT_TRUE(sr_pdu.has_value());
    ASSERT_EQ(i, sr_pdu->sr_detected);
  }
}

TEST(uci_indication_builder, valid_pucch_f01_pdu_harq_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pucch_pdu_format_0_1         pdu;
    uci_pucch_pdu_format_0_1_builder builder(pdu);

    static_vector<uci_pucch_f0_or_f1_harq_values, uci_harq_format_0_1::MAX_NUM_HARQ> payload = {
        uci_pucch_f0_or_f1_harq_values::ack, uci_pucch_f0_or_f1_harq_values::ack};

    builder.set_harq_parameters({payload});

    const auto& harq = pdu.harq;
    ASSERT_TRUE(harq.has_value());
    ASSERT_EQ(payload, harq->harq_values);
  }
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_metrics_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uci_pucch_pdu_format_2_3_4         pdu;
    uci_pucch_pdu_format_2_3_4_builder builder(pdu);

    std::optional<float>           ul_sinr_metric;
    std::optional<phy_time_unit>   timing_advance_offset;
    std::optional<fapi_power_unit> rssi;
    std::optional<fapi_power_unit> rsrp;

    if (i) {
      ul_sinr_metric.emplace(-30.1F);
      timing_advance_offset.emplace(phy_time_unit::from_seconds(10));
      rssi.emplace(fapi_power_unit{-59.3F, 0, 0});
      rsrp.emplace(fapi_power_unit{-36.2F, 0, 0});
    }

    builder.set_metrics_parameters(ul_sinr_metric, timing_advance_offset, rssi, rsrp);

    ASSERT_EQ(ul_sinr_metric, pdu.ul_sinr_metric_dB);
    ASSERT_EQ(timing_advance_offset, pdu.timing_advance_offset);
    ASSERT_EQ(rssi, pdu.rssi);
    ASSERT_EQ(rsrp, pdu.rsrp);
  }
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_sr_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  bounded_bitset<sr_pdu_format_2_3_4::MAX_SR_PAYLOAD_SIZE_BITS> payload(2);

  builder.set_sr_parameters(payload);

  const auto& sr_pdu = pdu.sr;
  ASSERT_TRUE(sr_pdu.has_value());
  ASSERT_EQ(payload, sr_pdu->sr_payload);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_harq_passes)
{
  std::uniform_int_distribution<unsigned> detection_dist(1, 5);
  std::uniform_int_distribution<unsigned> length_dist(1, 1706);

  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(4);
  units::bits                                bit_length(40);
  uci_payload_type                           payload(bit_length.value());

  builder.set_harq_parameters(status, bit_length, payload);

  const auto& harq = pdu.harq;
  ASSERT_TRUE(harq.has_value());
  ASSERT_EQ(status, harq->detection_status);
  ASSERT_EQ(bit_length, harq->expected_bit_length);
  ASSERT_EQ(payload, harq->payload);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_csi_part1_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(3);
  units::bits                                bit_length(28);
  uci_payload_type                           payload(bit_length.value());

  builder.set_csi_part1_parameters(status, bit_length, payload);

  const auto& csi = pdu.csi_part1;
  ASSERT_TRUE(csi.has_value());
  ASSERT_EQ(status, csi->detection_status);
  ASSERT_EQ(bit_length, csi->expected_bit_length);
  ASSERT_EQ(payload, csi->payload);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_csi_part2_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(2);
  units::bits                                bit_length(15);
  uci_payload_type                           payload(bit_length.value());

  builder.set_csi_part2_parameters(status, bit_length, payload);

  const auto& csi = pdu.csi_part2;
  ASSERT_TRUE(csi.has_value());
  ASSERT_EQ(status, csi->detection_status);
  ASSERT_EQ(bit_length, csi->expected_bit_length);
  ASSERT_EQ(payload, csi->payload);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_csi_payoad_part1_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(2);
  units::bits                                bit_length(3);
  uci_payload_type                           payload(bit_length.value());

  // Builder won't allow to add an UCI PDU if it's not present a CSI PDU.
  builder.set_csi_part1_parameters(status, bit_length, payload);

  ASSERT_TRUE(pdu.csi_part1.has_value());
  ASSERT_EQ(bit_length, pdu.csi_part1->expected_bit_length);
  ASSERT_EQ(status, pdu.csi_part1->detection_status);
  ASSERT_EQ(payload, pdu.csi_part1->payload);
}

TEST(uci_indication_builder, valid_pucch_f234_pdu_csi_payoad_part2_passes)
{
  uci_pucch_pdu_format_2_3_4         pdu;
  uci_pucch_pdu_format_2_3_4_builder builder(pdu);

  uci_pusch_or_pucch_f2_3_4_detection_status status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(3);
  units::bits                                bit_length(4);
  uci_payload_type                           payload(bit_length.value());

  // Builder won't allow to add an UCI PDU if it's not present a CSI PDU.
  builder.set_csi_part2_parameters(status, bit_length, payload);

  ASSERT_TRUE(pdu.csi_part2.has_value());
  ASSERT_EQ(bit_length, pdu.csi_part2->expected_bit_length);
  ASSERT_EQ(status, pdu.csi_part2->detection_status);
  ASSERT_EQ(payload, pdu.csi_part2->payload);
}

TEST(uci_indication_builder, valid_basic_parameters_passes)
{
  uci_indication         msg;
  uci_indication_builder builder(msg);

  auto     scs        = subcarrier_spacing::kHz30;
  unsigned sfn        = 13;
  unsigned slot_index = 14;
  auto     slot       = slot_point(scs, sfn, slot_index);

  builder.set_slot(slot);

  ASSERT_EQ(slot, msg.slot);
}

TEST(uci_indication_builder, add_pusch_pdu_passes)
{
  uci_indication         msg;
  uci_indication_builder builder(msg);

  rnti_t rnti = to_rnti(9);

  auto pusch_builder = builder.add_pusch_pdu();
  pusch_builder.set_ue_specific_parameters(rnti);

  const auto* pdu = std::get_if<uci_pusch_pdu>(&msg.pdu);
  ASSERT_TRUE(pdu != nullptr);
  ASSERT_EQ(rnti, pdu->rnti);
  ASSERT_EQ(0U, pdu->handle);
}

TEST(uci_indication_builder, add_pucch_f01_passes)
{
  uci_indication         msg;
  uci_indication_builder builder(msg);

  rnti_t       rnti   = to_rnti(5);
  pucch_format format = static_cast<pucch_format>(0);

  auto builder_format01 = builder.add_format_0_1_pucch_pdu();

  builder_format01.set_ue_specific_parameters(rnti).set_format(format);

  const auto* pdu = std::get_if<uci_pucch_pdu_format_0_1>(&msg.pdu);
  ASSERT_TRUE(pdu != nullptr);
  ASSERT_EQ(rnti, pdu->rnti);
  ASSERT_EQ(0U, pdu->handle);
  ASSERT_EQ((format == ocudu::pucch_format::FORMAT_0) ? uci_pucch_pdu_format_0_1::format_type::format_0
                                                      : uci_pucch_pdu_format_0_1::format_type::format_1,
            pdu->pucch_format);
}

TEST(uci_indication_builder, add_pucch_f234_passes)
{
  uci_indication         msg;
  uci_indication_builder builder(msg);

  rnti_t       rnti   = to_rnti(4);
  pucch_format format = static_cast<pucch_format>(3);

  auto builder_format234 = builder.add_format_2_3_4_pucch_pdu();

  builder_format234.set_ue_specific_parameters(rnti).set_format(format);

  const auto* pdu = std::get_if<uci_pucch_pdu_format_2_3_4>(&msg.pdu);
  ASSERT_TRUE(pdu != nullptr);
  ASSERT_EQ(rnti, pdu->rnti);
  ASSERT_EQ(0U, pdu->handle);
  ASSERT_EQ((static_cast<unsigned>(format) - 2U), static_cast<unsigned>(pdu->pucch_format));
}
