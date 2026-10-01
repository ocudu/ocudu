// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_manager/du_configurator.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"
#include "ocudu/f1ap/du/f1ap_du_connection_manager.h"
#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace odu {

class du_cell_manager;

async_task<du_ntn_param_update_response> start_du_ntn_param_update(const du_ntn_param_update_request& req,
                                                                   const du_manager_params&           params,
                                                                   du_cell_manager&                   cell_mng);

/// Procedure that propagates NTN parameter updates to MAC for each affected cell.
class du_mac_ntn_param_update_procedure
{
public:
  du_mac_ntn_param_update_procedure(const du_ntn_param_update_request& sib_update_,
                                    const du_manager_params&           du_params_,
                                    du_cell_manager&                   cell_mng_);

  void operator()(coro_context<async_task<du_ntn_param_update_response>>& ctx);

  const char* name() const { return "NTN Parameters Update"; }

private:
  /// Handle NTN parameter update for a single cell (including optional SI version bump).
  async_task<mac_cell_reconfig_response> handle_cell_update(const du_cell_ntn_param_update_request& cell_req,
                                                            du_cell_index_t                         cell_idx);

  /// Update NTN assistance information in cell configuration.
  bool update_ntn_assistance_info(du_cell_index_t cell_idx, const du_cell_ntn_param_update_request& cell_req) const;

  /// Send F1AP gNB-DU Configuration Update notification to CU.
  async_task<gnbdu_config_update_response> handle_f1_gnbdu_config_update();

  const du_ntn_param_update_request request;
  const du_manager_params&          du_params;
  du_cell_manager&                  cell_mng;
  error_type<std::string>           req_validation_outcome;
  ocudulog::basic_logger&           logger;

  unsigned                                next_cell_idx    = 0;
  du_cell_index_t                         cell_index       = INVALID_DU_CELL_INDEX;
  const du_cell_ntn_param_update_request* current_cell_req = nullptr;
  du_ntn_param_update_response            resp;

  static_vector<du_cell_index_t, MAX_NOF_DU_CELLS> cells_requiring_cu_notif;
};

} // namespace odu
} // namespace ocudu
