// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/pdcp/pdcp_entity.h"

namespace ocudu {

/// CU-UP PDCP metrics.
class cu_up_pdcp_metrics_impl final
{
  pdcp_metrics_container metrics;

public:
  explicit cu_up_pdcp_metrics_impl(const pdcp_metrics_container& metrics_) : metrics(metrics_) {}

  const pdcp_metrics_container& get_metrics() const { return metrics; }
};

} // namespace ocudu
