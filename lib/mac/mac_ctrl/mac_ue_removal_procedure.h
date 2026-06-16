// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../mac_config_interfaces.h"
#include "mac_config.h"
#include "mac_scheduler_configurator.h"

namespace ocudu {

/// Procedure to remove UE from MAC UL, MAC DL, MAC CTRL and MAC scheduler.
class mac_ue_removal_procedure
{
public:
  explicit mac_ue_removal_procedure(const mac_ue_delete_request& msg,
                                    mac_control_config&          cfg_,
                                    mac_ctrl_configurator&       mac_ctrl_,
                                    mac_ul_configurator&         mac_ul_,
                                    mac_dl_configurator&         mac_dl_,
                                    mac_scheduler_configurator&  sched_configurator_) :
    req(msg),
    cfg(cfg_),
    logger(cfg.logger),
    ctrl_mac(mac_ctrl_),
    ul_mac(mac_ul_),
    dl_mac(mac_dl_),
    sched_configurator(sched_configurator_)
  {
  }

  void operator()(coro_context<async_task<mac_ue_delete_response>>& ctx);

  static const char* name() { return "MAC UE Removal"; }

private:
  /// Builds the concurrently-run MAC DL and MAC UL removal tasks (helper needed because an initializer list cannot
  /// hold move-only async_task objects).
  std::vector<async_task<void>> create_ul_dl_rem_tasks();

  mac_ue_delete_request       req;
  mac_control_config&         cfg;
  ocudulog::basic_logger&     logger;
  mac_ctrl_configurator&      ctrl_mac;
  mac_ul_configurator&        ul_mac;
  mac_dl_configurator&        dl_mac;
  mac_scheduler_configurator& sched_configurator;
};

} // namespace ocudu
