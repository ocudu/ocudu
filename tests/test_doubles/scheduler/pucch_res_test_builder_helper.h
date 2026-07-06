// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../lib/du/du_high/du_manager/ran_resource_management/du_pucch_resource_manager.h"
#include "ocudu/scheduler/config/ran_cell_config.h"

namespace ocudu {

class cell_configuration;

/// Provides a wrapper for \ref du_pucch_resource_manager to build the PUCCH dedicated configuration for UEs to be used
/// in tests.
class pucch_res_builder_test_helper
{
public:
  explicit pucch_res_builder_test_helper(unsigned max_pucchs_per_slot = 64);

  void setup(const ran_cell_config& cell_cfg);

  /// Build a new UE's PUCCH config (embedded in \ref ue_cell_config) and add this to the
  /// \ref du_pucch_resource_manager to keep track of which cell PUCCH resources have been used.
  bool add_build_new_ue_pucch_cfg(ue_cell_config& ue_cell_cfg);

  /// Release the cell PUCCH resources previously allocated to a UE (via \ref add_build_new_ue_pucch_cfg), so they
  /// return to the free pool and can be reused by later UEs (mirrors the DU freeing resources on UE detach).
  void remove_ue_pucch_cfg(ue_cell_config& ue_cell_cfg);

private:
  // Build the \ref du_pucch_resource_manager with the given configuration. This function is called only once, at the
  // time the firt UE is added.
  void init_pucch_res_mgr(const serving_cell_config& base_ue_cfg);

  odu::du_pucch_resource_manager pucch_res_mgr;
};

} // namespace ocudu
