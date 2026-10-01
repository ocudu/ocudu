// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_timing_context.h"

namespace ocudu {

/// Lower physical layer interface to notify timing related events.
class lower_phy_timing_notifier
{
public:
  /// Default destructor.
  virtual ~lower_phy_timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// Notifies that the processing of a new slot has started. It indicates in \c context the next slot available for
  /// transmission.
  ///
  /// \param[in] context Notification context.
  virtual void on_tti_boundary(const lower_phy_timing_context& context) = 0;

  /// \brief Notifies that an uplink half slot has been received and processed by the lower PHY.
  ///
  /// \param[in] context Notification context.
  virtual void on_ul_half_slot_boundary(const lower_phy_timing_context& context) = 0;

  /// \brief Notifies that an uplink full slot has been received and processed by the lower PHY.
  ///
  /// After this notification, the lower PHY does not report more uplink receive symbols events for the given context.
  ///
  /// \param[in] context Notification context.
  virtual void on_ul_full_slot_boundary(const lower_phy_timing_context& context) = 0;
};

} // namespace ocudu
