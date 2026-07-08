// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/ntn.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/format/delimited_formatter.h"
#include <chrono>
#include <variant>

namespace ocudu {
namespace ocudu_ntn {

/// Structure containing assistance information for formatting.
struct assistance_info_wrapper {
  slot_point                            si_window_start;
  slot_point                            si_window_end;
  slot_point                            epoch_slot;
  std::chrono::system_clock::time_point epoch_time;
  std::optional<ta_info_t>              ta_info;
  ntn_ephemeris_info_t                  ephemeris_info;
};

} // namespace ocudu_ntn
} // namespace ocudu

namespace fmt {

/// \brief Custom formatter for \c assistance_info_wrapper.
template <>
struct formatter<ocudu::ocudu_ntn::assistance_info_wrapper> {
  ocudu::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::ocudu_ntn::assistance_info_wrapper& info, FormatContext& ctx) const
  {
    // Format SIB19 update information.
    helper.format_always(ctx, "si_window={}-{}", info.si_window_start, info.si_window_end);
    helper.format_always(ctx, "epoch_slot={}", info.epoch_slot);
    helper.format_always(ctx, "epoch_time={:%T}", info.epoch_time);

    // Format TA-info.
    if (info.ta_info) {
      helper.format_if_verbose(ctx, "ta_common={:.3f}us", info.ta_info->ta_common);
      helper.format_if_verbose(ctx, "ta_common_drift={:.3f}us/s", info.ta_info->ta_common_drift);
      helper.format_if_verbose(ctx, "ta_common_drift_variant={:.3f}us/s2", info.ta_info->ta_common_drift_variant);
      if (info.ta_info->ta_common_offset) {
        helper.format_if_verbose(ctx, "ta_common_offset={:.3f}us", *info.ta_info->ta_common_offset);
      }
    }

    // Format ephemeris information.
    if (const auto* ecef = std::get_if<ocudu::ecef_coordinates_t>(&info.ephemeris_info)) {
      helper.format_if_verbose(ctx, "ephemeris_type=ecef");
      helper.format_if_verbose(
          ctx, "pos_x={:.3f}m pos_y={:.3f}m pos_z={:.3f}m", ecef->position_x, ecef->position_y, ecef->position_z);
      helper.format_if_verbose(ctx,
                               "vel_x={:.3f}m/s vel_y={:.3f}m/s vel_z={:.3f}m/s",
                               ecef->velocity_vx,
                               ecef->velocity_vy,
                               ecef->velocity_vz);
    } else if (const auto* orbital = std::get_if<ocudu::orbital_coordinates_t>(&info.ephemeris_info)) {
      helper.format_if_verbose(ctx, "ephemeris_type=orbital");
      helper.format_if_verbose(ctx, "semi_major_axis={:.3f}m", orbital->semi_major_axis);
      helper.format_if_verbose(ctx, "eccentricity={:.6f}", orbital->eccentricity);
      helper.format_if_verbose(ctx, "periapsis={:.3f}rad", orbital->periapsis);
      helper.format_if_verbose(ctx, "longitude={:.3f}rad", orbital->longitude);
      helper.format_if_verbose(ctx, "mean_anomaly={:.3f}rad", orbital->mean_anomaly);
      helper.format_if_verbose(ctx, "inclination={:.3f}rad", orbital->inclination);
    }

    return ctx.out();
  }
};

} // namespace fmt
