// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pusch/pusch_tpmi_select.h"
#include "ocudu/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c pusch_tpmi_select_info.
template <>
struct formatter<ocudu::pusch_tpmi_select_info> {
public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::pusch_tpmi_select_info& context, FormatContext& ctx) const
  {
    unsigned max_nof_layers = context.get_max_nof_layers();
    for (unsigned i_nof_layers = 1; i_nof_layers <= max_nof_layers; ++i_nof_layers) {
      const auto& info = context.get_tpmi_select(i_nof_layers);
      helper.format_always(ctx,
                           "{}_layer: tpmi={} sinr={::+.1f}dB",
                           i_nof_layers,
                           info.tpmi,
                           ocudu::span<const float>(info.sinr_dB_layer));
    }

    return ctx.out();
  }

private:
  ocudu::delimited_formatter helper;
};

} // namespace fmt
