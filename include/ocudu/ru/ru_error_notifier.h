// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// Radio Unit error context.
struct ru_error_context {
  /// Slot context.
  slot_point slot;
  /// Radio sector identifier.
  unsigned sector;
};

/// Radio Unit interface error notifier.
class ru_error_notifier
{
public:
  /// Default destructor.
  virtual ~ru_error_notifier() = default;

  /// \brief Notifies a late downlink message.
  ///
  /// \param[in] context Context of the error.
  virtual void on_late_downlink_message(const ru_error_context& context) = 0;

  /// \brief Notifies a late uplink request message.
  ///
  /// \param[in] context Context of the error.
  virtual void on_late_uplink_message(const ru_error_context& context) = 0;

  /// \brief Notifies a late PRACH request message.
  ///
  /// \param[in] context Context of the error.
  virtual void on_late_prach_message(const ru_error_context& context) = 0;
};

} // namespace ocudu
