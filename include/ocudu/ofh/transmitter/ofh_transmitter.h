// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "ocudu/ofh/transmitter/ofh_downlink_handler.h"
#include "ocudu/ofh/transmitter/ofh_uplink_request_handler.h"

namespace ocudu {
namespace ofh {

class operation_controller;
class transmitter_metrics_collector;

/// Open Fronthaul transmitter interface.
class transmitter
{
public:
  /// Default destructor.
  virtual ~transmitter() = default;

  /// Returns the uplink request handler of this Open Fronthaul transmitter.
  virtual uplink_request_handler& get_uplink_request_handler() = 0;

  /// Returns the downlink handler of this Open Fronthaul transmitter.
  virtual downlink_handler& get_downlink_handler() = 0;

  /// Returns the controller of this Open Fronthaul transmitter.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the OTA symbol boundary notifier of this Open Fronthaul transmitter.
  virtual ota_symbol_boundary_notifier& get_ota_symbol_boundary_notifier() = 0;

  /// Returns the metrics collector of this Open Fronthaul transmitter or nullptr if metrics are disabled.
  virtual transmitter_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace ocudu
