// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../message_builder_helpers.h"
#include "pucch.h"
#include "ocudu/adt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittest;

static void assert_format(pucch_format expected_format, fapi::ul_pucch_pdu::ul_pucch_pdu_format fapi_format)
{
  switch (expected_format) {
    case pucch_format::FORMAT_0:
      ASSERT_TRUE(std::holds_alternative<fapi::ul_pucch_pdu_format_0>(fapi_format));
      break;
    case pucch_format::FORMAT_1:
      ASSERT_TRUE(std::holds_alternative<fapi::ul_pucch_pdu_format_1>(fapi_format));
      break;
    case pucch_format::FORMAT_2:
      ASSERT_TRUE(std::holds_alternative<fapi::ul_pucch_pdu_format_2>(fapi_format));
      break;
    case pucch_format::FORMAT_3:
      ASSERT_TRUE(std::holds_alternative<fapi::ul_pucch_pdu_format_3>(fapi_format));
      break;
    case pucch_format::FORMAT_4:
      ASSERT_TRUE(std::holds_alternative<fapi::ul_pucch_pdu_format_4>(fapi_format));
      break;
    case pucch_format::NOF_FORMATS:
    default:
      break;
  }
}

static void
check_context_parameters(const ul_pucch_context& context, const fapi::ul_pucch_pdu& fapi_pdu, slot_point slot)
{
  ASSERT_EQ(slot, context.slot);
  ASSERT_EQ(fapi_pdu.rnti, context.rnti);
  assert_format(context.format, fapi_pdu.format);
}

static void check_context_f1_parameters(const ul_pucch_context& context, const fapi::ul_pucch_pdu_format_1& format1)
{
  ASSERT_TRUE(context.context_f0_f1.has_value());
  const ul_pucch_f0_f1_context& ctx = context.context_f0_f1.value();
  ASSERT_EQ(format1.sr_present, ctx.is_sr_opportunity);
}

TEST(FAPIPPHYULPUCCHAdaptorTest, ValidFormat1PDUPass)
{
  fapi::ul_pucch_pdu fapi_pdu = build_valid_ul_pucch_f1_pdu();

  auto     scs             = to_numerology_value(fapi_pdu.scs);
  unsigned sfn             = 1U;
  unsigned slot_index      = 2U;
  auto     slot            = slot_point(scs, sfn, slot_index);
  unsigned nof_rx_antennas = 1U;
  unsigned ntn_k_mac_slots = 4U;

  uplink_pdu_slot_repository::pucch_pdu pdu;
  convert_pucch_fapi_to_phy(pdu, fapi_pdu, slot, nof_rx_antennas, ntn_k_mac_slots);

  const auto* format1 = std::get_if<fapi::ul_pucch_pdu_format_1>(&fapi_pdu.format);

  ASSERT_TRUE(format1 != nullptr);

  // Format 1 custom parameters.
  const auto& phy_pdu = std::get<pucch_processor::format1_configuration>(pdu.config);
  ASSERT_EQ(fapi_pdu.symbols.start(), phy_pdu.start_symbol_index);
  ASSERT_EQ(fapi_pdu.symbols.length(), phy_pdu.nof_symbols);
  ASSERT_EQ(format1->time_domain_occ_index, phy_pdu.time_domain_occ);
  ASSERT_EQ(format1->initial_cyclic_shift, phy_pdu.initial_cyclic_shift);

  // Common parameters.
  ASSERT_EQ(slot, phy_pdu.slot);
  ASSERT_EQ(ntn_k_mac_slots, phy_pdu.slot_offset);
  ASSERT_EQ(fapi_pdu.bwp.start(), phy_pdu.bwp_start_rb);
  ASSERT_EQ(fapi_pdu.bwp.length(), phy_pdu.bwp_size_rb);
  ASSERT_EQ(fapi_pdu.cp, phy_pdu.cp);

  ASSERT_EQ(fapi_pdu.prbs.start(), phy_pdu.starting_prb);
  if (phy_pdu.second_hop_prb.has_value()) {
    ASSERT_EQ(fapi_pdu.second_hop_prb, phy_pdu.second_hop_prb.value());
  }
  ASSERT_EQ(format1->bit_len_harq.value(), phy_pdu.nof_harq_ack);
  ASSERT_EQ(format1->nid_pucch_hopping, phy_pdu.n_id);

  // Ports.
  ASSERT_EQ(nof_rx_antennas, phy_pdu.ports.size());
  for (unsigned i = 0; i != nof_rx_antennas; ++i) {
    ASSERT_EQ(i, phy_pdu.ports[i]);
  }

  // Context parameters.
  check_context_parameters(pdu.context, fapi_pdu, slot);

  check_context_f1_parameters(pdu.context, *format1);
}

TEST(FAPIPPHYULPUCCHAdaptorTest, ValidFormat2PDUPass)
{
  fapi::ul_pucch_pdu fapi_pdu = build_valid_ul_pucch_f2_pdu();

  auto     scs             = to_numerology_value(fapi_pdu.scs);
  unsigned sfn             = 1U;
  unsigned slot_index      = 2U;
  auto     slot            = slot_point(scs, sfn, slot_index);
  unsigned nof_rx_antennas = 1U;
  unsigned ntn_k_mac_slots = 4U;

  uplink_pdu_slot_repository::pucch_pdu pdu;
  convert_pucch_fapi_to_phy(pdu, fapi_pdu, slot, nof_rx_antennas, ntn_k_mac_slots);

  const auto* format2 = std::get_if<fapi::ul_pucch_pdu_format_2>(&fapi_pdu.format);
  ASSERT_TRUE(format2 != nullptr);

  const auto& phy_pdu = std::get<pucch_processor::format2_configuration>(pdu.config);
  ASSERT_EQ(format2->csi_part1_bit_length.value(), phy_pdu.nof_csi_part1);
  ASSERT_TRUE(phy_pdu.csi_part2_size.entries.empty());
  ASSERT_EQ(format2->bit_len_harq.value(), phy_pdu.nof_harq_ack);
  ASSERT_EQ(static_cast<unsigned>(format2->sr_bit_len), phy_pdu.nof_sr);
  check_context_parameters(pdu.context, fapi_pdu, slot);
}

TEST(FAPIPPHYULPUCCHAdaptorTest, ValidFormat3PDUPass)
{
  fapi::ul_pucch_pdu fapi_pdu = build_valid_ul_pucch_f3_pdu();

  auto     scs             = to_numerology_value(fapi_pdu.scs);
  unsigned sfn             = 1U;
  unsigned slot_index      = 2U;
  auto     slot            = slot_point(scs, sfn, slot_index);
  unsigned nof_rx_antennas = 1U;
  unsigned ntn_k_mac_slots = 4U;

  uplink_pdu_slot_repository::pucch_pdu pdu;
  convert_pucch_fapi_to_phy(pdu, fapi_pdu, slot, nof_rx_antennas, ntn_k_mac_slots);

  const auto* format3 = std::get_if<fapi::ul_pucch_pdu_format_3>(&fapi_pdu.format);
  ASSERT_TRUE(format3 != nullptr);

  const auto& phy_pdu = std::get<pucch_processor::format3_configuration>(pdu.config);
  ASSERT_EQ(format3->csi_part1_bit_length.value(), phy_pdu.nof_csi_part1);
  ASSERT_TRUE(phy_pdu.csi_part2_size.entries.empty());
  ASSERT_EQ(format3->bit_len_harq.value(), phy_pdu.nof_harq_ack);
  ASSERT_EQ(static_cast<unsigned>(format3->sr_bit_len), phy_pdu.nof_sr);
  check_context_parameters(pdu.context, fapi_pdu, slot);
}

TEST(FAPIPPHYULPUCCHAdaptorTest, ValidFormat4PDUPass)
{
  fapi::ul_pucch_pdu fapi_pdu = build_valid_ul_pucch_f4_pdu();

  auto     scs             = to_numerology_value(fapi_pdu.scs);
  unsigned sfn             = 1U;
  unsigned slot_index      = 2U;
  auto     slot            = slot_point(scs, sfn, slot_index);
  unsigned nof_rx_antennas = 1U;
  unsigned ntn_k_mac_slots = 4U;

  uplink_pdu_slot_repository::pucch_pdu pdu;
  convert_pucch_fapi_to_phy(pdu, fapi_pdu, slot, nof_rx_antennas, ntn_k_mac_slots);

  const auto* format4 = std::get_if<fapi::ul_pucch_pdu_format_4>(&fapi_pdu.format);
  ASSERT_TRUE(format4 != nullptr);

  const auto& phy_pdu = std::get<pucch_processor::format4_configuration>(pdu.config);
  ASSERT_EQ(format4->csi_part1_bit_length.value(), phy_pdu.nof_csi_part1);
  ASSERT_TRUE(phy_pdu.csi_part2_size.entries.empty());
  ASSERT_EQ(format4->bit_len_harq.value(), phy_pdu.nof_harq_ack);
  ASSERT_EQ(static_cast<unsigned>(format4->sr_bit_len), phy_pdu.nof_sr);
  check_context_parameters(pdu.context, fapi_pdu, slot);
}
