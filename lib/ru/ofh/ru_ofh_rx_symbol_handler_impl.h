// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "ocudu/ru/ru_uplink_plane.h"

namespace ocudu {

/// RU received symbol handler for the Open Fronthaul interface.
class ru_ofh_rx_symbol_handler_impl : public ofh::uplane_rx_symbol_notifier
{
public:
  explicit ru_ofh_rx_symbol_handler_impl(ru_uplink_plane_rx_symbol_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void
  on_new_uplink_symbol(const ofh::uplane_rx_symbol_context& context, shared_resource_grid grid, bool is_valid) override;

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, shared_prach_buffer buffer) override;

private:
  ru_uplink_plane_rx_symbol_notifier& notifier;
};

} // namespace ocudu
