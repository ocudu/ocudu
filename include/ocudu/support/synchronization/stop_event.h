// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/synchronization/futex_util.h"
#include <atomic>
#include <thread>
#include <utility>

namespace ocudu {
namespace detail {

/// \brief Blocking policy based on Linux futexes.
///
/// This implementation provides a minimal synchronization primitive that allows a waiting thread to block on an atomic
/// state value and be woken by another thread via a futex wake.
struct futex_blocker {
  /// Blocks the calling thread while state == expected.
  static void wait(std::atomic<uint32_t>& state, uint32_t expected) { futex_util::wait(state, expected); }

  /// Wakes the thread currently waiting on state.
  static void wake(std::atomic<uint32_t>& state) { futex_util::wake_all(state); }
};

/// \brief Blocking policy based on busy waiting.
///
/// This implementation is a blocking policy that avoids OS-level synchronization primitives. Instead of truly blocking,
/// waiting threads periodically sleep for a short, fixed duration to reduce CPU consumption while polling for progress.
///
/// \note This implementation provides the same interface as other blocking polcies but does not require any wake-up
/// coordination or destructor-safety mechanisms.
struct busy_wait_blocker {
  /// Waits for progress by sleeping for a fixed duration.
  static void wait(std::atomic<uint32_t>& state, uint32_t expected)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }

  /// No-op wake operation.
  static void wake(std::atomic<uint32_t>& state) {}
};

template <typename BlockPolicy>
class stop_event_token_impl;

/// \brief Event to signal stop to multiple observers.
///
/// The stop_event blocks on stop() until all observers are gone.
/// This class is similar to \c sync_event. However, it also supports the ability to notify a stop request that is
/// visible to the tokens and allows checking if a stop was requested.
template <typename BlockPolicy>
class stop_event_source_impl
{
  static constexpr uint32_t stop_bit   = 1U << 31U;
  static constexpr uint32_t count_mask = stop_bit - 1;

  [[nodiscard]] bool was_stop_requested() const { return token_count.load(std::memory_order_acquire) >= stop_bit; }

public:
  ~stop_event_source_impl() { stop(); }

  /// Creates a new observer of stop() requests.
  [[nodiscard]] stop_event_token_impl<BlockPolicy> get_token();

  /// Requests all tokens to stop and blocks until all tokens are reset.
  void stop()
  {
    // Mark as stopped.
    auto cur = token_count.fetch_or(stop_bit, std::memory_order_acq_rel) | stop_bit;

    // Block waiting until all observers are gone.
    while (cur > stop_bit) {
      BlockPolicy::wait(token_count, cur);
      cur = token_count.load(std::memory_order_acquire);
    }
  }

  /// Resets stop_requested back to false. If there are tokens yet to complete an earlier stop(), this call will block.
  void reset()
  {
    auto cur = token_count.load(std::memory_order_acquire);
    while (cur >= stop_bit) {
      // In case a stop is ongoing (cur > stop_bit), we need to wait until it completes before resetting.
      while (cur > stop_bit) {
        BlockPolicy::wait(token_count, cur);
        cur = token_count.load(std::memory_order_acquire);
      }
      // cur <= stop_bit, which means that the stop() either completed or was not requested.

      // In case cur == stop_bit, we try to reset the stop request.
      if (cur == stop_bit and
          token_count.compare_exchange_strong(cur, 0, std::memory_order_acq_rel, std::memory_order_relaxed)) {
        // Successfully reset stop request.
        return;
      }
    }
  }

  [[nodiscard]] uint32_t nof_tokens_approx() const { return token_count.load(std::memory_order_relaxed) & count_mask; }

private:
  /// State variable composed by 1 MSB bit for signalling stop and 31 LSB bits for counting observers.
  std::atomic<uint32_t> token_count{0};
};

/// \brief Observer of calls to stop() on the associated stop_event_source.
///
/// When it gets reset or destroyed, it notifies the stop_event_source that one observer is gone.
template <typename BlockPolicy>
class stop_event_token_impl
{
  static constexpr uint32_t stop_bit   = 1U << 31U;
  static constexpr uint32_t count_mask = stop_bit - 1;

  friend class stop_event_source_impl<BlockPolicy>;

  explicit stop_event_token_impl(std::atomic<uint32_t>& token_count_) : token_count(&token_count_) { inc_token(); }

public:
  stop_event_token_impl() = default;

  stop_event_token_impl(const stop_event_token_impl& other) : token_count(other.token_count) { inc_token(); }
  stop_event_token_impl(stop_event_token_impl&& other) noexcept : token_count(std::exchange(other.token_count, nullptr))
  {
  }

  stop_event_token_impl& operator=(const stop_event_token_impl& other)
  {
    if (this != &other) {
      reset();
      token_count = other.token_count;
      inc_token();
    }
    return *this;
  }
  stop_event_token_impl& operator=(stop_event_token_impl&& other) noexcept
  {
    reset();
    token_count = std::exchange(other.token_count, nullptr);
    return *this;
  }

  ~stop_event_token_impl()
  {
    if (token_count != nullptr) {
      auto cur = token_count->fetch_sub(1, std::memory_order_acq_rel) - 1;
      if ((cur & count_mask) == 0) {
        // Count reached zero.
        // Wake all stoppers.
        BlockPolicy::wake(*token_count);
      }
      token_count = nullptr;
    }
  }

  /// Checks if the associated stop_event_source has been stopped.
  [[nodiscard]] bool is_stop_requested() const
  {
    return (token_count == nullptr) or ((token_count->load(std::memory_order_acquire) & stop_bit) > 0);
  }

  /// Destroys the observer.
  void reset() { stop_event_token_impl{}.swap(*this); }

  void swap(stop_event_token_impl& other) noexcept { std::swap(token_count, other.token_count); }

private:
  void inc_token()
  {
    if (token_count == nullptr) {
      return;
    }
    auto prev = token_count->fetch_add(1, std::memory_order_relaxed);
    if (prev & stop_bit) {
      // Stop was already requested. Release token.
      reset();
    }
  }

  std::atomic<uint32_t>* token_count = nullptr;
};

template <typename BlockPolicy>
stop_event_token_impl<BlockPolicy> stop_event_source_impl<BlockPolicy>::get_token()
{
  return was_stop_requested() ? stop_event_token_impl<BlockPolicy>{} : stop_event_token_impl<BlockPolicy>{token_count};
}

} // namespace detail

/// Type alias for the stop event source when using the futex_blocker blocking policy.
///
/// \note Use this type in non realtime contexts where performing syscalls is allowed.
using stop_event_source = detail::stop_event_source_impl<detail::futex_blocker>;

/// Type alias for the stop event token when using the futex_blocker blocking policy.
using stop_event_token = detail::stop_event_token_impl<detail::futex_blocker>;

/// Type alias for the stop event source when using the busy_wait_blocker blocking policy.
///
/// \note Use this type in realtime contexts where a more lightweight blocking mechanism is desired without requiring
/// OS-level synchronization primitives.
using rt_stop_event_source = detail::stop_event_source_impl<detail::busy_wait_blocker>;

/// Type alias for the stop event token when using the busy_wait_blocker blocking policy.
using rt_stop_event_token = detail::stop_event_token_impl<detail::busy_wait_blocker>;

} // namespace ocudu
