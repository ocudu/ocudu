// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/messages/tx_precoding_and_beamforming_pdu.h"

namespace fmt {

/// \brief Custom formatter for \c ocudu::fapi::tx_precoding_and_beamforming_pdu::prg_precoding.
///
/// It formats the complete field name, as the name depends on the alternative that the PRG precoding holds.
template <>
struct formatter<ocudu::fapi::tx_precoding_and_beamforming_pdu::prg_precoding> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::tx_precoding_and_beamforming_pdu::prg_precoding& prg, FormatContext& ctx) const
  {
    if (std::holds_alternative<std::monostate>(prg)) {
      return format_to(ctx.out(), "pm_index=na");
    }

    if (const auto* index = std::get_if<ocudu::fapi::precoding_matrix_index>(&prg)) {
      return format_to(ctx.out(), "pm_index={}", *index);
    }

    const auto& weights = std::get<ocudu::fapi::prg_precoding_weights>(prg);

    return format_to(ctx.out(), "pm_weights={}x{}", weights.get_nof_layers(), weights.get_nof_ports());
  }
};

} // namespace fmt
