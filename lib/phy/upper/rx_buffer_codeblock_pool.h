// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bit_buffer.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include "ocudu/phy/upper/rx_buffer_decoder_callback.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include <mutex>
#include <vector>

namespace ocudu {

/// Manages a codeblock buffer pool.
class rx_buffer_codeblock_pool
{
public:
  /// \brief Maximum number of repetitions.
  ///
  /// The maximum number of repetitions is given by the maximum value of the parameter \e numberOfRepetitionsExt-r17 in
  /// the Information Element \e PUSCH-Allocation-r16 defined in TS38.331 Section 6.3.2.
  static constexpr unsigned max_nof_repetitions = 32;

private:
  /// Codeblock identifier list type.
  using codeblock_identifier_list =
      concurrent_queue<unsigned, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

  /// Bitset used for unlocked repetitions.
  class unlocked_repetition_bitset : public bounded_bitset<max_nof_repetitions>
  {
  public:
    /// Default constructor - creates a bitset of the maximum size without any unlocked repetition bitset.
    unlocked_repetition_bitset() : bounded_bitset(max_nof_repetitions) {}
  };

  /// Collects a codeblock call back context.
  struct codeblock_callback_context {
    /// Codeblock identifier within the transport block.
    unsigned codeblock_id;
    /// Decoder callback.
    rx_buffer_decoder_callback* callback = nullptr;
  };

  /// Describes a codeblock buffer entry.
  struct codeblock_container {
    /// Contains the codeblock soft bits.
    std::vector<log_likelihood_ratio> soft_bits;
    /// Contains the codeblock data bits.
    dynamic_bit_buffer data_bits;
    /// Mutex protection for callbacks.
    std::mutex callbacks_mutex;
    /// Current repetition.
    unsigned repetition;
    /// Unlocked repetitions.
    unlocked_repetition_bitset repetition_mask;
    /// Decoder callbacks.
    slotted_array<codeblock_callback_context, max_nof_repetitions> callbacks;
  };

  /// Stores all codeblock entries.
  std::vector<codeblock_container> entries;
  /// List containing the free codeblocks identifiers.
  codeblock_identifier_list free_list;

public:
  /// \brief Creates a receive buffer codeblock pool.
  /// \param[in] nof_codeblocks      Total number of codeblocks, shared across the pool.
  /// \param[in] max_codeblock_size  Maximum size of a single codeblock.
  /// \param[in] external_soft_bits  Set to true to indicate that soft bits are not stored in the buffer.
  rx_buffer_codeblock_pool(unsigned nof_codeblocks, unsigned max_codeblock_size, bool external_soft_bits) :
    entries(nof_codeblocks), free_list(nof_codeblocks)
  {
    unsigned cb_id = 0;
    for (codeblock_container& e : entries) {
      e.soft_bits.resize(external_soft_bits ? 0 : max_codeblock_size);
      // The maximum number of data bits is
      // max_codeblock_size * max(BG coding rate) = max_codeblock_size * (1/3)
      e.data_bits.resize(divide_ceil(max_codeblock_size, 3));
      // Push codeblock identifier into the free list.
      while (!free_list.try_push(cb_id++)) {
      }
    }
  }

  /// \brief Reserves a codeblock buffer.
  /// \return The codeblock identifier in the pool if it is reserved successfully. Otherwise, \c std::nullopt
  std::optional<unsigned> reserve()
  {
    // Try to get an available codeblock.
    unsigned obj;
    if (free_list.try_pop(obj)) {
      return obj;
    }
    return std::nullopt;
  }

  /// \brief Frees a codeblock buffer.
  /// \param[in] cb_id Indicates the codeblock identifier in the pool.
  /// \remark An assertion is triggered if the callback list for the codeblock is not empty.
  void free(unsigned cb_id)
  {
    ocudu_assert(entries[cb_id].callbacks.empty(), "Callback list is not empty.");

    // Push codeblock identifier back in the pool.
    while (!free_list.try_push(cb_id)) {
    }
  }

  /// \brief Resets the repetition counter.
  /// \param[in] cb_id Indicates the codeblock identifier in the pool.
  /// \remark An assertion is triggered if the callback list for the codeblock is not empty.
  void reset_repetition(unsigned cb_id)
  {
    // Select codeblock.
    codeblock_container& codeblock = entries[cb_id];

    ocudu_assert(codeblock.callbacks.empty(), "Callback list is not empty.");

    codeblock.repetition = 0;
    codeblock.repetition_mask.fill(false);
  }

  /// \brief Gets a codeblock soft-bit buffer.
  /// \param[in] cb_id Indicates the codeblock identifier.
  /// \return A view to the codeblock soft-bit buffer.
  span<log_likelihood_ratio> get_soft_bits(unsigned cb_id)
  {
    ocudu_assert(cb_id < entries.size(), "Codeblock index ({}) is out of range ({}).", cb_id, entries.size());
    return entries[cb_id].soft_bits;
  }

  /// \brief Gets a codeblock data-bit buffer.
  /// \param[in] cb_id Indicates the codeblock identifier.
  /// \return A view to the codeblock data-bit buffer.
  bit_buffer& get_data_bits(unsigned cb_id)
  {
    ocudu_assert(cb_id < entries.size(), "Codeblock index ({}) is out of range ({}).", cb_id, entries.size());
    return entries[cb_id].data_bits;
  }

  /// \brief Enqueues a repetition.
  /// \param[in] pool_cb_id        Codeblock identifier within the codeblock pool.
  /// \param[in] repetition        Repetition identifier.
  /// \param[in] tb_cb_id          Codeblock identifier within the transport block.
  /// \param[in] decoder_callback  Decoder callback object. The implementing \c codeblock_decode method is invoked when
  ///                              the codeblock is ready to be decoded.
  /// \remark An assertion is triggered if the repetition identifier exceeds the maximum number of repetitions.
  /// \remark An assertion is triggered if the repetition identifier is smaller than the latest decoded repetition.
  /// \remark An assertion is triggered if a previous callback is available for the same repetition.
  void decode_cb_in_sequence(unsigned                    pool_cb_id,
                             unsigned                    repetition,
                             unsigned                    tb_cb_id,
                             rx_buffer_decoder_callback& decoder_callback)
  {
    ocudu_assert(repetition < max_nof_repetitions,
                 "Repetition identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                 repetition,
                 max_nof_repetitions);

    // Select codeblock.
    codeblock_container& codeblock = entries[pool_cb_id];

    // Protect concurrent access.
    std::unique_lock lock(codeblock.callbacks_mutex);

    // If this repetition matches the current repetition...
    if (repetition == codeblock.repetition) {
      // The lock is no longer required.
      lock.unlock();
      decoder_callback.codeblock_decode(tb_cb_id);
      return;
    }

    // Verify the repetition is valid.
    ocudu_assert(repetition > codeblock.repetition, "Detected repetition in the past.");
    ocudu_assert(!codeblock.callbacks.contains(repetition), "Decoder callback repetition is already present.");

    // Enqueue decoder callback.
    codeblock.callbacks.emplace(repetition, codeblock_callback_context{tb_cb_id, &decoder_callback});
  }

  /// \brief Unlocks a repetition for a selected codeblock.
  /// \param[in] codeblock_id  Codeblock identifier within the codeblock pool.
  /// \param[in] repetition    Repetition identifier that was unlocked.
  /// \remark An assertion is triggered if the unlocked repetition has a pending callback.
  void on_unlock_repetition(unsigned codeblock_id, unsigned repetition)
  {
    // Select codeblock.
    codeblock_container& codeblock = entries[codeblock_id];

    std::optional<codeblock_callback_context> callback;

    // Advance repetition until a callback is found.
    {
      // Protect concurrent access.
      std::scoped_lock lock(codeblock.callbacks_mutex);

      // The receive buffer is designed to be unlocked/released after decoding the codeblock. It is not possible that
      // a buffer was unlocked (or destroyed) leaving a callback behind.
      ocudu_assert(!codeblock.callbacks.contains(repetition),
                   "An out of order unlocked codeblock left an unattended callback.");

      // Mark repetition as completed.
      codeblock.repetition_mask.set(repetition);

      // Skip further steps if the unlocked repetition is not the current one.
      if (codeblock.repetition != repetition) {
        return;
      }

      // Advance from the repetition counter to the most advanced marked completed repetition.
      int next_repetition = codeblock.repetition_mask.find_lowest(repetition, max_nof_repetitions, false);
      if (next_repetition < 0) {
        // If all repetitions have been unlocked, leave the maximum number of repetitions and return.
        codeblock.repetition = max_nof_repetitions;
        return;
      }

      // Move to the next repetition.
      codeblock.repetition = next_repetition;

      // Save the decode callback for being invoked in the unprotected region.
      if (codeblock.callbacks.contains(next_repetition)) {
        callback = codeblock.callbacks.take(next_repetition);
      }
    }

    // Invoke callback if there is any.
    if (callback.has_value()) {
      codeblock_callback_context callback_ctx = callback.value();
      callback_ctx.callback->codeblock_decode(callback_ctx.codeblock_id);
    }
  }
};

} // namespace ocudu
