// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp_cho_types.h"
#include "cu_cp_intra_cu_ho_types.h"
#include "ocudu/support/async/async_task.h"
#include <chrono>

namespace ocudu::ocucp {

/// \brief Mobility manager configuration.
struct mobility_manager_cfg {
  bool trigger_handover_from_measurements = false; ///< Set to true to trigger HO when neighbor becomes stronger.
  bool enable_ngap_metrics                = false; ///< Set to true to enable inter gNB handover metrics collection.
  bool enable_rrc_metrics                 = false; ///< Set to true to enable intra gNB metrics collection.
  /// Auto-trigger CHO after UE setup if UE/cell readiness checks pass.
  bool trigger_cho_on_ue_setup = false;
  /// Default CHO execution timeout. If it expires before CHO completion, CHO is cancelled.
  std::chrono::milliseconds cho_timeout{10000};
};

/// Methods used by mobility manager to signal handover events to the CU-CP.
class mobility_manager_cu_cp_notifier
{
public:
  virtual ~mobility_manager_cu_cp_notifier() = default;

  /// \brief Notify the CU-CP about an required intra-CU handover.
  virtual async_task<cu_cp_intra_cu_handover_response>
  on_intra_cu_handover_required(const cu_cp_intra_cu_handover_request& request,
                                cu_cp_du_index_t                       source_du_index,
                                cu_cp_du_index_t                       target_du_index) = 0;

  /// \brief Notify the CU-CP to run intra-CU CHO coordinator flow.
  ///
  /// This entrypoint executes preparation and (auto) execution/cancellation orchestration.
  virtual async_task<cu_cp_intra_cu_cho_response>
  on_intra_cu_cho_required(const cu_cp_intra_cu_cho_request& request) = 0;
};

} // namespace ocudu::ocucp
