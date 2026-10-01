// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/format.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_detector.h"
#include "ocudu/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c prach_detector::configuration.
template <>
struct formatter<ocudu::prach_detector::configuration> {
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
  auto format(const ocudu::prach_detector::configuration& config, FormatContext& ctx) const
  {
    helper.format_always(ctx, "rsi={}", config.root_sequence_index);
    helper.format_if_verbose(ctx,
                             "preambles=[{}, {})",
                             config.start_preamble_index,
                             config.start_preamble_index + config.nof_preamble_indices);
    helper.format_if_verbose(ctx, "format={}", to_string(config.format));
    helper.format_if_verbose(ctx, "set={}", to_string(config.restricted_set));
    helper.format_if_verbose(ctx, "zcz={}", config.zero_correlation_zone);
    helper.format_if_verbose(ctx, "scs={}", to_string(config.ra_scs));
    helper.format_if_verbose(ctx, "nof_rx_ports={}", config.nof_rx_ports);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detection_result::preamble_indication.
template <>
struct formatter<ocudu::prach_detection_result::preamble_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::prach_detection_result::preamble_indication& preamble, FormatContext& ctx) const
  {
    format_to(ctx.out(),
              "{{idx={} ta={:.2f}us detection_metric={:.1f} power_dB={:.2f}}}",
              preamble.preamble_index,
              preamble.time_advance.to_seconds() * 1e6,
              preamble.detection_metric,
              preamble.preamble_power_dB);
    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detection_result.
template <>
struct formatter<ocudu::prach_detection_result> {
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
  auto format(const ocudu::prach_detection_result& result, FormatContext& ctx) const
  {
    helper.format_always(ctx, "rssi={:+.1f}dB", result.rssi_dB);
    helper.format_if_verbose(ctx, "res={:.1f}us", result.time_resolution.to_seconds() * 1e6);
    helper.format_if_verbose(ctx, "max_ta={:.2f}us", result.time_advance_max.to_seconds() * 1e6);
    helper.format_always(ctx,
                         "detected_preambles={}",
                         ocudu::span<const ocudu::prach_detection_result::preamble_indication>(result.preambles));

    return ctx.out();
  }
};

} // namespace fmt
