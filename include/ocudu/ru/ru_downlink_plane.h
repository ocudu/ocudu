// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct resource_grid_context;
class shared_resource_grid;

/// \brief Radio Unit resource grid handler interface.
///
/// The resource grid handler modulates and transmits the given downlink resource grid through the underlying radio
/// device.
class ru_downlink_plane_handler
{
public:
  /// Default destructor.
  virtual ~ru_downlink_plane_handler() = default;

  /// \brief Handles the given downlink data to be transmitted.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid Downlink data to transmit.
  virtual void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) = 0;
};

} // namespace ocudu
