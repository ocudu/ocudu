// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ngap/mobility_management_metrics.h"

namespace ocudu::ocucp {

class mobility_manager_metrics_aggregator
{
public:
  /// \brief Aggregates the metrics for the requested handover preparation.
  void aggregate_requested_handover_preparation();

  /// \brief Aggregates the metrics for the successful handover preparation.
  void aggregate_successful_handover_preparation();

  /// \brief Aggregates the metrics for the requested handover execution.
  void aggregate_requested_handover_execution();

  /// \brief Aggregates the metrics for the successful handover execution.
  void aggregate_successful_handover_execution();

  mobility_management_metrics request_metrics_report() const;

private:
  mobility_management_metrics aggregated_mobility_manager_metrics;
};

} // namespace ocudu::ocucp
