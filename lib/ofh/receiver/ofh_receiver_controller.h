// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ofh_closed_rx_window_handler.h"
#include "ofh_message_receiver_impl.h"
#include "ocudu/ofh/ofh_controller.h"

namespace ocudu {
namespace ofh {

/// Open Fronthaul receiver controller.
class receiver_controller : public operation_controller
{
  message_receiver&         msg_receiver;
  closed_rx_window_handler& window_handler;

public:
  receiver_controller(message_receiver& msg_receiver_, closed_rx_window_handler& window_handler_) :
    msg_receiver(msg_receiver_), window_handler(window_handler_)
  {
  }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;
};

} // namespace ofh
} // namespace ocudu
