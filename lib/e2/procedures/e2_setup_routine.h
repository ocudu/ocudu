// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_node_component_config_provider.h"
#include "ocudu/e2/e2ap_configuration.h"
#include "ocudu/e2/e2sm/e2sm_manager.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/timers.h"
#include <map>

namespace ocudu {

class e2_setup_routine
{
public:
  e2_setup_routine(const e2ap_configuration&          cfg,
                   e2_node_component_config_provider& node_cfg_provider,
                   e2sm_manager&                      e2sm_mngr,
                   e2_connection_manager&             e2_conn_mng,
                   timer_factory                      timers,
                   ocudulog::basic_logger&            logger);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "E2 Setup Routine"; }

private:
  async_task<e2_setup_response_message> start_e2_setup_request();
  void                                  handle_e2_setup_response(const e2_setup_response_message& msg);

  const e2ap_configuration&          cfg;
  e2_node_component_config_provider& node_cfg_provider;
  e2sm_manager&                      e2sm_mngr;
  e2_connection_manager&             e2_conn_mng;
  timer_factory                      timers;
  ocudulog::basic_logger&            logger;

  e2_setup_response_message                           response_msg = {};
  std::map<uint16_t, asn1::e2ap::ran_function_item_s> candidate_ran_functions;
};

} // namespace ocudu
