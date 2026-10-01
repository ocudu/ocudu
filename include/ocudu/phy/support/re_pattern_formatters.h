// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/re_pattern.h"
#include "ocudu/support/format/delimited_formatter.h"
#include "fmt/base.h"

namespace fmt {

/// \brief Custom formatter for \c re_pattern.
template <>
struct formatter<ocudu::re_pattern> {
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
  auto format(const ocudu::re_pattern& pattern, FormatContext& ctx) const
  {
    helper.format_always(
        ctx, "symb={:n}", static_cast<ocudu::bounded_bitset<ocudu::MAX_NSYMB_PER_SLOT>>(pattern.symbols));
    helper.format_always(ctx, "rb={:n}", pattern.crb_mask);
    helper.format_always(
        ctx, "re={:n}", static_cast<ocudu::bounded_bitset<ocudu::NOF_SUBCARRIERS_PER_RB>>(pattern.re_mask));

    return ctx.out();
  }
};

} // namespace fmt
