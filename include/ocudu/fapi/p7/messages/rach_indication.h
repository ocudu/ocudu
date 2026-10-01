// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "formatter/formatter_helpers.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/fapi/fapi_power_unit.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {
namespace fapi {

/// RACH indication pdu preamble.
struct rach_indication_pdu_preamble {
  uint8_t                        preamble_index;
  std::optional<phy_time_unit>   timing_advance_offset;
  std::optional<fapi_power_unit> preamble_pwr;
  std::optional<float>           preamble_snr_dB;
};

/// RACH indication pdu.
struct rach_indication_pdu {
  uint32_t                                                                      handle = 0;
  uint8_t                                                                       symbol_index;
  uint8_t                                                                       slot_index;
  uint8_t                                                                       ra_index;
  std::optional<fapi_power_unit>                                                avg_rssi;
  std::optional<float>                                                          avg_snr_dB;
  static_vector<rach_indication_pdu_preamble, MAX_PREAMBLES_PER_PRACH_OCCASION> preambles;
};

/// RACH indication message
struct rach_indication {
  slot_point          slot;
  rach_indication_pdu pdu;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::rach_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::rach_indication& msg, FormatContext& ctx) const
  {
    format_to(ctx.out(),
              "RACH.indication slot={} symb_idx={} slot_idx={} ra_index={} nof_preambles={}",
              msg.slot,
              msg.pdu.symbol_index,
              msg.pdu.slot_index,
              msg.pdu.ra_index,
              msg.pdu.preambles.size());

    if (msg.pdu.avg_snr_dB.has_value()) {
      format_to(ctx.out(), " avg_snr={:.1f}dB", *msg.pdu.avg_snr_dB);
    }

    if (msg.pdu.avg_rssi.has_value()) {
      format_to(ctx.out(), " rssi={}", *msg.pdu.avg_rssi);
    }

    // Log the preambles.
    for (const auto& preamble : msg.pdu.preambles) {
      format_to(ctx.out(), "\n\t\t- PREAMBLE index={}", preamble.preamble_index);

      ocudu::fapi::append_time_advance(ctx, preamble.timing_advance_offset, msg.slot.scs());

      if (preamble.preamble_pwr.has_value()) {
        format_to(ctx.out(), " pwr={}", *preamble.preamble_pwr);
      }
      if (preamble.preamble_snr_dB.has_value()) {
        format_to(ctx.out(), " snr={:.1f}dB", *preamble.preamble_snr_dB);
      }
    }

    return ctx.out();
  }
};
} // namespace fmt
