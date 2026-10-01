// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/o_cu_cp.h"
#include <memory>

namespace ocudu {
namespace ocucp {

struct o_cu_cp_config;
struct o_cu_cp_dependencies;

std::unique_ptr<o_cu_cp> create_o_cu_cp(const o_cu_cp_config& config, const o_cu_cp_dependencies& dependencies);

} // namespace ocucp
} // namespace ocudu
