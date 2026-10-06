// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mobility_manager_metrics_aggregator.h"

using namespace ocudu;
using namespace ocucp;

void mobility_manager_metrics_aggregator::aggregate_requested_handover_preparation()
{
  ++aggregated_mobility_manager_metrics.nof_handover_preparations_requested;
}

void mobility_manager_metrics_aggregator::aggregate_successful_handover_preparation()
{
  ++aggregated_mobility_manager_metrics.nof_successful_handover_preparations;
}

void mobility_manager_metrics_aggregator::aggregate_requested_inter_gnb_handover_resource_allocation()
{
  ++aggregated_mobility_manager_metrics.nof_inter_gnb_handover_resource_allocations_requested;
}

void mobility_manager_metrics_aggregator::aggregate_successful_inter_gnb_handover_resource_allocation()
{
  ++aggregated_mobility_manager_metrics.nof_successful_inter_gnb_handover_resource_allocations;
}

void mobility_manager_metrics_aggregator::aggregate_requested_inter_gnb_handover_execution()
{
  ++aggregated_mobility_manager_metrics.nof_inter_gnb_handover_executions_requested;
}

void mobility_manager_metrics_aggregator::aggregate_successful_inter_gnb_handover_execution()
{
  ++aggregated_mobility_manager_metrics.nof_successful_inter_gnb_handover_executions;
}

void mobility_manager_metrics_aggregator::aggregate_requested_intra_gnb_handover_preparation()
{
  ++aggregated_mobility_manager_metrics.nof_intra_gnb_handover_preparations_requested;
}

void mobility_manager_metrics_aggregator::aggregate_successful_intra_gnb_handover_preparation()
{
  ++aggregated_mobility_manager_metrics.nof_successful_intra_gnb_handover_preparations;
}

/// \brief Aggregates the metrics for the requested handover execution.
void mobility_manager_metrics_aggregator::aggregate_requested_handover_execution()
{
  ++aggregated_mobility_manager_metrics.nof_handover_executions_requested;
}

/// \brief Aggregates the metrics for the successful handover execution.
void mobility_manager_metrics_aggregator::aggregate_successful_handover_execution()
{
  ++aggregated_mobility_manager_metrics.nof_successful_handover_executions;
}

void mobility_manager_metrics_aggregator::aggregate_ue_configured_with_cho(bool has_intra_gnb_candidate,
                                                                           bool has_inter_gnb_candidate)
{
  if (has_intra_gnb_candidate) {
    ++aggregated_mobility_manager_metrics.nof_ues_configured_with_intra_gnb_cho;
  }
  if (has_inter_gnb_candidate) {
    ++aggregated_mobility_manager_metrics.nof_ues_configured_with_inter_gnb_cho;
  }
}

void mobility_manager_metrics_aggregator::aggregate_successful_intra_gnb_cho_execution()
{
  ++aggregated_mobility_manager_metrics.nof_successful_intra_gnb_cho_executions;
}

mobility_management_metrics mobility_manager_metrics_aggregator::request_metrics_report() const
{
  return aggregated_mobility_manager_metrics;
}
