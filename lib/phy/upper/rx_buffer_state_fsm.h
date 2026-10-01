// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <atomic>
#include <cstdint>

namespace ocudu {

/// \brief Receive buffer state finite state machine.
///
/// Methods \ref on_reserve() and \ref on_expire() are not expected to be called concurrently.
class rx_buffer_state_fsm
{
public:
  /// \brief Method on the event of a new reservation.
  ///
  /// The reservation fails if:
  /// - the current state is releasing; or
  /// - the reception is marked CRC reset while the current state is locked.
  ///
  /// \param[in] reset_crc       Set to true if the reservation is for a new transmission.
  /// \return True if the reservation is successful.
  bool on_reserve(bool reset_crc)
  {
    unsigned current_state = internal_state.load(std::memory_order_acquire);
    unsigned next_state;
    do {
      // The reservation fails if the current buffer state is releasing buffers.
      if (current_state == state_releasing) {
        return false;
      }

      // The reservation fails if the current state is locked and reset CRC is required.
      if (is_state_locked(current_state) && reset_crc) {
        return false;
      }

      // If the current state is available, transition to locked with one active reserved scope.
      if (current_state == state_available) {
        next_state = 1;
        continue;
      }

      // Otherwise increment the number of reserved scopes.
      next_state = current_state + 1;
    } while (!internal_state.compare_exchange_weak(current_state, next_state));

    return true;
  }

  /// \brief Method on the event of insufficient codeblocks.
  /// \remark An assertion is triggered if the current state is available.
  void on_insufficient_cb()
  {
    [[maybe_unused]] unsigned prev_state = internal_state.exchange(state_available);
    ocudu_assert(prev_state == 1, "The buffer was expected to be one scope.");
  }

  /// \brief  Method on the event of unlocking the buffer.
  ///
  /// Decrements the number of active reserved scopes by one.
  ///
  /// \remark An assertion is triggered if the current buffer is not locked.
  void on_unlock()
  {
    unsigned current_state = internal_state.load(std::memory_order_acquire);
    unsigned next_state;
    do {
      ocudu_assert(is_state_locked(current_state), "The current buffer state is not locked.");
      next_state = current_state - 1;
    } while (!internal_state.compare_exchange_weak(current_state, next_state));
  }

  /// \brief Method on the event of releasing the buffer.
  ///
  /// Decrements the number of active reserved scopes by one. It transitions to releasing state if it is the last active
  /// scope.
  ///
  /// \return True if the buffer is no longer actively reserved in any scope.
  /// \remark  An assertion is triggered if the buffer is not locked.
  bool on_release()
  {
    unsigned current_state = internal_state.load(std::memory_order_acquire);
    unsigned next_state;
    do {
      ocudu_assert(is_state_locked(current_state), "The current buffer state is not locked.");
      if (current_state == 1) {
        // If the current number of active reserved scopes is one then transition to releasing.
        next_state = state_releasing;
      } else {
        // Otherwise decrement the number of states.
        next_state = current_state - 1;
      }
    } while (!internal_state.compare_exchange_weak(current_state, next_state));

    return next_state == state_releasing;
  }

  /// \brief Method on the event of expiring a buffer.
  ///
  /// Transitions to releasing state if the buffer is not actively reserved in any scope.
  ///
  /// \return True if the buffer transitioned to releasing state.
  bool on_expire()
  {
    unsigned expected_state = state_reserved;
    return internal_state.compare_exchange_strong(expected_state, state_releasing);
  }

  /// \brief Method on the event of completing the release of the buffer.
  ///
  /// Notifies the completion of freeing the codeblocks.
  ///
  /// \remark  An assertion is triggered if the current buffer is not in releasing state.
  void on_release_complete()
  {
    unsigned              expected_state = state_releasing;
    [[maybe_unused]] bool success        = internal_state.compare_exchange_strong(expected_state, state_available);
    ocudu_assert(success, "Unexpected buffer state 0x{:08x}", expected_state);
  }

  /// Determines whether the current buffer is locked.
  bool is_locked() const { return is_state_locked(internal_state.load(std::memory_order_relaxed)); }

  /// Determines whether the current buffer is available.
  bool is_available() const { return internal_state.load(std::memory_order_relaxed) == state_available; }

private:
  /// Available state: no codeblocks allocated, no in-flight consumers.
  static constexpr unsigned state_available = std::numeric_limits<unsigned>::max();
  /// Releasing state: buffer is in process of being released.
  static constexpr unsigned state_releasing = state_available - 1;
  /// Reserved state: buffer is reserved but not active in any scope.
  static constexpr unsigned state_reserved = 0;

  /// A state is locked if it is not available, releasing, nor reserved.
  static bool is_state_locked(unsigned state)
  {
    return (state != state_available) && (state != state_releasing) && (state != state_reserved);
  }

  /// \brief Current buffer state stored as an atomic counter.
  ///
  /// The value encodes the buffer state and the number of concurrent reservations:
  /// - `state_available`: available, no codeblocks reserved.
  /// - `state_reserved`: reserved, codeblocks are allocated, no in-flight consumers.
  /// - `1, 2, 3, ...`: locked, codeblocks are allocated, one or more in-flight consumers
  ///   (e.g. concurrent decoders for retransmissions of the same HARQ process).
  std::atomic<unsigned> internal_state = state_available;
};

} // namespace ocudu
