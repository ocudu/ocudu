// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "event_builder_pool.h"
#include "fbs/slot_input_generated.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/du_cell_index.h"
#include <thread>

namespace ocudu::schedtrace {

/// \brief Channel of finished cell event buffers (producer -> consumer).
///
/// The producer builds each event bottom-up into the builder returned by next(). Slot inputs arriving before the slot
/// decision are built immediately as finished sub-tables (nothing else may be mid-construction at that point — the
/// builder does not support nested table construction) and their union offsets buffered via add_input() until the
/// decision completes the event.
class cell_event_channel
{
public:
  /// Event queue (producer -> consumer): carries builders holding finished event buffers.
  using event_queue_type = concurrent_queue<event_builder*, concurrent_queue_policy::lockfree_spsc>;

  /// Maximum number of slot inputs buffered per slot.
  static constexpr unsigned max_inputs_per_slot = 256;

  cell_event_channel(du_cell_index_t cell_idx, unsigned qsize, ocudulog::basic_logger& logger_) :
    cell_index(cell_idx), ev_pool(qsize), ev_queue(qsize), logger(logger_)
  {
  }

  /// Returns the builder of the event being built for the current slot.
  flatbuffers::FlatBufferBuilder& next()
  {
    if (current_slot_ev == nullptr) {
      current_slot_ev = ev_pool.allocate();
      report_fatal_error_if_not(current_slot_ev != nullptr, "Event allocation should never fail");
    }
    return current_slot_ev->fbb();
  }

  /// Buffers a finished slot input until the slot decision completes the event.
  void add_input(fbs::SlotInput type, flatbuffers::Offset<void> value)
  {
    if (input_types.full()) {
      logger.warning("cell={}: Discarding slot input trace event. Cause: Too many inputs in a single slot.",
                     cell_index);
      return;
    }
    input_types.push_back(type);
    input_values.push_back(value);
  }

  /// Slot inputs buffered for the current slot, as the parallel type/value arrays of a union vector.
  span<const fbs::SlotInput>            pending_input_types() const { return input_types; }
  span<const flatbuffers::Offset<void>> pending_input_values() const { return input_values; }

  /// \brief Pushes the completed event to the backend.
  /// The producer must have finished the buffer (FinishSizePrefixedCellEventBuffer) before calling this.
  [[nodiscard]] bool commit()
  {
    // The buffered inputs are baked into the finished buffer (or dropped with it below).
    clear_inputs();

    if (not ev_queue.try_push(current_slot_ev)) {
      // Drop the event and reuse the builder for the next slot. The builder must be cleared here on the producer:
      // it never reaches the consumer, so the pool's clear-on-deallocate cannot cover it.
      current_slot_ev->clear();
      return false;
    }

    // Nullify so the next next() call allocates a fresh builder on whatever thread runs it.
    current_slot_ev = nullptr;
    return true;
  }

  void force_commit()
  {
    clear_inputs();
    while (not ev_queue.try_push(current_slot_ev)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      logger.warning("cell={}: Failed to push cell stop trace event. Cause: Event queue is full. Retrying...",
                     cell_index);
    }
    current_slot_ev = nullptr;
  }

  /// Calls \c fn for the oldest event sent by the producer. If no event is pending, returns false.
  template <typename Fn>
  bool consume(const Fn& fn)
  {
    event_builder* ev;
    if (ev_queue.try_pop(ev)) {
      // Dispatches the finished event bytes to the provided functor.
      fn(ev->finished());
      // Deallocate the builder so it can be reused by the producer.
      ev_pool.deallocate(*ev);
      return true;
    }
    return false;
  }

  bool empty() const { return ev_queue.empty(); }

  size_t size() const { return ev_queue.size(); }

  size_t capacity() const { return ev_pool.capacity(); }

private:
  void clear_inputs()
  {
    input_types.clear();
    input_values.clear();
  }

  const du_cell_index_t   cell_index;
  event_builder_pool      ev_pool;
  event_queue_type        ev_queue;
  ocudulog::basic_logger& logger;

  /// Builder of the event currently being prepared. Null between commit() and the next next() call.
  event_builder* current_slot_ev = nullptr;

  /// Slot inputs of the event currently being prepared, buffered until the slot decision arrives.
  static_vector<fbs::SlotInput, max_inputs_per_slot>            input_types;
  static_vector<flatbuffers::Offset<void>, max_inputs_per_slot> input_values;
};

} // namespace ocudu::schedtrace
