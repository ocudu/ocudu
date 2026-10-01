// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/timing/slot_symbol_point.h"
#include <chrono>

namespace ocudu {
namespace ofh {

/// Slot symbol time point context.
struct slot_symbol_point_context {
  slot_symbol_point symbol_point;
  /// Hyperframe number.
  unsigned                                           hfn;
  std::chrono::time_point<std::chrono::system_clock> time_point;
};

/// This interface notifies the timing boundary of an OFDM symbol over the air.
class ota_symbol_boundary_notifier
{
public:
  virtual ~ota_symbol_boundary_notifier() = default;

  /// \brief Notifies a new OTA symbol boundary event.
  ///
  /// Notifies that the beginning of a new OTA symbol has started.
  ///
  /// \param[in] symbol_point_context Current slot and symbol point.
  virtual void on_new_symbol(const slot_symbol_point_context& symbol_point_context) = 0;
};

} // namespace ofh
} // namespace ocudu
