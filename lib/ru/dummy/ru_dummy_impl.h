// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ru_dummy_metrics_collector.h"
#include "ru_dummy_sector.h"
#include "ocudu/adt/interval.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_context.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ru/dummy/ru_dummy_configuration.h"
#include "ocudu/ru/ru.h"
#include "ocudu/ru/ru_controller.h"
#include "ocudu/ru/ru_downlink_plane.h"
#include "ocudu/ru/ru_timing_notifier.h"
#include "ocudu/ru/ru_uplink_plane.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/ocudu_assert.h"
#include "ocudu/support/synchronization/stop_event.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <vector>

namespace ocudu {

/// \brief Implements a dummy radio unit.
///
/// The dummy radio unit is a radio unit designed for performance and stability testing purposes. It does not connect to
/// any RF device.
///
/// It determines the timing from the system time.
class ru_dummy_impl : public radio_unit,
                      private ru_controller,
                      private ru_operation_controller,
                      private ru_downlink_plane_handler,
                      private ru_uplink_plane_handler
{
public:
  /// Creates a dummy radio unit from its configuration.
  explicit ru_dummy_impl(const ru_dummy_configuration& config, ru_dummy_dependencies dependencies) noexcept;

  // See the radio_unit interface for documentation.
  ru_controller& get_controller() override { return *this; }

  // See the radio_unit interface for documentation.
  ru_downlink_plane_handler& get_downlink_plane_handler() override { return *this; }

  // See the radio_unit interface for documentation.
  ru_uplink_plane_handler& get_uplink_plane_handler() override { return *this; }

  // See the radio_unit interface for documentation.
  ru_metrics_collector* get_metrics_collector() override { return are_metrics_enabled ? &metrics_collector : nullptr; }

  // See the radio_unit interface for documentation.
  ru_center_frequency_controller* get_center_frequency_controller() override { return nullptr; }

private:
  /// Minimum loop time.
  const std::chrono::microseconds minimum_loop_time = std::chrono::microseconds(10);

  // See ru_controller for documentation.
  void start() override;

  // See ru_controller for documentation.
  void stop() override;

  // See interface for documentation.
  ru_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  ru_gain_controller* get_gain_controller() override { return nullptr; }

  // See interface for documentation.
  ru_cfo_controller* get_cfo_controller() override { return nullptr; }

  // See interface for documentation.
  ru_tx_time_offset_controller* get_tx_time_offset_controller() override { return nullptr; }

  // See ru_downlink_plane_handler for documentation.
  void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) override
  {
    interval<unsigned> sector_range(0, sectors.size());
    ocudu_assert(sector_range.contains(context.sector),
                 "Sector identifier (i.e., {}) is out-of-bounds {}.",
                 context.sector,
                 sector_range);
    sectors[context.sector]->handle_dl_data(context, grid);
  }

  // See ru_uplink_plane_handler for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, shared_prach_buffer buffer) override
  {
    interval<unsigned> sector_range(0, sectors.size());
    ocudu_assert(sector_range.contains(context.sector),
                 "Sector identifier (i.e., {}) is out-of-bounds {}.",
                 context.sector,
                 sector_range);
    sectors[context.sector]->handle_prach_occasion(context, std::move(buffer));
  }

  // See ru_uplink_plane_handler for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override
  {
    interval<unsigned> sector_range(0, sectors.size());
    ocudu_assert(sector_range.contains(context.sector),
                 "Sector identifier (i.e., {}) is out-of-bounds {}.",
                 context.sector,
                 sector_range);
    sectors[context.sector]->handle_new_uplink_slot(context, grid);
  }

  /// Main radio dummy loop.
  void radio_loop(const rt_stop_event_token& tk) noexcept;

  /// Run the execution of each slot.
  void run_slot();

  /// Flag that enables (or not) metrics.
  const bool are_metrics_enabled;
  /// Ru logger.
  ocudulog::basic_logger& logger;
  /// Internal executor.
  task_executor& executor;
  /// Radio Unit timing notifier.
  ru_timing_notifier& timing_notifier;
  /// Stop control.
  rt_stop_event_source stop_control;
  /// Slot time in microseconds.
  std::chrono::microseconds slot_duration;
  /// Number of slots is notified in advance of the transmission time.
  const unsigned max_processing_delay_slots;
  /// Current slot.
  slot_point_extended current_slot;
  /// Radio unit sectors.
  std::vector<std::unique_ptr<ru_dummy_sector>> sectors;
  /// RU dummy metrics collector.
  ru_dummy_metrics_collector metrics_collector;
  /// Whether the RU is being stopped, i.e., it was signaled to stop and is being stopped.
  std::atomic<bool> is_stop_requested = false;
};

} // namespace ocudu
