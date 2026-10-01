// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "../support/uplink_notified_grid_symbol_repository.h"
#include "ocudu/ofh/ofh_uplane_rx_symbol_notifier.h"
#include <memory>

namespace ocudu {
namespace ofh {

/// \brief Received symbol reorderer.
///
/// This object is a decorator that receives symbols out of order and notifies them to the configured notifier in
/// ascending order.
class rx_symbol_reorderer : public uplane_rx_symbol_notifier
{
  uplane_rx_symbol_notifier&                              notifier;
  std::shared_ptr<uplink_notified_grid_symbol_repository> uplink_symbol_repo;

public:
  rx_symbol_reorderer(uplane_rx_symbol_notifier&                              notifier_,
                      std::shared_ptr<uplink_notified_grid_symbol_repository> uplink_symbol_repo_) :
    notifier(notifier_), uplink_symbol_repo(uplink_symbol_repo_)
  {
    ocudu_assert(uplink_symbol_repo, "Invalid notified uplink grid symbol repository");
  }

  // See interface for documentation.
  void on_new_uplink_symbol(const uplane_rx_symbol_context& context, shared_resource_grid grid, bool is_valid) override;

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    // PRACH notifies all symbols at once, no need to reorder.
    notifier.on_new_prach_window_data(context, std::move(buffer));
  }
};

} // namespace ofh
} // namespace ocudu
