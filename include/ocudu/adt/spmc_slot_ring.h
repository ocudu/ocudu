// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

namespace ocudu {

/// \brief Ring buffer of key-tagged entries with single-producer, multiple-consumer concurrent access.
///
/// Each entry is addressed by an integer key (typically derived from a slot point) that maps to a ring index via
/// \c key % size. An entry carries the key it currently holds, so a read for a key whose slot has already been recycled
/// by the ring wrap-around correctly misses instead of returning stale data. Callers are responsible for deriving the
/// key from their own index type.
///
/// Access to an entry is mediated by a lock-free try-rwlock:
/// - A single producer calls write() to take exclusive access to an entry.
/// - Multiple consumers may call read() and hold shared access to the same entry concurrently.
/// - When the producer wraps around the ring and catches up with consumers still reading an aliasing slot, the
///   acquisition fails and an empty handle is returned. Acquisitions never block; the caller decides how to react
///   to such an overflow.
///
/// Acquisition is exposed through RAII handles (\ref reader / \ref writer) that release the entry on destruction.
///
/// \tparam T Per-entry payload. Must be default-constructible. The ring tags entries with their key but never touches
///           the payload; resetting and populating it is the caller's responsibility.
/// \note Technically, this is not a lock-free container. If a reader thread preempts while holding a slot, the writer
/// cannot write to it. However, the plan is to use this container in a such a way that readers rarely contend with
/// the writer, and vice-versa. It's mostly a protection mechanism.
template <typename T>
class spmc_slot_ring
{
  static_assert(std::is_default_constructible_v<T>, "Payload type must be default-constructible");

  struct ring_entry {
    /// Access guard: 0 == idle, < 0 == writer holds it, > 0 == number of readers currently holding it.
    std::atomic<int32_t> lock{0};
    /// Key whose payload is currently stored in this entry, or nullopt if the entry is empty.
    std::optional<std::size_t> key;
    /// Caller-owned payload for the stored key.
    T payload;
  };

public:
  /// Releases shared (reader) access on handle destruction.
  class reader_deleter
  {
  public:
    reader_deleter() = default;
    explicit reader_deleter(std::atomic<int32_t>& lock_) : lock(&lock_) {}

    void operator()(const T* ptr) const
    {
      if (ptr != nullptr) {
        lock->fetch_sub(1, std::memory_order_release);
      }
    }

  private:
    std::atomic<int32_t>* lock = nullptr;
  };

  /// Releases exclusive (writer) access on handle destruction.
  class writer_deleter
  {
  public:
    writer_deleter() = default;
    explicit writer_deleter(std::atomic<int32_t>& lock_) : lock(&lock_) {}

    void operator()(T* ptr) const
    {
      if (ptr != nullptr) {
        lock->store(0, std::memory_order_release);
      }
    }

  private:
    std::atomic<int32_t>* lock = nullptr;
  };

  /// Shared, read-only access handle to an entry's payload.
  using reader = std::unique_ptr<const T, reader_deleter>;
  /// Exclusive, mutable access handle to an entry's payload.
  using writer = std::unique_ptr<T, writer_deleter>;

  explicit spmc_slot_ring(std::size_t nof_slots) : entries(nof_slots)
  {
    ocudu_assert(nof_slots > 0, "spmc_slot_ring size must be positive");
  }

  /// Number of slots managed by the ring.
  std::size_t size() const { return entries.size(); }

  /// \brief Tries to acquire shared access to the entry holding the given key.
  /// \return A reader handle to the payload, or an empty handle if the producer currently holds the entry or the
  /// entry has been recycled for a different key.
  reader read(std::size_t k)
  {
    ring_entry& e   = entries[index_of(k)];
    int32_t     cur = e.lock.load(std::memory_order_acquire);
    while (cur >= 0) {
      // Keep retrying as long as only readers are accessing the entry.
      if (e.lock.compare_exchange_weak(cur, cur + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
        if (e.key.has_value() and *e.key == k) {
          return reader{&e.payload, reader_deleter{e.lock}};
        }
        // The entry does not hold the requested key; release and report a miss.
        e.lock.fetch_sub(1, std::memory_order_release);
        return reader{};
      }
    }
    return reader{};
  }

  /// \brief Tries to acquire exclusive access to the entry that the given key maps to, tagging it with the key.
  /// \return A writer handle to the payload, or an empty handle if any reader or writer currently holds the entry.
  /// \note The payload is not reset; the caller is responsible for clearing and populating it.
  writer write(std::size_t k)
  {
    ring_entry& e        = entries[index_of(k)];
    int32_t     expected = 0;
    if (e.lock.compare_exchange_strong(expected, -1, std::memory_order_acquire, std::memory_order_relaxed)) {
      e.key = k;
      return writer{&e.payload, writer_deleter{e.lock}};
    }
    return writer{};
  }

  /// \brief Invalidates the entry that the given key maps to, so subsequent reads of that key miss.
  /// \return False if a reader or writer currently holds the entry (no change made), true otherwise.
  /// \note The entry is only invalidated if it still holds \c k; an entry already recycled for another key is left
  /// untouched.
  bool clear(std::size_t k)
  {
    ring_entry& e        = entries[index_of(k)];
    int32_t     expected = 0;
    if (not e.lock.compare_exchange_strong(expected, -1, std::memory_order_acquire, std::memory_order_relaxed)) {
      return false;
    }
    if (e.key.has_value() and *e.key == k) {
      e.key.reset();
    }
    e.lock.store(0, std::memory_order_release);
    return true;
  }

private:
  std::size_t index_of(std::size_t k) const { return k % entries.size(); }

  std::vector<ring_entry> entries;
};

} // namespace ocudu
