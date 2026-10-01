// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "resource_grid_pool_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/instrumentation/traces/du_traces.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_context.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/ocudu_assert.h"
#include "ocudu/support/rtsan.h"

using namespace ocudu;

shared_resource_grid resource_grid_pool_wrapper::try_reserve(rt_stop_event_token token)
{
  // Try to transition from available to one scope.
  unsigned expected = ref_counter_available;
  bool     success  = scope_count.compare_exchange_strong(expected, 1, std::memory_order_acq_rel);

  // Return an invalid shared grid if the state transition is unsuccessful.
  if (!success) {
    return {};
  }

  // Keep token if successful.
  stop_token = std::move(token);

  // Create valid shared resource grid.
  return shared_resource_grid{*this, scope_count};
}

void resource_grid_pool_wrapper::release()
{
  // Move token to the scope of this method before transitioning to available.
  auto token = std::move(stop_token);

  // Transition to available.
  [[maybe_unused]] unsigned prev_scope_count = scope_count.exchange(ref_counter_available, std::memory_order_acq_rel);

  // Assert that the grid is not present in any scope before becoming available.
  ocudu_assert((prev_scope_count == ref_counter_available) || (prev_scope_count == 0),
               "The resource grid state cannot transition to available while it is present in {} scopes.",
               prev_scope_count);
}

void resource_grid_pool_wrapper::notify_release_scope()
{
  // Skip zeroing if the grid is empty.
  if (grid->get_reader().is_empty()) {
    release();
    return;
  }

  // If the pool is not configured with an asynchronous executor, it skips the zeroing process.
  if (async_executor == nullptr) {
    release();
    return;
  }

  // Create lambda function for setting the grid to zero.
  auto set_all_zero_func = [this]() noexcept OCUDU_RTSAN_NONBLOCKING {
    // Set grid to zero.
    grid->set_all_zero();

    // Make the grid available.
    release();
  };

  // Try to execute the asynchronous housekeeping task.
  bool success = async_executor->defer(set_all_zero_func);

  // Ensure the resource grid is marked as available even if it is not empty.
  // Avoid warnings about failure to prevent false alarms during gNb teardown.
  if (!success) {
    release();
  }
}

resource_grid& resource_grid_pool_wrapper::get()
{
  return *grid;
}

resource_grid_pool_impl::~resource_grid_pool_impl()
{
  stop_control.stop();
}

shared_resource_grid resource_grid_pool_impl::allocate_resource_grid(slot_point slot)
{
  // Get a stop token, return an invalid grid if it is stopping or stopped.
  auto token = stop_control.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return {};
  }

  // Select an identifier from the current request counter.
  unsigned identifier = counter;

  // Increment request counter and wrap it with the number of grids.
  counter = (counter + 1) % grids.size();

  // Try reserving the resource grid.
  shared_resource_grid grid = grids[identifier].try_reserve(std::move(token));

  // Log a warning message if the grid is not available.
  if (!grid) {
    logger.warning(slot.sfn(), slot.slot_index(), "Resource grid with identifier {} is not available.", identifier);
  }

  return grid;
}
