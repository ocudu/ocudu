// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "test_rng_seed.h"
#include "ocudu/support/error_handling.h"
#include <atomic>

using namespace ocudu;

namespace {

using test_counter_type = uint64_t;

/// Integer mixing function to turn an integer into a well-scrambled bit output.
constexpr uint32_t split_mix_32(uint32_t x)
{
  x += 0x9e3779b9u;
  x = (x ^ (x >> 16)) * 0x85ebca6bu;
  x = (x ^ (x >> 13)) * 0xc2b2ae35u;
  return x ^ (x >> 16);
}

/// First step in the iter seed generation.
constexpr uint32_t scramble_base_seed_step1(uint32_t base_seed, uint32_t iter)
{
  return base_seed ^ (iter * 0x9e3779b9u);
}

/// Function used to generate iteration seeds of the base seed and iteration counter.
constexpr uint32_t generate_iter_seed(uint32_t base_seed, uint32_t iter)
{
  return split_mix_32(scramble_base_seed_step1(base_seed, iter));
}

/// Type that holds the state of the random seed used in tests.
struct seed_state {
  /// Sentinel value for \c test_counter in uninitialized state.
  static constexpr test_counter_type invalid_test_counter = std::numeric_limits<test_counter_type>::max();

  /// Global base random seed that can be either set via command line (--gtest_random_seed=) or automatically generated.
  uint32_t base_seed = 0;
  /// \brief Global counter value to track the start of new tests and whether the random generator state needs to be
  /// rewinded back to iter_seed.
  std::atomic<test_counter_type> test_counter = invalid_test_counter;
  /// Current iteration random seed. This seed is a product of the base_seed and the last test iteration index.
  std::atomic<uint32_t> iter_seed = generate_iter_seed(0, 0);
};

/// Global state of the random seed.
seed_state state;

} // namespace

void test_rng::init_base_seed(uint32_t base_seed_)
{
  report_error_if_not(state.base_seed == 0, "Reinitialization of base seed not supported");
  state.base_seed = base_seed_;
  advance_iter_seed(0);
}

uint32_t test_rng::base_seed()
{
  return state.base_seed;
}

uint32_t test_rng::seed()
{
  return state.iter_seed.load(std::memory_order_acquire);
}

std::mt19937& test_rng::tls_gen()
{
  thread_local std::mt19937      rng(seed());
  thread_local test_counter_type last_counter{0};

  // Fetch global test counter.
  test_counter_type cur_test_counter = state.test_counter.load(std::memory_order_acquire);
  report_fatal_error_if_not(cur_test_counter != seed_state::invalid_test_counter,
                            "OCUDUTestEnvironment called before being setup. Are you trying to generate random "
                            "parameters before main() is called?");
  if (cur_test_counter != last_counter) {
    // There was an update the random seed test counter.

    // We save the new counter update.
    last_counter = cur_test_counter;

    // Rewind generator state back to original iter_seed.
    rng.seed(seed());
  }

  return rng;
}

/// Called each time the seed is updated.
void test_rng::advance_iter_seed(uint32_t iteration_counter)
{
  // Note: Apply golden ratio hashing number to spread the iteration index before combining it with base_seed.
  auto new_seed = generate_iter_seed(state.base_seed, iteration_counter);
  auto old_seed = state.iter_seed.exchange(new_seed, std::memory_order_acq_rel);

  if (new_seed != old_seed) {
    // This also marks seed as initialized.
    rewind_rng();

    // Note: we cast seed to int, because that's what gtest_random_seed uses.
    fmt::print("[   TEST   ] OCUDU Random Seed: base_seed={}, iteration={} -> iter_seed={}.\n",
               static_cast<int32_t>(state.base_seed),
               static_cast<int32_t>(iteration_counter),
               static_cast<int32_t>(new_seed));
  }
}

void test_rng::rewind_rng()
{
  // On each test start, we bump the test counter.
  state.test_counter.fetch_add(1, std::memory_order_acq_rel);
}

uint32_t test_rng::compute_base_seed_at_iter0(uint32_t iter)
{
  return scramble_base_seed_step1(state.base_seed, iter);
}
