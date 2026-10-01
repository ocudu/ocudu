// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// Upper physical layer error handler.
class upper_phy_error_handler
{
public:
  /// Default destructor.
  virtual ~upper_phy_error_handler() = default;

  /// \brief Handles a late downlink message.
  ///
  /// \param[in] slot Slot of the downlink message.
  virtual void handle_late_downlink_message(slot_point slot) = 0;

  /// \brief Handles a late uplink message.
  ///
  /// \param[in] slot Slot of the uplink message.
  virtual void handle_late_uplink_message(slot_point slot) = 0;

  /// \brief Handles a late PRACH request message.
  ///
  /// \param[in] slot Slot of the PRACH request message.
  virtual void handle_late_prach_message(slot_point slot) = 0;
};

} // namespace ocudu
