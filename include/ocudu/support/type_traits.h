// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <type_traits>

namespace ocudu {

namespace detail {

// To allow ADL with custom begin/end.
using std::begin;
using std::end;

template <typename T>
auto is_iterable_impl(int)
    -> decltype(begin(std::declval<T&>()) != end(std::declval<T&>()),   // begin/end and operator !=
                void(),                                                 // Handle evil operator ,
                ++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
                void(*begin(std::declval<T&>())),                       // operator*
                std::true_type{});

template <typename T>
std::false_type is_iterable_impl(...);

} // namespace detail

template <typename T>
constexpr bool is_iterable = decltype(detail::is_iterable_impl<T>(0))::value;

} // namespace ocudu
