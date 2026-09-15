// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_metrics_handler.h"

namespace ocudu::ocucp {

/// Interface used to capture the DU metrics from a single CU-CP DU.
class du_metrics_handler
{
public:
  virtual ~du_metrics_handler() = default;

  /// \brief Handle new request for metrics relative to a connected DU.
  virtual cu_cp_metrics_report::du_info handle_du_metrics_report_request() const = 0;
};

/// Interface used to capture the DU metrics from all the connected DUs to the CU-CP.
class du_repository_metrics_handler
{
public:
  virtual ~du_repository_metrics_handler() = default;

  /// \brief Handle new metrics request for all the DU nodes connected to the CU-CP.
  virtual std::vector<cu_cp_metrics_report::du_info> handle_du_metrics_report_request() const = 0;
};

} // namespace ocudu::ocucp
