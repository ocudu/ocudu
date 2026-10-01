// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "ocudu/phy/support/shared_resource_grid.h"

namespace ocudu {
namespace ofh {
namespace testing {

/// User-Plane received symbol notifier spy.
class uplane_rx_symbol_notifier_spy : public uplane_rx_symbol_notifier
{
  bool new_uplink_symbol_function_called = false;
  bool new_prach_function_called         = false;

public:
  // See interface for documentation.
  void on_new_uplink_symbol(const uplane_rx_symbol_context& context, shared_resource_grid grid, bool is_valid) override
  {
    new_uplink_symbol_function_called = true;
  }

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    new_prach_function_called = true;
  }

  /// Returns true if on_new_uplink_symbol function has been called, otherwise false.
  bool has_new_uplink_symbol_function_been_called() const { return new_uplink_symbol_function_called; }

  /// Returns true if on_new_prach_window_data function has been called, otherwise false.
  bool has_new_prach_function_been_called() const { return new_prach_function_called; }
};

} // namespace testing
} // namespace ofh
} // namespace ocudu
