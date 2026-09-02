// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_high_ntn_config_translators.h"
#include "apps/helpers/ntn/ntn_config_translators.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config.h"
#include "du_high_unit_cell_ntn_config.h"
#include "ocudu/du/du_high/du_qos_config.h"
#include "ocudu/du/du_high/du_srb_config.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/qos/five_qi.h"
#include "ocudu/rlc/rlc_config.h"
#include "ocudu/support/error_handling.h"
#include "fmt/format.h"

using namespace ocudu;

static void ntn_augment_rlc_config(const du_high_unit_cell_ntn_config& ntn_cfg, rlc_config& rlc)
{
  if (!ntn_cfg.serving) {
    return;
  }
  const auto koffset_ms = ntn_cfg.serving->cell_specific_koffset.count();
  if (koffset_ms > 1000) {
    rlc.am.tx.t_poll_retx = std::max(rlc.am.tx.t_poll_retx, 4000);
  } else if (koffset_ms > 800) {
    rlc.am.tx.t_poll_retx = std::max(rlc.am.tx.t_poll_retx, 2000);
  } else if (koffset_ms > 500) {
    rlc.am.tx.t_poll_retx = std::max(rlc.am.tx.t_poll_retx, 2000);
  } else if (koffset_ms > 300) {
    rlc.am.tx.t_poll_retx = std::max(rlc.am.tx.t_poll_retx, 1000);
  } else if (koffset_ms > 200) {
    rlc.am.tx.t_poll_retx = std::max(rlc.am.tx.t_poll_retx, 800);
  } else if (koffset_ms > 100) {
    rlc.am.tx.t_poll_retx = std::max(rlc.am.tx.t_poll_retx, 400);
  } else if (koffset_ms > 50) {
    rlc.am.tx.t_poll_retx = std::max(rlc.am.tx.t_poll_retx, 200);
  } else if (koffset_ms > 10) {
    rlc.am.tx.t_poll_retx = std::max(rlc.am.tx.t_poll_retx, 100);
  } else {
    rlc.am.tx.t_poll_retx = std::max(rlc.am.tx.t_poll_retx, 50);
  }
}

void ocudu::ntn_augment_du_srb_config(const du_high_unit_cell_ntn_config&     ntn_cfg,
                                      std::map<srb_id_t, odu::du_srb_config>& srb_cfgs)
{
  // NTN is enabled, so we need to augment the RLC parameters for the NTN cell.
  for (auto& srb : srb_cfgs) {
    ntn_augment_rlc_config(ntn_cfg, srb.second.rlc);
  }
}

void ocudu::ntn_augment_du_qos_config(const du_high_unit_cell_ntn_config&      ntn_cfg,
                                      std::map<five_qi_t, odu::du_qos_config>& qos_cfgs)
{
  // NTN is enabled, so we need to augment the QoS parameters for the NTN cell.
  for (auto& qos : qos_cfgs) {
    ntn_augment_rlc_config(ntn_cfg, qos.second.rlc);
  }
}

/// Converts app-level ntn_config to library-level ntn_serving_cell_config. Returns std::nullopt for a TN-band cell
/// that only reports NTN neighbor cells. \p resolved_satellites must already contain an entry for the cell's
/// satellite_idx (resolved, including any inline satellite definitions, before this is called).
static std::optional<ocudu_ntn::ntn_serving_cell_config>
convert_ntn_config_to_serving_cell_config(const du_high_unit_cell_ntn_config&         cfg,
                                          span<const ocudu_ntn::ntn_satellite_config> resolved_satellites)
{
  if (!cfg.serving) {
    return std::nullopt;
  }
  const auto& serving = *cfg.serving;

  ocudu_ntn::ntn_serving_cell_config info = {};

  info.satellite_index = *serving.sat_ref.satellite_idx;

  // SIB19 fields exempt from valuetag.
  info.moving_reference_location = serving.moving_ref_location;
  if (serving.sat_ref.ta_info) {
    info.ta_common_offset = serving.sat_ref.ta_info->ta_common_offset;
  }
  info.ntn_ul_sync_validity_dur = serving.ntn_ul_sync_validity_dur;

  // SIB19 fields tracked by valuetag.
  info.reference_location    = serving.reference_location;
  info.distance_threshold    = serving.distance_threshold;
  info.t_service             = serving.t_service;
  info.cell_specific_koffset = serving.cell_specific_koffset;
  info.k_mac                 = serving.k_mac;
  info.polarization          = serving.polarization;
  info.ta_report             = serving.ta_report;

  // Metadata fields.
  info.feeder_link_info = serving.feeder_link_info;

  info.use_state_vector = derive_use_state_vector(
      serving.use_state_vector, serving.sat_ref.ephemeris_info, info.satellite_index, resolved_satellites);

  return info;
}

ocudu_ntn::ntn_configuration_manager_config
ocudu::generate_ntn_configuration_manager_config(const gnb_id_t&                          gnb_id,
                                                 span<const du_high_unit_cell_config>     du_hi_cells,
                                                 const std::vector<ntn_satellite_config>& ntn_satellites)
{
  ocudu_ntn::ntn_configuration_manager_config out_cfg = {};

  // Add globally-defined satellites first. Use user-defined satellite_idx as internal satellite_index.
  unsigned next_satellite_idx = add_global_ntn_satellites(ntn_satellites, out_cfg.satellites);

  // Resolve satellite_idx for every serving cell, sat-switch target and neighbor cell: reuse the global satellite
  // if satellite_idx is set, else create one inline (1-to-1). After this loop, satellite_idx is guaranteed set
  // wherever a satellite reference is present. Neighbor cells are resolved regardless of whether this is an
  // NTN serving cell or a TN-band cell that only reports NTN neighbor cells.
  std::vector<std::optional<du_high_unit_cell_ntn_config>> resolved_ntn_cfgs(du_hi_cells.size());
  for (unsigned phy_sector_idx = 0; phy_sector_idx != du_hi_cells.size(); ++phy_sector_idx) {
    const auto& cell_cfg = du_hi_cells[phy_sector_idx].cell;
    if (!cell_cfg.ntn_cfg) {
      continue;
    }
    du_high_unit_cell_ntn_config ntn_cfg = *cell_cfg.ntn_cfg;

    if (ntn_cfg.serving) {
      auto& serving = *ntn_cfg.serving;
      resolve_ntn_satellite_ref(serving.sat_ref,
                                out_cfg.satellites,
                                next_satellite_idx,
                                serving.sat_ref.ta_info,
                                fmt::format("cells[{}].ntn", phy_sector_idx));

      if (serving.sat_switch_with_resync) {
        auto& sat_sw = *serving.sat_switch_with_resync;
        resolve_ntn_satellite_ref(sat_sw.sat_ref,
                                  out_cfg.satellites,
                                  next_satellite_idx,
                                  std::nullopt,
                                  fmt::format("cells[{}].ntn.sat_switch_with_resync", phy_sector_idx));
      }
    }

    for (auto& ncell : ntn_cfg.ncells) {
      resolve_ntn_satellite_ref(ncell.sat_ref,
                                out_cfg.satellites,
                                next_satellite_idx,
                                ncell.sat_ref.ta_info,
                                fmt::format("cells[{}].ntn.ncells[pci={}]",
                                            phy_sector_idx,
                                            ncell.phys_cell_id ? static_cast<unsigned>(*ncell.phys_cell_id) : 0U));
    }

    resolved_ntn_cfgs[phy_sector_idx] = std::move(ntn_cfg);
  }

  // Build the cell configs from the resolved NTN configs (satellite_idx guaranteed set).
  for (unsigned phy_sector_idx = 0; phy_sector_idx != du_hi_cells.size(); ++phy_sector_idx) {
    if (!resolved_ntn_cfgs[phy_sector_idx]) {
      continue;
    }
    const auto& cell_cfg = du_hi_cells[phy_sector_idx].cell;
    const auto& ntn_cfg  = *resolved_ntn_cfgs[phy_sector_idx];

    // Build cell config.
    auto&                      out_cell = out_cfg.cells.emplace_back();
    expected<plmn_identity>    plmn     = plmn_identity::parse(cell_cfg.plmn);
    expected<nr_cell_identity> nci      = nr_cell_identity::create(gnb_id, cell_cfg.sector_id.value());
    if (not plmn) {
      report_error("Invalid PLMN: {}", cell_cfg.plmn);
    }
    if (not nci) {
      report_error("Invalid NR-NCI");
    }
    out_cell.sector_id      = phy_sector_idx;
    out_cell.nr_cgi.plmn_id = plmn.value();
    out_cell.nr_cgi.nci     = nci.value();
    out_cell.ntn_cfg        = convert_ntn_config_to_serving_cell_config(ntn_cfg, out_cfg.satellites);
    out_cell.common_scs     = cell_cfg.common_scs;

    // Build sat-switch target satellite (if configured).
    if (ntn_cfg.serving && ntn_cfg.serving->sat_switch_with_resync) {
      const auto& sat_sw  = *ntn_cfg.serving->sat_switch_with_resync;
      out_cell.sat_switch = {*sat_sw.sat_ref.satellite_idx,
                             sat_sw.t_service_start,
                             sat_sw.ssb_time_offset_sf,
                             sat_sw.ntn_ul_sync_validity_dur,
                             sat_sw.cell_specific_koffset,
                             sat_sw.k_mac,
                             sat_sw.polarization,
                             sat_sw.ta_report,
                             derive_use_state_vector(sat_sw.use_state_vector,
                                                     sat_sw.sat_ref.ephemeris_info,
                                                     *sat_sw.sat_ref.satellite_idx,
                                                     out_cfg.satellites),
                             sat_sw.promote_to_serving,
                             sat_sw.promote_neighbors};
    }

    // Build neighbors' cell configs.
    for (const auto& ncell : ntn_cfg.ncells) {
      auto& nc_cfg                    = out_cell.ncells.emplace_back();
      nc_cfg.satellite_index          = *ncell.sat_ref.satellite_idx;
      nc_cfg.carrier_freq             = ncell.carrier_freq;
      nc_cfg.phys_cell_id             = ncell.phys_cell_id;
      nc_cfg.cell_specific_koffset    = ncell.cell_specific_koffset;
      nc_cfg.ntn_ul_sync_validity_dur = ncell.ntn_ul_sync_validity_dur;
      nc_cfg.k_mac                    = ncell.k_mac;
      nc_cfg.polarization             = ncell.polarization;
      nc_cfg.ta_report                = ncell.ta_report;
      nc_cfg.has_feeder_link          = ncell.has_feeder_link;
      nc_cfg.use_state_vector         = derive_use_state_vector(
          ncell.use_state_vector, ncell.sat_ref.ephemeris_info, nc_cfg.satellite_index, out_cfg.satellites);
    }

    // SIB19 Scheduling info.
    const auto& sib_cfg = cell_cfg.sib_cfg;
    for (const auto& si_msg : sib_cfg.si_sched_info) {
      const auto& sibs = si_msg.sib_mapping_info;
      if (std::find(sibs.begin(), sibs.end(), 19) != sibs.end()) {
        out_cell.si_sched = ocudu_ntn::ntn_si_scheduling_info{
            si_msg.si_period_rf, sib_cfg.si_window_len_slots, si_msg.si_window_position.value()};
      }
    }

    // Each NTN cell must configure exactly one of SIB19 scheduling info or an explicit update period.
    if (out_cell.si_sched.has_value() == out_cell.update_period.has_value()) {
      report_error("NTN cell={:#x} must configure exactly one of SIB19 scheduling info or an explicit update period",
                   out_cell.nr_cgi.nci);
    }
  }
  return out_cfg;
}
