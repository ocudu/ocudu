// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point.h"

namespace ocudu {

struct lower_phy_timing_context;

/// \brief Lower physical layer downlink processor - Notifier interface.
///
/// Notifies time boundary events.
class downlink_processor_notifier
{
public:
  /// Default destructor.
  virtual ~downlink_processor_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// See \ref lower_phy_timing_notifier::on_tti_boundary for more information.
  /// \param[in] context Notification context.
  virtual void on_tti_boundary(const lower_phy_timing_context& context) = 0;
};

} // namespace ocudu
