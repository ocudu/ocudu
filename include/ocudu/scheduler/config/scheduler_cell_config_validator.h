// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/scheduler/scheduler_configurator.h"

namespace ocudu {
namespace config_validators {

/// \brief Validates \c sched_cell_configuration_request_message used to add a cell.
/// \param[in] msg scheduler cell configuration message to be validated.
/// \param[in] expert_cfg static configuration for the scheduler.
/// \return In case an invalid parameter is detected, returns a string containing an error message.
error_type<std::string>
validate_sched_cell_configuration_request_message(const sched_cell_configuration_request_message& msg,
                                                  const scheduler_expert_config&                  expert_cfg);

} // namespace config_validators
} // namespace ocudu
