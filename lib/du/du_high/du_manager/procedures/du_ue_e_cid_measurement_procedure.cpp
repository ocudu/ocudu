// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_ue_e_cid_measurement_procedure.h"
#include "du_positioning_meas_timeout.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/positioning/ul_aoa_mapping.h"
#include <algorithm>

using namespace ocudu;
using namespace odu;

du_ue_e_cid_measurement_procedure::du_ue_e_cid_measurement_procedure(const du_e_cid_meas_request& req_,
                                                                     du_cell_manager&             du_cells_,
                                                                     du_ue_manager_repository&    ue_mng_,
                                                                     const du_manager_params&     du_params_) :
  req(req_), du_cells(du_cells_), ue_mng(ue_mng_), du_params(du_params_), logger(ocudulog::fetch_basic_logger("DU-MNG"))
{
}

void du_ue_e_cid_measurement_procedure::operator()(coro_context<async_task<du_e_cid_meas_response>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not prepare_mac_request()) {
    CORO_EARLY_RETURN(du_e_cid_meas_response{});
  }

  if (not aoa_requested) {
    // Only the serving cell information was requested. No SRS measurement is needed.
    CORO_EARLY_RETURN(create_response());
  }

  CORO_AWAIT_VALUE(mac_resp,
                   du_params.mac.mgr.get_positioning_handler().handle_positioning_measurement_request(mac_req));

  CORO_RETURN(create_response());
}

bool du_ue_e_cid_measurement_procedure::prepare_mac_request()
{
  du_ue* ue = ue_mng.find_ue(req.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: E-CID measurement failed. Cause: UE not found", fmt::underlying(req.ue_index));
    return false;
  }

  aoa_requested = std::find(req.quantities.begin(), req.quantities.end(), e_cid_meas_quantity::nr_angle_of_arrival) !=
                  req.quantities.end();

  auto pcell_it = ue->resources->cell_group.cells.find(SERVING_PCELL_IDX);
  if (pcell_it == ue->resources->cell_group.cells.end()) {
    logger.warning("ue={}: E-CID measurement failed. Cause: UE has no serving cell configured",
                   fmt::underlying(req.ue_index));
    return false;
  }
  const serving_cell_config& pcell_cfg = pcell_it->second.serv_cell_cfg;
  const du_cell_config&      cell_cmn  = du_cells.get_cell_cfg(pcell_cfg.cell_index);

  if (cell_cmn.trp_geo_coordinates.has_value()) {
    geographical_coordinates_t coords;
    coords.trp_position_definition_type = trp_position_direct_t{cell_cmn.trp_geo_coordinates.value()};
    geo_coords                          = coords;
  }

  if (not aoa_requested) {
    return true;
  }

  if (not pcell_cfg.ul_config.has_value() or not pcell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value()) {
    logger.warning("ue={}: E-CID measurement failed. Cause: No SRS configured in the serving cell",
                   fmt::underlying(req.ue_index));
    return false;
  }

  // The request is UE-associated, so the MAC measures the SRS of this UE by its RNTI.
  mac_req.cells.resize(1);
  mac_req.cells[0].cell_index  = pcell_cfg.cell_index;
  mac_req.cells[0].ue_index    = ue->ue_index;
  mac_req.cells[0].rnti        = ue->rnti;
  mac_req.cells[0].srs_to_meas = pcell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
  // The measurement completes on an SRS occasion, so the time that the MAC waits follows the SRS period.
  mac_req.timeout = get_positioning_meas_timeout(pcell_cfg.ul_config->init_ul_bwp.srs_cfg.value(),
                                                 cell_cmn.ran.ul_cfg_common.init_ul_bwp.generic_params.scs);

  return true;
}

du_e_cid_meas_response du_ue_e_cid_measurement_procedure::create_response()
{
  du_e_cid_meas_response resp;
  resp.geo_coords = geo_coords;

  if (not aoa_requested) {
    resp.success = true;
    return resp;
  }

  if (mac_resp.cell_results.empty() or mac_resp.cell_results[0].ul_srs_pos_meass.empty()) {
    logger.warning("ue={}: E-CID measurement failed. Cause: MAC reported no SRS measurement",
                   fmt::underlying(req.ue_index));
    return resp;
  }

  const auto& mac_meas = mac_resp.cell_results[0].ul_srs_pos_meass[0];
  if (not mac_meas.azimuth_aoa_deg.has_value()) {
    logger.warning("ue={}: E-CID measurement failed. Cause: MAC reported no angle of arrival",
                   fmt::underlying(req.ue_index));
    return resp;
  }

  pos_meas_result_ul_aoa aoa_result{.azimuth_aoa = azimuth_aoa_to_reported_value(*mac_meas.azimuth_aoa_deg)};
  if (mac_meas.zenith_aoa_deg.has_value()) {
    aoa_result.zenith_aoa = zenith_aoa_to_reported_value(*mac_meas.zenith_aoa_deg);
  }
  resp.ul_aoa_results.push_back(aoa_result);
  resp.success = true;

  return resp;
}
