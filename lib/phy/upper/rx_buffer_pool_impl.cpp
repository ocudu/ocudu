// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rx_buffer_pool_impl.h"
#include "rx_buffer_impl.h"
#include "ocudu/phy/upper/rx_buffer_pool.h"
#include "ocudu/phy/upper/trx_buffer_identifier.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/ran/slot_point.h"
#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>

using namespace ocudu;

unique_rx_buffer
rx_buffer_pool_impl::reserve(slot_point slot, trx_buffer_identifier id, unsigned nof_codeblocks, bool new_data)
{
  // No more reservations are allowed if the pool is stopped.
  if (stopped.load(std::memory_order_relaxed)) {
    return unique_rx_buffer();
  }

  // Try to find the HARQ identifier.
  auto id_found = std::find(identifiers.begin(), identifiers.end(), id);

  // Find an available buffer if no buffer was found with the same identifier if new data is true, otherwise return an
  // invalid buffer.
  if (id_found == identifiers.end()) {
    if (new_data) {
      id_found = std::find(identifiers.begin(), identifiers.end(), trx_buffer_identifier::invalid());
    } else {
      logger.warning(slot.sfn(),
                     slot.slot_index(),
                     "UL HARQ {}: failed to reserve buffer, identifier for retransmissions not found.",
                     id);
      return unique_rx_buffer();
    }
  }

  // Report warning and return invalid buffer if no available buffer has been found.
  if (id_found == identifiers.end()) {
    logger.warning(
        slot.sfn(), slot.slot_index(), "UL HARQ {}: failed to reserve buffer, insufficient buffers in the pool.", id);
    return unique_rx_buffer();
  }

  // Get buffer index within the pool.
  unsigned i_buffer = id_found - identifiers.begin();

  // Get reference to the buffer.
  rx_buffer_impl& buffer = buffers[i_buffer];

  // Reserve codeblocks.
  expected<unsigned, rx_buffer_error> status = buffer.reserve(nof_codeblocks, new_data);

  // Report warning and return invalid buffer if the reservation is not successful.
  if (!status.has_value()) {
    logger.warning(slot.sfn(),
                   slot.slot_index(),
                   "UL HARQ {}: failed to reserve buffer, {}.",
                   id,
                   (status.error() == rx_buffer_error::already_in_use) ? "HARQ already in use" : "exhausted CB pool");
    return unique_rx_buffer();
  }

  // Update identifier and expiration.
  identifiers[i_buffer] = id;
  expirations[i_buffer] = slot + expire_timeout_slots;

  // Create buffer.
  return unique_rx_buffer(buffer, status.value());
}

void rx_buffer_pool_impl::run_slot(slot_point slot)
{
  // Predicate for finding reserved buffers.
  auto pred = [](trx_buffer_identifier id) { return id != trx_buffer_identifier::invalid(); };

  // Iterate over all the buffers that are currently reserved.
  for (auto it = std::find_if(identifiers.begin(), identifiers.end(), pred); it != identifiers.end();
       it      = std::find_if(++it, identifiers.end(), pred)) {
    // Calculate buffer index.
    unsigned i_buffer = it - identifiers.begin();

    // Get reference to the buffer.
    rx_buffer_impl& buffer = buffers[i_buffer];

    // Skip checks if the buffer is locked.
    if (buffer.is_locked()) {
      // Extend expiration time.
      expirations[i_buffer] = slot + expire_timeout_slots;
      continue;
    }

    // Determines whether the buffer is free.
    bool is_free = false;

    // A buffer is expired if the expiration slot is lower than or equal to the current slot.
    if ((expirations[i_buffer] != null_expiration) && (expirations[i_buffer] <= slot)) {
      // Try to expire the buffer.
      is_free = buffer.expire();
    } else {
      // Check if the buffer is free before expiring.
      is_free = buffer.is_free();
    }

    // Clear identifier and expiration.
    if (is_free) {
      identifiers[i_buffer] = trx_buffer_identifier::invalid();
      expirations[i_buffer] = null_expiration;
    }
  }
}

rx_buffer_pool& rx_buffer_pool_impl::get_pool()
{
  return *this;
}

void rx_buffer_pool_impl::stop()
{
  // Signals the stop of the pool. No more reservation are allowed after this point.
  stopped.store(true);

  // Makes sure all buffers are unlocked.
  for (const auto& buffer : buffers) {
    while (buffer.is_locked()) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }
}

std::unique_ptr<rx_buffer_pool_controller> ocudu::create_rx_buffer_pool(const rx_buffer_pool_config& config)
{
  return std::make_unique<rx_buffer_pool_impl>(config);
}
