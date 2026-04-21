// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/srs/srs_channel_matrix.h"

namespace fmt {

/// \brief Custom formatter for \c srs_channel_matrix.
template <>
struct formatter<ocudu::srs_channel_matrix> {
  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::srs_channel_matrix& matrix, FormatContext& ctx) const
  {
    unsigned nof_rx_ports = matrix.get_nof_rx_ports();
    unsigned nof_tx_ports = matrix.get_nof_tx_ports();

    format_to(ctx.out(), "[");
    for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
      if (i_rx_port != 0) {
        format_to(ctx.out(), ";");
      }
      for (unsigned i_tx_port = 0; i_tx_port != nof_tx_ports; ++i_tx_port) {
        if (i_tx_port != 0) {
          format_to(ctx.out(), ",");
        }
        format_to(ctx.out(), "{:+.3f}", matrix.get_coefficient(i_rx_port, i_tx_port));
      }
    }
    format_to(ctx.out(), "]");

    return ctx.out();
  }
};

} // namespace fmt
