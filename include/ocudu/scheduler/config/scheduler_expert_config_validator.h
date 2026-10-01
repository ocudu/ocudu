// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include <string>

namespace ocudu {

/// \brief Checks whether the provided scheduler expert configuration is valid.
///
/// \param config Scheduler expert configuration.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
inline error_type<std::string> is_scheduler_expert_config_valid(const scheduler_expert_config& config)
{
  // :TODO: Implement me!
  return {};
}

} // namespace ocudu
