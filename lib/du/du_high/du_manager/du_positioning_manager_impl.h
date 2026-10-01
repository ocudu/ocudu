// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_cell_manager.h"
#include "du_ue/du_ue_manager.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"

namespace ocudu::odu {

/// Handler of positioning related context and procedures in the DU.
class du_positioning_manager_impl : public f1ap_du_positioning_handler
{
public:
  du_positioning_manager_impl(const du_manager_params& du_params_,
                              du_cell_manager&         cell_mng_,
                              du_ue_manager&           ue_mng_,
                              ocudulog::basic_logger&  logger_);

  du_trp_info_response request_trp_info() override;

  async_task<du_positioning_info_response> request_positioning_info(const du_positioning_info_request& req) override;

  async_task<du_positioning_meas_response>
  request_positioning_measurement(const du_positioning_meas_request& req) override;

private:
  void update_trp_info();

  const du_manager_params& du_params;
  du_cell_manager&         cell_mng;
  du_ue_manager&           ue_mng;
  ocudulog::basic_logger&  logger;

  /// Map of TRP IDs to TRP information.
  std::map<trp_id_t, du_trp_info> trps;
};

} // namespace ocudu::odu
