// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_clock_controller.h"
#include <memory>

namespace ocudu {

class task_executor;
class io_broker;
class timer_manager;

namespace odu {

/// Creates a clock controller for the DU-high that allows switching between automatic and manual ticking modes.
std::unique_ptr<mac_clock_controller>
create_du_high_clock_controller(timer_manager& timers, io_broker& broker, task_executor& tick_exec);

} // namespace odu
} // namespace ocudu
