// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/rlc/rlc_metrics.h"

namespace ocudu {

/// RLC metrics.
class rlc_metrics_impl final
{
  rlc_metrics metrics;

public:
  explicit rlc_metrics_impl(const rlc_metrics& metrics_) : metrics(metrics_) {}

  const rlc_metrics& get_metrics() const { return metrics; }
};

} // namespace ocudu
