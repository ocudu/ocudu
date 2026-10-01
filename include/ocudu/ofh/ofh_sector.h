// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ofh {

class operation_controller;
class error_notifier;
class receiver;
class metrics_collector;
class transmitter;

/// \brief Open Fronthaul sector interface.
///
/// A sector represents the minimum entity to send and receive data from the Open Fronthaul.
class sector
{
public:
  virtual ~sector() = default;

  /// Returns the Open Fronthaul receiver of this sector.
  virtual receiver& get_receiver() = 0;

  /// Returns the Open Fronthaul transmitter of this sector.
  virtual transmitter& get_transmitter() = 0;

  /// Returns the Open Fronthaul controller of this sector.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the Open Fronthaul metrics collector of this sector or nullptr if metrics are disabled.
  virtual metrics_collector* get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace ocudu
