// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include <memory>

namespace ocudu {

/// Creates an instance of an CU-CP.
std::unique_ptr<ocucp::cu_cp> create_cu_cp(const ocucp::cu_cp_configuration& cfg);

} // namespace ocudu
