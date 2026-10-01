// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_metrics_handler.h"

namespace ocudu {
namespace ocucp {

/// Interface used to capture the UE metrics from the CU-CP UE manager.
class ue_metrics_handler
{
public:
  virtual ~ue_metrics_handler() = default;

  /// \brief Handle new request for UE metrics.
  virtual std::vector<cu_cp_metrics_report::ue_info> handle_ue_metrics_report_request() const = 0;
};

} // namespace ocucp
} // namespace ocudu
