// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "formatter/formatter_helpers.h"
#include "ocudu/fapi/fapi_power_unit.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/srs/srs_channel_matrix.h"
#include <optional>

namespace ocudu {
namespace fapi {

/// Encodes SRS positioning report.
struct srs_positioning_report {
  /// TUL-RTOA as defined in TS 38.215 on section 5.1.
  std::optional<phy_time_unit>   ul_relative_toa;
  std::optional<fapi_power_unit> rsrp;
  /// Azimuth Angle of Arrival, in degrees. Values: [-180, 180).
  std::optional<float> azimuth_aoa_deg;
  /// Zenith Angle of Arrival, in degrees. Values: [0, 180).
  std::optional<float> zenith_aoa_deg;
};

/// SRS indication pdu.
struct srs_indication_pdu {
  uint32_t                              handle = 0;
  rnti_t                                rnti;
  std::optional<phy_time_unit>          timing_advance_offset;
  std::optional<srs_channel_matrix>     matrix;
  std::optional<srs_positioning_report> positioning;
};

/// SRS indication message.
struct srs_indication {
  slot_point         slot;
  srs_indication_pdu pdu;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::srs_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::srs_indication& msg, FormatContext& ctx) const
  {
    format_to(ctx.out(), "SRS.indication slot={} rnti={}", msg.slot, msg.pdu.rnti);

    ocudu::fapi::append_time_advance(ctx, msg.pdu.timing_advance_offset, msg.slot.scs());

    if (msg.pdu.positioning.has_value()) {
      if (msg.pdu.positioning->ul_relative_toa.has_value()) {
        format_to(ctx.out(), " RTOA_s={}", msg.pdu.positioning->ul_relative_toa->to_seconds<>());
      }
      if (msg.pdu.positioning->rsrp.has_value()) {
        format_to(ctx.out(), " RSRP={}", *msg.pdu.positioning->rsrp);
      }
      if (msg.pdu.positioning->azimuth_aoa_deg.has_value()) {
        format_to(ctx.out(), " azimuth_AoA={:.1f}", *msg.pdu.positioning->azimuth_aoa_deg);
      }
      if (msg.pdu.positioning->zenith_aoa_deg.has_value()) {
        format_to(ctx.out(), " zenith_AoA={:.1f}", *msg.pdu.positioning->zenith_aoa_deg);
      }
    }

    if (msg.pdu.matrix.has_value()) {
      format_to(ctx.out(), " With channel matrix");
    }

    return ctx.out();
  }
};
} // namespace fmt
