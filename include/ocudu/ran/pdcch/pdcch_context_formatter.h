// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pdcch/pdcch_context.h"
#include "ocudu/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c pdcch_context.
template <>
struct formatter<ocudu::pdcch_context> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::pdcch_context& context, FormatContext& ctx) const
  {
    helper.format_always(ctx, "ss_id={}", fmt::underlying(context.ss_id));
    helper.format_always(ctx, "format={}", context.dci_format);
    if (context.harq_feedback_timing.has_value()) {
      helper.format_if_verbose(ctx, "harq_feedback_timing={}", context.harq_feedback_timing.value());
    }
    return ctx.out();
  }

private:
  ocudu::delimited_formatter helper;
};

} // namespace fmt
