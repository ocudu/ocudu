// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/p7/builders/tx_data_request_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(tx_data_request_builder, valid_basic_parameters_passes)
{
  tx_data_request         msg;
  tx_data_request_builder builder(msg);

  auto     scs        = subcarrier_spacing::kHz240;
  unsigned sfn        = 100;
  unsigned slot_index = 20;
  auto     slot       = slot_point(scs, sfn, slot_index);

  builder.set_slot(slot);
  unsigned                  pdu_index = 3;
  unsigned                  cw_index  = 0;
  static_vector<uint8_t, 5> payload   = {5, 3, 4, 5};

  shared_transport_block buffer(payload);
  builder.add_pdu(pdu_index, cw_index, buffer);

  ASSERT_EQ(slot, msg.slot);

  const auto& pdu = msg.pdus[0];
  ASSERT_EQ(pdu_index, pdu.pdu_index);
  ASSERT_EQ(cw_index, pdu.cw_index);
  ASSERT_EQ(payload.size(), buffer.get_buffer().size());
}
