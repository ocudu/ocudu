// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstdint>

namespace ocudu {

/// Checks if alignment is power of 2.
constexpr bool is_alignment_valid(std::size_t alignment)
{
  return alignment > 0 && (alignment & (alignment - 1U)) == 0U;
}

/// Checks whether the pointer is set with the correct alignment.
constexpr bool is_aligned(std::uintptr_t ptr, std::size_t alignment)
{
  return (ptr & (alignment - 1)) == 0;
}
inline bool is_aligned(void* ptr, std::size_t alignment)
{
  return is_aligned(reinterpret_cast<std::uintptr_t>(ptr), alignment);
}

/// \brief Moves the pointer by the given size in bytes.
constexpr void* advance_ptr(void* pos, std::size_t sz)
{
  return static_cast<char*>(pos) + sz;
}

/// Moves the pointer to the next aligned position.
constexpr std::uintptr_t align_next(std::uintptr_t pos, std::size_t alignment)
{
  return (pos + (alignment - 1)) & ~(alignment - 1);
}
inline void* align_next(void* pos, std::size_t alignment)
{
  return reinterpret_cast<void*>(align_next(reinterpret_cast<std::uintptr_t>(pos), alignment));
}

} // namespace ocudu
