// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <chrono>

namespace ocudu::ocucp {

/// Mobility manager configuration.
struct mobility_manager_config {
  bool trigger_handover_from_measurements = false; ///< Set to true to trigger HO when neighbor becomes stronger.
  bool enable_ngap_metrics                = false; ///< Set to true to enable inter gNB handover metrics collection.
  bool enable_rrc_metrics                 = false; ///< Set to true to enable intra gNB metrics collection.
  /// Auto-trigger CHO after UE setup if UE/cell readiness checks pass.
  bool trigger_cho_on_ue_setup = false;
  /// Default CHO execution timeout. If it expires before CHO completion, CHO is cancelled.
  std::chrono::milliseconds cho_timeout{10000};
};

} // namespace ocudu::ocucp
