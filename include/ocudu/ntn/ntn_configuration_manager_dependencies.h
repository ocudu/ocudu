// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ntn/ntn_doppler_compensation_handler.h"
#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include "ocudu/ntn/ntn_time_provider.h"

namespace ocudu {

class timer_manager;
class task_executor;

namespace ocudu_ntn {

/// NTN configuration manager implementation dependencies.
struct ntn_configuration_manager_dependencies {
  std::unique_ptr<ntn_sib19_update_handler>         sib19_msg_update_handler;
  std::unique_ptr<ntn_time_provider>                time_provider;
  std::unique_ptr<ntn_doppler_compensation_handler> doppler_handler;
  timer_manager&                                    timers;
  task_executor&                                    executor;
};

} // namespace ocudu_ntn
} // namespace ocudu
