// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "ocudu/ru/sdr/ru_sdr_configuration.h"

namespace ocudu {

struct ru_sdr_unit_config;
struct worker_manager_config;

/// Converts and returns the given RU SDR application unit configuration to a SDR RU configuration.
ru_sdr_configuration generate_ru_sdr_config(const ru_sdr_unit_config&                        ru_cfg,
                                            span<const flexible_o_du_ru_config::cell_config> cells,
                                            unsigned max_processing_delay_slots);

/// Fills the SDR worker manager parameters of the given worker manager configuration.
void fill_sdr_worker_manager_config(worker_manager_config& config, const ru_sdr_unit_config& ru_cfg);

} // namespace ocudu
