// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_low/du_low_metrics_collector.h"
#include "ocudu/phy/upper/upper_phy_metrics_collector.h"

namespace ocudu {
namespace odu {

class du_low_metrics_collector_impl : public du_low_metrics_collector
{
  upper_phy_metrics_collector* upper_phy_collector = nullptr;

public:
  du_low_metrics_collector_impl() = default;

  explicit du_low_metrics_collector_impl(upper_phy_metrics_collector* upper_phy_collector_) :
    upper_phy_collector(upper_phy_collector_)
  {
  }

  // See interface for documentation.
  void collect_metrics(du_low_metrics& metrics) override;

  /// Returns true if metrics are disabled, otherwise false.
  bool disabled() const { return upper_phy_collector == nullptr; }

  /// Returns true if metrics are enabled, otherwise false.
  bool enabled() const { return !disabled(); }
};

} // namespace odu
} // namespace ocudu
