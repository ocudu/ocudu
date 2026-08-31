// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/rx_buffer.h"
#include "ocudu/phy/upper/rx_buffer_decoder_callback.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

/// \brief Wraps a receive buffer instance and locks it inside a scope.
///
/// The locking mechanism prevents the pool from reserving and freeing a buffer as long as it is being used inside a
/// block scope.
class unique_rx_buffer
{
public:
  /// \brief Buffer management interface.
  ///
  /// Public interface to access the buffer pool for sequenced decoding, locking, and unlocking the underlying buffer.
  ///
  /// The implementation must be thread safe. In other words, unlock() and release() might be called from different
  /// threads.
  class buffer_management : public rx_buffer
  {
  public:
    /// \brief Execute a codeblock decode in sequence.
    ///
    /// \param[in] retransmission    Retransmission identifier.
    /// \param[in] codeblock_id      Codeblock identifier within the transport block to decode.
    /// \param[in] decoder_callback  Decoder callback object. The implementing \c codeblock_decode method is invoked
    ///                              when the codeblock is ready to be decoded.
    virtual void decode_cb_in_sequence(unsigned                    retransmission,
                                       unsigned                    codeblock_id,
                                       rx_buffer_decoder_callback& decoder_callback) = 0;

    /// \brief Unlocks the buffer.
    /// \param[in] retransmission Retransmission identifier.
    virtual void unlock(unsigned retransmission) = 0;

    /// \brief Releases (after unlocking) the buffer resources.
    /// \param[in] retransmission Retransmission identifier.
    virtual void release(unsigned retransmission) = 0;
  };

  /// Builds an invalid buffer.
  explicit unique_rx_buffer() = default;

  /// \brief Builds a unique buffer from a buffer reference.
  /// \param[in] instance_        Reference to the actual receive buffer.
  /// \param[in] retransmission_  Retransmission identifier.
  unique_rx_buffer(buffer_management& instance_, unsigned retransmission_ = 0) :
    ptr(&instance_), retransmission(retransmission_)
  {
  }

  /// Destructor - it unlocks the buffer.
  ~unique_rx_buffer()
  {
    if (ptr != nullptr) {
      ptr->unlock(retransmission);
      ptr = nullptr;
    }
  }

  /// Copy constructor is deleted to prevent the unique buffer from being shared across multiple scopes.
  unique_rx_buffer(const unique_rx_buffer& /**/) = delete;

  /// Move constructor is the only way to move the buffer to a different scope.
  unique_rx_buffer(unique_rx_buffer&& other) noexcept
  {
    ptr            = other.ptr;
    retransmission = other.retransmission;
    other.ptr      = nullptr;
  }

  /// Move assignment operator.
  unique_rx_buffer& operator=(unique_rx_buffer&& other) noexcept
  {
    // Unlock current soft buffer if it is actually not unlocked.
    if (ptr != nullptr) {
      ptr->unlock(retransmission);
    }

    // Move the other soft buffer ownership to the current soft buffer.
    ptr            = other.ptr;
    retransmission = other.retransmission;
    other.ptr      = nullptr;

    return *this;
  }

  unique_rx_buffer& operator=(const unique_rx_buffer& other) = delete;

  /// Gets the buffer.
  rx_buffer& get()
  {
    ocudu_assert(is_valid(), "Invalid buffer.");
    return *ptr;
  }

  /// Gets a read-only buffer.
  const rx_buffer& get() const
  {
    ocudu_assert(is_valid(), "Invalid buffer.");
    return *ptr;
  }

  rx_buffer&       operator*() { return get(); }
  rx_buffer*       operator->() { return &get(); }
  const rx_buffer& operator*() const { return get(); }
  const rx_buffer* operator->() const { return &get(); }

  /// Returns true if the unique buffer contains a valid buffer.
  bool is_valid() const { return ptr != nullptr; }

  /// Overload conversion to bool.
  explicit operator bool() const noexcept { return is_valid(); }

  /// Unlock and releases the buffer resources.
  void release()
  {
    ocudu_assert(ptr != nullptr, "Invalid buffer for releasing.");
    ptr->release(retransmission);
    ptr = nullptr;
  }

  /// Unlocks the buffer resources.
  void unlock()
  {
    ocudu_assert(ptr != nullptr, "Invalid buffer for unlocking.");
    ptr->unlock(retransmission);
    ptr = nullptr;
  }

  /// \brief Execute a codeblock decode in sequence.
  ///
  /// Enqueues the given decoder callback for the specified codeblock and either executes it immediately or queues it
  /// for deferred sequential execution.
  ///
  /// This function is thread-safe but not suitable for real-time operation because it locks a mutex for protecting the
  /// concurrent access.
  ///
  /// \param[in] codeblock_id      Codeblock identifier within the transport block to decode.
  /// \param[in] decoder_callback  Decoder callback object. The implementing \c codeblock_decode method is invoked when
  ///                              the codeblock is ready to be decoded.
  void decode_cb_in_sequence(unsigned codeblock_id, rx_buffer_decoder_callback& decoder_callback)
  {
    ptr->decode_cb_in_sequence(retransmission, codeblock_id, decoder_callback);
  }

  unsigned get_retransmission() const { return retransmission; }

private:
  /// Underlying pointer to the buffer. Set to nullptr for an invalid buffer.
  buffer_management* ptr = nullptr;
  /// \brief Current retransmission index (0...31) configured in the buffer.
  ///
  /// The retransmission index is used for the sequential decode.
  unsigned retransmission = std::numeric_limits<unsigned>::max();
};

} // namespace ocudu
