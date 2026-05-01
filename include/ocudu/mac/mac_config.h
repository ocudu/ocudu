// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/mac/mac_executor_mapper.h"
#include "ocudu/mac/mac_pdu_handler.h"
#include "ocudu/pcap/mac_pcap.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"

namespace ocudu {

class timer_manager;
class mac_metrics_notifier;
class mac_clock_controller;

/// \brief Implementation-specific parameters used to tune MAC operation.
struct mac_expert_config {
  /// Initial C-RNTI to assign to created UEs.
  rnti_t initial_crnti = to_rnti(0x4601);
  /// \brief Implementation-specific parameters used to tune MAC operation per cell.
  struct mac_expert_cell_config {
    /// \brief Maximum number of consecutive DL KOs before an RLF is reported.
    unsigned max_consecutive_dl_kos = 100;
    /// \brief Maximum number of consecutive UL KOs before an RLF is reported.
    unsigned max_consecutive_ul_kos = 100;
    /// \brief Maximum number of consecutive non-decoded CSI before an RLF is reported.
    unsigned max_consecutive_csi_dtx = 100;
  };
  std::vector<mac_expert_cell_config> configs;
};

/// \brief Configuration passed to MAC during its instantiation.
struct mac_config {
  struct metrics_config {
    std::chrono::milliseconds period{1000};
    bool                      mac_enabled;
    bool                      sched_enabled;
    unsigned                  max_nof_ue_events = 64;
    mac_metrics_notifier&     notifier;
  };

  mac_ul_ccch_notifier&     ul_ccch_notifier;
  mac_ue_executor_mapper&   ue_exec_mapper;
  mac_cell_executor_mapper& cell_exec_mapper;
  task_executor&            ctrl_exec;
  mac_result_notifier&      phy_notifier;
  mac_expert_config         mac_cfg;
  mac_pcap&                 pcap;
  mac_clock_controller&     timers;
  metrics_config            metrics;
  // Parameters passed to MAC scheduler.
  scheduler_expert_config sched_cfg;
};

} // namespace ocudu
