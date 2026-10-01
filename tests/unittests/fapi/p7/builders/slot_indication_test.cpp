// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/p7/builders/slot_indication_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(slot_indication_builder, valid_basic_parameters_passes)
{
  auto                                               scs        = subcarrier_spacing::kHz30;
  unsigned                                           sfn        = 419;
  unsigned                                           slot_index = 12;
  auto                                               slot       = slot_point_extended(slot_point{scs, sfn, slot_index});
  std::chrono::time_point<std::chrono::system_clock> time_point = std::chrono::system_clock::now();

  const auto& msg = build_slot_indication(slot, time_point);

  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(time_point, msg.time_point);
}
