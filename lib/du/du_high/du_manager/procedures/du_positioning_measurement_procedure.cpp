// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_positioning_measurement_procedure.h"
#include "ocudu/adt/format.h"
#include <algorithm>
#include <cmath>

using namespace ocudu;
using namespace odu;

/// Finds the requested positioning measurement quantity of a given type, if present.
static const positioning_meas_quantity* find_pos_meas_quantity(const std::vector<positioning_meas_quantity>& quants,
                                                               pos_meas_type                                 type)
{
  auto it = std::find_if(quants.begin(), quants.end(), [type](const positioning_meas_quantity& quantity) {
    return quantity.meas_type == type;
  });
  return it == quants.end() ? nullptr : &(*it);
}

/// \brief Maps an Azimuth Angle of Arrival in degrees to the TS 38.473, Section 9.3.1.157 0.1 degree resolution
/// encoding, wrapping the value into its valid, circular range {0,...,3599} (i.e. [0, 360) degrees).
static uint16_t degrees_to_f1ap_azimuth_aoa(float degrees)
{
  static constexpr unsigned AZIMUTH_AOA_MODULUS = 3600U;

  float wrapped_deg = std::fmod(degrees, 360.0F);
  if (wrapped_deg < 0.0F) {
    wrapped_deg += 360.0F;
  }
  return static_cast<uint16_t>(std::lround(wrapped_deg * 10.0F) % AZIMUTH_AOA_MODULUS);
}

/// \brief Maps a Zenith Angle of Arrival in degrees to the TS 38.473, Section 9.3.1.157 0.1 degree resolution
/// encoding, clamping the value into its valid range {0,...,1799} (i.e. [0, 180) degrees).
static uint16_t degrees_to_f1ap_zenith_aoa(float degrees)
{
  static constexpr float MAX_ZENITH_AOA_DEG = 179.9F;

  const float clamped_deg = std::clamp(degrees, 0.0F, MAX_ZENITH_AOA_DEG);
  return static_cast<uint16_t>(std::lround(clamped_deg * 10.0F));
}

/// Maps RSRP in dBFS to L3 RSRP (reported value) as per TS 38.133, Table 10.1.6.1-1.
static uint8_t rsrp_dbfs_to_l3_rsrp_mapping(float rsrp_dbfs)
{
  // The value reported by the PHY maps [-144dBFs, 0dBFS] into [-156dBm, -12dBm]
  constexpr int dbfs_to_dbm_offset = -12;
  auto          rsrp_dbfs_dbm      = static_cast<int>(rsrp_dbfs) + dbfs_to_dbm_offset;
  // As per TS 38.133, Table 10.1.6.1-1, the encoding of RSRP for L3 reporting uses values [0, 126], which maps to
  // [-156dBm, -31dBm] as:
  // 0 corresponding to RSRP < -156dBm;
  // 1 corresponding to -156dBm <= RSRP < -155dBm;
  // ...
  // 126 corresponding to RSRP >= -31dBm;
  if (rsrp_dbfs_dbm < -156) {
    return 0; // RSRP < -156 dBm.
  }
  if (rsrp_dbfs_dbm >= -31) {
    return 126; // RSRP >= -31 dBm.
  }
  return static_cast<uint8_t>(rsrp_dbfs_dbm + 157);
}

positioning_measurement_procedure::positioning_measurement_procedure(const du_positioning_meas_request&     req_,
                                                                     du_cell_manager&                       du_cells_,
                                                                     du_ue_manager&                         ue_mng_,
                                                                     const du_manager_params&               du_params_,
                                                                     const std::map<trp_id_t, du_trp_info>& trps_) :
  req(req_), du_cells(du_cells_), ue_mng(ue_mng_), du_params(du_params_), trps(trps_)
{
}

void positioning_measurement_procedure::operator()(coro_context<async_task<du_positioning_meas_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Request MAC measurement.
  CORO_AWAIT(handle_mac_meas_request());

  // Prepare and send F1AP response.
  CORO_RETURN(prepare_f1ap_response());
}

async_task<void> positioning_measurement_procedure::handle_mac_meas_request()
{
  ocudu_assert(not req.srs_carriers.empty(), "Positioning measurement request does not contain SRS carriers.");

  return launch_async([this, i = 0U, trp_it = trps.end(), cell_index = INVALID_DU_CELL_INDEX](
                          coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Prepare MAC positioning request.
    for (; i != req.trp_meas_req_list.size(); ++i) {
      // Fetch cell index from TRP contained in the list.
      // TODO: Once the F1AP we defined which elements of Positioning measurement request are mandatory, review this
      //       logic for fetching the TRP from the saves ones vs the one in the request.
      trp_it     = trps.find(req.trp_meas_req_list[i].trp_id);
      cell_index = du_cells.get_cell_index(trp_it != trps.end() and trp_it->second.cgi.has_value()
                                               ? trp_it->second.cgi.value()
                                               : req.trp_meas_req_list[i].cgi.value());

      // Request MAC cell for new positioning measurement.
      // NOTE: we assume there is only one SRS Carrier in the request, which holds an SRS-Config which is
      // common to all TRPs.
      // TODO: Review this assumption once the F1AP we defined which elements of Positioning measurement request are
      //       mandatory.
      mac_req.cells.push_back(prepare_mac_cell_positioning_request(cell_index, req.srs_carriers[0]));
    }

    // NOTE: we implicitly map mac_resps[i] to req.trp_meas_req_list[i].trp_id.
    // TODO: Review this assumption once the F1AP we defined which elements of Positioning measurement request are
    //       mandatory.
    CORO_AWAIT_VALUE(mac_resp,
                     du_params.mac.mgr.get_positioning_handler().handle_positioning_measurement_request(mac_req));

    CORO_RETURN();
  });
}

du_positioning_meas_response positioning_measurement_procedure::prepare_f1ap_response()
{
  ocudu_assert(not req.pos_meas_quants.empty(), "Positioning measurement quantities are empty.");
  ocudu_assert(mac_resp.cell_results.size() == req.trp_meas_req_list.size(),
               "MAC responses size does not match TRP measurement request list size.");

  du_positioning_meas_response resp;
  resp.pos_meas_list.reserve(req.trp_meas_req_list.size());
  // The MAC response mac_resps[i] corresponds to the TRP in trp_meas_req_list[i]; \ref handle_mac_meas_request().
  for (unsigned trp_idx = 0, nof_trps = req.trp_meas_req_list.size(); trp_idx != nof_trps; ++trp_idx) {
    pos_meas_result meas_res{};
    // TRP ID.
    meas_res.trp_id = req.trp_meas_req_list[trp_idx].trp_id;

    ocudu_assert(mac_resp.cell_results[trp_idx].ul_srs_pos_meass.size() == 1,
                 "Currently we only support 1 measurement result (SRS indication) per TRP.");
    static constexpr size_t srs_ind_meas_idx = 0U;
    const auto&             mac_meas         = mac_resp.cell_results[trp_idx].ul_srs_pos_meass[srs_ind_meas_idx];

    // Check what reports have been requested, and whether MAC was able to produce them.
    const positioning_meas_quantity* rtoa_quant  = find_pos_meas_quantity(req.pos_meas_quants, pos_meas_type::ul_rtoa);
    const bool                       rtoa_report = rtoa_quant != nullptr and mac_meas.ul_rtoa.has_value();
    const bool rsrp_report = find_pos_meas_quantity(req.pos_meas_quants, pos_meas_type::ul_srs_rsrp) != nullptr and
                             mac_meas.rsrp_dbfs.has_value();
    const bool aoa_report = find_pos_meas_quantity(req.pos_meas_quants, pos_meas_type::ul_aoa) != nullptr and
                            mac_meas.azimuth_aoa_deg.has_value();

    // NOTE: The UL-RTOA report (if requested) comes first, followed by the UL-RSRP report and the UL-AoA report (if
    // requested).
    meas_res.results.reserve((rtoa_report ? 1U : 0U) + (rsrp_report ? 1U : 0U) + (aoa_report ? 1U : 0U));
    if (rtoa_report) {
      ocudu_assert(rtoa_quant->granularity_factor.has_value(),
                   "No granularity factor provided for UL-RTOA measurement.");
      const uint8_t granularity = rtoa_quant->granularity_factor.value();
      meas_res.results.push_back(
          pos_meas_result_ul_rtoa{.granularity = granularity, .ul_rtoa = mac_meas.ul_rtoa->to_ul_rtoa(granularity)});
    }
    if (rsrp_report) {
      meas_res.results.push_back(
          pos_meas_result_ul_rsrp{.ul_rsrp = rsrp_dbfs_to_l3_rsrp_mapping(mac_meas.rsrp_dbfs.value())});
    }
    if (aoa_report) {
      pos_meas_result_ul_aoa aoa_result{.azimuth_aoa = degrees_to_f1ap_azimuth_aoa(*mac_meas.azimuth_aoa_deg)};
      if (mac_meas.zenith_aoa_deg.has_value()) {
        aoa_result.zenith_aoa = degrees_to_f1ap_zenith_aoa(*mac_meas.zenith_aoa_deg);
      }
      meas_res.results.push_back(aoa_result);
    }
    meas_res.sl_rx = mac_resp.sl_rx;

    resp.pos_meas_list.push_back(meas_res);
  }

  return resp;
}

bool srs_config_matches(const srs_config& ue_cfg, const srs_config& pos_cfg)
{
  if (ue_cfg.srs_res_list.size() != pos_cfg.srs_res_list.size() or
      ue_cfg.srs_res_set_list.size() != pos_cfg.srs_res_set_list.size()) {
    return false;
  }

  for (unsigned i = 0, e = ue_cfg.srs_res_list.size(); i != e; ++i) {
    const auto& srs_ue = ue_cfg.srs_res_list[i];
    const auto& pos_ue = pos_cfg.srs_res_list[i];

    // We do not use the operator!= directly on pos_ue, because some parameters of the SRS Config are not set in the
    // F1AP positioning request.
    auto cpy_pos_ue                  = pos_ue;
    cpy_pos_ue.id.cell_res_id        = srs_ue.id.cell_res_id;
    cpy_pos_ue.ptrs_port             = srs_ue.ptrs_port;
    cpy_pos_ue.spatial_relation_info = srs_ue.spatial_relation_info;

    if (cpy_pos_ue != srs_ue) {
      return false;
    }
  }

  return true;
}

mac_positioning_measurement_request::cell_info
positioning_measurement_procedure::prepare_mac_cell_positioning_request(du_cell_index_t    cell_index,
                                                                        const srs_carrier& carrier)
{
  mac_positioning_measurement_request::cell_info ret;
  ret.cell_index  = cell_index;
  ret.srs_to_meas = carrier.srs_cfg;

  // Matches SRS resources that need to be measured to existing UEs' resources.
  for (const auto& u : ue_mng.get_du_ues()) {
    for (const auto& cell_entry : u.resources->cell_group.cells) {
      const auto& c = cell_entry.second;
      if (c.serv_cell_cfg.cell_index != cell_index or not c.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value()) {
        continue;
      }
      const srs_config& ue_srs_cfg = c.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();

      if (srs_config_matches(ue_srs_cfg, carrier.srs_cfg)) {
        // Found a UE with matching SRSConfig
        ret.ue_index = u.ue_index;
        ret.rnti     = u.rnti;
        return ret;
      }
    }
  }

  return ret;
}
