// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu::ocucp {

struct xnap_message;

/// Notifier interface used to notify outgoing XNAP messages.
class xnap_message_notifier
{
public:
  virtual ~xnap_message_notifier() = default;

  /// This callback is invoked on each outgoing XNAP message aside the XN Setup request.
  virtual bool on_new_message(const xnap_message& msg) = 0;
};

} // namespace ocudu::ocucp
