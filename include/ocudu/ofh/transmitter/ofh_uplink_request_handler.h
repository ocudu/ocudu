// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/phy/support/shared_prach_buffer.h"

namespace ocudu {

struct prach_buffer_context;
struct resource_grid_context;
class shared_resource_grid;

namespace ofh {

class error_notifier;

/// \brief Open Fronthaul uplink request handler.
///
/// Handles PRACH and uplink data requests to capture uplink data. The uplink received data will be notified through the
/// \ref ofh_uplane_rx_symbol_notifier notifier.
class uplink_request_handler
{
public:
  /// Default destructor.
  virtual ~uplink_request_handler() = default;

  /// \brief Requests the Open Fronthaul to capture a PRACH window.
  ///
  /// The Open Fronthaul must capture the window identified by \c context.
  ///
  /// \param[in] context PRACH window context.
  /// \param[in] buffer  PRACH buffer used to write the PRACH window.
  virtual void handle_prach_occasion(const prach_buffer_context& context, shared_prach_buffer buffer) = 0;

  /// \brief Requests the Open Fronthaul to provide an uplink slot.
  ///
  /// The Open Fronthaul must process the slot described by \c context.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] buffer  Resource grid to store the processed slot.
  virtual void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) = 0;
};

} // namespace ofh
} // namespace ocudu
