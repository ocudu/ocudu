// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_cell_config.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"

namespace ocudu {
namespace config_helpers {

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline odu::du_cell_config make_default_du_cell_config(const cell_config_builder_params_extended& params = {})
{
  odu::du_cell_config cfg{};
  cfg.tac            = 1;
  cfg.nr_cgi.plmn_id = plmn_identity::test_value();
  cfg.nr_cgi.nci     = nr_cell_identity::create({411, 22}, 1).value();
  cfg.ran            = make_default_ran_cell_config(params);
  return cfg;
}

} // namespace config_helpers
} // namespace ocudu
