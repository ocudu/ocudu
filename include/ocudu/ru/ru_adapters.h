// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/instrumentation/traces/critical_traces.h"
#include "ocudu/instrumentation/traces/du_traces.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/upper_phy_error_handler.h"
#include "ocudu/phy/upper/upper_phy_rg_gateway.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_handler.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_request_notifier.h"
#include "ocudu/phy/upper/upper_phy_timing_context.h"
#include "ocudu/phy/upper/upper_phy_timing_handler.h"
#include "ocudu/ru/ru_downlink_plane.h"
#include "ocudu/ru/ru_error_notifier.h"
#include "ocudu/ru/ru_timing_notifier.h"
#include "ocudu/ru/ru_uplink_plane.h"

namespace ocudu {

/// Upper PHY - Radio Unit downlink adapter.
class upper_phy_ru_dl_rg_adapter : public upper_phy_rg_gateway
{
public:
  // See interface for documentation.
  void send(const resource_grid_context& context, shared_resource_grid grid) override
  {
    ocudu_assert(dl_handler, "Adapter is not connected.");
    dl_handler->handle_dl_data(context, grid);
  }

  /// Connects this adapter with the given RU downlink handler.
  void connect(ru_downlink_plane_handler& handler) { dl_handler = &handler; }

private:
  ru_downlink_plane_handler* dl_handler = nullptr;
};

/// Upper PHY - Radio Unit uplink request adapter.
class upper_phy_ru_ul_request_adapter : public upper_phy_rx_symbol_request_notifier
{
public:
  // See interface for documentation.
  void on_prach_capture_request(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    ocudu_assert(ul_handler, "Adapter is not connected");
    ul_handler->handle_prach_occasion(context, std::move(buffer));
  }

  // See interface for documentation.
  void on_uplink_slot_request(const resource_grid_context& context, const shared_resource_grid& grid) override
  {
    ocudu_assert(ul_handler, "Adapter is not connected");
    ul_handler->handle_new_uplink_slot(context, grid);
  }

  /// Connects this adapter with the given RU uplink handler.
  void connect(ru_uplink_plane_handler& handler) { ul_handler = &handler; }

private:
  ru_uplink_plane_handler* ul_handler = nullptr;
};

/// Upper PHY - Radio Unit uplink adapter.
class upper_phy_ru_ul_adapter : public ru_uplink_plane_rx_symbol_notifier
{
public:
  explicit upper_phy_ru_ul_adapter(unsigned nof_sectors) : handlers(nof_sectors) {}

  // See interface for documentation.
  void on_new_uplink_symbol(const ru_uplink_rx_symbol_context& context,
                            const shared_resource_grid&        grid,
                            bool                               is_valid) override
  {
    ocudu_assert(context.sector < handlers.size(), "Unsupported sector {}", context.sector);
    handlers[context.sector]->handle_rx_symbol({context.sector, context.slot, context.symbol_id}, grid, is_valid);
  }

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    ocudu_assert(context.sector < handlers.size(), "Unsupported sector {}", context.sector);
    handlers[context.sector]->handle_rx_prach_window(context, std::move(buffer));
  }

  /// Maps the given upper PHY received symbol handler and sector to this adapter.
  void map_handler(unsigned sector, upper_phy_rx_symbol_handler& hndlr)
  {
    ocudu_assert(sector < handlers.size(), "Unsupported sector {}", sector);

    handlers[sector] = &hndlr;
  }

private:
  std::vector<upper_phy_rx_symbol_handler*> handlers;
};

/// Upper PHY - Radio Unit timing adapter.
class upper_phy_ru_timing_adapter : public ru_timing_notifier
{
public:
  explicit upper_phy_ru_timing_adapter(unsigned nof_sectors) : handlers(nof_sectors) {}

  // See interface for documentation.
  void on_tti_boundary(const tti_boundary_context& slot_context) override
  {
    ocudu_assert(!handlers.empty(), "Adapter is not connected");
    for (auto& handler : handlers) {
      handler->handle_tti_boundary(
          upper_phy_timing_context{.slot = slot_context.slot, .time_point = slot_context.time_point});
    }
  }

  // See interface for documentation.
  void on_ul_half_slot_boundary(slot_point slot) override
  {
    ocudu_assert(!handlers.empty(), "Adapter is not connected");
    for (auto& handler : handlers) {
      handler->handle_ul_half_slot_boundary(
          upper_phy_timing_context{.slot = slot_point_extended(slot), .time_point = {}});
    }
  }

  // See interface for documentation.
  void on_ul_full_slot_boundary(slot_point slot) override
  {
    ocudu_assert(!handlers.empty(), "Adapter is not connected");
    for (auto& handler : handlers) {
      handler->handle_ul_full_slot_boundary(
          upper_phy_timing_context{.slot = slot_point_extended(slot), .time_point = {}});
    }
  }

  /// Maps the given upper PHY timing handler and sector to this adapter.
  void map_handler(unsigned sector, upper_phy_timing_handler& hndlr)
  {
    ocudu_assert(sector < handlers.size(), "Unsupported sector {}", sector);

    handlers[sector] = &hndlr;
  }

private:
  std::vector<upper_phy_timing_handler*> handlers;
};

/// Upper PHY - Radio Unit error adapter.
class upper_phy_ru_error_adapter : public ru_error_notifier
{
public:
  explicit upper_phy_ru_error_adapter(unsigned nof_sectors) : handlers(nof_sectors) {}

  // See interface for documentation.
  void on_late_downlink_message(const ru_error_context& context) override
  {
    ocudu_assert(context.sector < handlers.size(), "Invalid sector '{}'", context.sector);
    ocudu_assert(handlers[context.sector], "Adapter for sector '{}' is not connected", context.sector);

    handlers[context.sector]->handle_late_downlink_message(context.slot);
    general_critical_tracer << instant_trace_event{
        "handle_dl_data_late", instant_trace_event::cpu_scope::thread, instant_trace_event::event_criticality::severe};
  }

  // See interface for documentation.
  void on_late_uplink_message(const ru_error_context& context) override
  {
    ocudu_assert(context.sector < handlers.size(), "Invalid sector '{}'", context.sector);
    ocudu_assert(handlers[context.sector], "Adapter for sector '{}' is not connected", context.sector);

    handlers[context.sector]->handle_late_uplink_message(context.slot);
    general_critical_tracer << instant_trace_event{"handle_ul_request_late",
                                                   instant_trace_event::cpu_scope::thread,
                                                   instant_trace_event::event_criticality::severe};
  }

  // See interface for documentation.
  void on_late_prach_message(const ru_error_context& context) override
  {
    ocudu_assert(context.sector < handlers.size(), "Invalid sector '{}'", context.sector);
    ocudu_assert(handlers[context.sector], "Adapter for sector '{}' is not connected", context.sector);

    handlers[context.sector]->handle_late_prach_message(context.slot);
    general_critical_tracer << instant_trace_event{"handle_late_prach_message",
                                                   instant_trace_event::cpu_scope::thread,
                                                   instant_trace_event::event_criticality::severe};
  }

  /// Maps the given upper PHY error handler and sector to this adapter.
  void map_handler(unsigned sector, upper_phy_error_handler& hndlr)
  {
    ocudu_assert(sector < handlers.size(), "Unsupported sector {}", sector);

    handlers[sector] = &hndlr;
  }

private:
  std::vector<upper_phy_error_handler*> handlers;
};

} // namespace ocudu
