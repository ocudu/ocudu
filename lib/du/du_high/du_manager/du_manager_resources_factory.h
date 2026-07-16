// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_manager/du_manager_resources.h"

namespace ocudu::odu {

struct du_manager_params;

du_manager_resources create_du_manager_resources(const du_manager_params& du_params);

} // namespace ocudu::odu
