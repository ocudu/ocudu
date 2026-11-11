// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Helpers shared by the sources of the ASN.1 runtime.

#pragma once

#include <cstdint>

namespace asn1 {
namespace detail {

/// Number of bits needed to represent an unsigned value.
inline uint32_t bit_width(uint64_t v)
{
  return v == 0 ? 0 : 64 - static_cast<uint32_t>(__builtin_clzll(v));
}

/// Number of bits needed to encode one of n values, i.e. ceil(log2(n)).
inline uint32_t ceil_log2(uint64_t n)
{
  return n <= 1 ? 0 : bit_width(n - 1);
}

} // namespace detail
} // namespace asn1
