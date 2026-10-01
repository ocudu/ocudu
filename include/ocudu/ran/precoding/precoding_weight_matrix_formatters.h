// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/precoding/precoding_weight_matrix.h"
#include "ocudu/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c precoding_configuration
template <>
struct formatter<ocudu::precoding_weight_matrix> {
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
  auto format(const ocudu::precoding_weight_matrix& matrix, FormatContext& ctx) const
  {
    unsigned nof_layers = matrix.get_nof_layers();
    unsigned nof_ports  = matrix.get_nof_ports();

    format_to(ctx.out(), "[");
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      if (i_layer != 0) {
        format_to(ctx.out(), ";");
      }
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        if (i_port != 0) {
          format_to(ctx.out(), ",");
        }
        format_to(ctx.out(), "{:+.2f}", matrix.get_coefficient(i_layer, i_port));
      }
    }
    format_to(ctx.out(), "]");

    return ctx.out();
  }
};

} // namespace fmt
