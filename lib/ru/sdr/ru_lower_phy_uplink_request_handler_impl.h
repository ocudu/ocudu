// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ru/ru_uplink_plane.h"
#include <vector>

namespace ocudu {

class lower_phy_uplink_request_handler;
class shared_resource_grid;

/// Radio Unit to lower physical layer uplink request handler implementation.
class ru_lower_phy_uplink_request_handler_impl : public ru_uplink_plane_handler
{
public:
  explicit ru_lower_phy_uplink_request_handler_impl(std::vector<lower_phy_uplink_request_handler*> handlers_) :
    handlers(std::move(handlers_))
  {
  }

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, shared_prach_buffer buffer) override;

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override;

private:
  std::vector<lower_phy_uplink_request_handler*> handlers;
};

} // namespace ocudu
