// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ofh_controller.h"
#include "ocudu/ofh/timing/ofh_ota_symbol_boundary_notifier_manager.h"
#include "ocudu/ofh/timing/ofh_timing_metrics_collector.h"

namespace ocudu {
namespace ofh {

/// Open Fronthaul timing manager.
class timing_manager
{
public:
  /// Default destructor.
  virtual ~timing_manager() = default;

  /// Returns the controller of this timing manager.
  virtual operation_controller& get_controller() = 0;

  /// Returns the OTA symbol boundary notifier manager of this timing manager.
  virtual ota_symbol_boundary_notifier_manager& get_ota_symbol_boundary_notifier_manager() = 0;

  /// Returns the timing metrics collector of this timing manager.
  virtual timing_metrics_collector& get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace ocudu
