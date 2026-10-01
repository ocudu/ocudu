// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/common/error_indication_builder.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;
using namespace fapi;

TEST(error_indication_builder, valid_basic_parameters_passes)
{
  auto            scs        = subcarrier_spacing::kHz240;
  unsigned        sfn        = 100;
  unsigned        slot_index = 128;
  auto            slot       = slot_point(scs, sfn, slot_index);
  message_type_id msg_id     = static_cast<message_type_id>(4);
  error_code_id   error_id   = static_cast<error_code_id>(6);

  const auto& msg = build_error_indication(slot, msg_id, error_id);

  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(msg_id, msg.message_id);
  ASSERT_EQ(error_id, msg.error_code);
}

TEST(out_of_sync_error_indication_builder, valid_basic_parameters_passes)
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> msg_dist(0, 7);

  auto            scs                 = subcarrier_spacing::kHz240;
  unsigned        sfn                 = 198;
  unsigned        slot_index          = 21;
  auto            slot                = slot_point(scs, sfn, slot_index);
  message_type_id msg_id              = static_cast<message_type_id>(3);
  unsigned        expected_sfn        = 178;
  unsigned        expected_slot_index = 19;
  auto            expected_slot       = slot_point(scs, expected_sfn, expected_slot_index);

  const auto& msg = build_out_of_sync_error_indication(slot, msg_id, expected_slot);

  ASSERT_EQ(slot, msg.slot);
  ASSERT_EQ(msg_id, msg.message_id);
  ASSERT_EQ(error_code_id::out_of_sync, msg.error_code);
  ASSERT_EQ(expected_slot, msg.expected_slot);
}
