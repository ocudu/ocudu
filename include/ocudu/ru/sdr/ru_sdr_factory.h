// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ru/ru.h"
#include <memory>

namespace ocudu {

struct ru_sdr_configuration;
struct ru_sdr_dependencies;

/// Returns an SDR Radio Unit using the given configuration.
std::unique_ptr<radio_unit> create_sdr_ru(const ru_sdr_configuration& config, const ru_sdr_dependencies& dependencies);

} // namespace ocudu
