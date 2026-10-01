// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_ue_positioning_info_procedure.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace odu;

du_ue_positioning_info_procedure::du_ue_positioning_info_procedure(const du_positioning_info_request& req_,
                                                                   du_cell_manager&                   du_cells_,
                                                                   du_ue_manager&                     ue_mng_) :
  req(req_), du_cells(du_cells_), ue_mng(ue_mng_), ue(*ue_mng.find_ue(req.ue_index))
{
}

void du_ue_positioning_info_procedure::operator()(coro_context<async_task<du_positioning_info_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Initiate UE configuration to transmit SRS signals for positioning.
  // TODO

  // Prepare SRS configuration.
  CORO_RETURN(create_response(true));
}

du_positioning_info_response du_ue_positioning_info_procedure::create_response(bool success)
{
  du_positioning_info_response resp;

  if (not success) {
    return resp;
  }

  const serving_cell_config& pcell_cfg = ue.resources->cell_group.cells.at(SERVING_PCELL_IDX).serv_cell_cfg;
  du_cell_index_t            cell_idx  = pcell_cfg.cell_index;
  const du_cell_config&      cell_cmn  = du_cells.get_cell_cfg(cell_idx);

  if (not pcell_cfg.ul_config.has_value() or not pcell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value()) {
    // Failure case.
    return resp;
  }

  // Fill PCell params.
  resp.srs_carriers.resize(1);
  // pointA
  resp.srs_carriers[0].point_a = cell_cmn.ran.ul_cfg_common.freq_info_ul.absolute_freq_point_a;
  // freq shift 7.5kHz
  resp.srs_carriers[0].freq_shift_7p5khz_present = cell_cmn.ran.ul_cfg_common.freq_info_ul.freq_shift_7p5khz_present;
  // uplink ChannelBW per SCS list
  resp.srs_carriers[0].ul_ch_bw_per_scs_list = cell_cmn.ran.ul_cfg_common.freq_info_ul.scs_carrier_list;
  // active UL BWP.
  resp.srs_carriers[0].ul_bwp_cfg = cell_cmn.ran.ul_cfg_common.init_ul_bwp.generic_params;
  // srsConfig.
  resp.srs_carriers[0].srs_cfg = pcell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
  // PCI
  resp.srs_carriers[0].pci = cell_cmn.ran.pci;

  return resp;
}
