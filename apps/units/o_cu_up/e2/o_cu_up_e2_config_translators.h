// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2ap_config.h"
#include "ocudu/ran/gnb_cu_up_id.h"

namespace ocudu {

struct e2_config;
struct o_cu_up_e2_config;
struct worker_manager_config;

/// Generate E2AP configuration from the given E2 configuration.
e2ap_config generate_e2_config(const e2_config&              config,
                               gnb_id_t                      gnb_id,
                               const std::string&            plmn,
                               std::optional<gnb_cu_up_id_t> gnb_cu_up_id);

/// Fills the O-RAN CU UP E2 worker manager parameters of the given worker manager configuration.
void fill_o_cu_up_e2_worker_manager_config(worker_manager_config& config, const o_cu_up_e2_config& unit_cfg);

} // namespace ocudu
