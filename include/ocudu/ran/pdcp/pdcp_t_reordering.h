// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/enum_utils.h"
#include "fmt/base.h"
#include <cstdint>

namespace ocudu {

/// \brief PDCP NR t-Reordering timer values.
/// This timer is used to detect loss of PDCP Data PDUs.
/// See TS 38.322 for timer description and TS 38.331 for valid timer durations.
enum class pdcp_t_reordering : int16_t {
  ms0      = 0,
  ms1      = 1,
  ms2      = 2,
  ms4      = 4,
  ms5      = 5,
  ms8      = 8,
  ms10     = 10,
  ms15     = 15,
  ms20     = 20,
  ms30     = 30,
  ms40     = 40,
  ms50     = 50,
  ms60     = 60,
  ms80     = 80,
  ms100    = 100,
  ms120    = 120,
  ms140    = 140,
  ms160    = 160,
  ms180    = 180,
  ms200    = 200,
  ms220    = 220,
  ms240    = 240,
  ms260    = 260,
  ms280    = 280,
  ms300    = 300,
  ms500    = 500,
  ms750    = 750,
  ms1000   = 1000,
  ms1250   = 1250,
  ms1500   = 1500,
  ms1750   = 1750,
  ms2000   = 2000,
  ms2250   = 2250,
  ms2500   = 2500,
  ms2750   = 2750,
  ms3000   = 3000,
  infinity = -1
};

/// Conversion of t-Reordering from integer to enum.
inline bool pdcp_t_reordering_from_int(pdcp_t_reordering& t_reord, int num)
{
  switch (num) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
    case 8:
    case 10:
    case 15:
    case 20:
    case 30:
    case 40:
    case 50:
    case 60:
    case 80:
    case 100:
    case 120:
    case 140:
    case 160:
    case 180:
    case 200:
    case 220:
    case 240:
    case 260:
    case 280:
    case 300:
    case 500:
    case 750:
    case 1000:
    case 1250:
    case 1500:
    case 1750:
    case 2000:
    case 2250:
    case 2500:
    case 2750:
    case 3000:
    case -1:
      t_reord = static_cast<pdcp_t_reordering>(num);
      return true;
    default:
      return false;
  }
}

} // namespace ocudu

// Formatters.
namespace fmt {

template <>
struct formatter<ocudu::pdcp_t_reordering> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::pdcp_t_reordering t_reordering, FormatContext& ctx) const
  {
    if (t_reordering == ocudu::pdcp_t_reordering::infinity) {
      return format_to(ctx.out(), "infinity");
    }
    return format_to(ctx.out(), "{}", ocudu::to_underlying(t_reordering));
  }
};
} // namespace fmt
