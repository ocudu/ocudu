// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "ocudu/ru/ofh/ru_ofh_configuration.h"

namespace ocudu {

struct ru_ofh_unit_config;
struct worker_manager_config;

/// Converts and returns the given Open Fronthaul Radio Unit application unit configuration to a Open Fronthaul Radio
/// Unit configuration.
ru_ofh_configuration generate_ru_ofh_config(const ru_ofh_unit_config&                        ru_cfg,
                                            span<const flexible_o_du_ru_config::cell_config> cells,
                                            unsigned                                         max_processing_delay_slots,
                                            bool                                             uses_dpdk);

/// Fills the OFH worker manager parameters of the given worker manager configuration.
void fill_ofh_worker_manager_config(worker_manager_config& config, const ru_ofh_unit_config& ru_cfg);

} // namespace ocudu
