// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <type_traits>

namespace ocudu {

/// Convert an enum type to its respective underlying integer type.
template <typename Enum>
auto to_value(Enum e) -> std::underlying_type_t<Enum>
{
  return static_cast<std::underlying_type_t<Enum>>(e);
}

/// Convert an underlying integer type to an enum type.
template <typename Enum>
Enum to_enum(std::underlying_type_t<Enum> e)
{
  return static_cast<std::underlying_type_t<Enum>>(e);
}

} // namespace ocudu
