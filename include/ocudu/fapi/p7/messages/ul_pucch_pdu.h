// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/ran/pucch/pucch_uci_bits.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/support/units.h"
#include <optional>
#include <variant>

namespace ocudu {
namespace fapi {

/// Holds the definition of the structure for the PUCCH PDU format 0.
struct ul_pucch_pdu_format_0 {
  uint16_t    nid_pucch_hopping;
  uint16_t    initial_cyclic_shift;
  bool        sr_present;
  units::bits bit_len_harq;
};

/// Holds the definition of the structure for the PUCCH PDU format 1.
struct ul_pucch_pdu_format_1 {
  uint16_t    nid_pucch_hopping;
  uint16_t    initial_cyclic_shift;
  uint8_t     time_domain_occ_index;
  bool        sr_present;
  units::bits bit_len_harq;
};

/// Holds the definition of the structure for the PUCCH PDU format 2.
struct ul_pucch_pdu_format_2 {
  uint16_t    nid_pucch_scrambling;
  uint16_t    nid0_pucch_dmrs_scrambling;
  sr_nof_bits sr_bit_len;
  units::bits csi_part1_bit_length;
  units::bits bit_len_harq;
};

/// Holds the definition of the structure for the PUCCH PDU format 3.
struct ul_pucch_pdu_format_3 {
  bool        pi2_bpsk;
  uint16_t    nid_pucch_hopping;
  uint16_t    nid_pucch_scrambling;
  bool        add_dmrs_flag;
  uint16_t    nid0_pucch_dmrs_scrambling;
  uint8_t     m0_pucch_dmrs_cyclic_shift;
  sr_nof_bits sr_bit_len;
  units::bits csi_part1_bit_length;
  units::bits bit_len_harq;
};

/// Holds the definition of the structure for the PUCCH PDU format 4.
struct ul_pucch_pdu_format_4 {
  bool        pi2_bpsk;
  uint16_t    nid_pucch_hopping;
  uint16_t    nid_pucch_scrambling;
  uint8_t     pre_dft_occ_idx;
  uint8_t     pre_dft_occ_len;
  bool        add_dmrs_flag;
  uint16_t    nid0_pucch_dmrs_scrambling;
  uint8_t     m0_pucch_dmrs_cyclic_shift;
  sr_nof_bits sr_bit_len;
  units::bits csi_part1_bit_length;
  units::bits bit_len_harq;
};

/// Encodes PUCCH pdu.
struct ul_pucch_pdu {
  /// Holds the possible PUCCH PDU format.
  using ul_pucch_pdu_format = std::variant<std::monostate,
                                           ul_pucch_pdu_format_0,
                                           ul_pucch_pdu_format_1,
                                           ul_pucch_pdu_format_2,
                                           ul_pucch_pdu_format_3,
                                           ul_pucch_pdu_format_4>;

  rnti_t                  rnti;
  uint32_t                handle = 0;
  crb_interval            bwp;
  subcarrier_spacing      scs;
  cyclic_prefix           cp;
  ul_pucch_pdu_format     format;
  prb_interval            prbs;
  ofdm_symbol_range       symbols;
  std::optional<uint16_t> second_hop_prb;
  /// Flushes, keeps or combines the buffer used for multiple-slot PUCCH transmissions, as per TS 38.213, Section
  /// 9.2.6. Applicable to all PUCCH formats.
  pucch_repetition_tx_slot multi_slot_tx_indicator = pucch_repetition_tx_slot::no_multi_slot;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::ul_pucch_pdu_format_0> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_pucch_pdu_format_0& pdu, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "nid_pucch_hopping={} initial_cyclic_shift={} sr_present={} harq_bit_len={}",
                     pdu.nid_pucch_hopping,
                     pdu.initial_cyclic_shift,
                     pdu.sr_present,
                     pdu.bit_len_harq.value());
  }
};

template <>
struct formatter<ocudu::fapi::ul_pucch_pdu_format_1> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_pucch_pdu_format_1& pdu, FormatContext& ctx) const
  {
    return format_to(
        ctx.out(),
        "nid_pucch_hopping={} initial_cyclic_shift={} time_domain_occ_index={} sr_present={} harq_bit_len={}",
        pdu.nid_pucch_hopping,
        pdu.initial_cyclic_shift,
        pdu.time_domain_occ_index,
        pdu.sr_present,
        pdu.bit_len_harq.value());
  }
};

template <>
struct formatter<ocudu::fapi::ul_pucch_pdu_format_2> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_pucch_pdu_format_2& pdu, FormatContext& ctx) const
  {
    return format_to(
        ctx.out(),
        "nid_pucch_scrambling={} nid0_pucch_dmrs_scrambling={} sr_bit_len={} csi_part1_bit_length={} harq_bit_len={}",
        pdu.nid_pucch_scrambling,
        pdu.nid0_pucch_dmrs_scrambling,
        underlying(pdu.sr_bit_len),
        pdu.csi_part1_bit_length.value(),
        pdu.bit_len_harq.value());
  }
};

template <>
struct formatter<ocudu::fapi::ul_pucch_pdu_format_3> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_pucch_pdu_format_3& pdu, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "nid_pucch_hopping={} nid_pucch_scrambling={} add_dmrs_flag={} nid0_pucch_dmrs_scrambling={} "
                     "m0_pucch_dmrs_cyclic_shift={} sr_bit_len={} csi_part1_bit_length={} pi2_bpsk={} harq_bit_len={} ",
                     pdu.nid_pucch_hopping,
                     pdu.nid_pucch_scrambling,
                     pdu.add_dmrs_flag,
                     pdu.nid0_pucch_dmrs_scrambling,
                     pdu.m0_pucch_dmrs_cyclic_shift,
                     underlying(pdu.sr_bit_len),
                     pdu.csi_part1_bit_length.value(),
                     pdu.pi2_bpsk,
                     pdu.bit_len_harq.value());
  }
};

template <>
struct formatter<ocudu::fapi::ul_pucch_pdu_format_4> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_pucch_pdu_format_4& pdu, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "nid_pucch_hopping={} nid_pucch_scrambling={} pre_dft_occ_idx={} pre_dft_occ_len={} "
                     "add_dmrs_flag={} nid0_pucch_dmrs_scrambling={} m0_pucch_dmrs_cyclic_shift={} sr_bit_len={} "
                     "csi_part1_bit_length={} pi2_bpsk={} harq_bit_len={}",
                     pdu.nid_pucch_hopping,
                     pdu.nid_pucch_scrambling,
                     pdu.pre_dft_occ_idx,
                     pdu.pre_dft_occ_len,
                     pdu.add_dmrs_flag,
                     pdu.nid0_pucch_dmrs_scrambling,
                     pdu.m0_pucch_dmrs_cyclic_shift,
                     underlying(pdu.sr_bit_len),
                     pdu.csi_part1_bit_length.value(),
                     pdu.pi2_bpsk,
                     pdu.bit_len_harq.value());
  }
};

template <>
struct formatter<ocudu::fapi::ul_pucch_pdu> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::ul_pucch_pdu& pdu, FormatContext& ctx) const
  {
    format_to(ctx.out(),
              "\n\t- PUCCH rnti={} bwp={} scs={} cp={} prb={} symb={} multi_slot={}",
              pdu.rnti,
              pdu.bwp,
              to_string(pdu.scs),
              pdu.cp.to_string(),
              pdu.prbs,
              pdu.symbols,
              static_cast<unsigned>(pdu.multi_slot_tx_indicator));

    if (pdu.second_hop_prb.has_value()) {
      format_to(ctx.out(), " intra_slot_frequency_hopping={}", *pdu.second_hop_prb);
    }

    if (const auto* format0 = std::get_if<ocudu::fapi::ul_pucch_pdu_format_0>(&pdu.format)) {
      format_to(ctx.out(), " FORMAT_0: {}", *format0);
    } else if (const auto* format1 = std::get_if<ocudu::fapi::ul_pucch_pdu_format_1>(&pdu.format)) {
      format_to(ctx.out(), " FORMAT_1: {}", *format1);
    } else if (const auto* format2 = std::get_if<ocudu::fapi::ul_pucch_pdu_format_2>(&pdu.format)) {
      format_to(ctx.out(), " FORMAT_2: {}", *format2);
    } else if (const auto* format3 = std::get_if<ocudu::fapi::ul_pucch_pdu_format_3>(&pdu.format)) {
      format_to(ctx.out(), " FORMAT_3: {}", *format3);
    } else if (const auto* format4 = std::get_if<ocudu::fapi::ul_pucch_pdu_format_4>(&pdu.format)) {
      format_to(ctx.out(), " FORMAT_4: {}", *format4);
    }

    return ctx.out();
  }
};
} // namespace fmt
