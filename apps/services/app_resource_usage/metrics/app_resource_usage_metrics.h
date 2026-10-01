// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/resource_usage/resource_usage_metrics.h"

namespace ocudu {

/// System resources usage metrics.
class resource_usage_metrics_impl final
{
  resource_usage_metrics metrics;

public:
  explicit resource_usage_metrics_impl(const resource_usage_metrics& metrics_) : metrics(metrics_) {}

  const resource_usage_metrics& get_metrics() const { return metrics; }
};

} // namespace ocudu
