// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/uci/uci_part2_size_description.h"
#include "ocudu/support/format/delimited_formatter.h"

/// Custom formatter for \c ocudu::uci_part2_size_description::parameter.
template <>
struct fmt::formatter<ocudu::uci_part2_size_description::parameter> {
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
  auto format(const ocudu::uci_part2_size_description::parameter& value, FormatContext& ctx) const
  {
    helper.format_always(ctx, "offset={}", value.offset);
    helper.format_always(ctx, "width={}", value.width);
    return ctx.out();
  }
};

/// Custom formatter for \c ocudu::uci_part2_size_description::entry.
template <>
struct fmt::formatter<ocudu::uci_part2_size_description::entry> {
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
  auto format(const ocudu::uci_part2_size_description::entry& value, FormatContext& ctx) const
  {
    helper.format_always(
        ctx, "params={}", ocudu::span<const ocudu::uci_part2_size_description::parameter>(value.parameters));
    helper.format_always(ctx, "map={}", ocudu::span<const uint16_t>(value.map));
    return ctx.out();
  }
};

/// Custom formatter for \c ocudu::uci_part2_size_description.
template <>
struct fmt::formatter<ocudu::uci_part2_size_description> {
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
  auto format(const ocudu::uci_part2_size_description& description, FormatContext& ctx) const
  {
    helper.format_always(
        ctx, "entries={}", ocudu::span<const ocudu::uci_part2_size_description::entry>(description.entries));

    return ctx.out();
  }
};
