// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu::odu {

class du_cell_manager;
class du_ue_manager;
struct du_manager_params;

std::unique_ptr<f1ap_du_positioning_handler> create_du_positioning_handler(const du_manager_params& du_params,
                                                                           du_cell_manager&         cell_mng,
                                                                           du_ue_manager&           ue_mng,
                                                                           ocudulog::basic_logger&  logger);

} // namespace ocudu::odu
