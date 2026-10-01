// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/slot_point_extended.h"
#include <chrono>

namespace ocudu {
namespace fapi {

/// Slot indication message.
struct slot_indication {
  slot_point_extended slot;
  /// Vendor specific properties.
  std::chrono::time_point<std::chrono::system_clock> time_point;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::slot_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::slot_indication& msg, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "Slot.indication time_point={}", msg.time_point.time_since_epoch().count());
  }
};
} // namespace fmt
