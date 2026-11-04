// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/ntn.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/ran/tar_config.h"
#include <chrono>
#include <optional>
#include <variant>

namespace ocudu {

/// Derivation of the UE-specific Koffset from the reported T_TA, as per TS 38.213, Section 4.2 and TS 38.321,
/// Section 5.18.24.
struct ue_koffset_config {
  /// Margin added to the reported T_TA. At least \c tar_config::offset_threshold_ta.
  std::chrono::milliseconds margin;
};

/// NTN cell-level configuration parameters.
struct ntn_cell_params {
  /// NTN cell configuration.
  ntn_config ntn_cfg;

  /// Whether UL HARQ Mode B is enabled for this NTN cell (if there is at least one UL HARQ process in mode B).
  bool ul_harq_mode_b = false;

  /// \brief Timing Advance reporting configuration to signal to the UEs of this cell (\c tar-Config,
  /// TS 38.321, 5.4.8). Absent when variation-triggered TA reporting is not configured.
  std::optional<tar_config> tar_cfg;

  /// UE-specific Koffset derivation. Absent when every UE keeps the cell-specific Koffset.
  std::optional<ue_koffset_config> ue_koffset_cfg;

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

/// \brief Returns the UE-specific Koffset derivation of the cell, see \c ntn_cell_params::ue_koffset_cfg.
///
/// Absent when the cell has no NTN parameters, or when every UE keeps the cell-specific Koffset.
inline std::optional<ue_koffset_config> get_ue_koffset_cfg(const std::optional<ntn_cell_params>& ntn_params)
{
  return ntn_params.has_value() ? ntn_params->ue_koffset_cfg : std::nullopt;
}

/// \brief Returns the cell-specific K-offset of the cell in slots of \c scs, see \c ntn_config::cell_specific_koffset.
///
/// Zero in a terrestrial cell, and in an NTN cell that configures none.
inline unsigned get_ntn_cs_koffset_slots(const std::optional<ntn_cell_params>& ntn_params, subcarrier_spacing scs)
{
  if (not ntn_params.has_value()) {
    return 0;
  }
  return static_cast<unsigned>(
             ntn_params->ntn_cfg.cell_specific_koffset.value_or(std::chrono::milliseconds{0}).count()) *
         get_nof_slots_per_subframe(scs);
}

/// \brief Returns the k_mac of the cell in slots of \c scs, see \c ntn_config::k_mac.
///
/// Zero in a terrestrial cell, and in an NTN cell that configures none.
inline unsigned get_ntn_k_mac_slots(const std::optional<ntn_cell_params>& ntn_params, subcarrier_spacing scs)
{
  if (not ntn_params.has_value()) {
    return 0;
  }
  return static_cast<unsigned>(ntn_params->ntn_cfg.k_mac.value_or(std::chrono::milliseconds{0}).count()) *
         get_nof_slots_per_subframe(scs);
}

/// \brief Returns the uplink allocation delay of the cell in slots of \c scs: the cell-specific K-offset the UE
/// applies plus the k_mac the channel adds.
///
/// Zero in a terrestrial cell, and in an NTN cell that configures neither. The scheduler reads it from
/// \c cell_configuration::ntn_ul_alloc_delay_slots; this is for the layers that hold the cell configuration request
/// instead.
inline unsigned get_ntn_ul_alloc_delay_slots(const std::optional<ntn_cell_params>& ntn_params, subcarrier_spacing scs)
{
  return get_ntn_cs_koffset_slots(ntn_params, scs) + get_ntn_k_mac_slots(ntn_params, scs);
}

} // namespace ocudu
