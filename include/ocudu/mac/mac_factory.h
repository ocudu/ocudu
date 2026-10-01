// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac.h"
#include "ocudu/mac/mac_config.h"
#include <memory>

namespace ocudu {

std::unique_ptr<mac_interface> create_mac(const mac_config& mac_cfg);

} // namespace ocudu
