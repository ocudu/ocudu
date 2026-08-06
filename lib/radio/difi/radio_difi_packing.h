// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>

namespace ocudu {

///
/// Big-endian packing helpers shared by the DIFI packet builders.
///

inline void pack_u32(uint8_t* dst, uint32_t v)
{
  v = htonl(v);
  std::memcpy(dst, &v, 4);
}

inline void pack_u64(uint8_t* dst, uint64_t v)
{
  pack_u32(dst, static_cast<uint32_t>(v >> 32));
  pack_u32(dst + 4, static_cast<uint32_t>(v & 0xffffffffU));
}

inline void pack_i64(uint8_t* dst, int64_t v)
{
  pack_u64(dst, static_cast<uint64_t>(v));
}

} // namespace ocudu
