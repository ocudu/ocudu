// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch.h"

using namespace ocudu;
using namespace fapi_adaptor;

static void fill_format0_parameters(uplink_pdu_slot_repository::pucch_pdu& phy_pdu,
                                    ul_pucch_context&                      context,
                                    const fapi::ul_pucch_pdu&              fapi_pdu,
                                    slot_point                             slot,
                                    uint16_t                               num_rx_ant)
{
  phy_pdu.context.format = pucch_format::FORMAT_0;

  auto& format0 = phy_pdu.config.emplace<pucch_processor::format0_configuration>();

  format0.slot         = slot;
  format0.bwp_size_rb  = fapi_pdu.bwp.length();
  format0.bwp_start_rb = fapi_pdu.bwp.start();
  format0.cp           = fapi_pdu.cp;
  format0.starting_prb = fapi_pdu.prbs.start();
  if (fapi_pdu.second_hop_prb.has_value()) {
    format0.second_hop_prb.emplace(*fapi_pdu.second_hop_prb);
  }

  const auto* fapi_pdu_format_0 = std::get_if<fapi::ul_pucch_pdu_format_0>(&fapi_pdu.format);
  ocudu_assert(fapi_pdu_format_0 != nullptr, "Invalid PUCCH format 0");

  format0.n_id           = fapi_pdu_format_0->nid_pucch_hopping;
  format0.nof_harq_ack   = fapi_pdu_format_0->bit_len_harq.value();
  format0.sr_opportunity = fapi_pdu_format_0->sr_present;

  // Fill the antenna port indices starting from 0.
  format0.ports.resize(num_rx_ant);
  std::iota(format0.ports.begin(), format0.ports.end(), 0);

  format0.initial_cyclic_shift = fapi_pdu_format_0->initial_cyclic_shift;
  format0.nof_symbols          = fapi_pdu.symbols.length();
  format0.start_symbol_index   = fapi_pdu.symbols.start();

  // Fill PUCCH context for logging.
  format0.context = pucch_context(fapi_pdu.rnti);

  ul_pucch_f0_f1_context& ctx = context.context_f0_f1.emplace();
  ctx.is_sr_opportunity       = fapi_pdu_format_0->sr_present;
}

static void fill_format1_parameters(uplink_pdu_slot_repository::pucch_pdu& phy_pdu,
                                    ul_pucch_context&                      context,
                                    const fapi::ul_pucch_pdu&              fapi_pdu,
                                    slot_point                             slot,
                                    uint16_t                               num_rx_ant)
{
  phy_pdu.context.format = pucch_format::FORMAT_1;

  auto& format1 = phy_pdu.config.emplace<pucch_processor::format1_configuration>();

  format1.slot         = slot;
  format1.bwp_size_rb  = fapi_pdu.bwp.length();
  format1.bwp_start_rb = fapi_pdu.bwp.start();
  format1.cp           = fapi_pdu.cp;
  format1.starting_prb = fapi_pdu.prbs.start();
  if (fapi_pdu.second_hop_prb.has_value()) {
    format1.second_hop_prb.emplace(*fapi_pdu.second_hop_prb);
  }

  const auto* fapi_pdu_format_1 = std::get_if<fapi::ul_pucch_pdu_format_1>(&fapi_pdu.format);
  ocudu_assert(fapi_pdu_format_1 != nullptr, "Invalid PUCCH format 1");

  format1.n_id         = fapi_pdu_format_1->nid_pucch_hopping;
  format1.nof_harq_ack = fapi_pdu_format_1->bit_len_harq.value();

  // Fill the antenna port indices starting from 0.
  format1.ports.resize(num_rx_ant);
  std::iota(format1.ports.begin(), format1.ports.end(), 0);

  format1.initial_cyclic_shift = fapi_pdu_format_1->initial_cyclic_shift;
  format1.nof_symbols          = fapi_pdu.symbols.length();
  format1.start_symbol_index   = fapi_pdu.symbols.start();
  format1.time_domain_occ      = fapi_pdu_format_1->time_domain_occ_index;

  // Fill PUCCH context for logging.
  format1.context = pucch_context(fapi_pdu.rnti);

  ul_pucch_f0_f1_context& ctx = context.context_f0_f1.emplace();
  ctx.is_sr_opportunity       = fapi_pdu_format_1->sr_present;
}

static void fill_format2_parameters(uplink_pdu_slot_repository::pucch_pdu& phy_pdu,
                                    const fapi::ul_pucch_pdu&              fapi_pdu,
                                    slot_point                             slot,
                                    uint16_t                               num_rx_ant)
{
  phy_pdu.context.format = pucch_format::FORMAT_2;

  auto& format2 = phy_pdu.config.emplace<pucch_processor::format2_configuration>();

  format2.slot         = slot;
  format2.cp           = fapi_pdu.cp;
  format2.bwp_size_rb  = fapi_pdu.bwp.length();
  format2.bwp_start_rb = fapi_pdu.bwp.start();
  format2.prbs         = fapi_pdu.prbs;
  if (fapi_pdu.second_hop_prb.has_value()) {
    format2.second_hop_prb.emplace(*fapi_pdu.second_hop_prb);
  }
  format2.start_symbol_index = fapi_pdu.symbols.start();
  format2.nof_symbols        = fapi_pdu.symbols.length();

  const auto* fapi_pdu_format_2 = std::get_if<fapi::ul_pucch_pdu_format_2>(&fapi_pdu.format);
  ocudu_assert(fapi_pdu_format_2 != nullptr, "Invalid PUCCH format 2");

  format2.rnti   = to_value(fapi_pdu.rnti);
  format2.n_id   = fapi_pdu_format_2->nid_pucch_scrambling;
  format2.n_id_0 = fapi_pdu_format_2->nid0_pucch_dmrs_scrambling;

  format2.nof_harq_ack  = fapi_pdu_format_2->bit_len_harq.value();
  format2.nof_sr        = static_cast<uint8_t>(fapi_pdu_format_2->sr_bit_len);
  format2.nof_csi_part1 = fapi_pdu_format_2->csi_part1_bit_length.value();
  // CSI Part 2 disabled.
  format2.nof_csi_part2 = 0;

  // Fill PUCCH context for logging.
  format2.context = pucch_context(fapi_pdu.rnti);

  // Fill the antenna port indices starting from 0.
  format2.ports.resize(num_rx_ant);
  std::iota(format2.ports.begin(), format2.ports.end(), 0);
}

static void fill_format3_parameters(uplink_pdu_slot_repository::pucch_pdu& phy_pdu,
                                    const fapi::ul_pucch_pdu&              fapi_pdu,
                                    slot_point                             slot,
                                    uint16_t                               num_rx_ant)
{
  phy_pdu.context.format = pucch_format::FORMAT_3;

  auto& format3 = phy_pdu.config.emplace<pucch_processor::format3_configuration>();

  format3.slot         = slot;
  format3.cp           = fapi_pdu.cp;
  format3.bwp_size_rb  = fapi_pdu.bwp.length();
  format3.bwp_start_rb = fapi_pdu.bwp.start();
  format3.starting_prb = fapi_pdu.prbs.start();
  if (fapi_pdu.second_hop_prb.has_value()) {
    format3.second_hop_prb.emplace(*fapi_pdu.second_hop_prb);
  }
  format3.nof_prb            = fapi_pdu.prbs.length();
  format3.start_symbol_index = fapi_pdu.symbols.start();
  format3.nof_symbols        = fapi_pdu.symbols.length();

  format3.rnti = to_value(fapi_pdu.rnti);

  const auto* fapi_pdu_format_3 = std::get_if<fapi::ul_pucch_pdu_format_3>(&fapi_pdu.format);
  ocudu_assert(fapi_pdu_format_3 != nullptr, "Invalid PUCCH format 3");

  format3.n_id_hopping    = fapi_pdu_format_3->nid_pucch_hopping;
  format3.n_id_scrambling = fapi_pdu_format_3->nid_pucch_scrambling;

  format3.nof_harq_ack  = fapi_pdu_format_3->bit_len_harq.value();
  format3.nof_sr        = static_cast<uint8_t>(fapi_pdu_format_3->sr_bit_len);
  format3.nof_csi_part1 = fapi_pdu_format_3->csi_part1_bit_length.value();
  // CSI Part 2 disabled.
  format3.nof_csi_part2 = 0;

  format3.additional_dmrs = fapi_pdu_format_3->add_dmrs_flag;
  format3.pi2_bpsk        = fapi_pdu_format_3->pi2_bpsk;

  // Fill PUCCH context for logging.
  format3.context = pucch_context(fapi_pdu.rnti);

  // Fill the antenna port indices starting from 0.
  format3.ports.resize(num_rx_ant);
  std::iota(format3.ports.begin(), format3.ports.end(), 0);
}

static void fill_format4_parameters(uplink_pdu_slot_repository::pucch_pdu& phy_pdu,
                                    const fapi::ul_pucch_pdu&              fapi_pdu,
                                    slot_point                             slot,
                                    uint16_t                               num_rx_ant)
{
  phy_pdu.context.format = pucch_format::FORMAT_4;

  auto& format4 = phy_pdu.config.emplace<pucch_processor::format4_configuration>();

  format4.slot         = slot;
  format4.cp           = fapi_pdu.cp;
  format4.bwp_size_rb  = fapi_pdu.bwp.length();
  format4.bwp_start_rb = fapi_pdu.bwp.start();
  format4.starting_prb = fapi_pdu.prbs.start();
  if (fapi_pdu.second_hop_prb.has_value()) {
    format4.second_hop_prb.emplace(*fapi_pdu.second_hop_prb);
  }
  format4.start_symbol_index = fapi_pdu.symbols.start();
  format4.nof_symbols        = fapi_pdu.symbols.length();

  format4.rnti = to_value(fapi_pdu.rnti);

  const auto* fapi_pdu_format_4 = std::get_if<fapi::ul_pucch_pdu_format_4>(&fapi_pdu.format);
  ocudu_assert(fapi_pdu_format_4 != nullptr, "Invalid PUCCH format 4");

  format4.n_id_hopping    = fapi_pdu_format_4->nid_pucch_hopping;
  format4.n_id_scrambling = fapi_pdu_format_4->nid_pucch_scrambling;

  format4.nof_harq_ack  = fapi_pdu_format_4->bit_len_harq.value();
  format4.nof_sr        = static_cast<uint8_t>(fapi_pdu_format_4->sr_bit_len);
  format4.nof_csi_part1 = fapi_pdu_format_4->csi_part1_bit_length.value();
  // CSI Part 2 disabled.
  format4.nof_csi_part2 = 0;

  format4.additional_dmrs = fapi_pdu_format_4->add_dmrs_flag;
  format4.pi2_bpsk        = fapi_pdu_format_4->pi2_bpsk;
  format4.occ_index       = fapi_pdu_format_4->pre_dft_occ_idx;
  format4.occ_length      = fapi_pdu_format_4->pre_dft_occ_len;

  // Fill PUCCH context for logging.
  format4.context = pucch_context(fapi_pdu.rnti);

  // Fill the antenna port indices starting from 0.
  format4.ports.resize(num_rx_ant);
  std::iota(format4.ports.begin(), format4.ports.end(), 0);
}

void ocudu::fapi_adaptor::convert_pucch_fapi_to_phy(uplink_pdu_slot_repository::pucch_pdu& pdu,
                                                    const fapi::ul_pucch_pdu&              fapi_pdu,
                                                    slot_point                             slot,
                                                    uint16_t                               num_rx_ant)
{
  // Fill main context fields.
  ul_pucch_context& context = pdu.context;
  context.slot              = slot;
  context.rnti              = fapi_pdu.rnti;

  if (std::holds_alternative<fapi::ul_pucch_pdu_format_0>(fapi_pdu.format)) {
    fill_format0_parameters(pdu, context, fapi_pdu, slot, num_rx_ant);
  } else if (std::holds_alternative<fapi::ul_pucch_pdu_format_1>(fapi_pdu.format)) {
    fill_format1_parameters(pdu, context, fapi_pdu, slot, num_rx_ant);
  } else if (std::holds_alternative<fapi::ul_pucch_pdu_format_2>(fapi_pdu.format)) {
    fill_format2_parameters(pdu, fapi_pdu, slot, num_rx_ant);
  } else if (std::holds_alternative<fapi::ul_pucch_pdu_format_3>(fapi_pdu.format)) {
    fill_format3_parameters(pdu, fapi_pdu, slot, num_rx_ant);
  } else if (std::holds_alternative<fapi::ul_pucch_pdu_format_4>(fapi_pdu.format)) {
    fill_format4_parameters(pdu, fapi_pdu, slot, num_rx_ant);
  } else {
    ocudu_assert(0, "Unsupported PUCCH format {}", fmt::underlying(context.format));
  }
}
