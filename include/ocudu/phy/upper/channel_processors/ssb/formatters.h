// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "ocudu/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c ssb_processor::pdu_t.
template <>
struct formatter<ocudu::ssb_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  ocudu::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::ssb_processor::pdu_t& pdu, FormatContext& ctx) const
  {
    helper.format_always(ctx, "pci={}", pdu.phys_cell_id);
    helper.format_always(ctx, "ssb_idx={}", pdu.ssb_idx);
    helper.format_always(ctx, "L_max={}", pdu.L_max);
    helper.format_always(ctx, "common_scs={}", scs_to_khz(pdu.common_scs));
    helper.format_always(ctx, "sc_offset={}", pdu.subcarrier_offset.value());
    helper.format_always(ctx, "offset_PointA={}", pdu.offset_to_pointA.value());
    helper.format_always(ctx, "pattern={}", to_string(pdu.pattern_case));

    helper.format_if_verbose(ctx, "beta_pss={:+.1f}dB", ocudu::ssb_pss_to_sss_epre_to_dB(pdu.beta_pss));
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "ports={}", ocudu::span<const uint8_t>(pdu.ports));

    return ctx.out();
  }
};

} // namespace fmt
