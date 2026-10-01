// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point.h"
#include "ocudu/support/ocudu_assert.h"
#include <atomic>
#include <thread>

namespace ocudu {

/// \brief Notifier interface for uplink_processr_fsm.
///
/// Allows the uplink PDU slot repository to notify the finite-state machine about
class uplink_processor_fsm_notifier
{
public:
  /// Default destructor.
  virtual ~uplink_processor_fsm_notifier() = default;

  /// Notifies the FSM that a new PDU has been added to repository.
  /// \remark An assertion is triggered if the FSM is not currently accepting PDUs.
  virtual void increment_pending_pdu_count() = 0;

  /// Notifies the FSM that the repository is not accepting more PDUs.
  /// \remark An assertion is triggered if the FSM is not currently accepting PDUs.
  virtual void stop_accepting_pdu() = 0;
};

/// \brief Finite-state machine for the uplink processor.
///
/// The interaction between the uplink processor and the PDU slot repository evolves according to the following states.
/// - Idle: the uplink processor is ready to start processing a new slot.
/// - Accepting: the repository interface accepts the enqueueing of new PDUs.
/// - Waiting: the processor is waiting for the first OFDM symbol to be processed. Discarding the slot is only allowed
///            in this state.
/// - Receiving: the processor is receiving symbols.
/// - Completing: the processor is no longer receiving symbols, and it is waiting to complete all receptions.
class uplink_processor_fsm : public uplink_processor_fsm_notifier
{
public:
  /// \brief Starts accepting PDUs for the given slot.
  /// \return True if the transition from \e idle to <em>accepting PDUs</em> was successful, false otherwise.
  bool start_new_slot(slot_point slot)
  {
    uint32_t expected_pending_pdu_count = pending_pdu_count_idle;
    if (!pending_pdu_count.compare_exchange_strong(expected_pending_pdu_count, accepting_pdu_mask)) {
      return false;
    }
    configured_slot = slot;
    return true;
  }

  /// \brief Returns true if the uplink slot processor is ready to receive requests associated with the given slot.
  ///
  /// The processor is ready if:
  /// - the current state is waiting for the first OFDM symbol or receiving; and
  /// - the given slot is equal to the configured one.
  ///
  /// \param[in] slot Given slot.
  /// \return True if the processor is ready, false otherwise.
  bool is_ready_to_receive(slot_point slot) const
  {
    uint32_t current_state = pending_pdu_count.load();

    // Verify that the repository is in a valid state to process PDUs. Idle implies that there are no requests pending.
    if (!is_state_waiting_rx(current_state) && !is_state_receiving(current_state)) {
      return false;
    }

    // Checks the slot validity.
    return (configured_slot == slot);
  }

  // See the uplink_processor_fsm_notifier interface for the documentation.
  void stop_accepting_pdu() override
  {
    uint32_t current_state = pending_pdu_count.load();
    uint32_t next_state    = pending_pdu_count_idle;

    ocudu_assert(is_state_accepting_pdu(current_state), "Current state is not accepting PDUs.");

    // Transition to waiting for the first OFDM symbol if there are pending PDUs. Otherwise, transition to idle.
    unsigned nof_pending_pdu_in_queue = get_nof_pending_pdu_in_queue(current_state);
    if (nof_pending_pdu_in_queue != 0) {
      next_state = wait_rx_state_mask | nof_pending_pdu_in_queue;
    } else {
      next_state = pending_pdu_count_idle;
    }

    [[maybe_unused]] bool success = pending_pdu_count.compare_exchange_strong(current_state, next_state);
    ocudu_assert(success, "Unexpected state transition.");
  }

  // See the uplink_processor_fsm_notifier interface for the documentation.
  void increment_pending_pdu_count() override
  {
    [[maybe_unused]] uint32_t prev = pending_pdu_count.fetch_add(1);
    ocudu_assert(is_state_accepting_pdu(prev), "Cannot accept PDUs.");
  }

  /// \brief Notifies the beginning of a slot discarding process.
  ///
  /// The slot discarding is allowed if:
  /// - the state is waiting for the first OFDM symbol to be received; and
  /// - there are pending PDUs to process.
  ///
  /// \return True if the current state allows discarding the slot, false otherwise.
  bool on_discard_slot()
  {
    uint32_t current_state = pending_pdu_count.load();
    uint32_t next_state    = pending_pdu_count_idle;

    do {
      unsigned nof_pending_pdu_in_queue = get_nof_pending_pdu_in_queue(current_state);

      // Discarding is only allowed if the uplink processor is waiting for the first OFDM symbol to be received and
      // there are pending PDUs to be processed.
      if (!is_state_waiting_rx(current_state) || (nof_pending_pdu_in_queue == 0)) {
        return false;
      }

      // Transition to completing.
      next_state = wait_completion_state_mask | nof_pending_pdu_in_queue;
    } while (!pending_pdu_count.compare_exchange_weak(current_state, next_state));

    return true;
  }

  /// \brief Notifies the start of handling an OFDM symbol.
  ///
  /// Transitions to receiving if the current state is waiting for the first OFDM symbol, the OFDM symbol is valid, and
  /// it is not the last one.
  ///
  /// Transitions to wait for pending tasks to complete if the current state is receiving and the symbol is invalid, or
  /// it is marked as the last one.
  ///
  /// The current state allows processing the OFDM symbol if:
  /// - it is waiting for the first OFDM symbol to be received, or it is already in receiving state; and
  /// - has pending PDUs.
  /// \param[in] is_valid        Set to true if the OFDM symbol is valid, otherwise false.
  /// \return True if the current state allows processing the received OFDM symbol, false otherwise.
  bool on_handle_rx_symbol(bool is_valid)
  {
    uint32_t current_state    = pending_pdu_count.load();
    uint32_t next_state       = current_state;
    bool     allow_processing = true;

    do {
      unsigned nof_pending_pdu_in_queue = get_nof_pending_pdu_in_queue(current_state);

      // Actions from waiting the first OFDM symbol state.
      if (is_state_waiting_rx(current_state)) {
        // Transition to receiving if the OFDM symbol is valid.
        if (is_valid) {
          next_state       = nof_pending_pdu_in_queue | receiving_state_mask;
          allow_processing = true;
          continue;
        }

        // Transition to wait for completion if:
        // - the OFDM symbol is invalid; and
        // - PDUs are available.
        next_state       = nof_pending_pdu_in_queue | wait_completion_state_mask;
        allow_processing = true;
        continue;
      }

      // Actions from receiving state.
      if (is_state_receiving(current_state)) {
        // Keep the same state if the OFDM symbol is valid.
        if (is_valid) {
          next_state       = current_state;
          allow_processing = true;
          continue;
        }

        // Transition to idle if:
        // - the OFDM symbol is invalid; and
        // - the PDU queue is empty.
        if (nof_pending_pdu_in_queue == 0) {
          next_state       = pending_pdu_count_idle;
          allow_processing = false;
          continue;
        }

        // Transition to wait for completion if:
        // - the OFDM symbol is invalid; and
        // - PDUs are available.
        next_state       = nof_pending_pdu_in_queue | wait_completion_state_mask;
        allow_processing = true;
        continue;
      }

      // Otherwise, the state is not handled. Forbid the symbol processing.
      return false;
    } while (!pending_pdu_count.compare_exchange_weak(current_state, next_state));

    return allow_processing;
  }

  /// \brief Notifies on completing the processing for the last OFDM symbol.
  ///
  /// The state machine shall transition to \e idle if the PDU queue is empty, otherwise it transitions to <em>wait
  /// completion</em>.
  void on_processed_last_symbol()
  {
    uint32_t current_state = pending_pdu_count.load();
    uint32_t next_state    = current_state;

    do {
      unsigned nof_pending_pdu_in_queue = get_nof_pending_pdu_in_queue(current_state);

      // This method is only reached from receiving state.
      ocudu_assert(is_state_receiving(current_state), "Expected receiving state but got 0x{:08x}", current_state);

      // Transition to idle if the PDU queue is empty.
      if (nof_pending_pdu_in_queue == 0) {
        next_state = pending_pdu_count_idle;
        continue;
      }

      // Otherwise, transition to wait for completion.
      next_state = nof_pending_pdu_in_queue | wait_completion_state_mask;
    } while (!pending_pdu_count.compare_exchange_weak(current_state, next_state));
  }

  /// \brief Notifies the completion of a PDU processing.
  ///
  /// Decrements the pending PDU counter and transition to idle if there are no more pending PDUs.
  ///
  /// \remark An assertion is triggered if:
  /// - the current state is not receiving nor waiting for PDUs to complete; or
  /// - the pending number of PDUs counters is zero.
  void on_finish_processing_pdu()
  {
    uint32_t current_state = pending_pdu_count.load();
    uint32_t next_state    = pending_pdu_count_idle;

    do {
      unsigned nof_pending_pdu_in_queue = get_nof_pending_pdu_in_queue(current_state);

      // Assert previous state.
      ocudu_assert((is_state_receiving(current_state) || is_state_wait_pdu_completion(current_state)) &&
                       (nof_pending_pdu_in_queue > 0),
                   "The slot repository is in an unexpected state 0x{:08x}.",
                   current_state);

      // Transition to idle if it is the last PDU and the state is waiting for completion.
      if ((nof_pending_pdu_in_queue == 1) && is_state_wait_pdu_completion(current_state)) {
        next_state = pending_pdu_count_idle;
        continue;
      }

      // Decrement the count of pending PDUs.
      next_state = current_state - 1;
    } while (!pending_pdu_count.compare_exchange_weak(current_state, next_state));
  }

private:
  /// Pending PDU value when the processor is idle.
  static constexpr uint32_t pending_pdu_count_idle = 0x0;
  /// Accepting PDU state mask in the pending PDU count.
  static constexpr uint32_t accepting_pdu_mask = 0x80000000;
  /// Wait first symbol PDU state mask in the pending PDU count.
  static constexpr uint32_t wait_rx_state_mask = 0x40000000;
  /// Receiving PDU state mask in the pending PDU count.
  static constexpr uint32_t receiving_state_mask = 0x20000000;
  /// Waiting for PDUs to complete state mask in the pending PDU count.
  static constexpr uint32_t wait_completion_state_mask = 0x10000000;

  /// Counts the number of pending PDUs.
  std::atomic<uint32_t> pending_pdu_count = {};
  /// Current configured slot.
  slot_point configured_slot;

  /// Returns true if a state is idle.
  static bool is_state_idle(uint32_t state) { return (state == pending_pdu_count_idle); }
  /// Returns true if a state is accepting PDU.
  static bool is_state_accepting_pdu(uint32_t state) { return (state & accepting_pdu_mask); }
  /// Returns true if a state is waiting for the first OFDM symbol to be received.
  static bool is_state_waiting_rx(uint32_t state) { return (state & wait_rx_state_mask); }
  /// Returns true if a state is receiving.
  static bool is_state_receiving(uint32_t state) { return (state & receiving_state_mask); }
  /// Returns true if a state is waiting for PDUs to complete.
  static bool is_state_wait_pdu_completion(uint32_t state) { return (state & wait_completion_state_mask); }
  /// Returns the total number of pending PDUs in the queues.
  static unsigned get_nof_pending_pdu_in_queue(uint32_t state) { return state & 0xfff; }
};

} // namespace ocudu
