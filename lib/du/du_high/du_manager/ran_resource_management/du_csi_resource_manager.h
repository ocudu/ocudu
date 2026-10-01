// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_ue_resource_config.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/du/du_high/du_test_mode_config.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/rrm/ue_capability_summary.h"
#include <optional>

namespace ocudu {
namespace odu {

/// \brief Manages the CSI report configuration of DU UEs.
///
/// Applies the codebook that every UE supports on UE creation and selects the codebook offered by the cell once the UE
/// capabilities are known.
class du_csi_resource_manager
{
public:
  du_csi_resource_manager(span<const du_cell_config> cell_cfg_list_, const du_test_mode_config& test_cfg_);

  /// \brief Sets the CSI codebook of a newly created UE (before capabilities are decoded).
  void alloc_resources(cell_group_config& cell_grp_cfg, du_ue_index_t ue_index);

  /// \brief Updates the CSI codebook based on decoded UE capabilities.
  void update_resources(cell_group_config& cell_grp_cfg, const ue_capability_summary& ue_caps, du_ue_index_t ue_index);

  /// \brief Releases CSI resources on UE removal.
  void dealloc_resources(cell_group_config& cell_grp_cfg);

private:
  void apply_config(cell_group_config&                          cell_grp_cfg,
                    const std::optional<ue_capability_summary>& ue_caps,
                    du_ue_index_t                               ue_index);

  /// \brief Determines whether the Type-II codebook offered by a cell can be configured for a UE.
  ///
  /// The support is reported per band in field \e type2 of \e codebookParameters, in Information Element
  /// \e MIMO-ParametersPerBand.
  ///
  /// \return \c nullptr if the codebook can be configured, otherwise the reason why it cannot.
  const char* type2_unsupported_reason(du_cell_index_t                             cell_idx,
                                       const std::optional<ue_capability_summary>& ue_caps) const;

  span<const du_cell_config> cell_cfg_list;
  const du_test_mode_config& test_cfg;
  ocudulog::basic_logger&    logger;
};

} // namespace odu
} // namespace ocudu
