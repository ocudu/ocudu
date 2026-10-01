// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point_extended.h"

namespace ocudu {

/// TTI boundary context.
struct tti_boundary_context {
  /// Slot point associated with the time point.
  slot_point_extended slot;
  /// Time point associated to the slot point.
  std::chrono::time_point<std::chrono::system_clock> time_point;
};

/// Radio Unit interface to notify timing related events.
class ru_timing_notifier
{
public:
  /// Default destructor.
  virtual ~ru_timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// Notifies that the processing of a new slot has started.
  ///
  /// \param[in] slot_context Slot time point context.
  virtual void on_tti_boundary(const tti_boundary_context& slot_context) = 0;

  /// \brief Notifies that an uplink half slot has been received and processed by the Radio Unit.
  ///
  /// \param[in] slot Current slot.
  virtual void on_ul_half_slot_boundary(slot_point slot) = 0;

  /// \brief Notifies that an uplink full slot has been received and processed by the Radio Unit.
  ///
  /// \param[in] slot Current slot.
  virtual void on_ul_full_slot_boundary(slot_point slot) = 0;
};

} // namespace ocudu
