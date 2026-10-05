// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_timing_context.h"
#include "ocudu/phy/lower/processors/downlink/downlink_processor_notifier.h"
#include <vector>

namespace ocudu {

class downlink_processor_notifier_spy : public downlink_processor_notifier
{
public:
  void on_tti_boundary(const lower_phy_timing_context& context) override { tti_boundaries.emplace_back(context); }

  const std::vector<lower_phy_timing_context>& get_tti_boundaries() const { return tti_boundaries; }

  unsigned get_total_count() const { return tti_boundaries.size(); }

  void clear_notifications() { tti_boundaries.clear(); }

private:
  std::vector<lower_phy_timing_context> tti_boundaries;
};

} // namespace ocudu
