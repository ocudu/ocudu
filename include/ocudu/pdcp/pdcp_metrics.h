// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/pdcp/pdcp_entity.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/rb_id.h"

namespace ocudu {

struct pdcp_metrics_container;

/// \brief Notifier interface used to report PDCP metrics.
class pdcp_metrics_notifier
{
public:
  virtual ~pdcp_metrics_notifier() = default;

  /// \brief This method will be called periodically to report the latest PDCP metrics statistics.
  virtual void report_metrics(const pdcp_metrics_container& metrics) = 0;
};
} // namespace ocudu
