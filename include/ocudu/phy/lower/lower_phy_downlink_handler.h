// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/resource_grid_context.h"

namespace ocudu {

class shared_resource_grid;

/// \brief Lower physical layer downlink handler interface.
///
/// The downlink handler modulates and transmits the given downlink resource grid through the underlying radio device.
class lower_phy_downlink_handler
{
public:
  /// Default destructor.
  virtual ~lower_phy_downlink_handler() = default;

  /// \brief Handles the given resource grid to be transmitted.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid Resource grid to transmit.
  virtual void handle_resource_grid(const resource_grid_context& context, const shared_resource_grid& grid) = 0;
};

} // namespace ocudu
