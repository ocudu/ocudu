// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ue_context.h"
#include "ocudu/cu_up/cu_up_state.h"
#include <cstddef>
#include <vector>

namespace ocudu {

namespace ocuup {

class ue_manager_ctrl
{
public:
  virtual ~ue_manager_ctrl() = default;

  virtual async_task<void> stop()                                                          = 0;
  virtual ue_context*      add_ue(cu_up_e1_index_t e1_index, const ue_context_cfg& ue_cfg) = 0;
  virtual async_task<void> remove_all_ues()                                                = 0;

  /// \brief Removes the given UEs, flagging all of them for removal upfront.
  /// \remark \c ue_indexes is only read while this function runs, not while the returned routine is in flight.
  virtual async_task<void> remove_ues(const std::vector<cu_up_ue_index_t>& ue_indexes) = 0;

  virtual async_task<void> remove_e1_ues(cu_up_e1_index_t e1_index) = 0;

  /// \brief Removes the given UE.
  /// \remark The caller must have verified via \c find_ue that the UE exists and via \c ue_context::remove_pending
  /// that no other removal is already pending for it, without suspending in between.
  virtual async_task<void> remove_ue(cu_up_ue_index_t ue_index) = 0;

  virtual ue_context* find_ue(cu_up_ue_index_t ue_index) = 0;
  virtual size_t      get_nof_ues() const                = 0;
  virtual up_state_t  get_up_state() const               = 0;
};

} // namespace ocuup

} // namespace ocudu
