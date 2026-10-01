// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#ifdef OCUDU_HAVE_OPENSSL_DTLS

#include "fmt/std.h"
#include <array>
#include <openssl/err.h>

namespace ocudu {

struct openssl_error {
  explicit openssl_error(unsigned long code_) : code(code_) {}
  unsigned long code;
};

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::openssl_error> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::openssl_error& e, FormatContext& ctx) const
  {
    std::array<char, 256> buf{};
    ERR_error_string_n(e.code, buf.data(), buf.size());
    return format_to(ctx.out(), "{}", buf.data());
  }
};

} // namespace fmt

#endif // OCUDU_HAVE_OPENSSL_DTLS
