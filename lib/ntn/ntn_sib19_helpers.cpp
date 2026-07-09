// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_sib19_helpers.h"
#include "ocudu/adt/static_vector.h"

using namespace ocudu;
using namespace ocudu_ntn;

/// Two neighbor cell configs yield an identical ntn-Config: same satellite (same ephemeris_info) and same
/// ntn_ul_sync_validity_dur (this also feeds the OCM call and can otherwise change the computed ephemeris_info/
/// ta_info), plus the remaining per-cell static fields.
static bool ntn_cfg_would_be_identical(const ntn_neighbor_cell_config& a, const ntn_neighbor_cell_config& b)
{
  if (a.satellite_index != b.satellite_index || a.ntn_ul_sync_validity_dur != b.ntn_ul_sync_validity_dur ||
      a.cell_specific_koffset != b.cell_specific_koffset || a.k_mac != b.k_mac || a.ta_report != b.ta_report) {
    return false;
  }
  if (a.polarization.has_value() != b.polarization.has_value()) {
    return false;
  }
  return !a.polarization.has_value() ||
         (a.polarization->dl == b.polarization->dl && a.polarization->ul == b.polarization->ul);
}

/// Per TS 38.331, an absent epochTime within an otherwise explicit ntn-Config provided via NTN-NeighCellConfig or
/// SatSwitchWithReSync falls back to the serving NTN cell's own epoch time. sib19 must already have its serving
/// ntn_cfg populated.
static bool
matches_serving_ntn_epoch_time(const ntn_cell_config& cell_cfg, const sib19_info& sib19, slot_point epoch_slot)
{
  return cell_cfg.ntn_cfg.has_value() && sib19.ntn_cfg->epoch_time->sfn == epoch_slot.sfn() &&
         sib19.ntn_cfg->epoch_time->subframe_number == epoch_slot.subframe_index();
}

/// Per TS 38.331, an absent ntn-UlSyncValidityDuration within an otherwise explicit ntn-Config provided via
/// NTN-NeighCellConfig or SatSwitchWithReSync falls back to the serving NTN cell's own validity duration.
static bool matches_serving_ntn_ul_sync_validity_dur(const ntn_cell_config& cell_cfg, std::optional<unsigned> value)
{
  return cell_cfg.ntn_cfg.has_value() && value == cell_cfg.ntn_cfg->ntn_ul_sync_validity_dur;
}

sib19_info ocudu_ntn::generate_sib19_info(const ntn_cell_config&        cell_cfg,
                                          slot_point                    epoch_slot,
                                          const ntn_orbital_state&      serving_reply,
                                          const ntn_orbital_state*      sat_sw_reply,
                                          span<const ntn_orbital_state> ncell_replies)

{
  sib19_info sib19;
  if (cell_cfg.ntn_cfg) {
    const ntn_serving_cell_config& ntn_cfg = *cell_cfg.ntn_cfg;
    sib19.ref_location                     = ntn_cfg.reference_location;
    sib19.distance_thres                   = ntn_cfg.distance_threshold;
    sib19.t_service                        = ntn_cfg.t_service;
    sib19.moving_ref_location              = ntn_cfg.moving_reference_location;
    sib19.ntn_cfg.emplace();
    sib19.ntn_cfg->cell_specific_koffset = ntn_cfg.cell_specific_koffset;
    sib19.ntn_cfg->polarization          = ntn_cfg.polarization;
    sib19.ntn_cfg->ta_report             = ntn_cfg.ta_report;
    sib19.ntn_cfg->k_mac                 = ntn_cfg.k_mac;
    sib19.ntn_cfg->epoch_time.emplace();
    sib19.ntn_cfg->epoch_time->sfn             = epoch_slot.sfn();
    sib19.ntn_cfg->epoch_time->subframe_number = epoch_slot.subframe_index();
    sib19.ntn_cfg->ephemeris_info              = serving_reply.ephemeris_info;
    if (ntn_cfg.feeder_link_info) {
      sib19.ntn_cfg->ta_info = serving_reply.ta_info;
    }
    sib19.ntn_cfg->ntn_ul_sync_validity_dur = ntn_cfg.ntn_ul_sync_validity_dur;

    if (ntn_cfg.ta_common_offset) {
      if (!sib19.ntn_cfg->ta_info) {
        sib19.ntn_cfg->ta_info.emplace();
      }
      sib19.ntn_cfg->ta_info->ta_common_offset = ntn_cfg.ta_common_offset;
    }
  }

  // Populate sat-switch target ntn_cfg with propagated ephemeris from the sat-switch OCM. Per TS 38.331,
  // satSwitchWithReSync is only present in an NTN cell, and ntn-Config is mandatory within it (unlike a neighbor
  // cell's, which is optional and can be inherited), so the whole block is only broadcast when the serving cell is
  // NTN and the sat-switch OCM lookup actually succeeded -- never with an empty ntn-Config, and never in a TN cell.
  if (cell_cfg.ntn_cfg && cell_cfg.sat_switch && sat_sw_reply != nullptr && sat_sw_reply->success) {
    const auto&              sw_cfg = *cell_cfg.sat_switch;
    sat_switch_with_resync_t sat_sw;
    sat_sw.t_service_start               = sw_cfg.t_service_start;
    sat_sw.ssb_time_offset_sf            = sw_cfg.ssb_time_offset_sf;
    sat_sw.ntn_cfg.cell_specific_koffset = sw_cfg.cell_specific_koffset;
    sat_sw.ntn_cfg.k_mac                 = sw_cfg.k_mac;
    sat_sw.ntn_cfg.polarization          = sw_cfg.polarization;
    sat_sw.ntn_cfg.ta_report             = sw_cfg.ta_report;
    if (!matches_serving_ntn_epoch_time(cell_cfg, sib19, epoch_slot)) {
      sat_sw.ntn_cfg.epoch_time.emplace();
      sat_sw.ntn_cfg.epoch_time->sfn             = epoch_slot.sfn();
      sat_sw.ntn_cfg.epoch_time->subframe_number = epoch_slot.subframe_index();
    }
    if (!matches_serving_ntn_ul_sync_validity_dur(cell_cfg, sw_cfg.ntn_ul_sync_validity_dur)) {
      sat_sw.ntn_cfg.ntn_ul_sync_validity_dur = sw_cfg.ntn_ul_sync_validity_dur;
    }
    sat_sw.ntn_cfg.ephemeris_info = sat_sw_reply->ephemeris_info;
    sat_sw.ntn_cfg.ta_info        = sat_sw_reply->ta_info;
    sib19.sat_switch_with_resync  = sat_sw;
  }

  // Populate each neighbor NTN cell entry and fill OCM result.
  //
  // The first MAX_NOF_NTN_NEIGHBORS_BASE_LIST entries of sib19.ncells are broadcast in ntn-NeighCellConfigList
  // and the rest in ntn-NeighCellConfigListExt. Per TS 38.331 SIB19 field descriptions, the two lists have
  // different omission rules:
  //  - ntn-NeighCellConfigList: ntn-Config is mandatory for the first entry. If absent for any other entry, the
  //    ntn-Config of the previous entry in that same list applies.
  //  - ntn-NeighCellConfigListExt: if ntn-Config is absent, the ntn-Config provided in the entry at the same
  //    position in ntn-NeighCellConfigList applies (not the previous ext entry). Since the spec refers to the
  //    ntn-Config provided in that base entry, an ext entry only omits its ntn-Config when the same-position base
  //    entry carries an explicit one, not when the base entry itself omitted it.
  // Entries whose orbital state computation failed carry no valid ephemeris/TA-info and are dropped rather than
  // broadcast with an absent or stale ntn-Config, so they can never be inherited from either.
  static_vector<size_t, MAX_NOF_NTN_NEIGHBORS> included_idx;
  for (size_t i = 0, e = cell_cfg.ncells.size(); i != e; ++i) {
    if (!ncell_replies[i].success) {
      continue;
    }
    const auto&  nc_cfg = cell_cfg.ncells[i];
    const size_t pos    = sib19.ncells.size();
    const bool   can_inherit =
        pos < MAX_NOF_NTN_NEIGHBORS_BASE_LIST
              ? (pos > 0 && ntn_cfg_would_be_identical(nc_cfg, cell_cfg.ncells[included_idx[pos - 1]]))
              : (sib19.ncells[pos - MAX_NOF_NTN_NEIGHBORS_BASE_LIST].ntn_cfg.has_value() &&
               ntn_cfg_would_be_identical(nc_cfg,
                                          cell_cfg.ncells[included_idx[pos - MAX_NOF_NTN_NEIGHBORS_BASE_LIST]]));

    neighbor_ntn_cell ncell;
    ncell.carrier_freq = nc_cfg.carrier_freq;
    ncell.phys_cell_id = nc_cfg.phys_cell_id;
    if (!can_inherit) {
      ncell.ntn_cfg.emplace();
      ncell.ntn_cfg->cell_specific_koffset = nc_cfg.cell_specific_koffset;
      ncell.ntn_cfg->k_mac                 = nc_cfg.k_mac;
      ncell.ntn_cfg->polarization          = nc_cfg.polarization;
      ncell.ntn_cfg->ta_report             = nc_cfg.ta_report;
      if (!matches_serving_ntn_epoch_time(cell_cfg, sib19, epoch_slot)) {
        ncell.ntn_cfg->epoch_time.emplace();
        ncell.ntn_cfg->epoch_time->sfn             = epoch_slot.sfn();
        ncell.ntn_cfg->epoch_time->subframe_number = epoch_slot.subframe_index();
      }
      if (!matches_serving_ntn_ul_sync_validity_dur(cell_cfg, nc_cfg.ntn_ul_sync_validity_dur)) {
        ncell.ntn_cfg->ntn_ul_sync_validity_dur = nc_cfg.ntn_ul_sync_validity_dur;
      }
      ncell.ntn_cfg->ephemeris_info = ncell_replies[i].ephemeris_info;
      ncell.ntn_cfg->ta_info        = ncell_replies[i].ta_info;
    }
    sib19.ncells.push_back(ncell);
    included_idx.push_back(i);
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
