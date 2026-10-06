// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <chrono>
#include <optional>

namespace ocudu {

// Mobility Management metrics, see TS 28.552 section 5.1.1.6.
struct mobility_management_metrics {
  // Section 5.1.1.6.1: Inter-gNB handovers.
  unsigned nof_handover_preparations_requested                    = 0;
  unsigned nof_successful_handover_preparations                   = 0;
  unsigned nof_inter_gnb_handover_resource_allocations_requested  = 0;
  unsigned nof_successful_inter_gnb_handover_resource_allocations = 0;
  unsigned nof_inter_gnb_handover_executions_requested            = 0;
  unsigned nof_successful_inter_gnb_handover_executions           = 0;

  // Section 5.1.1.6.2: Intra-gNB handovers.
  unsigned nof_handover_executions_requested  = 0;
  unsigned nof_successful_handover_executions = 0;

  /// \brief Intra-gNB handover execution time, from the transmission of the RRC Reconfiguration to the source DU to the
  /// reception of the RRC Reconfiguration Complete from the target DU. TS 28.552 does not define this metric. The
  /// values are empty when no intra-gNB handover finished in the reporting period.
  std::optional<std::chrono::milliseconds> mean_intra_gnb_handover_execution_time;
  std::optional<std::chrono::milliseconds> max_intra_gnb_handover_execution_time;

  // Section 5.1.1.6.6: Inter-gNB conditional handovers.
  unsigned nof_ues_configured_with_inter_gnb_cho = 0;

  // Section 5.1.1.6.7: Intra-gNB conditional handovers.
  unsigned nof_ues_configured_with_intra_gnb_cho   = 0;
  unsigned nof_successful_intra_gnb_cho_executions = 0;

  // Section 5.1.3.7.1: Intra-gNB handovers in a split gNB deployment.
  unsigned nof_intra_gnb_handover_preparations_requested  = 0;
  unsigned nof_successful_intra_gnb_handover_preparations = 0;
};

} // namespace ocudu
