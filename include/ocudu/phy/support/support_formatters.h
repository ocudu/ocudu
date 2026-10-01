// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Support formatters used to log information related to the PHY layer.

#pragma once

#include "fmt/base.h"
#include <chrono>

namespace fmt {

/// Custom formatter for \c std::chrono::nanoseconds.
template <>
struct formatter<std::chrono::nanoseconds> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(std::chrono::nanoseconds nanoseconds, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "t={:.1f}us", static_cast<float>(nanoseconds.count()) * 1e-3F);
  }
};

} // namespace fmt
