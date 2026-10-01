// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ethernet/ethernet_frame_notifier.h"

namespace ocudu {
namespace ofh {

class operation_controller;

class message_receiver_metrics_collector;

/// Open Fronthaul message receiver interface.
///
/// This class listens to incoming Ethernet frames and decodes them as Open Fronthaul messages. Once a new message is
/// detected, is it handled to the corresponding data flow for further processing.
class message_receiver : public ether::frame_notifier
{
public:
  /// Default destructor.
  virtual ~message_receiver() = default;

  /// Returns the operation controller of this Open Fronthaul message receiver.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the message receiver performance metrics collector.
  virtual message_receiver_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace ocudu
