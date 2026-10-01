// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <array>
#include <type_traits>

namespace ocudu {
namespace detail {

template <typename U>
struct is_std_array : std::false_type {};
template <typename U, std::size_t N>
struct is_std_array<std::array<U, N>> : std::true_type {};

} // namespace detail
} // namespace ocudu
