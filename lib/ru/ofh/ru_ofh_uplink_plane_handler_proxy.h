// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ru/ru_uplink_plane.h"
#include <algorithm>
#include <vector>

namespace ocudu {

class task_executor;
class shared_resource_grid;

namespace ofh {
class uplink_request_handler;
} // namespace ofh

/// This proxy implementation dispatches the requests to the corresponding OFH sector.
class ru_uplink_plane_handler_proxy : public ru_uplink_plane_handler
{
public:
  ru_uplink_plane_handler_proxy() = default;

  explicit ru_uplink_plane_handler_proxy(std::vector<ofh::uplink_request_handler*> sectors_) :
    sectors(std::move(sectors_))
  {
    ocudu_assert(std::all_of(sectors.begin(), sectors.end(), [](const auto& elem) { return elem != nullptr; }),
                 "Invalid sector");
  }

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, shared_prach_buffer buffer) override;

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override;

private:
  std::vector<ofh::uplink_request_handler*> sectors;
};

} // namespace ocudu
