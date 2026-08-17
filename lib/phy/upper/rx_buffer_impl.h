// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rx_buffer_codeblock_pool.h"
#include "rx_buffer_state_fsm.h"
#include "ocudu/adt/bit_buffer.h"
#include "ocudu/adt/expected.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include "ocudu/phy/upper/rx_buffer_pool.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/ran/sch/sch_constants.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

/// Receive buffer reservation status codes.
enum class rx_buffer_status : uint8_t {
  /// Status code for successful reservation.
  successful = 0,
  /// The buffer is already reserved and in a state that cannot be reserved.
  already_in_use,
  /// The buffer was available but codeblocks could not be allocated.
  insufficient_cb,
  /// The buffer reservation is marked as a retransmission with a different number of codeblocks.
  retransmission_cb_mismatch,
};

/// Gets the receive buffer status code text string.
constexpr const char* to_string(rx_buffer_status status)
{
  switch (status) {
    case rx_buffer_status::successful:
      return "successful";
    case rx_buffer_status::already_in_use:
      return "already_in_use";
    case rx_buffer_status::insufficient_cb:
      return "insufficient_cb";
    case rx_buffer_status::retransmission_cb_mismatch:
      return "retransmission_cb_mismatch";
    default:
      return "unknown";
  }
}

/// Implements a receiver buffer interface.
class rx_buffer_impl : public unique_rx_buffer::buffer_management
{
private:
  /// Buffer internal finite state machine.
  rx_buffer_state_fsm state_machine;
  /// Reference to the codeblock pool.
  rx_buffer_codeblock_pool& codeblock_pool;
  /// Stores codeblocks CRCs.
  static_vector<bool, MAX_NOF_SEGMENTS> crc;
  /// Stores codeblock identifiers.
  static_vector<unsigned, MAX_NOF_SEGMENTS> codeblock_ids;
  /// Repetition counter - it becomes the identifier for the next reservation.
  std::atomic<unsigned> repetition_counter = 0;

  /// \brief Decodes enqueued callbacks for the next repetition.
  ///
  /// For each codeblock assigned to this buffer, advance the repetition counter.
  void increment_repetition()
  {
    for (unsigned cb_id : codeblock_ids) {
      codeblock_pool.advance_repetition(cb_id);
    }
  }

  /// Frees reserved codeblocks. The codeblocks are returned to the pool.
  void free()
  {
    // Free all codeblocks.
    for (unsigned cb_id : codeblock_ids) {
      codeblock_pool.free(cb_id);
    }

    // Indicate the buffer is available by clearing the codeblocks identifiers.
    codeblock_ids.clear();
    crc.clear();
  }

public:
  /// \brief Creates a receive buffer.
  /// \param pool Codeblock buffer pool.
  explicit rx_buffer_impl(rx_buffer_codeblock_pool& pool) : codeblock_pool(pool)
  {
    // Do nothing.
  }

  /// Copy constructor - creates another buffer with the same codeblock pool.
  rx_buffer_impl(const rx_buffer_impl& other) noexcept : codeblock_pool(other.codeblock_pool) {}

  /// Move constructor - creates another buffer with the same codeblock pool.
  rx_buffer_impl(rx_buffer_impl&& other) noexcept : codeblock_pool(other.codeblock_pool) {}

  /// \brief Reserves a number of codeblocks from the pool.
  ///
  /// It optionally resets the CRCs and dynamically reallocates codeblocks.
  ///
  /// \see rx_buffer_status for the different reasons the reservation fails.
  ///
  /// \param nof_codeblocks Number of codeblocks to reserve.
  /// \param reset_crc      Set to true for reset the codeblock CRCs.
  /// \return The reservation status.
  rx_buffer_status reserve(unsigned nof_codeblocks, bool reset_crc)
  {
    if (!state_machine.on_reserve(reset_crc)) {
      return rx_buffer_status::already_in_use;
    }

    // Early return if it is a not a new transmission.
    if (!reset_crc) {
      // Ensure the number of codeblocks remains the same.
      if (nof_codeblocks != codeblock_ids.size()) {
        state_machine.on_unlock();
        return rx_buffer_status::retransmission_cb_mismatch;
      }
      return rx_buffer_status::successful;
    }

    // If the current number of codeblocks is larger than required, free the excess of codeblocks.
    while (codeblock_ids.size() > nof_codeblocks) {
      // Get the codeblock identifier at the back and remove from the list.
      unsigned cb_id = codeblock_ids.back();
      codeblock_ids.pop_back();

      // Free the codeblock.
      codeblock_pool.free(cb_id);
    }

    // If the current number of codeblocks is less than required, reserve the remaining codeblocks.
    while (codeblock_ids.size() < nof_codeblocks) {
      // Reserve codeblock.
      std::optional<unsigned> cb_id = codeblock_pool.reserve();

      // Free the entire buffer if one codeblock cannot be reserved.
      if (!cb_id.has_value()) {
        free();
        state_machine.on_insufficient_cb();
        return rx_buffer_status::insufficient_cb;
      }

      // Append the codeblock identifier to the list.
      codeblock_ids.push_back(*cb_id);
    }

    // Resize CRCs.
    crc.resize(nof_codeblocks);

    // Reset CRCs and repetition counters if necessary.
    if (reset_crc) {
      reset_codeblocks_crc();
      repetition_counter = 0;
      for (auto cb_id : codeblock_ids) {
        codeblock_pool.reset_repetition(cb_id);
      }
    }

    return rx_buffer_status::successful;
  }

  // See interface for documentation.
  unsigned get_nof_codeblocks() const override { return codeblock_ids.size(); }

  // See interface for documentation.
  void reset_codeblocks_crc() override
  {
    // Set all codeblock CRC to false.
    for (bool& cb_crc : crc) {
      cb_crc = false;
    }
  }

  // See interface for documentation.
  span<bool> get_codeblocks_crc() override { return crc; }

  // See interface for documentation.
  unsigned get_absolute_codeblock_id(unsigned codeblock_id) const override
  {
    ocudu_assert(codeblock_id < codeblock_ids.size(),
                 "Codeblock index ({}) is out of range ({}).",
                 codeblock_id,
                 codeblock_ids.size());
    return codeblock_ids[codeblock_id];
  }

  // See interface for documentation.
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) override
  {
    ocudu_assert(codeblock_id < codeblock_ids.size(),
                 "Codeblock index ({}) is out of range ({}).",
                 codeblock_id,
                 codeblock_ids.size());
    unsigned cb_id       = codeblock_ids[codeblock_id];
    unsigned cb_max_size = codeblock_pool.get_soft_bits(cb_id).size();
    ocudu_assert(
        codeblock_size <= cb_max_size, "Codeblock size {} exceeds maximum size {}.", codeblock_size, cb_max_size);
    return codeblock_pool.get_soft_bits(cb_id).first(codeblock_size);
  }

  // See interface for documentation.
  bit_buffer get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) override
  {
    ocudu_assert(codeblock_id < codeblock_ids.size(),
                 "Codeblock index ({}) is out of range ({}).",
                 codeblock_id,
                 codeblock_ids.size());
    unsigned cb_id         = codeblock_ids[codeblock_id];
    unsigned data_max_size = codeblock_pool.get_data_bits(cb_id).size();
    ocudu_assert(
        data_size <= data_max_size, "Codeblock data size {} exceeds maximum size {}.", data_size, data_max_size);
    return codeblock_pool.get_data_bits(cb_id).first(data_size);
  }

  // See interface for documentation.
  void decode_cb_in_sequence(unsigned                    retransmission,
                             unsigned                    codeblock_id,
                             rx_buffer_decoder_callback& decoder_callback) override
  {
    codeblock_pool.decode_cb_in_sequence(codeblock_ids[codeblock_id], retransmission, codeblock_id, decoder_callback);
  }

  // See interface for documentation.
  void unlock() override
  {
    // Decode next repetition before unlocking the FSM.
    increment_repetition();

    // Notify unlock to the FSM.
    state_machine.on_unlock();
  }

  // See interface for documentation.
  void release() override
  {
    // Decode next repetition before releasing the FSM.
    increment_repetition();

    // Notify the release event to the state machine.
    bool released = state_machine.on_release();

    // Free codeblocks if the buffer was released.
    if (released) {
      // Release all reserved codeblocks.
      free();

      // Notify the completion of the release.
      state_machine.on_release_complete();
    }
  }

  /// Fetches and increment the repetition identifier.
  unsigned fetch_and_increment_repetition_id() { return repetition_counter.fetch_add(1); }

  /// Returns true if the buffer is free.
  bool is_free() const { return state_machine.is_available(); }

  /// Returns true if the buffer is locked.
  bool is_locked() const { return state_machine.is_locked(); }

  /// \brief Expires the buffer.
  ///
  /// The buffer pool shall use this method when the buffer expires. The buffer frees the reserved codeblocks if it is
  /// not blocked.
  ///
  /// \return \c true if the buffer is not locked.
  bool expire()
  {
    // Notify the expired event to the state machine.
    bool expired = state_machine.on_expire();

    // The buffer cannot be released if it is locked.
    if (expired) {
      // Release all reserved codeblocks.
      free();

      // Notify the completion of the release.
      state_machine.on_release_complete();
    }

    return expired;
  }
};

} // namespace ocudu
