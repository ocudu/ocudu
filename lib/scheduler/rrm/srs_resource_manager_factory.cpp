// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/rrm/srs_resource_manager_factory.h"
#include "srs_resource_manager_aperiodic.h"
#include "srs_resource_manager_periodic.h"
#include "ocudu/scheduler/config/ran_cell_config.h"

using namespace ocudu;

std::unique_ptr<srs_resource_manager> ocudu::create_srs_resource_manager(const ran_cell_config& first_cell_cfg)
{
  if (first_cell_cfg.init_bwp.srs_cfg.srs_type_enabled == srs_type::aperiodic) {
    return std::make_unique<srs_resource_manager_aperiodic>();
  }
  return std::make_unique<srs_resource_manager_periodic>();
}
