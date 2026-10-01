// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_processor.h"
#include "ocudu/ran/pdcch/pdcch_context_formatter.h"
#include "ocudu/ran/precoding_beamforming_formatters.h"

namespace fmt {

/// \brief Custom formatter for \c pdcch_processor::coreset_description.
template <>
struct formatter<ocudu::pdcch_processor::coreset_description> {
  /// Helper used to parse formatting options and format fields.
  ocudu::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::pdcch_processor::coreset_description& coreset, FormatContext& ctx) const
  {
    helper.format_always(ctx, "bwp=[{}, {})", coreset.bwp_start_rb, coreset.bwp_start_rb + coreset.bwp_size_rb);
    helper.format_always(
        ctx, "symb=[{}, {})", coreset.start_symbol_index, coreset.start_symbol_index + coreset.duration);
    helper.format_always(ctx, "f_re={}", coreset.frequency_resources);

    switch (coreset.cce_to_reg_mapping) {
      case ocudu::pdcch_processor::cce_to_reg_mapping_type::CORESET0:
        helper.format_if_verbose(ctx, "cce_to_reg_map=coreset0");
        break;
      case ocudu::pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED:
        helper.format_if_verbose(ctx, "cce_to_reg_map=interleaved");
        helper.format_if_verbose(ctx, "reg_bundle_size={}", coreset.reg_bundle_size);
        helper.format_if_verbose(ctx, "interleaver_size={}", coreset.interleaver_size);
        break;
      case ocudu::pdcch_processor::cce_to_reg_mapping_type::NON_INTERLEAVED:
        helper.format_if_verbose(ctx, "cce_to_reg_map=non-interleaved");
        break;
    }

    helper.format_if_verbose(ctx, "shift_idx={}", coreset.shift_index);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdcch_processor::pdu_t.
template <>
struct formatter<ocudu::pdcch_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  ocudu::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::pdcch_processor::pdu_t& pdu, FormatContext& ctx) const
  {
    helper.format_always(ctx, "rnti={}", pdu.dci.rnti);
    if (pdu.context.has_value()) {
      helper.format_always(ctx, *pdu.context);
    }
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "cp={}", pdu.cp.to_string());
    helper.format_if_verbose(ctx, pdu.coreset);
    helper.format_always(ctx, "cce={}", pdu.dci.cce_index);
    helper.format_always(ctx, "al={}", ocudu::to_nof_cces(pdu.dci.dci_aggregation_level));
    helper.format_if_verbose(ctx, "size={}", pdu.dci.payload.size());
    helper.format_if_verbose(ctx, "n_id_dmrs={}", pdu.dci.n_id_pdcch_dmrs);
    helper.format_if_verbose(ctx, "n_id_data={}", pdu.dci.n_id_pdcch_data);
    helper.format_if_verbose(ctx, "n_rnti={}", pdu.dci.n_rnti);
    helper.format_if_verbose(ctx, "power_dmrs={:+.1f}dB", pdu.dci.dmrs_power_offset_dB);
    helper.format_if_verbose(ctx, "power_data={:+.1f}dB", pdu.dci.data_power_offset_dB);

    if (helper.is_multiline()) {
      helper.format_if_verbose(ctx, "{:n}", pdu.dci.precoding_and_beamforming);
    } else {
      helper.format_if_verbose(ctx, "{}", pdu.dci.precoding_and_beamforming);
    }

    return ctx.out();
  }
};

} // namespace fmt
