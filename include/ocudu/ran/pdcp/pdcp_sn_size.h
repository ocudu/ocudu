// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include "fmt/format.h"
#include <cstdint>

namespace ocudu {

/// PDCP NR sequence number field.
enum class pdcp_sn_size : uint8_t { size12bits = 12, size18bits = 18, invalid };
inline bool pdcp_sn_size_from_uint(pdcp_sn_size& sn_size, uint16_t num)
{
  if (num == 12) {
    sn_size = pdcp_sn_size::size12bits;
    return true;
  }
  if (num == 18) {
    sn_size = pdcp_sn_size::size18bits;
    return true;
  }
  return false;
}

/// \brief Convert PDCP SN size from enum to unsigned integer.
constexpr uint8_t pdcp_sn_size_to_uint(pdcp_sn_size sn_size)
{
  ocudu_assert(sn_size != pdcp_sn_size::invalid, "Invalid PDCP SN size");
  return static_cast<uint8_t>(sn_size);
}

} // namespace ocudu

namespace fmt {

// SN size
template <>
struct formatter<ocudu::pdcp_sn_size> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::pdcp_sn_size sn_size, FormatContext& ctx) const
  {
    switch (sn_size) {
      case ocudu::pdcp_sn_size::invalid:
        return format_to(ctx.out(), "invalid");
      default:
        return format_to(ctx.out(), "{}", pdcp_sn_size_to_uint(sn_size));
    }
  }
};

} // namespace fmt
