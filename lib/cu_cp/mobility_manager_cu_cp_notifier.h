// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_intra_cu_ho_types.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// Methods used by mobility manager to signal handover events to the CU-CP.
class mobility_manager_cu_cp_notifier
{
public:
  virtual ~mobility_manager_cu_cp_notifier() = default;

  /// \brief Notify the CU-CP about a required intra-CU handover.
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
