// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../mac_config_interfaces.h"
#include "mac_config.h"
#include "mac_scheduler_configurator.h"
#include "ocudu/mac/mac.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

class mac_ue_create_request_procedure
{
public:
  explicit mac_ue_create_request_procedure(const mac_ue_create_request& req_,
                                           mac_control_config&          cfg_,
                                           mac_ctrl_configurator&       mac_ctrl_,
                                           mac_ul_configurator&         mac_ul_,
                                           mac_dl_configurator&         mac_dl_,
                                           mac_scheduler_configurator&  sched_configurator_) :
    req(req_),
    cfg(cfg_),
    logger(cfg.logger),
    ctrl_unit(mac_ctrl_),
    ul_unit(mac_ul_),
    dl_unit(mac_dl_),
    sched_configurator(sched_configurator_)
  {
  }

  void operator()(coro_context<async_task<mac_ue_create_response>>& ctx);

  static const char* name() { return "MAC UE Creation"; }

private:
  async_task<void>              cancel_ue_creation();
  mac_ue_create_response        handle_mac_ue_create_result(bool result);
  std::vector<async_task<bool>> create_ul_dl_ue_tasks();

  mac_ue_create_request       req;
  mac_control_config&         cfg;
  ocudulog::basic_logger&     logger;
  mac_ctrl_configurator&      ctrl_unit;
  mac_ul_configurator&        ul_unit;
  mac_dl_configurator&        dl_unit;
  mac_scheduler_configurator& sched_configurator;

  rnti_t            crnti_assigned = rnti_t::INVALID_RNTI;
  std::vector<bool> add_mac_ue_result;
  bool              add_sched_ue_result = false;
};

} // namespace ocudu
