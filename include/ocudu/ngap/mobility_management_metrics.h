// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

// Mobility Management metrics, see TS 28.552 section 5.1.1.6.
struct mobility_management_metrics {
  // Section 5.1.1.6.1: Inter-gNB handovers.
  unsigned nof_handover_preparations_requested  = 0;
  unsigned nof_successful_handover_preparations = 0;

  // Section 5.1.1.6.2: Intra-gNB handovers.
  unsigned nof_handover_executions_requested  = 0;
  unsigned nof_successful_handover_executions = 0;
};

} // namespace ocudu
