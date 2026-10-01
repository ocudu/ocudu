// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/phy_time_unit.h"
#include <optional>

namespace ocudu {
namespace fapi {

/// Appends the timing advance value to the given buffer if there is a timing advance.
template <typename FormatContext>
inline auto append_time_advance(FormatContext& ctx, std::optional<phy_time_unit> timing_advance, subcarrier_spacing scs)
{
  if (!timing_advance) {
    return ctx.out();
  }

  // The measured timing advance can be negative, use the signed conversion.
  return format_to(
      ctx.out(), " ta={} ({:.1f}ns)", timing_advance->to_Ta_signed(scs), timing_advance->to_seconds() * 1e9);
}

} // namespace fapi
} // namespace ocudu
