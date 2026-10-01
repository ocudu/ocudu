// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <limits>
#include <random>
#include <vector>

namespace ocudu::test_rng {

/// Fetch the seed under use for unit tests. The user can force a seed via --gtest_random_seed= command line option.
uint32_t seed();

/// Fetch thread-local random generator.
std::mt19937& tls_gen();

/// Returns a random integer with uniform distribution within the specified bounds.
template <typename Integer>
Integer uniform_int(Integer min, Integer max)
{
  return std::uniform_int_distribution<Integer>{min, max}(tls_gen());
}
template <typename Integer>
Integer uniform_int()
{
  return uniform_int(std::numeric_limits<Integer>::min(), std::numeric_limits<Integer>::max());
}

/// Returns a random boolean with probability \c p of being true.
inline bool bernoulli(double p = 0.5)
{
  return std::bernoulli_distribution(p)(tls_gen());
}

/// Returns a random floating point value with normal distribution.
template <typename FloatingPoint>
FloatingPoint normal_dist(FloatingPoint mean, FloatingPoint stddev)
{
  return std::normal_distribution<FloatingPoint>(mean, stddev)(tls_gen());
}

/// Return a vector of integers with specified size filled with random values.
template <typename Integer>
std::vector<Integer> vector_of_uniform_ints(size_t sz)
{
  std::uniform_int_distribution<Integer> dist{std::numeric_limits<Integer>::min(), std::numeric_limits<Integer>::max()};
  auto&                                  rng = tls_gen();

  std::vector<Integer> vec(sz);
  for (unsigned i = 0; i != sz; ++i) {
    vec[i] = dist(rng);
  }
  return vec;
}

} // namespace ocudu::test_rng
