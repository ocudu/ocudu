// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct e1ap_message;

/// This interface is used to push the received E1AP PDUs to the CU-CP or CU-UP.
class e1ap_message_handler
{
public:
  virtual ~e1ap_message_handler() = default;

  /// Handle the incoming E1AP message.
  virtual void handle_message(const e1ap_message& msg) = 0;
};

/// Interface used by E1AP to inform about events.
class e1ap_event_handler
{
public:
  virtual ~e1ap_event_handler()         = default;
  virtual void handle_connection_loss() = 0;
};

/// This interface notifies the reception of new E1AP messages over the E1AP interface.
class e1ap_message_notifier
{
public:
  virtual ~e1ap_message_notifier() = default;

  /// This callback is invoked on each received E1AP message.
  virtual void on_new_message(const e1ap_message& msg) = 0;
};

} // namespace ocudu
