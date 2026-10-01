// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fmt/base.h"
#include <chrono>
#include <cstdint>

namespace ocudu::odu {

/// \brief Configurable parameters of the F1-U bearer in the DU
struct f1u_config {
  /// Backoff timer for piggy-packing of DDDS (transmit and delivery notifications).
  std::chrono::milliseconds ul_t_notif_timer{5};
  /// RLC queue limit in bytes. Used for initial report of buffer space to the CU-UP.
  uint32_t rlc_queue_bytes_limit;
  /// Log a warning instead of an info message whenever a PDU is dropped.
  bool warn_on_drop = true;
  /// Buffer UL SDUs on startup. Useful during handover to avoid losses.
  bool buffer_ul_on_startup = false;
  /// Size of UL buffer for handover.
  uint32_t ul_buffer_size = 4096;

  bool operator==(const f1u_config& other) const
  {
    return ul_t_notif_timer == other.ul_t_notif_timer and rlc_queue_bytes_limit == other.rlc_queue_bytes_limit and
           warn_on_drop == other.warn_on_drop and buffer_ul_on_startup == other.buffer_ul_on_startup and
           ul_buffer_size == other.ul_buffer_size;
  }
};

} // namespace ocudu::odu

namespace fmt {

// F1-U config formatter
template <>
struct formatter<ocudu::odu::f1u_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::odu::f1u_config cfg, FormatContext& ctx) const
  {
    return format_to(
        ctx.out(),
        "ul_t_notif_timer={}ms rlc_queue_bytes_limit={} warn_on_drop={} buffer_ul_on_startup={} ul_buffer_size={}",
        cfg.ul_t_notif_timer.count(),
        cfg.rlc_queue_bytes_limit,
        cfg.warn_on_drop,
        cfg.buffer_ul_on_startup,
        cfg.ul_buffer_size);
  }
};

} // namespace fmt
