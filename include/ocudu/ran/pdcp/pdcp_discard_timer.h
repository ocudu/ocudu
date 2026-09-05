// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/enum_utils.h"
#include "fmt/base.h"
#include <cstdint>

namespace ocudu {

/// PDCP NR discard timer values.
/// This timer is configured only for DRBs. In the transmitter, a new timer is started upon reception of an SDU from
/// upper layer.
/// See TS 38.322 for timer description and TS 38.331 for valid timer durations.
enum class pdcp_discard_timer : int16_t {
  ms10     = 10,
  ms20     = 20,
  ms30     = 30,
  ms40     = 40,
  ms50     = 50,
  ms60     = 60,
  ms75     = 75,
  ms100    = 100,
  ms150    = 150,
  ms200    = 200,
  ms250    = 250,
  ms300    = 300,
  ms500    = 500,
  ms750    = 750,
  ms1500   = 1500,
  infinity = -1
};

constexpr bool pdcp_discard_timer_from_int(pdcp_discard_timer& discard_timer, int num)
{
  switch (num) {
    case 10:
    case 20:
    case 30:
    case 40:
    case 50:
    case 60:
    case 75:
    case 100:
    case 150:
    case 200:
    case 250:
    case 300:
    case 500:
    case 750:
    case 1500:
    case -1:
      discard_timer = static_cast<pdcp_discard_timer>(num);
      return true;
    default:
      return false;
  }
}

} // namespace ocudu

// Formatters.
namespace fmt {

template <>
struct formatter<ocudu::pdcp_discard_timer> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::pdcp_discard_timer discard_timer, FormatContext& ctx) const
  {
    if (discard_timer == ocudu::pdcp_discard_timer::infinity) {
      return format_to(ctx.out(), "infinity");
    }
    return format_to(ctx.out(), "{}", ocudu::to_underlying(discard_timer));
  }
};
} // namespace fmt
