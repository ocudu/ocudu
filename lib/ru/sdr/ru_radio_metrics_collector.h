// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/radio/radio_event_notifier.h"
#include "ocudu/ru/sdr/ru_sdr_metrics.h"
#include <atomic>

namespace ocudu {

/// Radio Unit radio notification counter.
class ru_radio_metrics_collector : public radio_event_notifier
{
public:
  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override;

  /// Collect radio metrics.
  void collect_metrics(radio_metrics& metrics);

private:
  std::atomic<unsigned> late_count      = {};
  std::atomic<unsigned> underflow_count = {};
  std::atomic<unsigned> overflow_count  = {};
};

} // namespace ocudu
