// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_positioning_manager_impl.h"
#include "du_positioning_handler_factory.h"
#include "procedures/du_positioning_measurement_procedure.h"
#include "procedures/du_ue_positioning_info_procedure.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/prs/prs_constants.h"

using namespace ocudu;
using namespace odu;

/// \brief Converts a DL-PRS muting pattern to the DL-PRS Muting Pattern IE, as per TS 38.473, Section 9.3.1.178.
///
/// The bit of the pattern applying to the first resource set instance, or repetition, is the most significant bit of
/// the encoded value.
static dl_prs_muting_pattern_t
make_muting_pattern(const bounded_bitset<prs_constants::VALID_MUTING_PATTERN_SIZES.back()>& pattern)
{
  return {.length = static_cast<uint8_t>(pattern.size()), .value = static_cast<uint32_t>(fliplr(pattern).to_uint64())};
}

/// Converts the DL-PRS configuration of a cell to the PRS Configuration IE, as per TS 38.473, Section 9.3.1.177.
static prs_cfg_t make_trp_prs_config(const du_cell_config& cell_cfg)
{
  const bwp_configuration& bwp_cfg = cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params;

  prs_cfg_t prs_cfg;
  prs_cfg.prs_res_set_list.reserve(cell_cfg.ran.prs_cfg.resource_sets.size());
  for (unsigned set_id = 0, nof_sets = cell_cfg.ran.prs_cfg.resource_sets.size(); set_id != nof_sets; ++set_id) {
    const prs_resource_set& res_set = cell_cfg.ran.prs_cfg.resource_sets[set_id];

    prs_resource_set_item_t& item = prs_cfg.prs_res_set_list.emplace_back();
    // The PRS Resource Set ID of a resource set is its index in the cell configuration.
    item.prs_res_set_id = static_cast<uint8_t>(set_id);
    item.scs            = bwp_cfg.scs;
    // The PRS Bandwidth IE counts PRBs in steps of the PRB granularity, starting from the minimum bandwidth.
    item.prs_bw =
        static_cast<uint8_t>(((res_set.bandwidth_prbs - prs_constants::MIN_PRBS) / prs_constants::PRB_GRANULARITY) + 1);
    item.start_prb           = res_set.start_prb;
    item.point_a             = cell_cfg.ran.dl_cfg_common.freq_info_dl.absolute_freq_point_a.value();
    item.comb_size           = to_underlying(res_set.comb_size);
    item.cp_type             = bwp_cfg.cp;
    item.res_set_periodicity = res_set.periodicity_slots;
    item.res_set_slot_offset = res_set.slot_offset;
    item.res_repeat_factor   = to_underlying(res_set.repetition_factor);
    item.res_time_gap        = to_underlying(res_set.time_gap);
    item.res_numof_symbols   = to_underlying(res_set.nof_symbols);
    item.prs_res_tx_pwr      = res_set.power_offset_db;

    if (res_set.muting_option1.has_value() or res_set.muting_option2.has_value()) {
      prs_muting_t& muting = item.prs_muting.emplace();
      if (res_set.muting_option1.has_value()) {
        muting.prs_muting_option1 = prs_muting_option1_t{
            .muting_pattern           = make_muting_pattern(res_set.muting_option1->muting_pattern),
            .muting_bit_repeat_factor = to_underlying(res_set.muting_option1->muting_bit_repetition_factor)};
      }
      if (res_set.muting_option2.has_value()) {
        muting.prs_muting_option2 =
            prs_muting_option2_t{.muting_pattern = make_muting_pattern(res_set.muting_option2->muting_pattern)};
      }
    }

    item.prs_res_list.reserve(res_set.resources.size());
    for (unsigned res_id = 0, nof_res = res_set.resources.size(); res_id != nof_res; ++res_id) {
      const prs_resource& res = res_set.resources[res_id];

      // The PRS Resource ID of a resource is its index in the cell configuration. QCL information is not configured.
      item.prs_res_list.push_back(prs_res_item_t{.prs_res_id        = static_cast<uint8_t>(res_id),
                                                 .seq_id            = res.sequence_id,
                                                 .re_offset         = res.re_offset,
                                                 .res_slot_offset   = res.slot_offset,
                                                 .res_symbol_offset = res.symbol_offset});
    }
  }

  return prs_cfg;
}

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
    if (not cell_cfg.ran.prs_cfg.resource_sets.empty()) {
      trp.prs_cfg = make_trp_prs_config(cell_cfg);
    }
    if (cell_cfg.trp_geo_coordinates.has_value()) {
      geographical_coordinates_t geo_coords;
      geo_coords.trp_position_definition_type = trp_position_direct_t{cell_cfg.trp_geo_coordinates.value()};
      trp.geo_coords                          = geo_coords;
    }
    trps.insert(std::make_pair(trp.trp_id, trp));
  }
}
