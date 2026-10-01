// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/support/math/pow2_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include <vector>

namespace ocudu {

/// \brief Circular vector container.
/// \tparam T Type of elements.
/// \tparam ForcePower2Size If true, the container size is rounded up to the closest power of 2, and indices are wrapped
/// with a bit-wise and instead of a modulo.
/// \tparam VectorContainer Underlying vector implementation. Must provide the same interface as std::vector.
template <typename T, bool ForcePower2Size = false, typename VectorContainer = std::vector<T>>
class circular_vector
{
public:
  using value_type     = T;
  using iterator       = typename VectorContainer::iterator;
  using const_iterator = typename VectorContainer::const_iterator;

  circular_vector() = default;
  circular_vector(size_t n) : data(adjust_size(n)) {}
  circular_vector(size_t n, const T& value) : data(adjust_size(n), value) {}

  auto begin() { return data.begin(); }
  auto begin() const { return data.begin(); }
  auto end() { return data.end(); }
  auto end() const { return data.end(); }

  size_t size() const { return data.size(); }
  bool   empty() const { return data.empty(); }

  T&       operator[](size_t pos) { return data[wrap(pos)]; }
  const T& operator[](size_t pos) const { return data[wrap(pos)]; }

  void resize(size_t n) { data.resize(adjust_size(n)); }
  void resize(size_t n, const T& value) { data.resize(adjust_size(n), value); }

  void reserve(size_t n) { data.reserve(adjust_size(n)); }
  void push_back(const T& value)
  {
    static_assert(not ForcePower2Size, "push_back would break the power of 2 size invariant");
    data.push_back(value);
  }
  void push_back(T&& value)
  {
    static_assert(not ForcePower2Size, "push_back would break the power of 2 size invariant");
    data.push_back(std::move(value));
  }
  template <typename... Args>
  void emplace_back(Args&&... args)
  {
    static_assert(not ForcePower2Size, "emplace_back would break the power of 2 size invariant");
    data.emplace_back(std::forward<Args>(args)...);
  }

  void clear() { data.clear(); }

private:
  /// Maps an unbounded position into a valid container index.
  size_t wrap(size_t pos) const
  {
    ocudu_assert(not empty(), "Access to an empty circular vector");
    if constexpr (ForcePower2Size) {
      // x mod y == x & (y - 1), when y is a power of 2.
      return pos & (size() - 1);
    } else {
      return pos % size();
    }
  }

  /// Rounds the requested size up to the closest power of 2 if required.
  static constexpr size_t adjust_size(size_t n)
  {
    if constexpr (ForcePower2Size) {
      return to_next_pow2(n);
    } else {
      return n;
    }
  }

  VectorContainer data;
};

template <typename T, size_t N, bool ForcePower2Size = false>
using static_circular_vector = circular_vector<T, ForcePower2Size, static_vector<T, N>>;

} // namespace ocudu
