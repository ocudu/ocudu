// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ofh_controller.h"
#include "ocudu/ofh/ofh_sector.h"
#include "ocudu/ofh/ofh_sector_config.h"
#include "ocudu/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "ocudu/ofh/receiver/ofh_sequence_id_checker.h"
#include "ocudu/ofh/timing/ofh_timing_manager.h"
#include <memory>

namespace ocudu {
class task_executor;

namespace ofh {

/// Creates an Open Fronthaul sequence identifier checker.
std::unique_ptr<sequence_id_checker> create_sequence_id_checker();

/// Open Fronthaul controller config.
struct controller_config {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta;
  /// If set to true, logs late events as warnings, otherwise as info.
  bool enable_log_warnings_for_lates;
  /// Busy waiting enabled flag.
  bool enable_busy_waiting;
};

/// Creates an Open Fronthaul timing manager with the given parameters.
std::unique_ptr<timing_manager>
create_ofh_timing_manager(const controller_config& config, ocudulog::basic_logger& logger, task_executor& executor);

/// Creates an Open Fronthaul sector.
std::unique_ptr<sector> create_ofh_sector(const sector_configuration& sector_cfg, sector_dependencies&& sector_deps);

} // namespace ofh
} // namespace ocudu
