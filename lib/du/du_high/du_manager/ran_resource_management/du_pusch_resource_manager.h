// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_ue_resource_config.h"
#include "ue_capability_summary.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/du/du_high/du_test_mode_config.h"
#include "ocudu/ran/harq_id.h"
#include <optional>

namespace ocudu {
namespace odu {

/// \brief Manages the PUSCH configuration for DU UEs.
///
/// Applies conservative defaults on UE creation and updates the configuration when UE capabilities become known.
class du_pusch_resource_manager
{
public:
  du_pusch_resource_manager(span<const du_cell_config> cell_cfg_list_, const du_test_mode_config& test_cfg_);

  /// \brief Sets conservative PUSCH defaults for a newly created UE (before capabilities are decoded).
  void alloc_resources(cell_group_config& cell_grp_cfg);

  /// \brief Updates PUSCH configuration based on decoded UE capabilities.
  void update_resources(cell_group_config& cell_grp_cfg, const ue_capability_summary& ue_caps);

  /// \brief Releases PUSCH resources on UE removal.
  void dealloc_resources(cell_group_config& cell_grp_cfg);

private:
  void apply_config(cell_group_config& cell_grp_cfg, const std::optional<ue_capability_summary>& ue_caps);

  pusch_mcs_table select_mcs_table(du_cell_index_t cell_idx, const std::optional<ue_capability_summary>& ue_caps) const;
  tx_scheme_codebook_subset select_tx_codebook_subset(du_cell_index_t                             cell_idx,
                                                      const std::optional<ue_capability_summary>& ue_caps) const;
  unsigned select_srs_nof_ports(du_cell_index_t cell_idx, const std::optional<ue_capability_summary>& ue_caps) const;
  unsigned select_max_rank(du_cell_index_t cell_idx, const std::optional<ue_capability_summary>& ue_caps) const;
  unsigned select_max_nof_harqs(du_cell_index_t cell_idx, const std::optional<ue_capability_summary>& ue_caps) const;
  unsigned select_dci_harq_field_size(du_cell_index_t                             cell_idx,
                                      const std::optional<ue_capability_summary>& ue_caps) const;
  harq_ul_mode_mask select_harq_mode(du_cell_index_t                             cell_idx,
                                     const std::optional<ue_capability_summary>& ue_caps) const;

  span<const du_cell_config> cell_cfg_list;
  const du_test_mode_config& test_cfg;
};

} // namespace odu
} // namespace ocudu
