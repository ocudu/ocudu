// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/mac/mac_positioning_measurement_handler.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::odu {

class positioning_measurement_procedure
{
public:
  positioning_measurement_procedure(const du_positioning_meas_request&     req_,
                                    du_cell_manager&                       du_cells_,
                                    du_ue_manager&                         ue_mng_,
                                    const du_manager_params&               du_params_,
                                    const std::map<trp_id_t, du_trp_info>& trps_);

  void operator()(coro_context<async_task<du_positioning_meas_response>>& ctx);

private:
  async_task<void> handle_mac_meas_request();

  du_positioning_meas_response prepare_f1ap_response();

  mac_positioning_measurement_request::cell_info prepare_mac_cell_positioning_request(du_cell_index_t    cell_index,
                                                                                      const srs_carrier& carrier);

  const du_positioning_meas_request      req;
  du_cell_manager&                       du_cells;
  du_ue_manager&                         ue_mng;
  const du_manager_params&               du_params;
  const std::map<trp_id_t, du_trp_info>& trps;

  mac_positioning_measurement_request  mac_req;
  mac_positioning_measurement_response mac_resp;
};

} // namespace ocudu::odu
