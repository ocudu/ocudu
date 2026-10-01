// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "fmt/args.h"
#include "fmt/ranges.h"
#include <array>
#include <iterator>
#include <type_traits>
#include <vector>

namespace ocudulog {

/// Type trait to indicate if a type that is going to be passed through a log channel is unsafe to be copied with the
/// default implementation and requires a user defined copy implementation.
template <typename T>
struct copy_loggable_type {
  static constexpr bool is_copyable = true;
  // static void copy (fmt::dynamic_format_arg_store<fmt::format_context>* store, <user-defined-type> a)
};

/// \brief Owns a copy of the range captured by an \c fmt::join_view, keeping its separator as a view.
///
/// \c fmt::join_view only stores iterators into the caller's range, so it cannot be handed as-is to a logger backend
/// that may render the log entry asynchronously, after the original range has gone out of scope. This type is what
/// \c ocudulog::copy_loggable_type copies a \c fmt::join_view into instead. The separator is kept as a
/// \c fmt::basic_string_view, same as \c fmt::join_view itself: this assumes, as \c fmt::join callers virtually
/// always do, that the separator is a string literal or otherwise outlives the log entry.
template <typename Container, typename Char = char>
struct owning_join_view {
  Container                    values;
  fmt::basic_string_view<Char> sep;
};

/// Container of up to \c MAX_SIZE elements, stored inline. Copying it only touches the elements in use.
/// \note It works like a static_vector, but the static_vector dependency is avoided here.
template <typename T, size_t MAX_SIZE>
struct owning_join_inline_array {
  owning_join_inline_array() = default;
  owning_join_inline_array(const owning_join_inline_array& other) : nof_values(other.nof_values) { copy_values(other); }
  owning_join_inline_array& operator=(const owning_join_inline_array& other)
  {
    nof_values = other.nof_values;
    copy_values(other);
    return *this;
  }

  void push_back(const T& value) { values[nof_values++] = value; }

  const T* begin() const { return values.data(); }
  const T* end() const { return values.data() + nof_values; }

private:
  void copy_values(const owning_join_inline_array& other)
  {
    for (unsigned i = 0; i != nof_values; ++i) {
      values[i] = other.values[i];
    }
  }

  /// Inline storage, of which only the first nof_values elements hold a value.
  std::array<T, MAX_SIZE> values;
  /// Number of elements in use.
  unsigned nof_values = 0;
};

/// \brief Owns a copy of the tuple captured by an \c fmt::tuple_join_view, keeping its separator as a view.
///
/// \c fmt::tuple_join_view is even more directly dangerous than \c fmt::join_view: it stores a plain reference to the
/// caller's tuple, so it dangles the moment that tuple goes out of scope, well before an async logging backend gets a
/// chance to render it. This type is what \c ocudulog::copy_loggable_type copies a \c fmt::tuple_join_view into
/// instead. As with \c owning_join_view, the separator is kept as a view, not owned.
template <typename Tuple, typename Char = char>
struct owning_tuple_join_view {
  Tuple                        values;
  fmt::basic_string_view<Char> sep;
};

/// Type trait specialization to instruct the logger to use a user defined copy implementation as it is unsafe to
/// directly copy an \c fmt::join_view: it only stores iterators into the caller's range, which is not guaranteed to
/// outlive the point where the logging backend asynchronously renders this entry. See \c ocudu::owning_join_view for
/// the separator's lifetime caveat, which this copy does not change.
template <typename It, typename Sentinel, typename Char>
struct copy_loggable_type<fmt::join_view<It, Sentinel, Char>> {
  static constexpr bool is_copyable = false;

  using value_type = std::remove_cv_t<typename std::iterator_traits<It>::value_type>;

  /// Byte budget of the inline storage.
  static constexpr size_t MAX_INLINE_BYTES = 128;

  /// Number of elements that fit in the inline storage.
  static constexpr size_t MAX_INLINE_ELEMENTS = MAX_INLINE_BYTES / sizeof(value_type);

  /// Inline storage type.
  using inline_storage = owning_join_inline_array<value_type, MAX_INLINE_ELEMENTS>;

  /// Set to true when the range can be kept inline, instead of on a heap allocated vector.
  static constexpr bool fits_inline = (MAX_INLINE_ELEMENTS != 0) and std::is_trivially_copyable_v<value_type> and
                                      std::is_default_constructible_v<value_type>;

  /// Set to true when the range is randomly accessible, so its length is known in constant time.
  static constexpr bool has_random_access =
      std::is_same_v<It, Sentinel> and
      std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<It>::iterator_category>;

  static void copy(fmt::dynamic_format_arg_store<fmt::format_context>* store, fmt::join_view<It, Sentinel, Char> j)
  {
    // This copy runs on the thread that emits the log entry, so short ranges, which are the common case, are kept
    // inline to keep that thread away from the allocator. Counting the range first picks the storage before any
    // element is copied, so no element is ever copied twice.
    if constexpr (fits_inline and has_random_access) {
      if (static_cast<size_t>(std::distance(j.begin, j.end)) <= MAX_INLINE_ELEMENTS) {
        push_inline(store, j);
        return;
      }
    }
    push_on_heap(store, j);
  }

private:
  /// Stores a range that is known to fit in the inline storage.
  static void push_inline(fmt::dynamic_format_arg_store<fmt::format_context>* store,
                          fmt::join_view<It, Sentinel, Char>                  j)
  {
    // Filled in place, so that the only copy of the inline storage is the one the argument store makes, and that copy
    // is sized by the number of elements held, not by the storage capacity.
    owning_join_view<inline_storage, Char> owned;
    owned.sep = j.sep;
    for (It it = j.begin; it != j.end; ++it) {
      owned.values.push_back(*it);
    }
    store->push_back(owned);
  }

  /// Stores a range on a heap allocated vector.
  static void push_on_heap(fmt::dynamic_format_arg_store<fmt::format_context>* store,
                           fmt::join_view<It, Sentinel, Char>                  j)
  {
    if constexpr (std::is_same_v<It, Sentinel>) {
      store->push_back(owning_join_view<std::vector<value_type>, Char>{std::vector<value_type>(j.begin, j.end), j.sep});
    } else {
      // The iterator and the sentinel differ in type, so the range has to be appended one element at a time.
      std::vector<value_type> values;
      for (It it = j.begin; it != j.end; ++it) {
        values.push_back(*it);
      }
      store->push_back(owning_join_view<std::vector<value_type>, Char>{std::move(values), j.sep});
    }
  }
};

/// Type trait specialization to instruct the logger to use a user defined copy implementation as it is unsafe to
/// directly copy an \c fmt::tuple_join_view: it only stores a reference to the caller's tuple, which is not
/// guaranteed to outlive the point where the logging backend asynchronously renders this entry. See
/// \c ocudu::owning_tuple_join_view for the separator's lifetime caveat, which this copy does not change.
template <typename Char, typename Tuple>
struct copy_loggable_type<fmt::tuple_join_view<Char, Tuple>> {
  static constexpr bool is_copyable = false;

  static void copy(fmt::dynamic_format_arg_store<fmt::format_context>* store, fmt::tuple_join_view<Char, Tuple> j)
  {
    store->push_back(owning_tuple_join_view<Tuple, Char>{Tuple(j.tuple), j.sep});
  }
};

} // namespace ocudulog

namespace fmt {

/// \brief Custom formatter for \c ocudu::owning_join_view<Container, Char>.
///
/// Reconstructs the \c fmt::join_view that was originally captured, over the owned copy of its range and its
/// (viewed, not owned) separator, and delegates to fmt's own join formatter so that per-element format specifiers
/// keep working as with a plain \c fmt::join call.
template <typename Container, typename Char>
struct formatter<ocudulog::owning_join_view<Container, Char>, Char> {
private:
  using const_iterator = decltype(std::declval<const Container&>().begin());
  using join_type      = join_view<const_iterator, const_iterator, Char>;

  formatter<join_type, Char> underlying;

public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return underlying.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudulog::owning_join_view<Container, Char>& v, FormatContext& ctx) const
  {
    return underlying.format(join_type(v.values.begin(), v.values.end(), v.sep), ctx);
  }
};

/// \brief Custom formatter for \c ocudu::owning_tuple_join_view<Tuple, Char>.
///
/// Reconstructs the \c fmt::tuple_join_view that was originally captured, referencing the owned copy of the tuple
/// (kept alive for the lifetime of \p v) and its (viewed, not owned) separator, and delegates to fmt's own tuple-join
/// formatter so that per-element formatting behaves as with a plain \c fmt::join call over a tuple.
template <typename Tuple, typename Char>
struct formatter<ocudulog::owning_tuple_join_view<Tuple, Char>, Char> {
private:
  formatter<tuple_join_view<Char, Tuple>, Char> underlying;

public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return underlying.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudulog::owning_tuple_join_view<Tuple, Char>& v, FormatContext& ctx) const
  {
    return underlying.format(tuple_join_view<Char, Tuple>(v.values, v.sep), ctx);
  }
};

} // namespace fmt
