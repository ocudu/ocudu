// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/ru/ru_error_notifier.h"
#include "ocudu/ru/ru_timing_notifier.h"
#include "ocudu/ru/ru_uplink_plane.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

/// Collects the necessary parameters for the dummy radio unit.
struct ru_dummy_configuration {
  /// Flag that enables (or not) metrics.
  bool are_metrics_enabled;
  /// Subcarrier spacing - Determines the slot timing.
  subcarrier_spacing scs;
  /// Number of sectors.
  unsigned nof_sectors;
  /// \brief Number of slots is notified in advance of the transmission time.
  ///
  /// Sets the maximum allowed processing delay in slots.
  unsigned max_processing_delay_slots;
  /// DL processing processing delay in slots. See \ref ru_dummy_appconfig::dl_processing_delay for more information.
  unsigned dl_processing_delay;
  /// Time scaling, \ref ru_dummy_unit_config::time_scaling for more details.
  float time_scaling;
};

/// Collects the necessary dependencies for the dummy radio unit.
struct ru_dummy_dependencies {
  /// Logger.
  ocudulog::basic_logger& logger;
  /// Asynchronous task executor.
  task_executor* executor = nullptr;
  /// Radio Unit uplink plane received symbol notifier.
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  /// Radio Unit timing notifier.
  ru_timing_notifier& timing_notifier;
  /// Radio Unit error notifier.
  ru_error_notifier& error_notifier;
};

} // namespace ocudu
