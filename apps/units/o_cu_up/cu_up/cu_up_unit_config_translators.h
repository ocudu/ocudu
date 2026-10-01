// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up_config.h"

namespace ocudu {

struct cu_up_unit_config;
struct worker_manager_config;

/// Converts and returns the given gnb application configuration to a CU-UP configuration.
ocuup::cu_up_config generate_cu_up_config(const cu_up_unit_config& config);

/// Converts and returns the given gnb application QoS configuration to a CU-UP configuration.
std::map<five_qi_t, ocuup::cu_up_qos_config> generate_cu_up_qos_config(const cu_up_unit_config& cu_up_config);

/// Fills the CU-UP worker manager parameters of the given worker manager configuration.
void fill_cu_up_worker_manager_config(worker_manager_config& config, const cu_up_unit_config& unit_cfg);

} // namespace ocudu
