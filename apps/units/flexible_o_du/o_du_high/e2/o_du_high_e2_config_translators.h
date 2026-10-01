// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2ap_config.h"

namespace ocudu {

struct o_du_high_e2_config;
struct worker_manager_config;

/// Converts and returns the given gnb application configuration to a E2 configuration.
e2ap_config generate_e2_config(const o_du_high_e2_config& config,
                               gnb_id_t                   gnb_id,
                               const std::string&         plmn,
                               std::optional<gnb_du_id_t> gnb_du_id);

/// Fills the O-RAN DU high E2 worker manager parameters of the given worker manager configuration.
void fill_o_du_high_e2_worker_manager_config(worker_manager_config& config, const o_du_high_e2_config& unit_cfg);

} // namespace ocudu
