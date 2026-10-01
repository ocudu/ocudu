// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/synchronization/futex_util.h"
#include <thread>
#include <utility>

namespace ocudu {

class scoped_sync_token;

/// \brief Synchronization event to wait until all tokens to be reset.
///
/// This class operates like a std::barrier, where the "arrive" call is performed by "tokens" in an RAII manner and
/// "wait" is performed by the \c sync_event. Contrarily to the std::barrier, the number of participants is not fixed.
class sync_event
{
public:
  ~sync_event() { wait(); }

  /// Creates a new observer of wait() requests.
  [[nodiscard]] scoped_sync_token get_token();

  /// Waits for all tokens to be reset. At the end of this call, all tokens are guaranteed to be reset.
  void wait()
  {
    // Block waiting until all observers are gone.
    auto cur = token_count.load(std::memory_order_acquire);
    while (cur > 0) {
      futex_util::wait(token_count, cur);
      cur = token_count.load(std::memory_order_acquire);
    }
  }

  [[nodiscard]] uint32_t nof_tokens_approx() const { return token_count.load(std::memory_order_relaxed); }

private:
  friend class scoped_sync_token;

  std::atomic<uint32_t> token_count{0};
};

/// Scoped token that notifies the associated sync_event when it gets destroyed or reset.
class scoped_sync_token
{
  friend class sync_event;
  explicit scoped_sync_token(std::atomic<uint32_t>& token_count_) : token_count(&token_count_) { inc_token(); }

public:
  scoped_sync_token() = default;

  scoped_sync_token(const scoped_sync_token& other) : token_count(other.token_count) { inc_token(); }

  scoped_sync_token(scoped_sync_token&& other) noexcept : token_count(std::exchange(other.token_count, nullptr)) {}

  scoped_sync_token& operator=(const scoped_sync_token& other)
  {
    if (this != &other) {
      reset();
      token_count = other.token_count;
      inc_token();
    }
    return *this;
  }
  scoped_sync_token& operator=(scoped_sync_token&& other) noexcept
  {
    reset();
    token_count = std::exchange(other.token_count, nullptr);
    return *this;
  }

  ~scoped_sync_token()
  {
    if (token_count != nullptr) {
      auto cur = token_count->fetch_sub(1, std::memory_order_acq_rel) - 1;
      if (cur == 0) {
        // Count is zero. Wake all waiters.
        // Note: wake_all may be called when \c token_count has already be freed. This is OK as per the futex API that
        // only depends on the bit representation of the \c token_count address.
        // Useful reference about the issue: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2616r4.html
        futex_util::wake_all(*token_count);
      }
      token_count = nullptr;
    }
  }

  /// Destroys the token and potentially unlocks sync_event::wait().
  void reset() { scoped_sync_token{}.swap(*this); }

  void swap(scoped_sync_token& other) noexcept { std::swap(token_count, other.token_count); }

private:
  void inc_token()
  {
    if (token_count != nullptr) {
      token_count->fetch_add(1, std::memory_order_relaxed);
    }
  }

  std::atomic<uint32_t>* token_count = nullptr;
};

inline scoped_sync_token sync_event::get_token()
{
  return scoped_sync_token{token_count};
}

} // namespace ocudu
