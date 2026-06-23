// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_impl.h"
#include "mac_sched/ocudu_scheduler_adapter.h"

using namespace ocudu;

mac_impl::mac_impl(const mac_config& params) :
  rnti_table(params.mac_cfg.initial_crnti),
  mac_sched(std::make_unique<ocudu_scheduler_adapter>(
      ocudu_mac_sched_config{params.mac_cfg, params.ctrl_exec, params.timers.get_timer_manager(), params.sched_cfg},
      rnti_table)),
  dl_unit(mac_dl_config{params.ue_exec_mapper,
                        params.cell_exec_mapper,
                        params.ctrl_exec,
                        params.phy_notifier,
                        params.pcap,
                        params.timers.get_timer_manager()},
          *mac_sched,
          rnti_table),
  ul_unit(mac_ul_config{params.ctrl_exec,
                        params.ue_exec_mapper,
                        params.ul_ccch_notifier,
                        *mac_sched,
                        rnti_table,
                        params.pcap,
                        params.timers.get_timer_manager()}),
  ctrl_unit(mac_control_config{params.ul_ccch_notifier,
                               params.ctrl_exec,
                               params.timers,
                               mac_control_config::metrics_config{params.metrics.period,
                                                                  params.metrics.notifier,
                                                                  params.metrics.max_nof_ue_events,
                                                                  params.metrics.sched_ue_enabled}},
            ul_unit,
            dl_unit,
            rnti_table,
            *mac_sched)
{
}
