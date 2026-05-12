// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_ue_resource_config.h"
#include "ue_capability_summary.h"
#include "ocudu/du/du_cell_config.h"
#include <optional>

namespace ocudu {
namespace odu {

class drx_offset_pool;

/// \brief Class responsible for deriving the DRX-Config and measGaps of a given UE.
class du_drx_resource_manager
{
public:
  du_drx_resource_manager(span<const du_cell_config> cell_cfg_list_);
  du_drx_resource_manager(du_drx_resource_manager&&) noexcept = default;
  ~du_drx_resource_manager();

  du_drx_resource_manager& operator=(du_drx_resource_manager&&) noexcept = default;

  /// \brief Set the default DRX config when UE is created.
  void handle_ue_creation(cell_group_config& cell_grp_cfg);

  /// \brief Reserve gaps for DRX/measGaps for a given UE. The resources are stored in the UE's cell group config.
  /// \return true if allocation was successful.
  void handle_ue_cap_update(cell_group_config& cell_grp_cfg, const std::optional<ue_capability_summary>& ue_caps);

  /// \brief Deallocate DRX/measGaps resources previously given to a UE. The resources are returned back to a pool.
  void handle_ue_removal(cell_group_config& cell_grp_cfg);

private:
  span<const du_cell_config> cell_cfg_list;

  // Pool of slot offsets.
  std::unique_ptr<drx_offset_pool> offset_pool;
};

} // namespace odu
} // namespace ocudu
