// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_sib19_helpers.h"

using namespace ocudu;
using namespace ocudu_ntn;

sib19_info ocudu_ntn::generate_sib19_info(const ntn_cell_config&        cell_cfg,
                                          slot_point                    epoch_slot,
                                          const ntn_orbital_state&      serving_reply,
                                          const ntn_orbital_state*      sat_sw_reply,
                                          span<const ntn_orbital_state> ncell_replies)

{
  unsigned   ntn_ul_sync_validity_dur = cell_cfg.ntn_cfg.ntn_ul_sync_validity_dur.value_or(5);
  sib19_info sib19;
  sib19.ref_location        = cell_cfg.ntn_cfg.reference_location;
  sib19.distance_thres      = cell_cfg.ntn_cfg.distance_threshold;
  sib19.t_service           = cell_cfg.ntn_cfg.t_service;
  sib19.moving_ref_location = cell_cfg.ntn_cfg.moving_reference_location;

  sib19.ntn_cfg.emplace();
  sib19.ntn_cfg->cell_specific_koffset = cell_cfg.ntn_cfg.cell_specific_koffset;
  sib19.ntn_cfg->polarization          = cell_cfg.ntn_cfg.polarization;
  sib19.ntn_cfg->ta_report             = cell_cfg.ntn_cfg.ta_report;
  sib19.ntn_cfg->k_mac                 = cell_cfg.ntn_cfg.k_mac;
  sib19.ntn_cfg->epoch_time.emplace();
  sib19.ntn_cfg->epoch_time->sfn             = epoch_slot.sfn();
  sib19.ntn_cfg->epoch_time->subframe_number = epoch_slot.subframe_index();
  sib19.ntn_cfg->ephemeris_info              = serving_reply.ephemeris_info;
  if (cell_cfg.ntn_cfg.feeder_link_info) {
    sib19.ntn_cfg->ta_info = serving_reply.ta_info;
  }
  sib19.ntn_cfg->ntn_ul_sync_validity_dur = ntn_ul_sync_validity_dur;

  if (cell_cfg.ntn_cfg.ta_common_offset) {
    if (!sib19.ntn_cfg->ta_info) {
      sib19.ntn_cfg->ta_info.emplace();
    }
    sib19.ntn_cfg->ta_info->ta_common_offset = *cell_cfg.ntn_cfg.ta_common_offset;
  }

  // Populate sat-switch target ntn_cfg with propagated ephemeris from the sat-switch OCM.
  if (cell_cfg.sat_switch) {
    const auto&              sw_cfg = *cell_cfg.sat_switch;
    sat_switch_with_resync_t sat_sw;
    sat_sw.t_service_start    = sw_cfg.t_service_start;
    sat_sw.ssb_time_offset_sf = sw_cfg.ssb_time_offset_sf;
    if (sat_sw_reply != nullptr && sat_sw_reply->success) {
      sat_sw.ntn_cfg.cell_specific_koffset    = sw_cfg.cell_specific_koffset;
      sat_sw.ntn_cfg.ntn_ul_sync_validity_dur = sw_cfg.ntn_ul_sync_validity_dur.value_or(ntn_ul_sync_validity_dur);
      sat_sw.ntn_cfg.k_mac                    = sw_cfg.k_mac;
      sat_sw.ntn_cfg.polarization             = sw_cfg.polarization;
      sat_sw.ntn_cfg.ta_report                = sw_cfg.ta_report;
      sat_sw.ntn_cfg.epoch_time.emplace();
      sat_sw.ntn_cfg.epoch_time->sfn             = epoch_slot.sfn();
      sat_sw.ntn_cfg.epoch_time->subframe_number = epoch_slot.subframe_index();
      sat_sw.ntn_cfg.ephemeris_info              = sat_sw_reply->ephemeris_info;
      sat_sw.ntn_cfg.ta_info                     = sat_sw_reply->ta_info;
    }
    sib19.sat_switch_with_resync = sat_sw;
  }

  // Populate each neighbor NTN cell entry and fill OCM result.
  // Per TS 38.331 SIB19: if ntn-Config is absent, the UE reuses the previous entry's ntn-Config.
  // Omit ntn_cfg for consecutive entries on the same satellite — ephemeris and static fields are identical.
  for (size_t i = 0, e = cell_cfg.ncells.size(); i != e; ++i) {
    const auto&       nc_cfg = cell_cfg.ncells[i];
    neighbor_ntn_cell ncell;
    ncell.carrier_freq = nc_cfg.carrier_freq;
    ncell.phys_cell_id = nc_cfg.phys_cell_id;
    if (ncell_replies[i].success) {
      const bool can_inherit =
          i > 0 && sib19.ncells.back().ntn_cfg && nc_cfg.satellite_index == cell_cfg.ncells[i - 1].satellite_index;
      if (!can_inherit) {
        ncell.ntn_cfg.emplace();
        ncell.ntn_cfg->cell_specific_koffset    = nc_cfg.cell_specific_koffset;
        ncell.ntn_cfg->ntn_ul_sync_validity_dur = nc_cfg.ntn_ul_sync_validity_dur;
        ncell.ntn_cfg->k_mac                    = nc_cfg.k_mac;
        ncell.ntn_cfg->polarization             = nc_cfg.polarization;
        ncell.ntn_cfg->ta_report                = nc_cfg.ta_report;
        ncell.ntn_cfg->epoch_time.emplace();
        ncell.ntn_cfg->epoch_time->sfn             = epoch_slot.sfn();
        ncell.ntn_cfg->epoch_time->subframe_number = epoch_slot.subframe_index();
        ncell.ntn_cfg->ephemeris_info              = ncell_replies[i].ephemeris_info;
        ncell.ntn_cfg->ta_info                     = ncell_replies[i].ta_info;
      }
    }
    sib19.ncells.push_back(ncell);
  }

  return sib19;
}

bool ocudu_ntn::sib19_tracked_fields_changed(const std::optional<sib19_info>& prev, const sib19_info& curr_sib19)
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
