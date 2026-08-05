// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/ntn.h"
#include "ocudu/ran/tar_config.h"
#include <chrono>
#include <optional>
#include <variant>

namespace ocudu {

/// NTN cell-level configuration parameters.
struct ntn_cell_params {
  /// NTN cell configuration.
  ntn_config ntn_cfg;

  /// Whether UL HARQ Mode B is enabled for this NTN cell (if there is at least one UL HARQ process in mode B).
  bool ul_harq_mode_b = false;

  /// \brief Timing Advance reporting configuration to signal to the UEs of this cell (\c tar-Config,
  /// TS 38.321, 5.4.8). Absent when variation-triggered TA reporting is not configured.
  std::optional<tar_config> tar_cfg;

  /// \brief Uplink timing advance T_TA of a UE at the cell reference location (TS 38.211, Section 4.3.1).
  ///
  /// Estimated from the ephemeris and the cell reference location - referenceLocation, or
  /// movingReferenceLocation for an Earth-moving cell - so off by the differential delay across the footprint.
  /// Recomputed as the satellite moves; absent until first computed or when no reference location is set.
  std::optional<std::chrono::microseconds> ref_location_ul_ta;

  /// Helper method to check if NTN is enabled.
  bool is_enabled() const
  {
    return (ntn_cfg.cell_specific_koffset.has_value() and ntn_cfg.cell_specific_koffset.value().count() > 0);
  }
};

/// \brief Returns the uplink timing advance at the cell reference location, see \c ntn_cell_params::ref_location_ul_ta.
///
/// Absent when the cell does not track it: either it has no NTN parameters, or no value has been computed yet.
inline std::optional<std::chrono::microseconds> get_ref_location_ul_ta(const std::optional<ntn_cell_params>& ntn_params)
{
  return ntn_params.has_value() ? ntn_params->ref_location_ul_ta : std::nullopt;
}

} // namespace ocudu
