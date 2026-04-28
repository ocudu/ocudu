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
#include "ocudu/f1u/du/f1u_gateway.h"
#include "ocudu/mac/mac_manager.h"
#include "ocudu/mac/mac_ue_control_information_handler.h"
#include "ocudu/pcap/rlc_pcap.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/rlc/rlc_metrics.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include <map>

namespace ocudu {

class timer_manager;
class mac_metrics_notifier;
class scheduler_metrics_notifier;

namespace odu {

class du_metrics_notifier;

struct du_manager_params {
  struct ran_params {
    std::string                        gnb_du_name;
    gnb_du_id_t                        gnb_du_id;
    uint8_t                            rrc_version;
    std::vector<du_cell_config>        cells;
    std::map<srb_id_t, du_srb_config>  srbs;
    std::map<five_qi_t, du_qos_config> qos;
  };

  struct service_params {
    timer_manager&                timers;
    task_executor&                du_mng_exec;
    du_high_ue_executor_mapper&   ue_execs;
    du_high_cell_executor_mapper& cell_execs;
  };

  struct f1ap_config_params {
    f1ap_connection_manager&       conn_mng;
    f1ap_ue_context_manager&       ue_mng;
    f1ap_metrics_collector&        metrics;
    du_f1_setup_complete_notifier* f1_setup_complete_notifier = nullptr;
  };

  struct f1u_config_params {
    gtpu_teid_pool& f1u_teid_allocator;
    f1u_du_gateway& f1u_gw;
  };

  struct rlc_config_params {
    mac_ue_control_information_handler&          mac_ue_info_handler;
    f1ap_message_handler&                        f1ap_rx_msg_handler;
    f1ap_rrc_message_transfer_procedure_handler& f1ap_rx_proc_handler;
    rlc_pcap&                                    pcap_writer;
    rlc_metrics_notifier*                        rlc_metrics_notif = nullptr;
  };

  struct mac_config_params {
    /// Interface to configure the MAC layer.
    mac_manager&            mgr;
    scheduler_expert_config sched_cfg;
  };

  struct metrics_config_params {
    std::chrono::milliseconds period{1000};
    du_metrics_notifier*      du_metrics    = nullptr;
    bool                      f1ap_enabled  = false;
    bool                      mac_enabled   = false;
    bool                      sched_enabled = false;
    bool                      proc_enabled  = false;
  };

  ran_params            ran;
  service_params        services;
  f1ap_config_params    f1ap;
  f1u_config_params     f1u;
  rlc_config_params     rlc;
  mac_config_params     mac;
  metrics_config_params metrics;
  du_test_mode_config   test_cfg;
};

} // namespace odu
} // namespace ocudu
