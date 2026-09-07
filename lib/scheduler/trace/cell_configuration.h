// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/bwp/bwp_configuration.h"
#include "ocudu/ran/du_cell_index.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_constants.h"

namespace ocudu::schedtrace {

struct cell_configuration {
  du_cell_index_t                                                            cell_index;
  bwp_configuration                                                          init_ul_bwp;
  bwp_configuration                                                          init_dl_bwp;
  static_vector<pucch_resource, pucch_constants::MAX_NOF_TOT_CELL_RESOURCES> pucch_resources;
};

} // namespace ocudu::schedtrace
