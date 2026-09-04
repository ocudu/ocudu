// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/rrm/configured_grant_rrm_factory.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/rrm/configured_grant_rrm.h"
#include "ocudu/scheduler/rrm/configured_grant_type1_rrm.h"
#include "ocudu/scheduler/rrm/configured_grant_type2_rrm.h"

using namespace ocudu;

std::unique_ptr<configured_grant_rrm> ocudu::create_configured_grant_rrm(const ran_cell_config& cell_cfg)
{
  if (cell_cfg.init_bwp.cg_cfg.has_value() and cell_cfg.init_bwp.cg_cfg.value().is_type2()) {
    return std::make_unique<configured_grant_type2_rrm>();
  }
  return std::make_unique<configured_grant_type1_rrm>();
}
