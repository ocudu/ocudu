// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/slot_point.h"

namespace ocudu {
namespace fapi {

/// \brief Slot-based, last message notifier interface.
///
/// This interface notifies, for each slot, the transmission of the last FAPI request that has been sent through the
/// FAPI message gateway.
class p7_last_request_notifier
{
public:
  virtual ~p7_last_request_notifier() = default;

  /// \brief Notifies the transmission of the last message corresponding to the given slot.
  ///
  /// \param[in] slot Current slot.
  virtual void on_last_message(slot_point slot) = 0;
};

} // namespace fapi
} // namespace ocudu
