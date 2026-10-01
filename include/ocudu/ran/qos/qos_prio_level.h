// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "fmt/base.h"

namespace ocudu {

/// \brief QoS Priority Level. See TS 38.473 QoS Priority Level. Values: (0..127).
struct qos_prio_level_t : public bounded_integer<uint8_t, 0, 127> {
  using bounded_integer::bounded_integer;
};

} // namespace ocudu

// Formatters
namespace fmt {
template <>
struct formatter<ocudu::qos_prio_level_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::qos_prio_level_t& qos_prio_level, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "QoS Priority Level={:#x}", qos_prio_level.value());
  }
};

} // namespace fmt
