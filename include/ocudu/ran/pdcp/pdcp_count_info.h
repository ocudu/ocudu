// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fmt/format.h"
#include <cstdint>

namespace ocudu {

struct pdcp_count_info {
  uint32_t sn  = 0;
  uint32_t hfn = 0;
};

} // namespace ocudu

namespace fmt {

// SN size
template <>
struct formatter<ocudu::pdcp_count_info> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::pdcp_count_info count_info, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "sn={} hfn={}", count_info.sn, count_info.hfn);
  }
};

} // namespace fmt
