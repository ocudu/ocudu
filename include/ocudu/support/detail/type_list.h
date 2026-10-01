// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstddef>
#include <type_traits>

namespace ocudu {

/// List of types.
template <typename... Args>
struct type_list {};

/// Get number of Types of type_list.
template <typename... Args>
constexpr std::size_t type_list_size(type_list<Args...> t)
{
  return sizeof...(Args);
}

namespace type_list_helper {

namespace detail {

// type_at_t helpers

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 14)

// Note: Version that leverages builtin __type_pack_element for faster compile times.

template <std::size_t I, typename TypeList>
struct type_at;

template <std::size_t I, typename... Ts>
struct type_at<I, type_list<Ts...>> {
  using type = __type_pack_element<I, Ts...>;
};

#else

template <std::size_t Index, typename List>
struct type_at;

template <std::size_t Index, typename Head, typename... Tail>
struct type_at<Index, type_list<Head, Tail...>> : type_at<Index - 1, type_list<Tail...>> {};

template <typename Head, typename... Tail>
struct type_at<0, type_list<Head, Tail...>> {
  using type = Head;
};

#endif

// concat_t helpers

template <typename... TypeLists>
struct concat;

template <typename... T>
struct concat<type_list<T...>> {
  using type = type_list<T...>;
};

template <typename... T1, typename... T2, typename... Rest>
struct concat<type_list<T1...>, type_list<T2...>, Rest...> {
  using type = typename concat<type_list<T1..., T2...>, Rest...>::type;
};

} // namespace detail

/// Get type relative to a provided Index from a type_list.
template <std::size_t Index, typename... Types>
using type_at_t = typename detail::type_at<Index, Types...>::type;

/// Metafunction that concatenates the types of multiple type_lists.
template <typename... Lists>
using concat_t = typename detail::concat<Lists...>::type;

/// \brief Finds the index of type \c T in the type pack \c Types.
///
/// Uses a fold expression to avoid recursive template instantiation. When T is not found, returns sizeof...(Types).
template <typename T, typename... Types>
inline constexpr size_t type_index_v = []() constexpr noexcept {
  size_t idx = 0;
  (void)((std::is_same_v<T, Types> ? true : (++idx, false)) || ...);
  return idx;
}();

/// \brief Checks whether \c T appears in the type pack \c Types.
template <typename T, typename... Types>
inline constexpr bool contains_v = (std::is_same_v<T, Types> || ...);

} // namespace type_list_helper

} // namespace ocudu
