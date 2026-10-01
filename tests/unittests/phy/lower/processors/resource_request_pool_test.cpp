// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "resource_request_pool.h"
#include <atomic>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

using namespace ocudu;

/// Utility function to create a slot point with a given system slot and default numerology.
static slot_point create_slot(unsigned count)
{
  return {to_numerology_value(subcarrier_spacing::kHz15), count};
}

namespace {

// Move-only resource carrying a unique identifier, mirroring the real move-only payloads (e.g. slot_result). A default
// constructed resource (null pointer) represents an empty request.
using test_resource = std::unique_ptr<unsigned>;
using pool_type     = resource_request_pool<test_resource>;
using request_type  = pool_type::request_type;

} // namespace

/// Popping an empty slot returns an empty request.
TEST(resource_request_pool, pop_empty_slot_returns_empty)
{
  pool_type pool;
  auto      popped = pool.exchange(request_type{.slot = create_slot(3), .resource = {}});
  EXPECT_FALSE(popped.resource);
}

// Pushing into an empty slot returns empty; the consumer then pops the pushed resource back.
TEST(resource_request_pool, push_then_pop_returns_pushed_resource)
{
  pool_type pool;
  auto      slot = create_slot(3);

  auto overwritten = pool.exchange(request_type{.slot = slot, .resource = std::make_unique<unsigned>(42)});
  EXPECT_FALSE(overwritten.resource);

  auto popped = pool.exchange(request_type{.slot = slot, .resource = {}});
  ASSERT_TRUE(popped.resource);
  EXPECT_EQ(*popped.resource, 42U);
  EXPECT_EQ(popped.slot, slot);
}

// Pushing over a stored request returns the previously stored one.
TEST(resource_request_pool, overwrite_returns_previous_request)
{
  pool_type pool;
  auto      slot = create_slot(5);

  EXPECT_FALSE(pool.exchange(request_type{.slot = slot, .resource = std::make_unique<unsigned>(1)}).resource);

  request_type overwritten = pool.exchange(request_type{.slot = slot, .resource = std::make_unique<unsigned>(2)});
  ASSERT_TRUE(overwritten.resource);
  EXPECT_EQ(*overwritten.resource, 1U);

  request_type popped = pool.exchange(request_type{.slot = slot, .resource = {}});
  ASSERT_TRUE(popped.resource);
  EXPECT_EQ(*popped.resource, 2U);
}

// Slots whose system slots differ by the pool size map to the same storage, while distinct indices are independent.
TEST(resource_request_pool, requests_are_indexed_modulo_pool_size)
{
  pool_type pool;

  EXPECT_FALSE(
      pool.exchange(request_type{.slot = create_slot(1U), .resource = std::make_unique<unsigned>(10)}).resource);

  // Same index as slot 1 (1 + pool_size): overwrites it and returns the stored request.
  request_type overwritten = pool.exchange(request_type{.slot     = create_slot(1U + pool.get_request_array_size()),
                                                        .resource = std::make_unique<unsigned>(20)});
  ASSERT_TRUE(overwritten.resource);
  EXPECT_EQ(*overwritten.resource, 10U);

  // A different index is unaffected by the writes above.
  EXPECT_FALSE(
      pool.exchange(request_type{.slot = create_slot(2U), .resource = std::make_unique<unsigned>(30)}).resource);
  request_type popped = pool.exchange(request_type{.slot = create_slot(2U), .resource = {}});
  ASSERT_TRUE(popped.resource);
  EXPECT_EQ(*popped.resource, 30U);
}

// Stress test: under concurrent multi-producer pushes and single-consumer pops, every request is accounted for exactly
// once - either popped by the consumer or handed back to a caller (the overwritten request, or its own request when
// dropped on a collision). Nothing is lost or duplicated. Run under TSAN to also exercise the lock-free guard for
// races.
TEST(resource_request_pool, concurrent_access_conserves_all_requests)
{
  static constexpr unsigned nof_producers       = 4;
  static constexpr unsigned requests_per_thread = 20000;
  static constexpr unsigned total_requests      = nof_producers * requests_per_thread;

  pool_type pool;

  unsigned                           pool_size = pool.get_request_array_size();
  std::atomic<bool>                  stop_consumer{false};
  std::vector<std::vector<unsigned>> produced_returns(nof_producers);
  std::vector<unsigned>              consumed;
  consumed.reserve(total_requests);

  // Consumer: continuously sweeps every slot index, recording the ids it pops.
  std::thread consumer([&]() {
    while (!stop_consumer.load(std::memory_order_relaxed)) {
      for (unsigned idx = 0; idx != pool_size; ++idx) {
        request_type popped = pool.exchange({create_slot(idx), {}});
        if (popped.resource) {
          consumed.push_back(*popped.resource);
        }
      }
    }
  });

  // Producers: each pushes a disjoint range of ids, recording any non-empty request handed back (an overwritten
  // request, or its own id when dropped on a collision). The slot maps the id onto a pool index to create contention.
  std::vector<std::thread> producers;
  producers.reserve(nof_producers);
  for (unsigned p = 0; p != nof_producers; ++p) {
    producers.emplace_back([&, p]() {
      const unsigned base = p * requests_per_thread;
      for (unsigned i = 0; i != requests_per_thread; ++i) {
        const unsigned id   = base + i;
        request_type   back = pool.exchange({create_slot(id % pool_size), std::make_unique<unsigned>(id)});
        if (back.resource) {
          produced_returns[p].push_back(*back.resource);
        }
      }
    });
  }

  for (std::thread& producer : producers) {
    producer.join();
  }
  stop_consumer.store(true, std::memory_order_relaxed);
  consumer.join();

  // Drain whatever is still stored in the pool. This runs uncontended, so every stored request is recovered.
  for (unsigned idx = 0; idx != pool_size; ++idx) {
    request_type popped = pool.exchange({create_slot(idx), {}});
    if (popped.resource) {
      consumed.push_back(*popped.resource);
    }
  }

  // Every id must have been observed exactly once across the consumer and all producers.
  std::vector<unsigned> counts(total_requests, 0);
  for (unsigned id : consumed) {
    ASSERT_LT(id, total_requests);
    ++counts[id];
  }
  for (const std::vector<unsigned>& returns : produced_returns) {
    for (unsigned id : returns) {
      ASSERT_LT(id, total_requests);
      ++counts[id];
    }
  }

  for (unsigned id = 0; id != total_requests; ++id) {
    EXPECT_EQ(counts[id], 1U) << "id=" << id << " was observed " << counts[id] << " time(s)";
  }
}
