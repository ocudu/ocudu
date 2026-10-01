// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ntn/ntn_doppler_compensation_handler.h"
#include "ocudu/ntn/ntn_meas_info_update_handler.h"
#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include "ocudu/ntn/ntn_time_provider.h"

namespace ocudu {

class timer_manager;
class task_executor;

namespace ocudu_ntn {

/// NTN configuration manager implementation dependencies.
struct ntn_configuration_manager_dependencies {
  std::unique_ptr<ntn_sib19_update_handler>     sib19_msg_update_handler;
  std::unique_ptr<ntn_time_provider>            time_provider;
  std::unique_ptr<ntn_meas_info_update_handler> meas_info_update_handler;
  /// Handler applying the feeder link Doppler compensation. Not owned: it bridges to the RU, which is created after
  /// and destroyed before this manager, so the handler is owned above both. Left unset when the deployment applies
  /// no Doppler compensation, in which case the computed values are simply not applied.
  ntn_doppler_compensation_handler* doppler_handler = nullptr;
  timer_manager&                    timers;
  task_executor&                    executor;
};

} // namespace ocudu_ntn
} // namespace ocudu
