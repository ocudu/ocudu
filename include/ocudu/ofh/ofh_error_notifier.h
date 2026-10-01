// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/slot_point.h"

namespace ocudu {
namespace ofh {

/// Open Fronthaul error context.
struct error_context {
  /// Slot context.
  slot_point slot;
  /// Radio sector identifier.
  unsigned sector;
};

/// Open Fronthaul error notifier.
class error_notifier
{
public:
  virtual ~error_notifier() = default;

  /// \brief Notifies a late downlink message.
  ///
  /// \param[in] context Context of the error.
  virtual void on_late_downlink_message(const error_context& context) = 0;

  /// \brief Notifies a late uplink request message.
  ///
  /// \param[in] context Context of the error.
  virtual void on_late_uplink_message(const error_context& context) = 0;

  /// \brief Notifies a late PRACH request message.
  ///
  /// \param[in] context Context of the error.
  virtual void on_late_prach_message(const error_context& context) = 0;
};

} // namespace ofh
} // namespace ocudu
