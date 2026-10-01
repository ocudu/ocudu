// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

struct resource_grid_context;
class shared_resource_grid;

namespace ofh {

class error_notifier;

/// Open Fronthaul downlink handler.
class downlink_handler
{
public:
  /// Default destructor.
  virtual ~downlink_handler() = default;

  /// \brief Handles the given downlink data to be transmitted.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid Downlink data to transmit.
  virtual void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) = 0;
};

} // namespace ofh
} // namespace ocudu
