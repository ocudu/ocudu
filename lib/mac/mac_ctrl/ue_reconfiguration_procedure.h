// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../mac_config_interfaces.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

struct mac_control_config;
class mac_scheduler_configurator;

class mac_ue_reconfiguration_procedure
{
public:
  explicit mac_ue_reconfiguration_procedure(const mac_ue_reconfiguration_request& req_,
                                            mac_control_config&                   cfg_,
                                            mac_ul_configurator&                  mac_ul_,
                                            mac_dl_configurator&                  mac_dl_,
                                            mac_scheduler_configurator&           sched_cfg_,
                                            std::optional<rnti_t>                 allocated_cs_rnti_ = {});

  void operator()(coro_context<async_task<mac_ue_reconfiguration_response>>& ctx);

  static const char* name() { return "MAC UE Reconfiguration"; }

private:
  mac_ue_reconfiguration_response handle_result(bool result);

  mac_ue_reconfiguration_request req;
  mac_control_config&            cfg;
  ocudulog::basic_logger&        logger;
  mac_ul_configurator&           ul_unit;
  mac_dl_configurator&           dl_unit;
  mac_scheduler_configurator&    sched_cfg;
  std::optional<rnti_t>          allocated_cs_rnti;

  bool add_ue_result  = false;
  bool sched_conf_res = true;
};

} // namespace ocudu
