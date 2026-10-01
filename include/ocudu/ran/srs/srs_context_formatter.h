// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/srs/srs_context.h"
#include "ocudu/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c srs_context.
template <>
struct formatter<ocudu::srs_context> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::srs_context& context, FormatContext& ctx) const
  {
    helper.format_always(ctx, "sector_id={}", context.sector_id);
    helper.format_always(ctx, "rnti={}", context.rnti);
    return ctx.out();
  }

private:
  ocudu::delimited_formatter helper;
};

} // namespace fmt
