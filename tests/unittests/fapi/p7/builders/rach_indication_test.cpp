// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/p7/builders/rach_indication_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(rach_indication_builder, valid_basic_parameters_passes)
{
  rach_indication         msg;
  rach_indication_builder builder(msg);

  auto                           scs        = subcarrier_spacing::kHz30;
  unsigned                       sfn        = 13;
  unsigned                       slot_index = 12;
  auto                           slot       = slot_point(scs, sfn, slot_index);
  unsigned                       symb_id    = 3;
  unsigned                       slot_id    = 45;
  unsigned                       ra_id_d    = 4;
  unsigned                       index      = 32;
  std::optional<fapi_power_unit> rssi;
  std::optional<float>           snr = 10;

  builder.set_slot(slot);
  auto pdu_builder = builder.set_pdu(symb_id, slot_id, ra_id_d, rssi, snr);

  std::optional<phy_time_unit>   timing         = phy_time_unit::from_seconds(0);
  std::optional<fapi_power_unit> preamble_power = fapi_power_unit(0, 1, 2);
  std::optional<float>           preamble_snr;

  pdu_builder.add_preamble(index, timing, preamble_power, preamble_snr);

  ASSERT_EQ(slot, msg.slot);

  ASSERT_EQ(0, msg.pdu.handle);
  ASSERT_EQ(symb_id, msg.pdu.symbol_index);
  ASSERT_EQ(slot_id, msg.pdu.slot_index);
  ASSERT_EQ(ra_id_d, msg.pdu.ra_index);
  ASSERT_EQ(rssi, msg.pdu.avg_rssi);
  ASSERT_EQ(snr, msg.pdu.avg_snr_dB);
  ASSERT_EQ(1, msg.pdu.preambles.size());

  const auto& preamble = msg.pdu.preambles.back();
  ASSERT_EQ(index, preamble.preamble_index);
  ASSERT_EQ(timing, preamble.timing_advance_offset);
  ASSERT_EQ(preamble_power, preamble.preamble_pwr);
  ASSERT_EQ(preamble_snr, preamble.preamble_snr_dB);
}
