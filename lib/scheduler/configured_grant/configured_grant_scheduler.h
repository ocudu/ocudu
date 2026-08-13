// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct cell_resource_allocator;
class ue_cell_configuration;

/// Configured grant scheduler interface, which handles the scheduling of configured grant opportunities.
class configured_grant_scheduler
{
public:
  virtual ~configured_grant_scheduler() = default;

  /// Runs the configured grant scheduler for the current slot.
  /// \param[out,in] res_alloc struct with scheduling results.
  virtual void run_slot(cell_resource_allocator& res_alloc) = 0;

  /// Removes the UE from the internal list of UEs to be scheduled.
  /// \param[in] ue_cfg UE dedicated configuration of the UE to be removed.
  virtual void rem_ue(const ue_cell_configuration& ue_cfg) = 0;

  /// Updates the configured grant configuration of this UE, if there are any changes w.r.t. the previous configuration.
  /// \param[in] new_ue_cfg New UE dedicated configuration of the UE to be reconfigured.
  /// \param[in] old_ue_cfg If not nullptr, old UE dedicated configuration of the UE to be reconfigured.
  /// \remark nullptr is passed to \ref old_ue_cfg when the UE is created through F1AP.
  virtual void add_reconf_ue(const ue_cell_configuration& new_ue_cfg, const ue_cell_configuration* old_ue_cfg) = 0;
};

} // namespace ocudu
