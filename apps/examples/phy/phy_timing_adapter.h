// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_timing_notifier.h"
#include "ocudu/phy/upper/upper_phy_timing_context.h"
#include "ocudu/phy/upper/upper_phy_timing_handler.h"

namespace ocudu {

/// Implements a generic physical layer timing adapter.
class phy_timing_adapter : public lower_phy_timing_notifier
{
private:
  upper_phy_timing_handler* timing_handler;

public:
  /// Connects the adaptor to the upper physical layer handler.
  void connect(upper_phy_timing_handler* upper_handler) { timing_handler = upper_handler; }

  // See interface for documentation.
  void on_tti_boundary(const lower_phy_timing_context& context) override
  {
    report_fatal_error_if_not(timing_handler, "Adapter is not connected.");
    upper_phy_timing_context upper_context;
    upper_context.slot       = context.slot;
    upper_context.time_point = context.time_point;
    timing_handler->handle_tti_boundary(upper_context);
  }

  // See interface for documentation.
  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override
  {
    report_fatal_error_if_not(timing_handler, "Adapter is not connected.");
    upper_phy_timing_context upper_context;
    upper_context.slot = context.slot;
    timing_handler->handle_ul_half_slot_boundary(upper_context);
  }

  // See interface for documentation.
  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override
  {
    report_fatal_error_if_not(timing_handler, "Adapter is not connected.");
    upper_phy_timing_context upper_context;
    upper_context.slot = context.slot;
    timing_handler->handle_ul_full_slot_boundary(upper_context);
  }
};

} // namespace ocudu
