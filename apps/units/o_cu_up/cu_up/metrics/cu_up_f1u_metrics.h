// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/cu_up/f1u_bearer.h"
#include "ocudu/f1u/cu_up/f1u_metrics.h"

namespace ocudu {

/// CU-UP F1-U metrics.
class cu_up_f1u_metrics_impl final
{
  ocuup::f1u_metrics_container metrics;

public:
  explicit cu_up_f1u_metrics_impl(const ocuup::f1u_metrics_container& metrics_) : metrics(metrics_) {}

  const ocuup::f1u_metrics_container& get_metrics() const { return metrics; }
};

} // namespace ocudu
