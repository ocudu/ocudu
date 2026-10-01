// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/signal_processors/prs/prs_generator_configuration.h"
#include "ocudu/ran/precoding_beamforming_formatters.h"

namespace fmt {

/// \brief Custom formatter for \c pdcch_processor::coreset_description.
template <>
struct formatter<ocudu::prs_generator_configuration> {
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
  auto format(const ocudu::prs_generator_configuration& config, FormatContext& ctx) const
  {
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_if_verbose(ctx, "cp={}", config.cp);
    helper.format_always(ctx, "n_id={}", config.n_id_prs);
    helper.format_always(ctx, "comb_size={}", fmt::underlying(config.comb_size));
    helper.format_always(ctx, "comb_offset={}", config.comb_offset);
    helper.format_always(
        ctx,
        "t_alloc={}",
        ocudu::interval<unsigned>(config.start_symbol, config.start_symbol + static_cast<unsigned>(config.duration)));
    helper.format_if_verbose(ctx, "rb_start={}", config.prb_start);
    helper.format_always(ctx, "f_alloc={}", config.freq_alloc);
    helper.format_if_verbose(ctx, "power_offset={}", config.power_offset_dB);

    if (helper.is_multiline()) {
      helper.format_if_verbose(ctx, "{:n}", config.precoding_and_beamforming);
    } else {
      helper.format_if_verbose(ctx, "{}", config.precoding_and_beamforming);
    }

    return ctx.out();
  }
};

} // namespace fmt
