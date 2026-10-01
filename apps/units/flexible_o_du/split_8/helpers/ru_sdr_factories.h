// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ru_sdr_config.h"
#include "ocudu/ru/ru.h"
#include <memory>

namespace ocudu {
struct flexible_o_du_ru_dependencies;
struct flexible_o_du_ru_config;

/// Creates and returns a SDR RU with the given configuration and dependencies.
std::unique_ptr<radio_unit> create_sdr_radio_unit(const ru_sdr_unit_config&            ru_sdr_cfg,
                                                  const flexible_o_du_ru_config&       du_ru_config,
                                                  const flexible_o_du_ru_dependencies& ru_dependencies);

} // namespace ocudu
