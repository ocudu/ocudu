// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/detail/type_traits.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <array>
#include <iterator>
#include <type_traits>

namespace ocudu {

template <typename T>
class span;

namespace detail {

/// Helper traits used by SFINAE expressions in constructors.

template <typename U>
struct is_span : std::false_type {};
template <typename U>
struct is_span<span<U>> : std::true_type {};

template <typename U>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<U>>;

template <class Container, class U, class = void>
struct is_container_compatible : public std::false_type {};
template <class Container, class U>
struct is_container_compatible<
    Container,
    U,
    std::void_t<
        // Check if the container type has data and size members.
        decltype(std::declval<Container>().data()),
        decltype(std::declval<Container>().size()),
        // Container should not be a span.
        std::enable_if_t<!is_span<remove_cvref_t<Container>>::value, int>,
        // Container should not be a std::array.
        std::enable_if_t<!is_std_array<remove_cvref_t<Container>>::value, int>,
        // Container should not be an array.
        std::enable_if_t<!std::is_array_v<remove_cvref_t<Container>>, int>,
        // Check type compatibility between the contained type and the span type.
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container>().data())> (*)[], U (*)[]>,
            int>>> : public std::true_type {};

} // namespace detail

/// The class template span describes an object that can refer to a contiguous sequence of objects with the first
/// element of the sequence at position zero.
template <typename T>
class span
{
public:
  /// Member types.
  using element_type     = T;
  using value_type       = std::remove_cv_t<T>;
  using size_type        = std::size_t;
  using difference_type  = std::ptrdiff_t;
  using pointer          = element_type*;
  using const_pointer    = const element_type*;
  using reference        = element_type&;
  using const_reference  = const element_type&;
  using iterator         = pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;

  /// Constructs an empty span with data() == nullptr and size() == 0.
  constexpr span() noexcept = default;

  constexpr span(const span&) noexcept        = default;
  span& operator=(const span& other) noexcept = default;

  /// Constructs a span that is a view over the range [ptr, ptr + len).
  constexpr span(pointer ptr_, size_type len_) noexcept : ptr(ptr_), len(len_) {}

  /// Constructs a span that is a view over the range [first, last).
  constexpr span(pointer first, pointer last) noexcept : ptr(first), len(last - first) {}

  /// Constructs a span that is a view over the array arr.
  template <std::size_t N>
  constexpr span(element_type (&arr)[N]) noexcept : ptr(arr), len(N)
  {
  }

  /// Constructs a span that is a view over the array arr.
  template <typename U, std::size_t N, std::enable_if_t<std::is_convertible_v<U (*)[], element_type (*)[]>, int> = 0>
  constexpr span(std::array<U, N>& arr) noexcept : ptr(arr.data()), len(N)
  {
  }

  /// Constructs a span that is a view over the array arr.
  template <typename U,
            std::size_t N,
            std::enable_if_t<std::is_convertible_v<const U (*)[], element_type (*)[]>, int> = 0>
  constexpr span(const std::array<U, N>& arr) noexcept : ptr(arr.data()), len(N)
  {
  }

  /// Constructs a span that is a view over the container c.
  template <typename Container,
            std::enable_if_t<detail::is_container_compatible<Container, element_type>::value, int> = 0>
  constexpr span(Container& container) noexcept : ptr(container.data()), len(container.size())
  {
  }

  /// Constructs a span that is a view over the container c.
  template <typename Container,
            std::enable_if_t<detail::is_container_compatible<const Container, element_type>::value, int> = 0>
  constexpr span(const Container& container) noexcept : ptr(container.data()), len(container.size())
  {
  }

  template <typename U, std::enable_if_t<std::is_convertible_v<U (*)[], element_type (*)[]>, int> = 0>
  constexpr span(const span<U>& other) noexcept : ptr(other.data()), len(other.size())
  {
  }

  /// Returns the number of elements in the span.
  constexpr size_type size() const noexcept { return len; }

  /// Returns the size of the sequence in bytes.
  constexpr size_type size_bytes() const noexcept { return len * sizeof(element_type); }

  /// Checks if the span is empty.
  constexpr bool empty() const noexcept { return size() == 0; }

  /// Returns a reference to the first element in the span.
  /// NOTE: Calling front on an empty span results in undefined behavior.
  reference front() const
  {
    ocudu_assert(!empty(), "Called front() with empty span");
    return *data();
  }

  /// Returns a reference to the last element in the span.
  /// NOTE: Calling back on an empty span results in undefined behavior.
  reference back() const
  {
    ocudu_assert(!empty(), "called back with empty span");
    return *(data() + size() - 1);
  }

  /// Returns a reference to the idx-th element of the sequence.
  /// NOTE: The behavior is undefined if idx is out of range.
  reference operator[](size_type idx) const
  {
    ocudu_assert(idx < len, "Index out of bounds!");
    return ptr[idx];
  }

  /// Returns a pointer to the beginning of the sequence.
  constexpr pointer data() const noexcept { return ptr; }

  /// Returns an iterator to the first element of the span.
  constexpr iterator begin() const noexcept { return data(); }

  /// Returns an iterator to the element following the last element of the span.
  constexpr iterator end() const noexcept { return data() + size(); }

  /// Returns a reverse iterator to the first element of the reversed span.
  constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }

  /// Returns a reverse iterator to the element following the last element of the reversed span.
  constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

  /// Obtains a span that is a view over the first count elements of this span.
  /// NOTE: The behavior is undefined if count > size().
  span<element_type> first(size_type count) const
  {
    ocudu_assert(count <= size(), "Count is out of range");
    return subspan(0, count);
  }

  /// Obtains a span that is a view over the last count elements of this span.
  /// NOTE: The behavior is undefined if count > size().
  span<element_type> last(size_type count) const
  {
    ocudu_assert(count <= size(), "Count is out of range");
    return subspan(size() - count, count);
  }

  /// Obtains a span that is a view over the count elements of this span starting at offset offset.
  span<element_type> subspan(size_type offset, size_type count) const
  {
    ocudu_assert(offset <= size(), "Offset is out of bounds!");
    ocudu_assert(count <= size() - offset, "Offset plus size is out of bounds!");
    return {data() + offset, count};
  }

  /// Returns true if the input span has the same elements as this.
  bool equals(span rhs) const { return std::equal(begin(), end(), rhs.begin(), rhs.end()); }

private:
  pointer   ptr = nullptr;
  size_type len = 0;
};

template <typename T>
inline bool operator==(span<T> lhs, span<T> rhs)
{
  return lhs.equals(rhs);
}

template <typename T>
inline bool operator!=(span<T> lhs, span<T> rhs)
{
  return not lhs.equals(rhs);
}

template <typename T>
using const_span = span<const T>;

} // namespace ocudu
