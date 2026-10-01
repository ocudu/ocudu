// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_radio_metrics_collector.h"

using namespace ocudu;

void ru_radio_metrics_collector::on_radio_rt_event(const event_description& description)
{
  // Count events.
  switch (description.type) {
    case radio_event_type::LATE:
      late_count.fetch_add(1, std::memory_order_relaxed);
      break;
    case radio_event_type::UNDERFLOW:
      underflow_count.fetch_add(1, std::memory_order_relaxed);
      break;
    case radio_event_type::OVERFLOW:
      overflow_count.fetch_add(1, std::memory_order_relaxed);
      break;
    case radio_event_type::UNDEFINED:
    case radio_event_type::START_OF_BURST:
    case radio_event_type::END_OF_BURST:
    case radio_event_type::OTHER:
      // Uncounted.
      break;
  }
}

void ru_radio_metrics_collector::collect_metrics(radio_metrics& metrics)
{
  metrics.late_count      = late_count.exchange(0, std::memory_order_relaxed);
  metrics.underflow_count = underflow_count.exchange(0, std::memory_order_relaxed);
  metrics.overflow_count  = overflow_count.exchange(0, std::memory_order_relaxed);
}
