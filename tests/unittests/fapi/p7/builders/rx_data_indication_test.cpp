// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/p7/builders/rx_data_indication_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(rx_data_indication_builder, valid_basic_parameters_passes)
{
  auto     scs        = subcarrier_spacing::kHz240;
  unsigned sfn        = 600;
  unsigned slot_index = 40;
  auto     slot       = slot_point(scs, sfn, slot_index);

  rx_data_indication         msg;
  rx_data_indication_builder builder(msg);

  builder.set_slot(slot);

  ASSERT_EQ(slot, msg.slot);
}

TEST(rx_data_indication_builder, add_custom_pdu_passes)
{
  static_vector<uint8_t, 18> transport_block = {1, 2, 3, 4, 5};

  rnti_t    rnti = to_rnti(29);
  harq_id_t harq = to_harq_id(14);

  rx_data_indication         msg;
  rx_data_indication_builder builder(msg);

  builder.set_pdu(rnti, harq, {transport_block});

  ASSERT_EQ(0, msg.pdu.handle);
  ASSERT_EQ(rnti, msg.pdu.rnti);
  ASSERT_EQ(harq, msg.pdu.harq_id);
  ASSERT_EQ(transport_block.size(), msg.pdu.transport_block.size());
  ASSERT_EQ(transport_block.data(), msg.pdu.transport_block.data());
}

TEST(rx_data_indication_builder, valid_rapid_passes)
{
  rx_data_indication         msg;
  rx_data_indication_builder builder(msg);

  ASSERT_FALSE(msg.pdu.rapid.has_value());

  uint8_t rapid = 10;

  builder.set_rapid_parameter(rapid);

  ASSERT_EQ(msg.pdu.rapid, rapid);
}
