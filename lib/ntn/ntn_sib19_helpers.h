// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/ntn.h"
#include "ocudu/ran/sib/system_info_config.h"
#include <optional>

namespace ocudu {
namespace ocudu_ntn {

/// \brief Returns true when any SIB1 value-tag-tracked SIB19 field differs between \p prev and \p curr,
/// or when \p prev has no value (first update).
///
/// Exempt fields (not compared): moving_ref_location, ntn_config::{epoch_time, ntn_ul_sync_validity_dur,
/// ta_info, ephemeris_info}.
inline bool sib19_tracked_fields_changed(const std::optional<sib19_info>& prev, const sib19_info& curr_sib19)
{
  if (!prev.has_value()) {
    return true;
  }
  const sib19_info& prev_sib19 = *prev;

  // ntn_cfg tracked subfields.
  if (prev_sib19.ntn_cfg.has_value() != curr_sib19.ntn_cfg.has_value()) {
    return true;
  }
  if (curr_sib19.ntn_cfg.has_value()) {
    const ntn_config& prev_ntn = *prev_sib19.ntn_cfg;
    const ntn_config& curr_ntn = *curr_sib19.ntn_cfg;
    if (prev_ntn.cell_specific_koffset != curr_ntn.cell_specific_koffset) {
      return true;
    }
    if (prev_ntn.k_mac != curr_ntn.k_mac) {
      return true;
    }
    if (prev_ntn.ta_report != curr_ntn.ta_report) {
      return true;
    }
    if (prev_ntn.polarization.has_value() != curr_ntn.polarization.has_value()) {
      return true;
    }
    if (prev_ntn.polarization.has_value() && (prev_ntn.polarization->dl != curr_ntn.polarization->dl ||
                                              prev_ntn.polarization->ul != curr_ntn.polarization->ul)) {
      return true;
    }
  }

  // t_service.
  if (prev_sib19.t_service != curr_sib19.t_service) {
    return true;
  }

  // ref_location (geodetic_coordinates_t: config values, exact equality is correct).
  if (prev_sib19.ref_location.has_value() != curr_sib19.ref_location.has_value()) {
    return true;
  }
  if (prev_sib19.ref_location.has_value() &&
      (prev_sib19.ref_location->latitude != curr_sib19.ref_location->latitude ||
       prev_sib19.ref_location->longitude != curr_sib19.ref_location->longitude ||
       prev_sib19.ref_location->altitude != curr_sib19.ref_location->altitude)) {
    return true;
  }

  // distance_thres.
  if (prev_sib19.distance_thres != curr_sib19.distance_thres) {
    return true;
  }

  // ncells (ordered comparison; reordering counts as a change).
  if (prev_sib19.ncells.size() != curr_sib19.ncells.size()) {
    return true;
  }
  for (size_t i = 0, e = curr_sib19.ncells.size(); i != e; ++i) {
    const neighbor_ntn_cell& prev_neigh = prev_sib19.ncells[i];
    const neighbor_ntn_cell& curr_neigh = curr_sib19.ncells[i];
    if (prev_neigh.carrier_freq != curr_neigh.carrier_freq) {
      return true;
    }
    if (prev_neigh.phys_cell_id != curr_neigh.phys_cell_id) {
      return true;
    }
    if (prev_neigh.ntn_cfg.has_value() != curr_neigh.ntn_cfg.has_value()) {
      return true;
    }
    if (curr_neigh.ntn_cfg.has_value()) {
      if (prev_neigh.ntn_cfg->cell_specific_koffset != curr_neigh.ntn_cfg->cell_specific_koffset) {
        return true;
      }
      if (prev_neigh.ntn_cfg->k_mac != curr_neigh.ntn_cfg->k_mac) {
        return true;
      }
      if (prev_neigh.ntn_cfg->ta_report != curr_neigh.ntn_cfg->ta_report) {
        return true;
      }
      if (prev_neigh.ntn_cfg->polarization.has_value() != curr_neigh.ntn_cfg->polarization.has_value()) {
        return true;
      }
      if (curr_neigh.ntn_cfg->polarization.has_value() &&
          (prev_neigh.ntn_cfg->polarization->dl != curr_neigh.ntn_cfg->polarization->dl ||
           prev_neigh.ntn_cfg->polarization->ul != curr_neigh.ntn_cfg->polarization->ul)) {
        return true;
      }
    }
  }

  // coverage_enhancements (ntn_cov_enh_t: two optional<unsigned> fields).
  if (prev_sib19.coverage_enhancements.has_value() != curr_sib19.coverage_enhancements.has_value()) {
    return true;
  }
  if (prev_sib19.coverage_enhancements.has_value() &&
      (prev_sib19.coverage_enhancements->nof_msg4_harq_ack_rep !=
           curr_sib19.coverage_enhancements->nof_msg4_harq_ack_rep ||
       prev_sib19.coverage_enhancements->rsrp_thres_msg4_harq_ack !=
           curr_sib19.coverage_enhancements->rsrp_thres_msg4_harq_ack)) {
    return true;
  }

  // sat_switch_with_resync: compare tracked ntn_cfg subfields, t_service_start, ssb_time_offset_sf.
  if (prev_sib19.sat_switch_with_resync.has_value() != curr_sib19.sat_switch_with_resync.has_value()) {
    return true;
  }
  if (prev_sib19.sat_switch_with_resync.has_value()) {
    const auto& prev_satswitch = *prev_sib19.sat_switch_with_resync;
    const auto& curr_satswitch = *curr_sib19.sat_switch_with_resync;
    if (prev_satswitch.t_service_start != curr_satswitch.t_service_start) {
      return true;
    }
    if (prev_satswitch.ssb_time_offset_sf != curr_satswitch.ssb_time_offset_sf) {
      return true;
    }
    if (prev_satswitch.ntn_cfg.cell_specific_koffset != curr_satswitch.ntn_cfg.cell_specific_koffset) {
      return true;
    }
    if (prev_satswitch.ntn_cfg.k_mac != curr_satswitch.ntn_cfg.k_mac) {
      return true;
    }
    if (prev_satswitch.ntn_cfg.ta_report != curr_satswitch.ntn_cfg.ta_report) {
      return true;
    }
    if (prev_satswitch.ntn_cfg.polarization.has_value() != curr_satswitch.ntn_cfg.polarization.has_value()) {
      return true;
    }
    if (curr_satswitch.ntn_cfg.polarization.has_value() &&
        (prev_satswitch.ntn_cfg.polarization->dl != curr_satswitch.ntn_cfg.polarization->dl ||
         prev_satswitch.ntn_cfg.polarization->ul != curr_satswitch.ntn_cfg.polarization->ul)) {
      return true;
    }
  }

  return false;
}

} // namespace ocudu_ntn
} // namespace ocudu
