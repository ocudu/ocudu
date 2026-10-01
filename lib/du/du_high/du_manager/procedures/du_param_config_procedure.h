// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager_repository.h"
#include "ocudu/du/du_high/du_manager/du_configurator.h"
#include "ocudu/mac/mac_cell_manager.h"

namespace ocudu {
namespace odu {

/// DU procedure to dynamically change the parameters of the DU during its operation.
class du_param_config_procedure
{
public:
  du_param_config_procedure(const du_param_config_request& request_,
                            const du_manager_params&       init_du_params_,
                            du_cell_manager&               du_cells_);

  void operator()(coro_context<async_task<du_param_config_response>>& ctx);

private:
  /// Helper to configure the DU cell parameters.
  bool handle_cell_config_updates();

  /// Helper to run F1AP gNB-DU config update procedure.
  async_task<gnbdu_config_update_response> handle_f1_gnbdu_config_update();

  /// Helper to update MAC of the new cell parameters.
  async_task<mac_cell_reconfig_response> handle_mac_cell_update(const du_cell_reconfig_result& changed_cell) const;

  const du_param_config_request request;
  const du_manager_params&      du_params;
  du_cell_manager&              du_cells;
  ocudulog::basic_logger&       logger;

  unsigned next_cell_idx = 0;

  static_vector<du_cell_reconfig_result, MAX_NOF_DU_CELLS> changed_cells;

  du_param_config_response resp{};
};

} // namespace odu
} // namespace ocudu
