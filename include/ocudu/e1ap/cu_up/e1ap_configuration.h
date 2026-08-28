// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/support/timers.h"

namespace ocudu::ocuup {

class e1ap_cu_up_manager_notifier;
class e1_connection_client;

/// Holds the E1AP CU-CP implementation dependencies.
struct e1ap_cu_up_impl_dependencies {
  e1_connection_client&        e1_client_handler;
  e1ap_cu_up_manager_notifier& cu_up_notifier;
  timer_manager&               timers;
  task_executor&               cu_up_exec;
};

/// Configuration for E1AP CU-UP.
struct e1ap_configuration {
  uint32_t max_nof_ues = 16384;
  /// Whether to enable JSON logging of E1AP Tx and Rx messages.
  bool             json_log_enabled = false;
  timer_duration   metrics_period{0};
  cu_up_e1_index_t e1_index{cu_up_e1_index_t::invalid};
};

} // namespace ocudu::ocuup
