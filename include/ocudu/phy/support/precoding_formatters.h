// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/precoding_configuration.h"
#include "ocudu/ran/precoding/precoding_weight_matrix_formatters.h"

namespace fmt {

/// \brief Custom formatter for \c precoding_configuration
template <>
struct formatter<ocudu::precoding_configuration> {
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
  auto format(const ocudu::precoding_configuration& config, FormatContext& ctx) const
  {
    format_to(ctx.out(), "prg_size={} ", config.get_prg_size());

    unsigned nof_prg = config.get_nof_prg();

    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      if (i_prg != 0) {
        format_to(ctx.out(), " ");
      }
      format_to(ctx.out(), "prg{}={}", i_prg, config.get_prg_coefficients(i_prg));
    }

    return ctx.out();
  }
};

} // namespace fmt
