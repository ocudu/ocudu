// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ru_radio_event_handler.h"
#include "ocudu/phy/lower/lower_phy_timing_notifier.h"
#include "ocudu/ru/ru_timing_notifier.h"

namespace ocudu {

/// Implements a lower physical layer to Radio Unit timing adapter.
class ru_lower_phy_timing_adapter : public lower_phy_timing_notifier
{
public:
  explicit ru_lower_phy_timing_adapter(ru_timing_notifier& timing_handler_) : timing_handler(timing_handler_) {}

  // See interface for documentation.
  void on_tti_boundary(const lower_phy_timing_context& context) override
  {
    timing_handler.on_tti_boundary(tti_boundary_context{.slot = context.slot, .time_point = context.time_point});
  }

  // See interface for documentation.
  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override
  {
    timing_handler.on_ul_half_slot_boundary(context.slot.without_hyper_sfn());
  }

  // See interface for documentation.
  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override
  {
    timing_handler.on_ul_full_slot_boundary(context.slot.without_hyper_sfn());
  }

private:
  ru_timing_notifier& timing_handler;
};

} // namespace ocudu
