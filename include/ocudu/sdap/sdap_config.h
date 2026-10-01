// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/sdap/sdap_config.h"
#include "fmt/base.h"

namespace ocudu {

/// Configurable parameters an SDAP entity.
/// Extra configuration parameters specific to the SDAP can be added here.
struct sdap_config : sdap_ran_config {};

} // namespace ocudu

// Formatters
namespace fmt {

// SDAP config
template <>
struct formatter<ocudu::sdap_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::sdap_config cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}", static_cast<ocudu::sdap_ran_config>(cfg));
  }
};

} // namespace fmt
