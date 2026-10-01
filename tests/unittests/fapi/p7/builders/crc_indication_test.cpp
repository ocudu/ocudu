// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/p7/builders/crc_indication_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(crc_indication_builder, valid_indication_passes)
{
  for (unsigned i = 0; i != 2; ++i) {
    crc_indication         msg;
    crc_indication_builder builder(msg);

    auto     scs        = subcarrier_spacing::kHz30;
    unsigned sfn        = 100;
    unsigned slot_index = 15;
    auto     slot       = slot_point(scs, sfn, slot_index);

    builder.set_slot(slot);

    rnti_t    rnti          = to_rnti(10);
    harq_id_t harq_id       = to_harq_id(0);
    uint8_t   tb_crc_status = 0;

    std::optional<float> ul_sinr_dB;
    ul_sinr_dB.emplace(-65);

    std::optional<phy_time_unit> timing_advance_offset;
    timing_advance_offset.emplace(phy_time_unit::from_seconds(30));

    std::optional<fapi_power_unit> rssi;
    rssi.emplace(fapi_power_unit{-64, 0, 0});

    std::optional<fapi_power_unit> rsrp;
    rsrp.emplace(fapi_power_unit{-100, 0, 0});

    builder.set_pdu(rnti, harq_id, tb_crc_status, ul_sinr_dB, timing_advance_offset, rssi, rsrp);

    ASSERT_EQ(slot, msg.slot);

    ASSERT_EQ(0, msg.pdu.handle);
    ASSERT_EQ(harq_id, msg.pdu.harq_id);
    ASSERT_EQ(rnti, msg.pdu.rnti);
    ASSERT_EQ(tb_crc_status, msg.pdu.tb_crc_status_ok);
    ASSERT_EQ(ul_sinr_dB, msg.pdu.ul_sinr_metric_dB);
    ASSERT_EQ(timing_advance_offset ? timing_advance_offset.value() : phy_time_unit(), msg.pdu.timing_advance_offset);
    ASSERT_EQ(rssi, msg.pdu.rssi);
    ASSERT_EQ(rsrp, msg.pdu.rsrp);
  }
}

TEST(crc_indication_builder, valid_indication_with_no_metrics_passes)
{
  crc_indication         msg;
  crc_indication_builder builder(msg);

  auto     scs        = subcarrier_spacing::kHz30;
  unsigned sfn        = 100;
  unsigned slot_index = 15;
  auto     slot       = slot_point(scs, sfn, slot_index);

  builder.set_slot(slot);

  rnti_t    rnti          = to_rnti(10);
  harq_id_t harq_id       = to_harq_id(0);
  uint8_t   tb_crc_status = 0;

  std::optional<float>           ul_sinr_dB;
  std::optional<phy_time_unit>   timing_advance_offset;
  std::optional<fapi_power_unit> rssi;
  std::optional<fapi_power_unit> rsrp;

  builder.set_pdu(rnti, harq_id, tb_crc_status, ul_sinr_dB, timing_advance_offset, rssi, rsrp);

  ASSERT_EQ(slot, msg.slot);

  ASSERT_EQ(0, msg.pdu.handle);
  ASSERT_EQ(harq_id, msg.pdu.harq_id);
  ASSERT_EQ(rnti, msg.pdu.rnti);
  ASSERT_EQ(tb_crc_status, msg.pdu.tb_crc_status_ok);
  ASSERT_EQ(ul_sinr_dB, msg.pdu.ul_sinr_metric_dB);
  ASSERT_EQ(timing_advance_offset, msg.pdu.timing_advance_offset);
  ASSERT_FALSE(msg.pdu.rssi.has_value());
  ASSERT_FALSE(msg.pdu.rsrp.has_value());
}

TEST(crc_indication_builder, valid_rapid_passes)
{
  crc_indication         msg;
  crc_indication_builder builder(msg);

  ASSERT_FALSE(msg.pdu.rapid.has_value());

  uint8_t rapid = 10;

  builder.set_rapid_parameter(rapid);

  ASSERT_EQ(msg.pdu.rapid, rapid);
}
