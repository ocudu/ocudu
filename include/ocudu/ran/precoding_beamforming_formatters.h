// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/format.h"
#include "ocudu/ran/beamforming/beam_identifier_formatters.h"
#include "ocudu/ran/precoding/precoding_weight_matrix_formatters.h"
#include "ocudu/ran/precoding_beamforming_configuration.h"
#include "ocudu/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c precoding_beamforming_configuration
template <>
struct formatter<ocudu::precoding_beamforming_configuration> {
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
  auto format(const ocudu::precoding_beamforming_configuration& config, FormatContext& ctx) const
  {
    helper.format_always(ctx, "prg_size={} ", config.get_prg_size());

    unsigned nof_prg = config.get_nof_prg();

    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      const ocudu::precoding_beamforming_composite& prg = config.get_prg(i_prg);
      helper.format_always(
          ctx, "prg{}=[beams={} mimo={}]", i_prg, ocudu::span<const ocudu::beam_identifier>(prg.beams), prg.mimo);
    }

    return ctx.out();
  }
};

} // namespace fmt
