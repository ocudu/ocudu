// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2_cu.h"
#include "ocudu/e2/e2ap_config.h"
#include "ocudu/e2/gateways/e2_connection_client.h"
#include "ocudu/support/timers.h"
#include <memory>

namespace ocudu {

class e2_node_component_config_provider;

/// Creates a instance of an E2 interface (with subscription manager)
std::unique_ptr<e2_agent>
create_e2_cu_up_agent(const e2ap_config&                                 e2ap_cfg_,
                      e2_connection_client&                              e2_client_,
                      e2_cu_metrics_interface*                           e2_metrics_,
                      timer_factory                                      timers_,
                      task_executor&                                     e2_exec_,
                      std::unique_ptr<e2_node_component_config_provider> node_component_config_provider_);

} // namespace ocudu
