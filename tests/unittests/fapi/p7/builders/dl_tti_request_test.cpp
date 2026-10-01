// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/dl_tti_request_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(dl_ssb_pdu_builder, valid_slot_point_passes)
{
  auto     scs        = subcarrier_spacing::kHz30;
  unsigned sfn        = 599;
  unsigned slot_index = 13;
  auto     slot       = slot_point(scs, sfn, slot_index);

  dl_tti_request         msg;
  dl_tti_request_builder builder(msg);

  builder.set_slot(slot);

  ASSERT_EQ(slot, msg.slot);
  ASSERT_TRUE(msg.pdus.empty());
}

TEST(dl_ssb_pdu_builder, add_pdcch_pdu_passes)
{
  dl_tti_request         msg;
  dl_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  builder.add_pdcch_pdu();

  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_TRUE(std::holds_alternative<dl_pdcch_pdu>(msg.pdus.back().pdu));
}

TEST(dl_ssb_pdu_builder, add_pdsch_pdu_passes)
{
  dl_tti_request         msg;
  dl_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  builder.add_pdsch_pdu();

  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_TRUE(std::holds_alternative<dl_pdsch_pdu>(msg.pdus.back().pdu));
}

TEST(dl_ssb_pdu_builder, add_ssb_pdu_passes)
{
  dl_tti_request         msg;
  dl_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  builder.add_ssb_pdu();

  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_TRUE(std::holds_alternative<dl_ssb_pdu>(msg.pdus.back().pdu));
}

TEST(dl_ssb_pdu_builder, add_csi_pdu_passes)
{
  dl_tti_request         msg;
  dl_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  uint16_t                  start_rb      = 3;
  uint16_t                  nof_rbs       = 4;
  csi_rs_type               type          = csi_rs_type::CSI_RS_NZP;
  uint8_t                   row           = 3;
  bounded_bitset<12, false> freq_domain   = {0, 0, 1};
  uint8_t                   symb_l0       = 5;
  uint8_t                   symb_l1       = 2;
  csi_rs_cdm_type           cdm_type      = csi_rs_cdm_type::cdm8_FD2_TD4;
  csi_rs_freq_density_type  freq_density  = csi_rs_freq_density_type::dot5_even_RB;
  uint16_t                  scrambling_id = 56;

  auto csi_builder = builder.add_csi_rs_pdu();

  csi_builder.set_csi_resource_config_parameters(type, row, cdm_type, scrambling_id)
      .set_frequency_domain_parameters(freq_domain, freq_density)
      .set_time_domain_parameters(symb_l0, symb_l1)
      .set_resource_block_parameters({start_rb, nof_rbs});

  ASSERT_EQ(1U, msg.pdus.size());
  ASSERT_TRUE(std::holds_alternative<dl_csi_rs_pdu>(msg.pdus.back().pdu));
}
