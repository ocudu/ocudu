// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_config_translators.h"
#include "ocudu/support/error_handling.h"
#include <algorithm>

using namespace ocudu;

unsigned ocudu::add_global_ntn_satellites(span<const ntn_satellite_config>              ntn_satellites,
                                          std::vector<ocudu_ntn::ntn_satellite_config>& out_satellites)
{
  unsigned next_satellite_idx = 0;
  for (const ntn_satellite_config& global_sat : ntn_satellites) {
    if (!global_sat.satellite_idx) {
      report_error("ntn.satellites: satellite_idx has to be provided for a global satellite definition");
    }
    auto& sat                = out_satellites.emplace_back();
    sat.satellite_index      = *global_sat.satellite_idx;
    sat.epoch_timestamp      = global_sat.epoch_timestamp;
    sat.ephemeris_info       = *global_sat.ephemeris_info;
    sat.ntn_gateway_location = global_sat.gateway_location;
    sat.ta_info              = global_sat.ta_info;
    sat.propagator_type      = global_sat.propagator_type;
    // Ensure auto-assigned indices for inline satellite definitions don't collide with global ones.
    if (*global_sat.satellite_idx >= next_satellite_idx) {
      next_satellite_idx = *global_sat.satellite_idx + 1;
    }
  }
  return next_satellite_idx;
}

void ocudu::resolve_ntn_satellite_ref(ntn_satellite_config&                         sat_ref,
                                      std::vector<ocudu_ntn::ntn_satellite_config>& out_satellites,
                                      unsigned&                                     next_satellite_idx,
                                      const std::optional<ta_info_t>&               ta_info,
                                      const std::string&                            err_context)
{
  if (sat_ref.satellite_idx) {
    return;
  }
  if (!sat_ref.epoch_timestamp || !sat_ref.ephemeris_info) {
    report_error("{}: either satellite_idx or inline ephemeris definition (epoch_timestamp and ephemeris_info) must "
                 "be provided",
                 err_context);
  }
  unsigned sat_idx = next_satellite_idx++;
  out_satellites.push_back({sat_idx,
                            sat_ref.epoch_timestamp,
                            *sat_ref.ephemeris_info,
                            sat_ref.gateway_location,
                            ta_info,
                            sat_ref.propagator_type});
  sat_ref.satellite_idx = sat_idx;
}

/// Returns the ephemeris_info of the satellite with the given index, or nullptr if not found.
static const ntn_ephemeris_info_t* find_satellite_ephemeris_info(unsigned satellite_index,
                                                                 span<const ocudu_ntn::ntn_satellite_config> satellites)
{
  auto it = std::find_if(satellites.begin(), satellites.end(), [satellite_index](const auto& sat) {
    return sat.satellite_index == satellite_index;
  });
  return it != satellites.end() ? &it->ephemeris_info : nullptr;
}

std::optional<bool> ocudu::derive_use_state_vector(std::optional<bool>                         configured_value,
                                                   const std::optional<ntn_ephemeris_info_t>&  own_ephemeris_info,
                                                   unsigned                                    satellite_index,
                                                   span<const ocudu_ntn::ntn_satellite_config> resolved_satellites)
{
  if (configured_value.has_value()) {
    return configured_value;
  }
  if (own_ephemeris_info.has_value()) {
    return std::holds_alternative<ecef_coordinates_t>(*own_ephemeris_info);
  }
  if (const ntn_ephemeris_info_t* ephemeris_info =
          find_satellite_ephemeris_info(satellite_index, resolved_satellites)) {
    return std::holds_alternative<ecef_coordinates_t>(*ephemeris_info);
  }
  return std::nullopt;
}
