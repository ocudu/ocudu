// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/enum_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include "fmt/base.h"

namespace ocudu {

/// See TS 38.463: Five QI valid values: (0..255)
constexpr uint16_t MAX_FIVEQI = 255;

/// Five QI.
enum class five_qi_t : uint16_t { min = 0, max = MAX_FIVEQI, invalid = MAX_FIVEQI + 1 };

/// Convert integer to Five QI type.
constexpr five_qi_t uint_to_five_qi(uint16_t five_qi)
{
  ocudu_assert(
      five_qi < MAX_FIVEQI, "Invalid 5QI {} - Must be between [{}..{}]", five_qi, five_qi_t::min, five_qi_t::max);
  return static_cast<five_qi_t>(five_qi);
}

} // namespace ocudu

// Formatters
namespace fmt {
template <>
struct formatter<ocudu::five_qi_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::five_qi_t five_qi, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "5QI={:#x}", to_underlying(five_qi));
  }
};

} // namespace fmt
