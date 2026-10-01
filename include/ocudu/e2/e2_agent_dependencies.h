// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2ap_config.h"
#include "ocudu/e2/e2sm/e2sm.h"
#include "ocudu/support/timers.h"
#include <memory>
#include <vector>

namespace ocudu {

class e2_connection_client;
class e2_node_component_config_provider;

/// E2SM Service Module to be added into E2 agent.
struct e2sm_module {
  uint32_t                        ran_func_id;
  std::string                     oid;
  std::unique_ptr<e2sm_handler>   e2sm_packer;
  std::unique_ptr<e2sm_interface> e2sm_iface;
};

/// O-RAN E2 Agent dependencies.
struct e2_agent_dependencies {
  ocudulog::basic_logger&                            logger;
  e2_connection_client&                              e2_client;
  timer_factory&                                     timers;
  task_executor&                                     task_exec;
  std::unique_ptr<e2_node_component_config_provider> node_component_config_provider;
  std::vector<e2sm_module>                           e2sm_modules;
};

} // namespace ocudu
