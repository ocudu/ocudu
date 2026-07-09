// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_positioning_manager_impl.h"
#include "du_positioning_handler_factory.h"
#include "procedures/du_positioning_measurement_procedure.h"
#include "procedures/du_ue_positioning_info_procedure.h"

using namespace ocudu;
using namespace odu;

du_positioning_manager_impl::du_positioning_manager_impl(const du_manager_params& du_params_,
                                                         du_cell_manager&         cell_mng_,
                                                         du_ue_manager&           ue_mng_,
                                                         ocudulog::basic_logger&  logger_) :
  du_params(du_params_), cell_mng(cell_mng_), ue_mng(ue_mng_), logger(logger_)
{
  (void)logger;
}

du_trp_info_response du_positioning_manager_impl::request_trp_info()
{
  // Update TRP information based on the current cell configuration.
  update_trp_info();

  du_trp_info_response resp;
  resp.trps.reserve(trps.size());
  for (auto& trp : trps) {
    resp.trps.push_back(trp.second);
  }
  return resp;
}

async_task<du_positioning_info_response>
du_positioning_manager_impl::request_positioning_info(const du_positioning_info_request& req)
{
  return launch_async<du_ue_positioning_info_procedure>(req, cell_mng, ue_mng);
}

async_task<du_positioning_meas_response>
du_positioning_manager_impl::request_positioning_measurement(const du_positioning_meas_request& req)
{
  // Update TRP information based on the current cell configuration.
  update_trp_info();

  return launch_async<positioning_measurement_procedure>(req, cell_mng, ue_mng, du_params, trps);
}

void du_positioning_manager_impl::update_trp_info()
{
  for (unsigned i = 0, e = cell_mng.nof_cells(); i != e; ++i) {
    const du_cell_config& cell_cfg = cell_mng.get_cell_cfg(to_du_cell_index(i));

    du_trp_info trp;
    trp.trp_id = uint_to_trp_id(i + 1); // TRP IDs start from 1.
    trp.pci    = cell_cfg.ran.pci;
    trp.cgi    = cell_cfg.nr_cgi;
    trp.arfcn  = cell_cfg.ran.ul_cfg_common.freq_info_ul.absolute_freq_point_a;
    if (cell_cfg.trp_geo_coordinates.has_value()) {
      geographical_coordinates_t geo_coords;
      geo_coords.trp_position_definition_type = trp_position_direct_t{cell_cfg.trp_geo_coordinates.value()};
      trp.geo_coords                          = geo_coords;
    }
    trps.insert(std::make_pair(trp.trp_id, trp));
  }
}
