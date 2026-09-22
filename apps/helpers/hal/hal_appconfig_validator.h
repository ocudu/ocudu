// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "hal_appconfig.h"
#include "ocudu/adt/format.h"
#include <optional>

namespace ocudu {

/// Validates the given HAL application configuration. Returns true on success, false otherwise.
inline bool validate_hal_appconfig(const std::optional<hal_appconfig>& config)
{
#ifdef DPDK_FOUND
  if (config && config->eal_args.empty()) {
    fmt::print("It is mandatory to fill the EAL configuration arguments to initialize DPDK correctly\n");
    return false;
  }
#endif
  return true;
}

} // namespace ocudu
