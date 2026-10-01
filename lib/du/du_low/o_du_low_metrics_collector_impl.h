// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_low/o_du_low_metrics_collector.h"

namespace ocudu {
namespace odu {

class du_low_metrics_collector;

/// O-DU low metrics collector implementation.
class o_du_low_metrics_collector_impl : public o_du_low_metrics_collector
{
  du_low_metrics_collector* collector = nullptr;

public:
  explicit o_du_low_metrics_collector_impl(du_low_metrics_collector* collector_) : collector(collector_) {}

  // See interface for documentation.
  void collect_metrics(o_du_low_metrics& metrics) override;

  /// Returns true if the metrics are enabled, otherwise false.
  bool enabled() const { return collector != nullptr; }
};

} // namespace odu
} // namespace ocudu
