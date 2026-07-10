// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_sat_switch_helpers.h"

using namespace ocudu;
using namespace ocudu_ntn;

std::optional<ntn_cell_config> ocudu_ntn::derive_post_switch_config(const ntn_cell_config& current)
{
  if (!current.ntn_cfg || !current.sat_switch || !current.ntn_cfg->t_service ||
      !current.sat_switch->promote_to_serving) {
    return std::nullopt;
  }

  ntn_cell_config derived = current;

  derived.ntn_cfg->satellite_index = current.sat_switch->satellite_index;
  if (current.sat_switch->ntn_ul_sync_validity_dur) {
    derived.ntn_cfg->ntn_ul_sync_validity_dur = *current.sat_switch->ntn_ul_sync_validity_dur;
  }
  if (current.sat_switch->cell_specific_koffset) {
    derived.ntn_cfg->cell_specific_koffset = *current.sat_switch->cell_specific_koffset;
  }
  if (current.sat_switch->k_mac) {
    derived.ntn_cfg->k_mac = current.sat_switch->k_mac;
  }
  if (current.sat_switch->polarization) {
    derived.ntn_cfg->polarization = current.sat_switch->polarization;
  }
  if (current.sat_switch->ta_report) {
    derived.ntn_cfg->ta_report = current.sat_switch->ta_report;
  }
  if (current.sat_switch->use_state_vector) {
    derived.ntn_cfg->use_state_vector = current.sat_switch->use_state_vector;
  }
  if (!current.sat_switch->promote_neighbors) {
    derived.ncells.clear();
  }
  // t_service described when the source satellite stops serving; that moment has passed once the derived config is
  // active, and the new satellite's own service stop time is unknown here.
  derived.ntn_cfg->t_service.reset();
  derived.sat_switch.reset();

  return derived;
}
