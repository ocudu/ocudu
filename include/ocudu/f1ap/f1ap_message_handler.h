// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct f1ap_message;

/// Handles incoming F1AP messages.
class f1ap_message_handler
{
public:
  virtual ~f1ap_message_handler() = default;

  /// Handles the given F1AP message.
  virtual void handle_message(const f1ap_message& msg) = 0;
};

} // namespace ocudu
