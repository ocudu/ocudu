// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_positioning_handler_factory.h"
#include "du_positioning_manager_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace odu;

std::unique_ptr<f1ap_du_positioning_handler> odu::create_du_positioning_handler(const du_manager_params& du_params,
                                                                                du_cell_manager&         cell_mng,
                                                                                du_ue_manager&           ue_mng,
                                                                                ocudulog::basic_logger&  logger)
{
  return std::make_unique<du_positioning_manager_impl>(du_params, cell_mng, ue_mng, logger);
}
