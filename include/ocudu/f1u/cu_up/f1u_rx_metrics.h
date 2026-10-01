// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/engineering_notation.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/timers.h"
#include "fmt/format.h"

/*
 * This file will hold the interfaces and structures for the
 * F1-U RX metrics collection. This also includes formatting
 * helpers for printing the metrics.
 */
namespace ocudu {
namespace ocuup {

/// This struct will hold relevant metrics for the F1-U RX
struct f1u_rx_metrics_container {
  uint32_t num_pdus;         ///< Number of handled PDUs.
  uint32_t num_dropped_pdus; ///< Number of dropped PDUs.
  uint32_t num_sdus;         ///< Number of forwarded SDUs (transport PDUs).
  uint32_t num_sdu_bytes;    ///< Number of forwarded SDU bytes (transport PDU bytes).
  uint32_t num_dds;          ///< Number of handled data delivery status messages.
  uint32_t num_dds_failures; ///< Number of failures handling data delivery status messages.
  unsigned counter;

  f1u_rx_metrics_container copy()
  {
    f1u_rx_metrics_container copy = *this;
    ++counter;
    return copy;
  }
};

inline std::string format_f1u_rx_metrics(timer_duration metrics_period, const f1u_rx_metrics_container& m)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "num_pdus={} num_dropped_pdus={} num_sdus={} sdu_rate={}bps num_dds={} num_dds_failures={}",
                 scaled_fmt_integer(m.num_pdus, false),
                 scaled_fmt_integer(m.num_dropped_pdus, false),
                 scaled_fmt_integer(m.num_sdus, false),
                 float_to_eng_string(static_cast<float>(m.num_sdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
                 scaled_fmt_integer(m.num_dds, false),
                 scaled_fmt_integer(m.num_dds_failures, false));
  return to_c_str(buffer);
}
} // namespace ocuup
} // namespace ocudu

namespace fmt {
// F1-U RX metrics formatter
template <>
struct formatter<ocudu::ocuup::f1u_rx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::ocuup::f1u_rx_metrics_container& m, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "num_pdus={} num_dropped_pdus={} num_sdus={} num_sdu_bytes={} num_dds={} num_dds_failures={}",
                     m.num_pdus,
                     m.num_dropped_pdus,
                     m.num_sdus,
                     m.num_sdu_bytes,
                     m.num_dds,
                     m.num_dds_failures);
  }
};
} // namespace fmt
