// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/common/f1ap_ref_time_info.h"
#include <chrono>
#include <optional>

namespace ocudu {
namespace ocucp {

/// Configuration for F1AP CU-CP.
struct f1ap_configuration {
  /// Timeout for waiting for the DU response during F1AP procedures (Implementation-defined).
  std::chrono::milliseconds proc_timeout{1000};
  /// Whether to enable JSON logging of F1AP Tx and Rx messages.
  bool json_log_enabled = false;
  /// If set, a REFERENCE TIME INFORMATION REPORTING CONTROL is sent to each DU immediately after a successful F1 Setup.
  std::optional<f1ap_ref_time_report_ctrl_request> ref_time_report_ctrl;
};

} // namespace ocucp
} // namespace ocudu
