// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fmt/base.h"

namespace ocudu {

/// SDAP header options for UL
enum class sdap_hdr_ul_cfg { present = 0, absent };

/// SDAP header options for DL
enum class sdap_hdr_dl_cfg { present = 0, absent };

/// Configurable parameters for SDAP mapping.
/// Based on SDAP-Config defined in E1AP specification (TS 38.463 clause 9.3.1.39),
/// as that is the simplest configuration defined in the standard.
///
/// Other layers (such as RRC and SDAP) can define their own configs, re-using
/// this existing config and adding their own extra configuration as required.
struct sdap_ran_config {
  bool            default_drb = false;
  sdap_hdr_ul_cfg header_ul   = sdap_hdr_ul_cfg::absent;
  sdap_hdr_dl_cfg header_dl   = sdap_hdr_dl_cfg::absent;
};

inline const char* format_as(sdap_hdr_ul_cfg hdr_cfg)
{
  static constexpr const char* options[] = {"present", "absent"};
  return options[static_cast<unsigned>(hdr_cfg)];
}

inline const char* format_as(sdap_hdr_dl_cfg hdr_cfg)
{
  static constexpr const char* options[] = {"present", "absent"};
  return options[static_cast<unsigned>(hdr_cfg)];
}

} // namespace ocudu

// Formatters
namespace fmt {

// SDAP config
template <>
struct formatter<ocudu::sdap_ran_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::sdap_ran_config cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "default_drb={} hdr_ul={} hdr_dl={}", cfg.default_drb, cfg.header_ul, cfg.header_dl);
  }
};

} // namespace fmt
