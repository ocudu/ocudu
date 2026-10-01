// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstddef>

namespace ocudu {
namespace detail {

template <typename IdType>
struct cast_to_size_operator {
  constexpr std::size_t get_index(const IdType& t) const { return static_cast<std::size_t>(t); }
  constexpr IdType      get_id(std::size_t idx) const { return static_cast<IdType>(idx); }
};

} // namespace detail
} // namespace ocudu
