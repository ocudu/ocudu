// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::odu {

class du_ue_positioning_info_procedure
{
public:
  du_ue_positioning_info_procedure(const du_positioning_info_request& req_,
                                   du_cell_manager&                   du_cells_,
                                   du_ue_manager&                     ue_mng_);

  void operator()(coro_context<async_task<du_positioning_info_response>>& ctx);

private:
  du_positioning_info_response create_response(bool success);

  const du_positioning_info_request req;
  du_cell_manager&                  du_cells;
  du_ue_manager&                    ue_mng;
  du_ue&                            ue;
};

} // namespace ocudu::odu
