// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/srs_indication_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(srs_indication_builder, valid_srs_indication_passes)
{
  srs_indication         msg;
  srs_indication_builder builder(msg);

  auto     scs        = subcarrier_spacing::kHz30;
  unsigned sfn        = 13;
  unsigned slot_index = 12;
  auto     slot       = slot_point(scs, sfn, slot_index);
  builder.set_slot(slot);

  rnti_t rnti        = to_rnti(3);
  auto   pdu_builder = builder.set_pdu(rnti);

  std::optional<phy_time_unit> timing = phy_time_unit::from_seconds(0);

  pdu_builder.set_metrics_parameters(timing);

  std::array<cf_t, 4> values = {0, 0, 0, 0};
  srs_channel_matrix  matrix(values, 2, 2);
  pdu_builder.set_codebook_report_matrix(matrix);

  ASSERT_EQ(slot, msg.slot);

  ASSERT_EQ(0U, msg.pdu.handle);
  ASSERT_EQ(rnti, msg.pdu.rnti);

  ASSERT_EQ(timing ? timing.value() : phy_time_unit(), msg.pdu.timing_advance_offset);
  ASSERT_NE(msg.pdu.matrix, std::nullopt);
  ASSERT_EQ(matrix.get_nof_rx_ports(), msg.pdu.matrix->get_nof_rx_ports());
  ASSERT_EQ(matrix.get_nof_tx_ports(), msg.pdu.matrix->get_nof_tx_ports());
}

TEST(srs_indication_builder, valid_srs_indication_with_positioning_report_passes)
{
  srs_indication         msg;
  srs_indication_builder builder(msg);

  auto     scs        = subcarrier_spacing::kHz30;
  unsigned sfn        = 13;
  unsigned slot_index = 12;
  auto     slot       = slot_point(scs, sfn, slot_index);
  builder.set_slot(slot);

  rnti_t rnti        = to_rnti(3);
  auto   pdu_builder = builder.set_pdu(rnti);

  std::optional<phy_time_unit> timing = phy_time_unit::from_seconds(0);

  pdu_builder.set_metrics_parameters(timing);

  std::optional<phy_time_unit>   ul_relative_toa = phy_time_unit::from_units_of_Tc(28);
  std::optional<fapi_power_unit> rsrp            = fapi_power_unit(-50, 0, 0);
  std::optional<float>           azimuth_aoa_deg = -45.5F;
  std::optional<float>           zenith_aoa_deg  = 90.1F;

  pdu_builder.set_positioning_report_parameters(ul_relative_toa, rsrp, azimuth_aoa_deg, zenith_aoa_deg);

  ASSERT_EQ(slot, msg.slot);

  ASSERT_EQ(0U, msg.pdu.handle);
  ASSERT_EQ(rnti, msg.pdu.rnti);

  ASSERT_EQ(timing ? timing.value() : phy_time_unit(), msg.pdu.timing_advance_offset);
  ASSERT_NE(msg.pdu.positioning, std::nullopt);

  ASSERT_EQ(ul_relative_toa, msg.pdu.positioning->ul_relative_toa);
  ASSERT_EQ(rsrp, msg.pdu.positioning->rsrp);
  ASSERT_EQ(azimuth_aoa_deg, msg.pdu.positioning->azimuth_aoa_deg);
  ASSERT_EQ(zenith_aoa_deg, msg.pdu.positioning->zenith_aoa_deg);
}
