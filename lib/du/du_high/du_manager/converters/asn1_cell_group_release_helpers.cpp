// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "asn1_cell_group_release_helpers.h"
#include <algorithm>

using namespace ocudu;
using namespace ocudu::odu;
using namespace asn1::rrc_nr;

/// Adds to the release list the ID of each element that the source list contains and the output list does not contain.
template <typename AddModList, typename ReleaseList, typename GetId>
static void calculate_list_release(ReleaseList&      out_release_list,
                                   const AddModList& out_add_mod_list,
                                   const AddModList& src_add_mod_list,
                                   const GetId&      id_func)
{
  for (const auto& src_elem : src_add_mod_list) {
    const auto id = id_func(src_elem);
    if (std::any_of(
            out_add_mod_list.begin(), out_add_mod_list.end(), [&](const auto& e) { return id_func(e) == id; })) {
      continue;
    }
    if (std::find(out_release_list.begin(), out_release_list.end(), id) != out_release_list.end()) {
      continue;
    }
    out_release_list.push_back(id);
  }
}

/// \brief Releases a SetupRelease field that the source sets up and the output does not set up.
/// \return True if both the source and the output set up the field, so the caller can compare their content.
template <typename T>
static bool calculate_field_release(bool&                           out_present,
                                    asn1::setup_release_c<T>&       out,
                                    bool                            src_present,
                                    const asn1::setup_release_c<T>& src)
{
  if (not src_present or not src.is_setup()) {
    return false;
  }
  if (not out_present) {
    out_present = true;
    out.set_release();
    return false;
  }
  return out.is_setup();
}

/// Releases a SetupRelease field of an extension group that the source sets up and the output does not set up.
template <typename T>
static void calculate_field_release(asn1::copy_ptr<asn1::setup_release_c<T>>&       out,
                                    const asn1::copy_ptr<asn1::setup_release_c<T>>& src)
{
  if (not src.is_present() or not src->is_setup() or out.is_present()) {
    return;
  }
  out.set_present();
  out->set_release();
}

static void calculate_pdcch_config_release(pdcch_cfg_s& out, const pdcch_cfg_s& src)
{
  calculate_list_release(out.coreset_to_release_list,
                         out.coreset_to_add_mod_list,
                         src.coreset_to_add_mod_list,
                         [](const coreset_s& e) { return e.coreset_id; });
  calculate_list_release(out.search_spaces_to_release_list,
                         out.search_spaces_to_add_mod_list,
                         src.search_spaces_to_add_mod_list,
                         [](const search_space_s& e) { return e.search_space_id; });
  calculate_field_release(out.dl_preemption_present, out.dl_preemption, src.dl_preemption_present, src.dl_preemption);
  calculate_field_release(out.tpc_pusch_present, out.tpc_pusch, src.tpc_pusch_present, src.tpc_pusch);
  calculate_field_release(out.tpc_pucch_present, out.tpc_pucch, src.tpc_pucch_present, src.tpc_pucch);
  calculate_field_release(out.tpc_srs_present, out.tpc_srs, src.tpc_srs_present, src.tpc_srs);
}

static void calculate_pdsch_config_release(pdsch_cfg_s& out, const pdsch_cfg_s& src)
{
  calculate_field_release(out.dmrs_dl_for_pdsch_map_type_a_present,
                          out.dmrs_dl_for_pdsch_map_type_a,
                          src.dmrs_dl_for_pdsch_map_type_a_present,
                          src.dmrs_dl_for_pdsch_map_type_a);
  calculate_field_release(out.dmrs_dl_for_pdsch_map_type_b_present,
                          out.dmrs_dl_for_pdsch_map_type_b,
                          src.dmrs_dl_for_pdsch_map_type_b_present,
                          src.dmrs_dl_for_pdsch_map_type_b);
  calculate_list_release(out.tci_states_to_release_list,
                         out.tci_states_to_add_mod_list,
                         src.tci_states_to_add_mod_list,
                         [](const tci_state_s& e) { return e.tci_state_id; });
  calculate_field_release(out.pdsch_time_domain_alloc_list_present,
                          out.pdsch_time_domain_alloc_list,
                          src.pdsch_time_domain_alloc_list_present,
                          src.pdsch_time_domain_alloc_list);
  calculate_list_release(out.rate_match_pattern_to_release_list,
                         out.rate_match_pattern_to_add_mod_list,
                         src.rate_match_pattern_to_add_mod_list,
                         [](const rate_match_pattern_s& e) { return e.rate_match_pattern_id; });
  calculate_list_release(out.zp_csi_rs_res_to_release_list,
                         out.zp_csi_rs_res_to_add_mod_list,
                         src.zp_csi_rs_res_to_add_mod_list,
                         [](const zp_csi_rs_res_s& e) { return e.zp_csi_rs_res_id; });
  calculate_list_release(out.aperiodic_zp_csi_rs_res_sets_to_release_list,
                         out.aperiodic_zp_csi_rs_res_sets_to_add_mod_list,
                         src.aperiodic_zp_csi_rs_res_sets_to_add_mod_list,
                         [](const zp_csi_rs_res_set_s& e) { return e.zp_csi_rs_res_set_id; });
  calculate_list_release(out.sp_zp_csi_rs_res_sets_to_release_list,
                         out.sp_zp_csi_rs_res_sets_to_add_mod_list,
                         src.sp_zp_csi_rs_res_sets_to_add_mod_list,
                         [](const zp_csi_rs_res_set_s& e) { return e.zp_csi_rs_res_set_id; });
  calculate_field_release(out.p_zp_csi_rs_res_set_present,
                          out.p_zp_csi_rs_res_set,
                          src.p_zp_csi_rs_res_set_present,
                          src.p_zp_csi_rs_res_set);
  calculate_field_release(out.pdsch_time_domain_alloc_list_r16, src.pdsch_time_domain_alloc_list_r16);
  calculate_field_release(out.repeat_scheme_cfg_r16, src.repeat_scheme_cfg_r16);
}

static void calculate_radio_link_monitoring_config_release(radio_link_monitoring_cfg_s&       out,
                                                           const radio_link_monitoring_cfg_s& src)
{
  calculate_list_release(out.fail_detection_res_to_release_list,
                         out.fail_detection_res_to_add_mod_list,
                         src.fail_detection_res_to_add_mod_list,
                         [](const radio_link_monitoring_rs_s& e) { return e.radio_link_monitoring_rs_id; });
}

static void calculate_bwp_dl_dedicated_release(bwp_dl_ded_s& out, const bwp_dl_ded_s& src)
{
  if (calculate_field_release(out.pdcch_cfg_present, out.pdcch_cfg, src.pdcch_cfg_present, src.pdcch_cfg)) {
    calculate_pdcch_config_release(out.pdcch_cfg.setup(), src.pdcch_cfg.setup());
  }
  if (calculate_field_release(out.pdsch_cfg_present, out.pdsch_cfg, src.pdsch_cfg_present, src.pdsch_cfg)) {
    calculate_pdsch_config_release(out.pdsch_cfg.setup(), src.pdsch_cfg.setup());
  }
  calculate_field_release(out.sps_cfg_present, out.sps_cfg, src.sps_cfg_present, src.sps_cfg);
  if (calculate_field_release(out.radio_link_monitoring_cfg_present,
                              out.radio_link_monitoring_cfg,
                              src.radio_link_monitoring_cfg_present,
                              src.radio_link_monitoring_cfg)) {
    calculate_radio_link_monitoring_config_release(out.radio_link_monitoring_cfg.setup(),
                                                   src.radio_link_monitoring_cfg.setup());
  }
}

static void calculate_pucch_config_release(pucch_cfg_s& out, const pucch_cfg_s& src)
{
  calculate_list_release(out.res_set_to_release_list,
                         out.res_set_to_add_mod_list,
                         src.res_set_to_add_mod_list,
                         [](const pucch_res_set_s& e) { return e.pucch_res_set_id; });
  calculate_list_release(out.res_to_release_list,
                         out.res_to_add_mod_list,
                         src.res_to_add_mod_list,
                         [](const pucch_res_s& e) { return e.pucch_res_id; });
  calculate_field_release(out.format1_present, out.format1, src.format1_present, src.format1);
  calculate_field_release(out.format2_present, out.format2, src.format2_present, src.format2);
  calculate_field_release(out.format3_present, out.format3, src.format3_present, src.format3);
  calculate_field_release(out.format4_present, out.format4, src.format4_present, src.format4);
  calculate_list_release(out.sched_request_res_to_release_list,
                         out.sched_request_res_to_add_mod_list,
                         src.sched_request_res_to_add_mod_list,
                         [](const sched_request_res_cfg_s& e) { return e.sched_request_res_id; });
  calculate_list_release(out.spatial_relation_info_to_release_list,
                         out.spatial_relation_info_to_add_mod_list,
                         src.spatial_relation_info_to_add_mod_list,
                         [](const pucch_spatial_relation_info_s& e) { return e.pucch_spatial_relation_info_id; });
}

static void calculate_pusch_power_control_release(pusch_pwr_ctrl_s& out, const pusch_pwr_ctrl_s& src)
{
  calculate_list_release(out.pathloss_ref_rs_to_release_list,
                         out.pathloss_ref_rs_to_add_mod_list,
                         src.pathloss_ref_rs_to_add_mod_list,
                         [](const pusch_pathloss_ref_rs_s& e) { return e.pusch_pathloss_ref_rs_id; });
  calculate_list_release(out.sri_pusch_map_to_release_list,
                         out.sri_pusch_map_to_add_mod_list,
                         src.sri_pusch_map_to_add_mod_list,
                         [](const sri_pusch_pwr_ctrl_s& e) { return e.sri_pusch_pwr_ctrl_id; });
}

static void calculate_pusch_config_release(pusch_cfg_s& out, const pusch_cfg_s& src)
{
  calculate_field_release(out.dmrs_ul_for_pusch_map_type_a_present,
                          out.dmrs_ul_for_pusch_map_type_a,
                          src.dmrs_ul_for_pusch_map_type_a_present,
                          src.dmrs_ul_for_pusch_map_type_a);
  calculate_field_release(out.dmrs_ul_for_pusch_map_type_b_present,
                          out.dmrs_ul_for_pusch_map_type_b,
                          src.dmrs_ul_for_pusch_map_type_b_present,
                          src.dmrs_ul_for_pusch_map_type_b);
  if (out.pusch_pwr_ctrl_present and src.pusch_pwr_ctrl_present) {
    calculate_pusch_power_control_release(out.pusch_pwr_ctrl, src.pusch_pwr_ctrl);
  }
  calculate_field_release(out.pusch_time_domain_alloc_list_present,
                          out.pusch_time_domain_alloc_list,
                          src.pusch_time_domain_alloc_list_present,
                          src.pusch_time_domain_alloc_list);
  calculate_field_release(out.uci_on_pusch_present, out.uci_on_pusch, src.uci_on_pusch_present, src.uci_on_pusch);
  calculate_field_release(out.pusch_time_domain_alloc_list_dci_0_1_r16, src.pusch_time_domain_alloc_list_dci_0_1_r16);
}

static void calculate_srs_config_release(srs_cfg_s& out, const srs_cfg_s& src)
{
  calculate_list_release(out.srs_res_set_to_release_list,
                         out.srs_res_set_to_add_mod_list,
                         src.srs_res_set_to_add_mod_list,
                         [](const srs_res_set_s& e) { return e.srs_res_set_id; });
  calculate_list_release(out.srs_res_to_release_list,
                         out.srs_res_to_add_mod_list,
                         src.srs_res_to_add_mod_list,
                         [](const srs_res_s& e) { return e.srs_res_id; });
}

static void calculate_bwp_ul_dedicated_release(bwp_ul_ded_s& out, const bwp_ul_ded_s& src)
{
  if (calculate_field_release(out.pucch_cfg_present, out.pucch_cfg, src.pucch_cfg_present, src.pucch_cfg)) {
    calculate_pucch_config_release(out.pucch_cfg.setup(), src.pucch_cfg.setup());
  }
  if (calculate_field_release(out.pusch_cfg_present, out.pusch_cfg, src.pusch_cfg_present, src.pusch_cfg)) {
    calculate_pusch_config_release(out.pusch_cfg.setup(), src.pusch_cfg.setup());
  }
  calculate_field_release(out.cfg_grant_cfg_present, out.cfg_grant_cfg, src.cfg_grant_cfg_present, src.cfg_grant_cfg);
  if (calculate_field_release(out.srs_cfg_present, out.srs_cfg, src.srs_cfg_present, src.srs_cfg)) {
    calculate_srs_config_release(out.srs_cfg.setup(), src.srs_cfg.setup());
  }
  calculate_field_release(out.beam_fail_recovery_cfg_present,
                          out.beam_fail_recovery_cfg,
                          src.beam_fail_recovery_cfg_present,
                          src.beam_fail_recovery_cfg);
}

static void calculate_csi_meas_config_release(csi_meas_cfg_s& out, const csi_meas_cfg_s& src)
{
  calculate_list_release(out.nzp_csi_rs_res_to_release_list,
                         out.nzp_csi_rs_res_to_add_mod_list,
                         src.nzp_csi_rs_res_to_add_mod_list,
                         [](const nzp_csi_rs_res_s& e) { return e.nzp_csi_rs_res_id; });
  calculate_list_release(out.nzp_csi_rs_res_set_to_release_list,
                         out.nzp_csi_rs_res_set_to_add_mod_list,
                         src.nzp_csi_rs_res_set_to_add_mod_list,
                         [](const nzp_csi_rs_res_set_s& e) { return e.nzp_csi_res_set_id; });
  calculate_list_release(out.csi_im_res_to_release_list,
                         out.csi_im_res_to_add_mod_list,
                         src.csi_im_res_to_add_mod_list,
                         [](const csi_im_res_s& e) { return e.csi_im_res_id; });
  calculate_list_release(out.csi_im_res_set_to_release_list,
                         out.csi_im_res_set_to_add_mod_list,
                         src.csi_im_res_set_to_add_mod_list,
                         [](const csi_im_res_set_s& e) { return e.csi_im_res_set_id; });
  calculate_list_release(out.csi_ssb_res_set_to_release_list,
                         out.csi_ssb_res_set_to_add_mod_list,
                         src.csi_ssb_res_set_to_add_mod_list,
                         [](const csi_ssb_res_set_s& e) { return e.csi_ssb_res_set_id; });
  calculate_list_release(out.csi_res_cfg_to_release_list,
                         out.csi_res_cfg_to_add_mod_list,
                         src.csi_res_cfg_to_add_mod_list,
                         [](const csi_res_cfg_s& e) { return e.csi_res_cfg_id; });
  calculate_list_release(out.csi_report_cfg_to_release_list,
                         out.csi_report_cfg_to_add_mod_list,
                         src.csi_report_cfg_to_add_mod_list,
                         [](const csi_report_cfg_s& e) { return e.report_cfg_id; });
  calculate_field_release(out.aperiodic_trigger_state_list_present,
                          out.aperiodic_trigger_state_list,
                          src.aperiodic_trigger_state_list_present,
                          src.aperiodic_trigger_state_list);
  calculate_field_release(out.semi_persistent_on_pusch_trigger_state_list_present,
                          out.semi_persistent_on_pusch_trigger_state_list,
                          src.semi_persistent_on_pusch_trigger_state_list_present,
                          src.semi_persistent_on_pusch_trigger_state_list);
}

static void calculate_uplink_config_release(ul_cfg_s& out, const ul_cfg_s& src)
{
  if (out.init_ul_bwp_present and src.init_ul_bwp_present) {
    calculate_bwp_ul_dedicated_release(out.init_ul_bwp, src.init_ul_bwp);
  }
  calculate_list_release(out.ul_bwp_to_release_list,
                         out.ul_bwp_to_add_mod_list,
                         src.ul_bwp_to_add_mod_list,
                         [](const bwp_ul_s& e) { return e.bwp_id; });
  if (calculate_field_release(out.pusch_serving_cell_cfg_present,
                              out.pusch_serving_cell_cfg,
                              src.pusch_serving_cell_cfg_present,
                              src.pusch_serving_cell_cfg)) {
    auto&       out_pusch = out.pusch_serving_cell_cfg.setup();
    const auto& src_pusch = src.pusch_serving_cell_cfg.setup();
    calculate_field_release(out_pusch.code_block_group_tx_present,
                            out_pusch.code_block_group_tx,
                            src_pusch.code_block_group_tx_present,
                            src_pusch.code_block_group_tx);
    calculate_field_release(out_pusch.ul_harq_mode_r17, src_pusch.ul_harq_mode_r17);
  }
  calculate_field_release(
      out.carrier_switching_present, out.carrier_switching, src.carrier_switching_present, src.carrier_switching);
}

static void calculate_serving_cell_config_release(serving_cell_cfg_s& out, const serving_cell_cfg_s& src)
{
  if (out.init_dl_bwp_present and src.init_dl_bwp_present) {
    calculate_bwp_dl_dedicated_release(out.init_dl_bwp, src.init_dl_bwp);
  }
  calculate_list_release(out.dl_bwp_to_release_list,
                         out.dl_bwp_to_add_mod_list,
                         src.dl_bwp_to_add_mod_list,
                         [](const bwp_dl_s& e) { return e.bwp_id; });
  if (out.ul_cfg_present and src.ul_cfg_present) {
    calculate_uplink_config_release(out.ul_cfg, src.ul_cfg);
  }
  calculate_field_release(out.pdcch_serving_cell_cfg_present,
                          out.pdcch_serving_cell_cfg,
                          src.pdcch_serving_cell_cfg_present,
                          src.pdcch_serving_cell_cfg);
  if (calculate_field_release(out.pdsch_serving_cell_cfg_present,
                              out.pdsch_serving_cell_cfg,
                              src.pdsch_serving_cell_cfg_present,
                              src.pdsch_serving_cell_cfg)) {
    auto&       out_pdsch = out.pdsch_serving_cell_cfg.setup();
    const auto& src_pdsch = src.pdsch_serving_cell_cfg.setup();
    calculate_field_release(out_pdsch.code_block_group_tx_present,
                            out_pdsch.code_block_group_tx,
                            src_pdsch.code_block_group_tx_present,
                            src_pdsch.code_block_group_tx);
    calculate_field_release(out_pdsch.dl_harq_feedback_disabled_r17, src_pdsch.dl_harq_feedback_disabled_r17);
  }
  if (calculate_field_release(out.csi_meas_cfg_present, out.csi_meas_cfg, src.csi_meas_cfg_present, src.csi_meas_cfg)) {
    calculate_csi_meas_config_release(out.csi_meas_cfg.setup(), src.csi_meas_cfg.setup());
  }
}

static void calculate_mac_cell_group_config_release(mac_cell_group_cfg_s& out, const mac_cell_group_cfg_s& src)
{
  calculate_field_release(out.drx_cfg_present, out.drx_cfg, src.drx_cfg_present, src.drx_cfg);
  if (out.sched_request_cfg_present and src.sched_request_cfg_present) {
    calculate_list_release(out.sched_request_cfg.sched_request_to_release_list,
                           out.sched_request_cfg.sched_request_to_add_mod_list,
                           src.sched_request_cfg.sched_request_to_add_mod_list,
                           [](const sched_request_to_add_mod_s& e) { return e.sched_request_id; });
  }
  if (out.tag_cfg_present and src.tag_cfg_present) {
    calculate_list_release(out.tag_cfg.tag_to_release_list,
                           out.tag_cfg.tag_to_add_mod_list,
                           src.tag_cfg.tag_to_add_mod_list,
                           [](const tag_s& e) { return e.tag_id; });
  }
  calculate_field_release(out.phr_cfg_present, out.phr_cfg, src.phr_cfg_present, src.phr_cfg);
  calculate_field_release(out.tar_cfg_r17, src.tar_cfg_r17);
}

/// Checks if two RLC bearers serve the same radio bearer. A bearer without servedRadioBearer keeps its radio bearer.
static bool serve_same_radio_bearer(const rlc_bearer_cfg_s& out, const rlc_bearer_cfg_s& src)
{
  if (not out.served_radio_bearer_present or not src.served_radio_bearer_present) {
    return true;
  }
  const auto& out_rb = out.served_radio_bearer;
  const auto& src_rb = src.served_radio_bearer;
  if (out_rb.type() != src_rb.type()) {
    return false;
  }
  return out_rb.type() == rlc_bearer_cfg_s::served_radio_bearer_c_::types::srb_id ? out_rb.srb_id() == src_rb.srb_id()
                                                                                  : out_rb.drb_id() == src_rb.drb_id();
}

void ocudu::odu::calculate_cell_group_config_release(cell_group_cfg_s& out, const cell_group_cfg_s& src)
{
  calculate_list_release(out.rlc_bearer_to_release_list,
                         out.rlc_bearer_to_add_mod_list,
                         src.rlc_bearer_to_add_mod_list,
                         [](const rlc_bearer_cfg_s& e) { return e.lc_ch_id; });

  if (out.mac_cell_group_cfg_present and src.mac_cell_group_cfg_present) {
    calculate_mac_cell_group_config_release(out.mac_cell_group_cfg, src.mac_cell_group_cfg);
  }

  if (out.phys_cell_group_cfg_present and src.phys_cell_group_cfg_present) {
    calculate_field_release(out.phys_cell_group_cfg.cs_rnti_present,
                            out.phys_cell_group_cfg.cs_rnti,
                            src.phys_cell_group_cfg.cs_rnti_present,
                            src.phys_cell_group_cfg.cs_rnti);
  }

  if (out.sp_cell_cfg_present and src.sp_cell_cfg_present) {
    calculate_field_release(out.sp_cell_cfg.rlf_timers_and_consts_present,
                            out.sp_cell_cfg.rlf_timers_and_consts,
                            src.sp_cell_cfg.rlf_timers_and_consts_present,
                            src.sp_cell_cfg.rlf_timers_and_consts);
    if (out.sp_cell_cfg.sp_cell_cfg_ded_present and src.sp_cell_cfg.sp_cell_cfg_ded_present) {
      calculate_serving_cell_config_release(out.sp_cell_cfg.sp_cell_cfg_ded, src.sp_cell_cfg.sp_cell_cfg_ded);
    }
  }

  calculate_list_release(out.scell_to_release_list,
                         out.scell_to_add_mod_list,
                         src.scell_to_add_mod_list,
                         [](const scell_cfg_s& e) { return e.scell_idx; });
}

void ocudu::odu::calculate_rlc_bearer_reestablishment(cell_group_cfg_s& out, const cell_group_cfg_s& src)
{
  for (auto& bearer : out.rlc_bearer_to_add_mod_list) {
    const auto* src_bearer =
        std::find_if(src.rlc_bearer_to_add_mod_list.begin(),
                     src.rlc_bearer_to_add_mod_list.end(),
                     [&bearer](const rlc_bearer_cfg_s& e) { return e.lc_ch_id == bearer.lc_ch_id; });
    if (src_bearer == src.rlc_bearer_to_add_mod_list.end()) {
      continue;
    }
    if (serve_same_radio_bearer(bearer, *src_bearer)) {
      bearer.rlc_cfg_present         = false;
      bearer.reestablish_rlc_present = true;
      continue;
    }
    if (std::find(out.rlc_bearer_to_release_list.begin(), out.rlc_bearer_to_release_list.end(), bearer.lc_ch_id) ==
        out.rlc_bearer_to_release_list.end()) {
      out.rlc_bearer_to_release_list.push_back(bearer.lc_ch_id);
    }
  }
}
