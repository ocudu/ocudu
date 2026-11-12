// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "phy_dummy_fapi_adaptor_impl.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include "ocudu/support/rtsan.h"
#include "ocudu/support/synchronization/sync_event.h"
#include "fmt/ranges.h"
#include <thread>
#include <vector>

using namespace ocudu;
using namespace fapi_adaptor;

/// Returns the count of the slot given by the system time.
static uint64_t get_system_time_slot(std::chrono::microseconds slot_duration, uint64_t nof_slots_in_all_hyper_sfns)
{
  auto time_since_epoch =
      std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());

  return (time_since_epoch / slot_duration) % nof_slots_in_all_hyper_sfns;
}

phy_dummy_fapi_adaptor_impl::phy_dummy_fapi_adaptor_impl(const phy_dummy_fapi_adaptor_config&       config,
                                                         const phy_dummy_fapi_adaptor_dependencies& dependencies) :
  clock_mode(config.clock_mode),
  slot_duration(1000U / pow2(to_numerology_value(config.scs))),
  max_processing_delay_slots(config.max_processing_delay_slots),
  nof_slots_in_all_hyper_sfns(slot_point_extended(config.scs, 0).nof_slots_in_all_hyper_sfns()),
  logger(dependencies.logger),
  current_slot(config.scs, 0)
{
  ocudu_assert(config.nof_sectors > 0, "The PHY dummy FAPI adaptor requires at least one sector");
  ocudu_assert(dependencies.sector_executors.size() == config.nof_sectors,
               "The number of sector executors (i.e., {}) does not match the number of sectors (i.e., {})",
               dependencies.sector_executors.size(),
               config.nof_sectors);

  sectors.reserve(config.nof_sectors);
  for (unsigned i_sector = 0; i_sector != config.nof_sectors; ++i_sector) {
    ocudu_assert(dependencies.sector_executors[i_sector] != nullptr, "Invalid executor of sector {}", i_sector);
    sectors.push_back(std::make_unique<phy_dummy_fapi_sector_adaptor>(
        phy_dummy_fapi_sector_adaptor_config{.sector_id                  = i_sector,
                                             .scs                        = config.scs,
                                             .ul_indication_delay_slots  = config.ul_indication_delay_slots,
                                             .max_processing_delay_slots = config.max_processing_delay_slots,
                                             .late_request_detection =
                                                 config.clock_mode == phy_dummy_fapi_adaptor_clock_mode::realtime},
        logger,
        *dependencies.sector_executors[i_sector]));
  }

  // The slots are aligned to the system time once, and then follow the steady clock, so that system time adjustments do
  // not make the clock jump.
  origin_system_slot = get_system_time_slot(slot_duration, nof_slots_in_all_hyper_sfns);
  origin_time        = std::chrono::steady_clock::now();
  current_slot       = slot_point_extended(config.scs, origin_system_slot);
  last_advance_time  = origin_time;

  sync_event start_event;
  if (!dependencies.clock_executor.defer(
          [this, start_token = start_event.get_token(), stop_token = stop_control.get_token()]() mutable {
            start_token.reset();
            clock_loop(stop_token);
          })) {
    report_fatal_error("Unable to start the PHY dummy FAPI adaptor clock loop");
  }
  start_event.wait();
}

phy_dummy_fapi_adaptor_impl::~phy_dummy_fapi_adaptor_impl()
{
  // The clock stops before the sectors, so no slot boundary is dispatched to a destroyed sector.
  stop_control.stop();
}

phy_fapi_sector_adaptor& phy_dummy_fapi_adaptor_impl::get_sector_adaptor(unsigned cell_id)
{
  ocudu_assert(cell_id < sectors.size(), "Invalid cell identifier {}", cell_id);
  return *sectors[cell_id];
}

void phy_dummy_fapi_adaptor_impl::clock_loop(const rt_stop_event_token& token)
{
  if (clock_mode == phy_dummy_fapi_adaptor_clock_mode::realtime) {
    while (!token.is_stop_requested()) {
      run_realtime_iteration();
    }
    return;
  }

  while (!token.is_stop_requested()) {
    run_free_running_iteration();
  }
}

uint64_t phy_dummy_fapi_adaptor_impl::get_current_system_slot() const
{
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - origin_time);
  return (origin_system_slot + elapsed / slot_duration) % nof_slots_in_all_hyper_sfns;
}

void phy_dummy_fapi_adaptor_impl::run_realtime_iteration()
{
  uint64_t slot_count = get_current_system_slot();

  if (slot_count == current_slot.system_slot()) {
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(minimum_loop_time);
    return;
  }

  while (slot_count != current_slot.system_slot()) {
    advance_slot();
  }
}

namespace {

/// Progress of all the sectors on the last notified slot boundary.
struct sectors_progress {
  /// Whether all the sectors finished the last notified slot boundary.
  bool all_done = true;
  /// Whether any sector that finished the last notified slot boundary is running.
  bool any_running = false;
};

} // namespace

/// Returns the progress of all the sectors on the last notified slot boundary.
static sectors_progress get_sectors_progress(span<const std::unique_ptr<phy_dummy_fapi_sector_adaptor>> sectors,
                                             std::optional<slot_point> last_notified_slot)
{
  sectors_progress result;
  if (!last_notified_slot.has_value()) {
    return result;
  }

  for (const auto& sector : sectors) {
    phy_dummy_fapi_sector_adaptor::slot_boundary_progress progress = sector->get_progress();
    bool done          = progress.slot.valid() && progress.slot == *last_notified_slot;
    result.all_done    = result.all_done && done;
    result.any_running = result.any_running || (done && progress.running);
  }
  return result;
}

/// Reports a fatal error that names the sectors that did not finish the last notified slot boundary.
[[noreturn]] static void report_stalled_sectors(span<const std::unique_ptr<phy_dummy_fapi_sector_adaptor>> sectors,
                                                slot_point                last_notified_slot,
                                                std::chrono::milliseconds timeout)
{
  std::vector<unsigned> stalled_sectors;
  for (unsigned i_sector = 0, e = sectors.size(); i_sector != e; ++i_sector) {
    phy_dummy_fapi_sector_adaptor::slot_boundary_progress progress = sectors[i_sector]->get_progress();
    if (!progress.slot.valid() || progress.slot != last_notified_slot) {
      stalled_sectors.push_back(i_sector);
    }
  }

  report_fatal_error("PHY dummy FAPI adaptor: Sectors {} did not complete slot {} within {} ms",
                     stalled_sectors,
                     last_notified_slot,
                     timeout.count());
}

void phy_dummy_fapi_adaptor_impl::run_free_running_iteration()
{
  auto now = std::chrono::steady_clock::now();

  sectors_progress progress = get_sectors_progress(sectors, last_notified_slot);

  if (!progress.all_done && now - last_advance_time >= free_running_timeout) {
    report_stalled_sectors(sectors, *last_notified_slot, free_running_timeout);
  }

  // Without running sectors, slot boundaries still advance at the slot rate to process START and STOP requests.
  bool advance = progress.all_done && (progress.any_running || now - last_advance_time >= slot_duration);

  if (!advance) {
    OCUDU_RTSAN_SCOPED_DISABLER(scoped_disabler);
    std::this_thread::sleep_for(minimum_loop_time);
    return;
  }

  last_advance_time = now;
  advance_slot();
}

void phy_dummy_fapi_adaptor_impl::advance_slot()
{
  ++current_slot;

  slot_point_extended notified_slot = current_slot + max_processing_delay_slots;
  auto                time_point    = std::chrono::system_clock::now() + slot_duration * max_processing_delay_slots;
  last_notified_slot                = notified_slot.without_hyper_sfn();

  for (auto& sector : sectors) {
    sector->handle_slot_boundary(notified_slot, time_point);
  }
}
