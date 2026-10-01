// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// Upper physical layer error notifier.
class upper_phy_error_notifier
{
public:
  /// Default destructor.
  virtual ~upper_phy_error_notifier() = default;

  /// \brief Notifies a late downlink message.
  ///
  /// \param[in] dl_msg_slot Slot of the downlink message.
  virtual void on_late_downlink_message(slot_point dl_msg_slot) = 0;

  /// \brief Notifies a late uplink request message.
  ///
  /// \param[in] ul_msg_slot Slot of the uplink message.
  virtual void on_late_uplink_message(slot_point ul_msg_slot) = 0;

  /// \brief Notifies a late PRACH request message.
  ///
  /// \param[in] prach_msg_slot Slot of the uplink message.
  virtual void on_late_prach_message(slot_point prach_msg_slot) = 0;
};

} // namespace ocudu
