// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_manager/du_manager.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"

namespace ocudu {
namespace odu {

/// Creates an instance of a DU manager.
std::unique_ptr<du_manager> create_du_manager(const du_manager_params& params);

} // namespace odu
} // namespace ocudu
