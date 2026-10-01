// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstddef>
#include <iterator>

namespace ocudu::views {

/// Range over the sequence of consecutive values [from, to).
template <typename T>
class iota_view
{
public:
  using value_type = T;

  class iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void;
    using reference         = T;

    explicit iterator(T val_) noexcept : val(val_) {}

    T operator*() const noexcept { return val; }

    iterator& operator++() noexcept
    {
      ++val;
      return *this;
    }

    bool operator==(const iterator& other) const noexcept { return val == other.val; }
    bool operator!=(const iterator& other) const noexcept { return val != other.val; }

  private:
    T val;
  };

  iota_view(T from_, T to_) noexcept : from(from_), to(to_) {}

  iterator begin() const noexcept { return iterator(from); }
  iterator end() const noexcept { return iterator(to); }

private:
  T from;
  T to;
};

/// Creates a range over the sequence of consecutive values [from, to).
template <typename T>
iota_view<T> iota(T from, T to)
{
  return {from, to};
}

} // namespace ocudu::views
