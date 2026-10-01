// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ether {

class receiver_metrics_collector;
class receiver_operation_controller;

/// \brief Describes an Ethernet receiver.
///
/// The receiver notifies incoming Ethernet frames through the \c frame_notifier interface.
class receiver
{
public:
  /// Default destructor.
  virtual ~receiver() = default;

  /// Returns the operation controller of this Ethernet receiver.
  virtual receiver_operation_controller& get_operation_controller() = 0;

  /// Returns the metrics collector of this Ethernet receiver or nullptr if metrics are disabled.
  virtual receiver_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ether
} // namespace ocudu
