// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up.h"
#include "ocudu/cu_up/cu_up_config.h"
#include <memory>

namespace ocudu {
namespace ocuup {

/// Creates an instance of a CU-UP with the given parameters.
std::unique_ptr<cu_up_interface> create_cu_up(const cu_up_config& cfg, cu_up_dependencies dependencies);

} // namespace ocuup
} // namespace ocudu
