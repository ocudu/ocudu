// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ru_ofh_config.h"
#include "ocudu/ru/ru.h"
#include <memory>

namespace ocudu {
struct flexible_o_du_ru_config;
struct flexible_o_du_ru_dependencies;

/// Creates an Open Fronthaul Radio Unit using the given configuration.
std::unique_ptr<radio_unit> create_ofh_radio_unit(const ru_ofh_unit_config&            ru_cfg,
                                                  const flexible_o_du_ru_config&       ru_config,
                                                  const flexible_o_du_ru_dependencies& ru_dependencies,
                                                  bool                                 uses_dpdk);

} // namespace ocudu
