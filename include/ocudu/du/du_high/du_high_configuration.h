// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_cell_config.h"
#include "ocudu/du/du_high/du_high_executor_mapper.h"
#include "ocudu/du/du_high/du_manager/du_f1_setup_notifier.h"
#include "ocudu/du/du_high/du_qos_config.h"
#include "ocudu/du/du_high/du_srb_config.h"
#include "ocudu/du/du_high/du_test_mode_config.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/f1ap/gateways/f1c_connection_client.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/mac/mac_config.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/pcap/rlc_pcap.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/rlc/rlc_metrics.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/scheduler/scheduler_metrics.h"
#include <map>

namespace ocudu {

class timer_manager;

namespace odu {

class f1u_du_gateway;
class du_metrics_notifier;

/// RAN-specific parameters of the DU-high.
struct du_high_ran_config {
  std::string                        gnb_du_name = "odu";
  gnb_du_id_t                        gnb_du_id   = gnb_du_id_t::min;
  std::vector<du_cell_config>        cells;
  std::map<srb_id_t, du_srb_config>  srbs;
  std::map<five_qi_t, du_qos_config> qos;
  mac_expert_config                  mac_cfg;
  scheduler_expert_config            sched_cfg;
};

/// Configuration passed to DU-High.
struct du_high_configuration {
  struct metrics_config {
    bool                      enable_sched   = false;
    bool                      enable_mac     = false;
    bool                      enable_rlc     = false;
    bool                      enable_f1ap    = false;
    bool                      enable_du_proc = false;
    std::chrono::milliseconds period{1000};
    unsigned                  max_nof_sched_ue_events = 64;
  };

  du_high_ran_config  ran;
  metrics_config      metrics;
  du_test_mode_config test_cfg;
};

/// DU high dependencies
struct du_high_dependencies {
  du_high_executor_mapper* exec_mapper        = nullptr;
  f1c_connection_client*   f1c_client         = nullptr;
  gtpu_teid_pool*          f1u_teid_allocator = nullptr;
  f1u_du_gateway*          f1u_gw             = nullptr;
  mac_result_notifier*     phy_adapter        = nullptr;
  mac_clock_controller*    timer_ctrl         = nullptr;
  du_metrics_notifier*     du_notifier        = nullptr;
  rlc_metrics_notifier*    rlc_metrics_notif  = nullptr;
  mac_pcap*                mac_p              = nullptr;
  rlc_pcap*                rlc_p              = nullptr;
  /// Optional notifier invoked once after a successful F1 Setup.
  du_f1_setup_complete_notifier* f1_setup_notifier = nullptr;
};

} // namespace odu
} // namespace ocudu
