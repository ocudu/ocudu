// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "fmt/format.h"
#include <cstdint>

namespace ocudu {
namespace security {

enum class security_status {
  success = 0,
  success_unprotected,
  buffer_failure,
  engine_failure,
  integrity_failure,
  ciphering_failure
};

inline bool is_success(security_status status)
{
  return status <= security_status::success_unprotected;
}

inline const char* to_string(security_status status)
{
  switch (status) {
    case security_status::success:
      return "success";
    case security_status::success_unprotected:
      return "success_unprotected";
    case security_status::buffer_failure:
      return "buffer_failure";
    case security_status::engine_failure:
      return "engine_failure";
    case security_status::integrity_failure:
      return "integrity_failure";
    case security_status::ciphering_failure:
      return "ciphering_failure";
    default:
      return "invalid";
  }
}

class security_engine_tx
{
public:
  virtual ~security_engine_tx() = default;

  virtual security_status encrypt_and_protect_integrity(byte_buffer& buf, size_t offset, uint32_t count) = 0;
};

class security_engine_rx
{
public:
  virtual ~security_engine_rx() = default;

  virtual security_status decrypt_and_verify_integrity(byte_buffer& buf, size_t offset, uint32_t count) = 0;
};

} // namespace security
} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::security::security_status> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::security::security_status status, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}", to_string(status));
  }
};
} // namespace fmt
