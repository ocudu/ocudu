// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/support/format/fmt_basic_parser.h"
#include "fmt/std.h"

namespace fmt {

template <>
struct formatter<ocudu::tdd_ul_dl_pattern> : public basic_parser {
  template <typename FormatContext>
  auto format(const ocudu::tdd_ul_dl_pattern& cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "period={} dl_slots={} dl_symbols={} ul_slots={} ul_symbols={}",
                     cfg.dl_ul_tx_period_nof_slots,
                     cfg.nof_dl_slots,
                     cfg.nof_dl_symbols,
                     cfg.nof_ul_slots,
                     cfg.nof_ul_symbols);
  }
};

template <>
struct formatter<ocudu::tdd_ul_dl_config_common> : public basic_parser {
  template <typename FormatContext>
  auto format(const ocudu::tdd_ul_dl_config_common& cfg, FormatContext& ctx) const
  {
    return format_to(
        ctx.out(), "scs={} pattern1={{{}}} pattern2={{{}}}", fmt::underlying(cfg.ref_scs), cfg.pattern1, cfg.pattern2);
  }
};

} // namespace fmt
