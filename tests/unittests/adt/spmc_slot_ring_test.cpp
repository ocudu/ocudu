// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/spmc_slot_ring.h"
#include <atomic>
#include <cstddef>
#include <gtest/gtest.h>
#include <thread>

using namespace ocudu;

namespace {

struct payload {
  std::size_t key_copy = 0;
  int         value    = 0;
};

} // namespace

TEST(spmc_slot_ring, read_of_empty_entry_misses)
{
  spmc_slot_ring<payload> ring(8);
  EXPECT_EQ(ring.read(0), nullptr);
  EXPECT_EQ(ring.read(3), nullptr);
}

TEST(spmc_slot_ring, write_then_read_round_trip)
{
  spmc_slot_ring<payload> ring(8);

  {
    auto w = ring.write(5);
    ASSERT_NE(w, nullptr);
    w->value = 42;
  }

  auto r = ring.read(5);
  ASSERT_NE(r, nullptr);
  EXPECT_EQ(r->value, 42);
}

TEST(spmc_slot_ring, read_misses_when_entry_recycled_for_aliasing_key)
{
  constexpr size_t        ring_size = 8;
  spmc_slot_ring<payload> ring(ring_size);

  ring.write(2); // index 2

  // key (2 + ring_size) maps to the same index but is a different key: the entry no longer holds it.
  EXPECT_EQ(ring.read(2 + ring_size), nullptr);
  // The original key still reads back.
  EXPECT_NE(ring.read(2), nullptr);
}

TEST(spmc_slot_ring, clear_invalidates_entry)
{
  spmc_slot_ring<payload> ring(8);

  ring.write(4);
  ASSERT_NE(ring.read(4), nullptr);

  EXPECT_TRUE(ring.clear(4));
  EXPECT_EQ(ring.read(4), nullptr);
}

TEST(spmc_slot_ring, clear_of_recycled_entry_leaves_current_occupant)
{
  constexpr size_t        ring_size = 8;
  spmc_slot_ring<payload> ring(ring_size);

  ring.write(1);
  // Clearing an aliasing key must not wipe the entry that currently holds key 1.
  EXPECT_TRUE(ring.clear(1 + ring_size));
  EXPECT_NE(ring.read(1), nullptr);
}

TEST(spmc_slot_ring, writer_is_excluded_while_a_reader_holds_the_entry)
{
  spmc_slot_ring<payload> ring(8);
  ring.write(6);

  auto r = ring.read(6);
  ASSERT_NE(r, nullptr);

  // Writer for the same index fails while the reader is alive.
  EXPECT_EQ(ring.write(6), nullptr);
  EXPECT_FALSE(ring.clear(6));

  r.reset();
  // Once released, the writer can acquire again.
  EXPECT_NE(ring.write(6), nullptr);
}

TEST(spmc_slot_ring, reader_is_excluded_while_the_writer_holds_the_entry)
{
  spmc_slot_ring<payload> ring(8);

  auto w = ring.write(6);
  ASSERT_NE(w, nullptr);

  // Reader fails while the writer is alive.
  EXPECT_EQ(ring.read(6), nullptr);
}

TEST(spmc_slot_ring, multiple_readers_acquire_concurrently)
{
  spmc_slot_ring<payload> ring(8);
  ring.write(7);

  auto r1 = ring.read(7);
  auto r2 = ring.read(7);
  auto r3 = ring.read(7);
  EXPECT_NE(r1, nullptr);
  EXPECT_NE(r2, nullptr);
  EXPECT_NE(r3, nullptr);

  // While readers are active, the writer is locked out.
  EXPECT_EQ(ring.write(7), nullptr);
}

// Single producer wrapping a small ring against many concurrent consumers. The payload carries a copy of its key, so
// any torn read between the writer and a reader would surface as a mismatch. ThreadSanitizer additionally flags races.
TEST(spmc_slot_ring, stress_single_writer_many_readers)
{
  constexpr std::size_t   ring_size = 16;
  constexpr std::size_t   nof_slots = 50000;
  spmc_slot_ring<payload> ring(ring_size);

  std::atomic<std::size_t> latest_written{0};
  std::atomic<bool>        done{false};

  std::thread producer([&]() {
    for (std::size_t k = 1; k <= nof_slots; ++k) {
      if (auto w = ring.write(k)) {
        w->key_copy = k;
        w->value    = static_cast<int>(k * 2);
      }
      latest_written.store(k, std::memory_order_release);
    }
    done.store(true, std::memory_order_release);
  });

  auto consumer = [&]() {
    while (not done.load(std::memory_order_acquire)) {
      const std::size_t k = latest_written.load(std::memory_order_acquire);
      if (k == 0) {
        continue;
      }
      if (auto r = ring.read(k)) {
        // If the read succeeded for key k, the payload must be exactly what the writer stored for k.
        ASSERT_EQ(r->key_copy, k);
        ASSERT_EQ(r->value, static_cast<int>(k * 2));
      }
    }
  };

  std::thread c1(consumer);
  std::thread c2(consumer);
  std::thread c3(consumer);

  producer.join();
  c1.join();
  c2.join();
  c3.join();
}
