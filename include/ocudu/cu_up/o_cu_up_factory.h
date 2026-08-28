// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/o_cu_up.h"
#include "ocudu/cu_up/o_cu_up_config.h"

namespace ocudu::ocuup {

/// O-RAN CU-UP interface with the given configuration and dependencies.
std::unique_ptr<o_cu_up> create_o_cu_up(const o_cu_up_config& config, o_cu_up_dependencies&& dependencies);

} // namespace ocudu::ocuup
