// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ngap_repository.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocucp {

async_task<bool>
start_amf_reconnection(ngap_interface& ngap, timer_factory timers, std::chrono::milliseconds reconnection_retry_time);

/// \brief Handles the reconnection between the CU-CP and AMF.
class amf_reconnection_routine
{
public:
  amf_reconnection_routine(ngap_interface&           ngap_,
                           timer_factory             timers,
                           std::chrono::milliseconds reconnection_retry_time_);

  static std::string name() { return "AMF Reconnection Routine"; }

  void operator()(coro_context<async_task<bool>>& ctx);

private:
  ngap_interface&         ngap;
  ocudulog::basic_logger& logger;

  unique_timer              amf_tnl_connection_retry_timer;
  std::chrono::milliseconds reconnection_retry_time;

  ngap_ng_setup_result result_msg = {};
  bool                 success    = false;
};

} // namespace ocucp
} // namespace ocudu
