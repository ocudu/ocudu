// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/instrumentation/traces/critical_traces.h"
#include "ocudu/instrumentation/traces/du_traces.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/lower/lower_phy_error_notifier.h"
#include "ocudu/phy/support/resource_grid_context.h"
#include "ocudu/ru/ru_error_notifier.h"

namespace ocudu {

/// Lower phy to RU error adapter class.
class ru_lower_phy_error_adapter : public lower_phy_error_notifier
{
public:
  ru_lower_phy_error_adapter(ocudulog::basic_logger& logger_, ru_error_notifier& notifier_) :
    logger(logger_), notifier(notifier_)
  {
  }

  // See interface for documentation.
  void on_late_resource_grid(const resource_grid_context& context) override
  {
    notifier.on_late_downlink_message(ru_error_context{.slot = context.slot, .sector = context.sector});
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in lower PHY: Downlink data late for sector {} and slot {}.",
                   context.sector,
                   context.slot);
    general_critical_tracer << instant_trace_event{"on_late_resource_grid",
                                                   instant_trace_event::cpu_scope::global,
                                                   instant_trace_event::event_criticality::severe};
  }

  // See interface for documentation.
  void on_prach_request_late(const prach_buffer_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in lower PHY: PRACH request late for sector {}, slot {} and start symbol {}.",
                   context.sector,
                   context.slot,
                   context.start_symbol);
    general_critical_tracer << instant_trace_event{"on_prach_request_late",
                                                   instant_trace_event::cpu_scope::global,
                                                   instant_trace_event::event_criticality::severe};
  }

  // See interface for documentation.
  void on_prach_request_overflow(const prach_buffer_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in lower PHY: PRACH request overflow for sector {}, slot {} and start symbol {}.",
                   context.sector,
                   context.slot,
                   context.start_symbol);
    general_critical_tracer << instant_trace_event{"on_prach_request_overflow",
                                                   instant_trace_event::cpu_scope::global,
                                                   instant_trace_event::event_criticality::severe};
  }

  // See interface for documentation.
  void on_puxch_request_late(const resource_grid_context& context) override
  {
    notifier.on_late_uplink_message(ru_error_context{.slot = context.slot, .sector = context.sector});
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in lower PHY: PUxCH request late for sector {}, slot {}.",
                   context.sector,
                   context.slot);
    general_critical_tracer << instant_trace_event{"on_puxch_request_late",
                                                   instant_trace_event::cpu_scope::global,
                                                   instant_trace_event::event_criticality::severe};
  }

private:
  /// Logger.
  ocudulog::basic_logger& logger;
  /// Radio Unit error notifier.
  ru_error_notifier& notifier;
};

} // namespace ocudu
