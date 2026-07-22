// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_high_ntn_config_yaml_writer.h"
#include "apps/helpers/ntn/ntn_config_yaml_writer.h"
#include "apps/helpers/ntn/ntn_satellite_config.h"
#include "du_high_unit_cell_ntn_config.h"
#include <vector>

using namespace ocudu;

void ocudu::fill_ntn_config_in_yaml_schema(YAML::Node& node, const du_high_unit_cell_ntn_config& config)
{
  auto ntn_node = node["ntn"];

  if (config.serving) {
    const auto& serving = *config.serving;

    fill_ntn_satellite_in_yaml_schema(ntn_node, serving.sat_ref);

    ntn_node["cell_specific_koffset"]    = static_cast<unsigned>(serving.cell_specific_koffset.count());
    ntn_node["ntn_ul_sync_validity_dur"] = serving.ntn_ul_sync_validity_dur;

    if (serving.feeder_link_info) {
      YAML::Node fl_node;
      fl_node["enable_doppler_compensation"] = serving.feeder_link_info->enable_doppler_compensation;
      fl_node["dl_freq"]                     = serving.feeder_link_info->dl_freq;
      fl_node["ul_freq"]                     = serving.feeder_link_info->ul_freq;
      ntn_node["feeder_link_info"]           = fl_node;
    }

    if (serving.epoch_time) {
      YAML::Node epoch_node;
      epoch_node["sfn"]             = serving.epoch_time->sfn;
      epoch_node["subframe_number"] = serving.epoch_time->subframe_number;
      ntn_node["epoch_time"]        = epoch_node;
    }

    if (serving.epoch_sfn_offset) {
      ntn_node["epoch_sfn_offset"] = *serving.epoch_sfn_offset;
    }

    if (serving.use_state_vector) {
      ntn_node["use_state_vector"] = *serving.use_state_vector;
    }

    if (serving.polarization) {
      ntn_node["polarization"] = build_ntn_polarization_yaml_node(*serving.polarization);
    }

    ntn_node["ta_report"] = serving.ta_report;

    if (serving.reference_location) {
      ntn_node["reference_location"] = build_geodetic_yaml_node(*serving.reference_location, false);
    }

    if (serving.distance_threshold) {
      ntn_node["distance_threshold"] = *serving.distance_threshold;
    }

    if (serving.t_service) {
      ntn_node["t_service"] = ntn_timepoint_to_iso8601(*serving.t_service);
    }

    if (serving.moving_ref_location) {
      ntn_node["moving_ref_location"] = build_geodetic_yaml_node(*serving.moving_ref_location, false);
    }

    // Satellite switch with resynchronization (R18 extension).
    if (serving.sat_switch_with_resync) {
      const auto& sw = *serving.sat_switch_with_resync;
      YAML::Node  sat_sw_node;

      fill_ntn_satellite_in_yaml_schema(sat_sw_node, sw.sat_ref);
      if (sw.t_service_start) {
        sat_sw_node["t_service_start"] = ntn_timepoint_to_iso8601(*sw.t_service_start);
      }
      if (sw.ssb_time_offset_sf) {
        sat_sw_node["ssb_time_offset_sf"] = *sw.ssb_time_offset_sf;
      }
      if (sw.ntn_ul_sync_validity_dur) {
        sat_sw_node["ntn_ul_sync_validity_dur"] = *sw.ntn_ul_sync_validity_dur;
      }
      if (sw.cell_specific_koffset) {
        sat_sw_node["cell_specific_koffset"] = static_cast<unsigned>(sw.cell_specific_koffset->count());
      }
      if (sw.k_mac) {
        sat_sw_node["k_mac"] = *sw.k_mac;
      }
      if (sw.ta_report) {
        sat_sw_node["ta_report"] = *sw.ta_report;
      }
      if (sw.use_state_vector) {
        sat_sw_node["use_state_vector"] = *sw.use_state_vector;
      }
      sat_sw_node["promote_to_serving"] = sw.promote_to_serving;
      sat_sw_node["promote_neighbors"]  = sw.promote_neighbors;

      ntn_node["sat_switch_with_resync"] = sat_sw_node;
    }
  }

  // NTN neighbor cells.
  if (!config.ncells.empty()) {
    YAML::Node ncells_node;
    for (const auto& ncell : config.ncells) {
      YAML::Node ncell_node;
      fill_ntn_satellite_in_yaml_schema(ncell_node, ncell.sat_ref);
      if (ncell.phys_cell_id) {
        ncell_node["pci"] = static_cast<unsigned>(*ncell.phys_cell_id);
      }
      if (ncell.carrier_freq) {
        ncell_node["carrier_freq"] = ncell.carrier_freq->value();
      }
      if (ncell.cell_specific_koffset) {
        ncell_node["cell_specific_koffset"] = static_cast<unsigned>(ncell.cell_specific_koffset->count());
      }
      if (ncell.ntn_ul_sync_validity_dur) {
        ncell_node["ntn_ul_sync_validity_dur"] = *ncell.ntn_ul_sync_validity_dur;
      }
      if (ncell.k_mac) {
        ncell_node["k_mac"] = *ncell.k_mac;
      }
      if (ncell.polarization) {
        ncell_node["polarization"] = build_ntn_polarization_yaml_node(*ncell.polarization);
      }
      if (ncell.ta_report) {
        ncell_node["ta_report"] = *ncell.ta_report;
      }
      if (ncell.use_state_vector) {
        ncell_node["use_state_vector"] = *ncell.use_state_vector;
      }
      ncell_node["has_feeder_link"] = ncell.has_feeder_link;
      ncells_node.push_back(ncell_node);
    }
    ntn_node["ncells"] = ncells_node;
  }
}
