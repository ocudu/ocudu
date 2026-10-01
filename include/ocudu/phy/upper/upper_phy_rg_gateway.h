// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/resource_grid_context.h"

namespace ocudu {

class shared_resource_grid;

/// Interface of the upper physical layer resource grid gateway.
class upper_phy_rg_gateway
{
public:
  /// Default destructor.
  virtual ~upper_phy_rg_gateway() = default;

  /// \brief Sends the given resource grid through the gateway.
  ///
  /// \param[in] context Context the resource grid belongs to.
  /// \param[in] grid    Resource grid.
  virtual void send(const resource_grid_context& context, shared_resource_grid grid) = 0;
};

} // namespace ocudu
