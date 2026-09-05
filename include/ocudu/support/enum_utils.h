// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <type_traits>

namespace ocudu {

/// Convert an enum type to its respective underlying integer type.
/// \remark Implements std::to_underlying (C++23) while the codebase has not adopted C++23 yet.
template <typename Enum>
constexpr auto to_underlying(Enum e) -> std::underlying_type_t<Enum>
{
  return static_cast<std::underlying_type_t<Enum>>(e);
}

/// Convert an underlying integer type to an enum type.
template <typename Enum>
constexpr Enum to_enum(std::underlying_type_t<Enum> e)
{
  return static_cast<Enum>(e);
}

} // namespace ocudu
