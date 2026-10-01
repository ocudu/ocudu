// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <array>
#include <cstddef>

namespace ocudu {

/// \brief Helper class to safely access elements of a std::array.
///
/// Protects from out-of-bounds access by applying modulo of its length when using the [] operator for element access.
template <typename T, std::size_t N>
class circular_array
{
  std::array<T, N> data;

public:
  using iterator       = T*;
  using const_iterator = const T*;

  circular_array() = default;
  circular_array(const T& value) { data.fill(value); }

  T&       operator[](std::size_t pos) { return data[pos % N]; }
  const T& operator[](std::size_t pos) const { return data[pos % N]; }

  T*       begin() { return data.begin(); }
  const T* begin() const { return data.begin(); }

  T*       end() { return data.end(); }
  const T* end() const { return data.end(); }

  size_t size() const { return N; }
};

} // namespace ocudu
