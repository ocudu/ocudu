// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_high_config_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "du_high_config.h"
#include "ntn/du_high_ntn_config_translators.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_cell_config_validation.h"
#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/du/du_high/du_qos_config_helpers.h"
#include "ocudu/du/du_update_config_helpers.h"
#include "ocudu/ran/duplex_mode.h"
#include "ocudu/ran/pdcch/pdcch_candidates.h"
#include "ocudu/ran/prach/prach_configuration.h"
#include "ocudu/ran/pucch/pucch_info.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/ran/sib/cell_reselection.h"
#include "ocudu/ran/sib/sib_helper.h"
#include "ocudu/ran/ssb/ssb_helper.h"
#include "ocudu/rlc/rlc_srb_config_factory.h"
#include "ocudu/scheduler/config/cell_config_builder_params.h"
#include "ocudu/scheduler/config/csi_helper.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/config/scheduler_expert_config_validator.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include <algorithm>

using namespace ocudu;

static tdd_ul_dl_config_common generate_tdd_pattern(subcarrier_spacing scs, const du_high_unit_tdd_ul_dl_config& config)
{
  tdd_ul_dl_config_common out;
  out.ref_scs = scs;

  out.pattern1.dl_ul_tx_period_nof_slots = config.pattern1.dl_ul_period_slots;
  out.pattern1.nof_dl_slots              = config.pattern1.nof_dl_slots;
  out.pattern1.nof_dl_symbols            = config.pattern1.nof_dl_symbols;
  out.pattern1.nof_ul_slots              = config.pattern1.nof_ul_slots;
  out.pattern1.nof_ul_symbols            = config.pattern1.nof_ul_symbols;

  if (config.pattern2.has_value()) {
    out.pattern2.emplace();
    out.pattern2->dl_ul_tx_period_nof_slots = config.pattern2->dl_ul_period_slots;
    out.pattern2->nof_dl_slots              = config.pattern2->nof_dl_slots;
    out.pattern2->nof_dl_symbols            = config.pattern2->nof_dl_symbols;
    out.pattern2->nof_ul_slots              = config.pattern2->nof_ul_slots;
    out.pattern2->nof_ul_symbols            = config.pattern2->nof_ul_symbols;
  }
  return out;
}

static pcch_config generate_pcch_config(const du_high_unit_base_cell_config& cell)
{
  pcch_config cfg{};
  switch (cell.paging_cfg.default_paging_cycle) {
    case 32:
      cfg.default_paging_cycle = paging_cycle::rf32;
      break;
    case 64:
      cfg.default_paging_cycle = paging_cycle::rf64;
      break;
    case 128:
      cfg.default_paging_cycle = paging_cycle::rf128;
      break;
    case 256:
      cfg.default_paging_cycle = paging_cycle::rf256;
      break;
    default:
      report_error(
          "Invalid default paging cycle={} for cell with pci={}\n", cell.paging_cfg.default_paging_cycle, cell.pci);
  }
  cfg.nof_pf = cell.paging_cfg.nof_pf;
  if (cell.paging_cfg.pf_offset > (static_cast<unsigned>(cfg.nof_pf) - 1)) {
    report_error("Invalid paging frame offset value={} for cell with pci={}. Value must be less than or equal to {}\n",
                 cell.paging_cfg.pf_offset,
                 cell.pci,
                 (static_cast<unsigned>(cfg.nof_pf) - 1));
  }
  cfg.paging_frame_offset = cell.paging_cfg.pf_offset;
  switch (cell.paging_cfg.nof_po_per_pf) {
    case 1:
      cfg.ns = pcch_config::nof_po_per_pf::one;
      break;
    case 2:
      cfg.ns = pcch_config::nof_po_per_pf::two;
      break;
    case 4:
      cfg.ns = pcch_config::nof_po_per_pf::four;
      break;
    default:
      report_error("Invalid number of paging occasions per paging frame={} for cell with pci={}\n",
                   cell.paging_cfg.nof_po_per_pf,
                   cell.pci);
  }

  return cfg;
}

static sib2_info create_sib2_info(const du_high_unit_sib_config::sib2_config& config)
{
  sib2_info sib2;
  sib2.q_hyst                    = static_cast<q_hyst_t>(config.q_hyst);
  sib2.thresh_serving_low_p      = config.thresh_serving_low_p;
  sib2.cell_reselection_priority = config.cell_reselection_priority;
  sib2.q_rx_lev_min              = config.q_rx_lev_min / 2;
  sib2.s_intra_search_p          = config.s_intra_search_p / 2;
  sib2.t_reselection_nr          = config.t_reselection_nr;
  return sib2;
}

static sib3_info create_sib3_info(const du_high_unit_sib_config::sib3_config& config)
{
  sib3_info sib3;
  for (const auto& neigh : config.intra_freq_neigh_cell_list) {
    intra_freq_neigh_cell_info info;
    info.pci           = neigh.pci;
    info.q_offset_cell = static_cast<q_offset_range_t>(neigh.q_offset_cell);
    sib3.intra_freq_neigh_cell_list.push_back(info);
  }
  for (const auto& range_cfg : config.intra_freq_excluded_cell_list) {
    pci_range_t range;
    range.start = range_cfg.start;
    range.size  = static_cast<pci_range_t::range_t>(range_cfg.size);
    sib3.intra_freq_excluded_cell_list.push_back(range);
  }
  return sib3;
}

static sib4_info create_sib4_info(const du_high_unit_sib_config::sib4_config& config)
{
  sib4_info sib4;
  for (const auto& carrier : config.inter_freq_carrier_freq_list) {
    inter_freq_carrier_freq_info info;
    info.arfcn                      = carrier.arfcn;
    info.ssb_scs                    = carrier.ssb_scs;
    info.derive_ssb_index_from_cell = carrier.derive_ssb_index_from_cell;
    info.q_rx_lev_min               = carrier.q_rx_lev_min / 2;
    info.thresh_x_high_p            = carrier.thresh_x_high_p / 2;
    info.thresh_x_low_p             = carrier.thresh_x_low_p / 2;
    info.q_offset_freq              = static_cast<q_offset_range_t>(carrier.q_offset_freq);
    sib4.inter_freq_carrier_freq_list.push_back(info);
  }
  return sib4;
}

static sib5_info create_sib5_info(const du_high_unit_sib_config::sib5_config& config)
{
  sib5_info sib5;
  sib5.t_reselection_eutra = config.t_reselection_eutra;
  for (const auto& carrier : config.carrier_freq_list_eutra) {
    carrier_freq_eutra info;
    info.earfcn                    = carrier.earfcn;
    info.allowed_meas_bandwidth    = static_cast<eutra_allowed_meas_bandwidth_t>(carrier.allowed_meas_bandwidth);
    info.presence_antenna_port1    = carrier.presence_antenna_port1;
    info.cell_reselection_priority = carrier.cell_reselection_priority;
    info.thresh_x_high             = carrier.thresh_x_high / 2;
    info.thresh_x_low              = carrier.thresh_x_low / 2;
    info.q_rx_lev_min              = carrier.q_rx_lev_min / 2;
    info.q_qual_min                = carrier.q_qual_min;
    info.p_max_eutra               = carrier.p_max_eutra;
    sib5.carrier_freq_list_eutra.push_back(info);
  }
  return sib5;
}

static sib6_info create_sib6_info(const du_high_unit_sib_config::etws_config& etws_cfg)
{
  sib6_info sib6;

  sib6.message_id    = etws_cfg.message_id;
  sib6.serial_number = etws_cfg.serial_num;
  sib6.warning_type  = etws_cfg.warning_type;

  return sib6;
}

static sib7_info create_sib7_info(const du_high_unit_sib_config::etws_config& etws_cfg)
{
  if (!etws_cfg.warning_message.has_value()) {
    report_error("Scheduled SIB-7 but no ETWS warning message is configured.");
  }
  if (etws_cfg.warning_message.value().empty()) {
    report_error("Scheduled SIB-7 but ETWS warning message is empty.");
  }

  sib7_info sib7;

  sib7.message_id              = etws_cfg.message_id;
  sib7.serial_number           = etws_cfg.serial_num;
  sib7.data_coding_scheme      = etws_cfg.data_coding_scheme;
  sib7.warning_message_segment = etws_cfg.warning_message.value();

  return sib7;
}

static sib8_info create_sib8_info(const du_high_unit_sib_config::cmas_config& cmas_cfg)
{
  if (cmas_cfg.warning_message.empty()) {
    report_error("Scheduled SIB-8 but CMAS warning message is empty.");
  }

  sib8_info sib8;

  sib8.message_id              = cmas_cfg.message_id;
  sib8.serial_number           = cmas_cfg.serial_num;
  sib8.data_coding_scheme      = cmas_cfg.data_coding_scheme;
  sib8.warning_message_segment = cmas_cfg.warning_message;
  return sib8;
}

static sib16_info create_sib16_info(const du_high_unit_sib_config::sib16_config& sib16_cfg)
{
  sib16_info sib16;

  for (const auto& freq_cfg : sib16_cfg.freq_prio_list_slicing) {
    sib16_info::freq_priority_slicing freq_info;
    freq_info.dl_implicit_carrier_freq = freq_cfg.dl_implicit_carrier_freq;
    for (const auto& slice_cfg : freq_cfg.slice_info_list) {
      sib16_info::slice_info slice_info;
      slice_info.nsag_id              = static_cast<uint8_t>(slice_cfg.nsag_id);
      slice_info.allowed              = slice_cfg.allowed;
      slice_info.reselection_priority = slice_cfg.reselection_priority;
      for (const auto& cell_cfg : slice_cfg.cells) {
        pci_range_t range;
        range.start = cell_cfg.start;
        range.size  = static_cast<pci_range_t::range_t>(cell_cfg.size);
        slice_info.cells_allowed.push_back(range);
      }
      freq_info.slice_info_list.push_back(slice_info);
    }
    sib16.freq_prio_list_slicing.push_back(freq_info);
  }

  return sib16;
}

static sib19_info create_sib19_info(const du_high_unit_cell_ntn_config& config)
{
  sib19_info sib19;
  sib19.t_service      = config.t_service;
  sib19.ref_location   = config.reference_location;
  sib19.distance_thres = config.distance_threshold;
  for (const auto& ncell : config.ncells) {
    sib19.ncells.push_back(ncell);
  }
  sib19.moving_ref_location    = config.moving_ref_location;
  sib19.sat_switch_with_resync = config.sat_switch_with_resync;

  sib19.ntn_cfg.emplace();
  sib19.ntn_cfg->cell_specific_koffset    = config.cell_specific_koffset;
  sib19.ntn_cfg->ephemeris_info           = config.ephemeris_info;
  sib19.ntn_cfg->epoch_time               = config.epoch_time;
  sib19.ntn_cfg->k_mac                    = config.k_mac;
  sib19.ntn_cfg->ta_info                  = config.ta_info;
  sib19.ntn_cfg->ntn_ul_sync_validity_dur = config.ntn_ul_sync_validity_dur;
  sib19.ntn_cfg->polarization             = config.polarization;
  sib19.ntn_cfg->ta_report                = config.ta_report;
  return sib19;
}

static void fill_csi_resources(odu::du_cell_config& out_cell, const du_high_unit_base_cell_config& cell_cfg)
{
  const auto& csi_cfg = cell_cfg.csi_cfg;

  // CSI-specific params.
  du_csi_params du_csi;
  du_csi.csi_rs_period = static_cast<csi_resource_periodicity>(csi_cfg.csi_rs_period_msec *
                                                               get_nof_slots_per_subframe(cell_cfg.common_scs));
  if (cell_cfg.csi_cfg.report_type == csi_report_type::aperiodic) {
    du_csi.csi_report_slot_offset  = std::nullopt;
    du_csi.enable_aperiodic_report = true;
  }
  du_csi.pwr_ctrl_offset = static_cast<int8_t>(cell_cfg.csi_cfg.pwr_ctrl_offset);

  // [Implementation-defined] The default CSI symbols are in symbols 4 and 8, the DM-RS for PDSCH might collide in
  // symbol index 8 when the number of DM-RS additional positions is 3.
  if (uint_to_dmrs_additional_positions(cell_cfg.pdsch_cfg.dmrs_add_pos) == dmrs_additional_positions::pos3) {
    du_csi.zp_csi_ofdm_symbol_index = 9;
    // As per TS 38.214, clause 5.1.6.1.1, following time-domain locations of the two CSI-RS resources in a slot, or of
    // the four CSI-RS resources in two consecutive slots are allowed:
    // {4,8}, {5,9}, or {6,10} for frequency range 1 and frequency range 2.
    // NOTE: As per TS 38.211, table 7.4.1.1.2-3, PDSCH DM-RS time-domain positions for single-symbol DM-RS
    // corresponding to ld >= 12 and dmrs-AdditionalPosition pos3 are l0, 5, 8, 11.
    du_csi.tracking_csi_ofdm_symbol_indices = {6, 10, 6, 10};
  }

  if (cell_cfg.tdd_ul_dl_cfg.has_value()) {
    const unsigned max_csi_symbol_index = *std::max_element(du_csi.tracking_csi_ofdm_symbol_indices.begin(),
                                                            du_csi.tracking_csi_ofdm_symbol_indices.end());
    const auto&    pdcch_common         = out_cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common;
    const auto     ssb_slots =
        ssb_helper::get_occupied_slot_offsets(out_cell.ran.ssb_cfg, out_cell.ran.dl_carrier.band, cell_cfg.common_scs);
    const auto              sib1_occ = sib_helper::get_occupied_slot_offsets(out_cell.ran.ssb_cfg,
                                                                out_cell.ran.dl_carrier.band,
                                                                cell_cfg.common_scs,
                                                                *pdcch_common.get_searchspace0(),
                                                                pdcch_common.get_coreset0()->value());
    std::array<unsigned, 1> zp_arr;
    span<const unsigned>    zp_offsets;
    if (csi_cfg.zp_csi_slot_offset.has_value()) {
      zp_arr     = {*csi_cfg.zp_csi_slot_offset};
      zp_offsets = zp_arr;
    }
    if (not csi_helper::derive_valid_csi_rs_slot_offsets(
            du_csi,
            csi_cfg.meas_csi_slot_offset,
            csi_cfg.tracking_csi_slot_offset,
            zp_offsets,
            generate_tdd_pattern(cell_cfg.common_scs, *cell_cfg.tdd_ul_dl_cfg),
            max_csi_symbol_index,
            static_cast<ssb_periodicity>(cell_cfg.ssb_cfg.ssb_period_msec),
            ssb_slots,
            sib1_occ.window_period_slots,
            sib1_occ.slot_offsets)) {
      report_error("Unable to derive valid CSI-RS slot offsets and period for cell with pci={}\n", cell_cfg.pci);
    }
  } else {
    du_csi.meas_csi_slot_offsets = {csi_cfg.meas_csi_slot_offset.has_value() ? *csi_cfg.meas_csi_slot_offset : 2U};
    if (csi_cfg.zp_csi_slot_offset.has_value()) {
      du_csi.zp_csi_slot_offsets = {*csi_cfg.zp_csi_slot_offset};
    }
    du_csi.tracking_csi_slot_offset =
        csi_cfg.tracking_csi_slot_offset.has_value() ? *csi_cfg.tracking_csi_slot_offset : 12;
  }

  // Store DU CSI parameters to generate csiMeasConfig inside the DU.
  out_cell.ran.init_bwp.csi = du_csi;
}

/// Converts and returns the given gnb application configuration to a DU slice RRM policy configuration list.
static std::vector<slice_rrm_policy_config>
generate_du_slicing_rrm_policy_config(span<const std::string>                    plmns,
                                      span<const du_high_unit_cell_slice_config> slice_cfg,
                                      unsigned                                   nof_cell_crbs,
                                      const scheduler_policy_config&             default_policy_sched_cfg)
{
  std::vector<slice_rrm_policy_config> rrm_policy_cfgs;
  for (const auto& plmn : plmns) {
    for (const auto& cfg : slice_cfg) {
      rrm_policy_cfgs.emplace_back();
      rrm_policy_cfgs.back().rrc_member.s_nssai =
          s_nssai_t{slice_service_type{cfg.sst}, slice_differentiator::create(cfg.sd).value()};
      rrm_policy_cfgs.back().rrc_member.plmn_id = plmn_identity::parse(plmn).value();
      unsigned min_rbs                          = (nof_cell_crbs * cfg.sched_cfg.min_prb_policy_ratio) / 100;
      unsigned max_rbs                          = (nof_cell_crbs * cfg.sched_cfg.max_prb_policy_ratio) / 100;
      unsigned ded_rbs                          = (nof_cell_crbs * cfg.sched_cfg.ded_prb_policy_ratio) / 100;
      rrm_policy_cfgs.back().rbs                = {ded_rbs, min_rbs, max_rbs};
      rrm_policy_cfgs.back().priority           = cfg.sched_cfg.priority;
      rrm_policy_cfgs.back().policy_sched_cfg   = cfg.sched_cfg.slice_policy_cfg.value_or(default_policy_sched_cfg);
    }
  }
  return rrm_policy_cfgs;
}

static ntn_cell_params make_ntn_cell_params(const du_high_unit_cell_ntn_config& cfg, bool ul_harq_mode_b)
{
  ntn_cell_params ntn;
  ntn.ntn_cfg.cell_specific_koffset    = cfg.cell_specific_koffset;
  ntn.ntn_cfg.k_mac                    = cfg.k_mac;
  ntn.ntn_cfg.ntn_ul_sync_validity_dur = cfg.ntn_ul_sync_validity_dur;
  ntn.ntn_cfg.epoch_time               = cfg.epoch_time;
  ntn.ntn_cfg.ta_info                  = cfg.ta_info;
  ntn.ntn_cfg.polarization             = cfg.polarization;
  ntn.ntn_cfg.ta_report                = cfg.ta_report;
  ntn.ntn_cfg.ephemeris_info           = cfg.ephemeris_info;

  // Derived from PUSCH config.
  ntn.ul_harq_mode_b = ul_harq_mode_b;

  return ntn;
}

/// Fill SI-Scheduling Information.
static std::optional<si_scheduling_info_config> make_si_sched_info_config(const du_high_unit_base_cell_config& cell_cfg)
{
  const auto& sib_cfg = cell_cfg.sib_cfg;
  if (sib_cfg.si_sched_info.empty()) {
    return std::nullopt;
  }
  si_scheduling_info_config out;
  out.si_window_len_slots = sib_cfg.si_window_len_slots;
  // Set SIB mapping info.
  out.si_sched_info.resize(sib_cfg.si_sched_info.size());
  std::vector<uint8_t> sibs_included;
  for (unsigned i = 0; i != sib_cfg.si_sched_info.size(); ++i) {
    auto& out_si                  = out.si_sched_info[i];
    out_si.si_period_radio_frames = sib_cfg.si_sched_info[i].si_period_rf;
    out_si.sib_mapping_info.resize(sib_cfg.si_sched_info[i].sib_mapping_info.size());
    out_si.si_window_position = sib_cfg.si_sched_info[i].si_window_position;
    for (unsigned j = 0; j != sib_cfg.si_sched_info[i].sib_mapping_info.size(); ++j) {
      sibs_included.push_back(sib_cfg.si_sched_info[i].sib_mapping_info[j]);
      out_si.sib_mapping_info[j] = static_cast<sib_type>(sibs_included.back());
    }
  }

  // Fill SIB parameters.
  for (const uint8_t sib_id : sibs_included) {
    sib_info item;
    switch (sib_id) {
      case 2: {
        if (!sib_cfg.sib2_cfg.has_value()) {
          report_error("SIB-2 cannot be scheduled without SIB2 config. Set the SIB2 config or remove SIB-2 from "
                       "the si_sched_info list");
        }
        item = create_sib2_info(sib_cfg.sib2_cfg.value());
      } break;
      case 3: {
        if (!sib_cfg.sib3_cfg.has_value()) {
          report_error("SIB-3 cannot be scheduled without SIB3 config. Set the SIB3 config or remove SIB-3 from "
                       "the si_sched_info list");
        }
        item = create_sib3_info(sib_cfg.sib3_cfg.value());
      } break;
      case 4: {
        if (!sib_cfg.sib4_cfg.has_value()) {
          report_error("SIB-4 cannot be scheduled without SIB4 config. Set the SIB4 config or remove SIB-4 from "
                       "the si_sched_info list");
        }
        item = create_sib4_info(sib_cfg.sib4_cfg.value());
      } break;
      case 5: {
        if (!sib_cfg.sib5_cfg.has_value()) {
          report_error("SIB-5 cannot be scheduled without SIB5 config. Set the SIB5 config or remove SIB-5 from "
                       "the si_sched_info list");
        }
        item = create_sib5_info(sib_cfg.sib5_cfg.value());
      } break;
      case 6: {
        if (!sib_cfg.etws_cfg.has_value()) {
          report_error("SIB-6 cannot be scheduled without ETWS config. Set the ETWS config or remove SIB-6 from "
                       "the si_sched_info list");
        }
        item = create_sib6_info(sib_cfg.etws_cfg.value());
      } break;
      case 7: {
        if (!sib_cfg.etws_cfg.has_value()) {
          report_error("SIB-7 cannot be scheduled without ETWS config. Set the ETWS config or remove SIB-7 from "
                       "the si_sched_info list");
        }
        item = create_sib7_info(sib_cfg.etws_cfg.value());
      } break;
      case 8: {
        if (!sib_cfg.cmas_cfg.has_value()) {
          report_error("SIB-8 cannot be scheduled without CMAS config. Set the CMAS config or remove SIB-8 from "
                       "the si_sched_info list");
        }
        item = create_sib8_info(sib_cfg.cmas_cfg.value());
      } break;
      case 16: {
        if (!sib_cfg.sib16_cfg.has_value()) {
          report_error("SIB-16 cannot be scheduled without SIB16 config. Set the SIB16 config or remove SIB-16 from "
                       "the si_sched_info list");
        }
        item = create_sib16_info(sib_cfg.sib16_cfg.value());
      } break;
      case 19: {
        if (!cell_cfg.ntn_cfg.has_value()) {
          report_error("SIB-19 cannot be scheduled without NTN config. Set the NTN config or remove SIB-19 from "
                       "the si_sched_info list\n");
        }
        item = create_sib19_info(cell_cfg.ntn_cfg.value());
      } break;
      default:
        report_error("SIB{} not supported\n", sib_id);
    }
    out.sibs.push_back({item, value_tag_t::min()});
  }

  return out;
}

/// Helper to generate MAC cell group parameters.
static mac_cell_group_params make_mac_cell_group_params(const du_high_unit_base_cell_config& cli_cfg)
{
  mac_cell_group_params mcg_params;
  mcg_params.periodic_timer = to_periodic_bsr_timer(cli_cfg.mcg_cfg.bsr_cfg.periodic_bsr_timer);
  mcg_params.retx_timer     = to_retx_bsr_timer(cli_cfg.mcg_cfg.bsr_cfg.retx_bsr_timer);
  if (cli_cfg.mcg_cfg.bsr_cfg.lc_sr_delay_timer.has_value()) {
    mcg_params.lc_sr_delay_timer = to_lc_sr_delay_timer(cli_cfg.mcg_cfg.bsr_cfg.lc_sr_delay_timer.value());
  }
  if (cli_cfg.mcg_cfg.sr_cfg.sr_prohibit_timer.has_value()) {
    mcg_params.sr_prohibit_timer.emplace(to_sr_prohib_timer(cli_cfg.mcg_cfg.sr_cfg.sr_prohibit_timer.value()));
  }
  mcg_params.max_tx           = to_sr_max_tx(cli_cfg.mcg_cfg.sr_cfg.sr_trans_max);
  mcg_params.phr_prohib_timer = to_phr_prohibit_timer(cli_cfg.mcg_cfg.phr_cfg.phr_prohib_timer);
  // DRX
  if (cli_cfg.drx_cfg.long_cycle != 0) {
    auto& drx            = mcg_params.drx.emplace();
    drx.on_duration      = std::chrono::milliseconds{cli_cfg.drx_cfg.on_duration_timer};
    drx.long_cycle       = std::chrono::milliseconds{cli_cfg.drx_cfg.long_cycle};
    drx.inactivity_timer = std::chrono::milliseconds{cli_cfg.drx_cfg.inactivity_timer};
    drx.retx_timer_dl    = cli_cfg.drx_cfg.retx_timer_dl;
    drx.retx_timer_ul    = cli_cfg.drx_cfg.retx_timer_ul;
  }
  return mcg_params;
}

static void fill_si_acquisition_info(si_acquisition_info& si, const du_high_unit_base_cell_config& cli_cfg)
{
  // Cell selection parameters.
  si.cell_sel_info.q_rx_lev_min = cli_cfg.q_rx_lev_min;
  si.cell_sel_info.q_qual_min   = cli_cfg.q_qual_min;
  // Cell access Related Info parameters.
  for (const auto& plmn : cli_cfg.additional_plmns) {
    si.cell_acc_rel_info.additional_plmns.push_back(plmn_identity::parse(plmn).value());
  }
  // SI message config.
  si.si_config = make_si_sched_info_config(cli_cfg);
  // UE timers and constants config.
  si.ue_timers_and_constants.t300 = std::chrono::milliseconds(cli_cfg.sib_cfg.ue_timers_and_constants.t300);
  si.ue_timers_and_constants.t301 = std::chrono::milliseconds(cli_cfg.sib_cfg.ue_timers_and_constants.t301);
  si.ue_timers_and_constants.t310 = std::chrono::milliseconds(cli_cfg.sib_cfg.ue_timers_and_constants.t310);
  si.ue_timers_and_constants.n310 = cli_cfg.sib_cfg.ue_timers_and_constants.n310;
  si.ue_timers_and_constants.t311 = std::chrono::milliseconds(cli_cfg.sib_cfg.ue_timers_and_constants.t311);
  si.ue_timers_and_constants.n311 = cli_cfg.sib_cfg.ue_timers_and_constants.n311;
  si.ue_timers_and_constants.t319 = std::chrono::milliseconds(cli_cfg.sib_cfg.ue_timers_and_constants.t319);
}

std::vector<odu::du_cell_config> ocudu::generate_du_cell_config(const du_high_unit_config& config)
{
  std::vector<odu::du_cell_config> out_cfg;
  out_cfg.reserve(config.cells_cfg.size());

  for (const auto& cell : config.cells_cfg) {
    nr_band         band       = cell.cell.band.value();
    frequency_range freq_range = band_helper::get_freq_range(band);
    duplex_mode     dplx_mode  = band_helper::get_duplex_mode(band);

    // Setup a cell configuration builder.
    cell_config_builder_params           param;
    const du_high_unit_base_cell_config& base_cell = cell.cell;
    param.pci                                      = base_cell.pci;
    param.scs_common                               = base_cell.common_scs;
    param.dl_carrier.carrier_bw                    = base_cell.channel_bw_mhz;
    param.dl_carrier.arfcn_f_ref                   = base_cell.dl_f_ref_arfcn;
    param.dl_carrier.band                          = band;
    // Enable CSI-RS if the PDSCH mcs is dynamic (min_ue_mcs != max_ue_mcs).
    param.csi_rs_enabled     = base_cell.csi_cfg.csi_rs_enabled;
    param.dl_carrier.nof_ant = base_cell.nof_antennas_dl;
    param.max_nof_layers     = base_cell.pdsch_cfg.max_rank;
    param.min_k1             = base_cell.pucch_cfg.min_k1;
    param.min_k2             = base_cell.pusch_cfg.min_k2;
    param.cs0_index          = base_cell.pdcch_cfg.common.coreset0_index;
    param.ss0_index          = base_cell.pdcch_cfg.common.ss0_index;
    // If the CORESET#0 maximum duration is not set, set maximum CORESET#0 duration to 1 OFDM symbol for BW > 50MHz in
    // FR1 to spread CORESET RBs across the BW. This results in one extra symbol to be used for PDSCH.
    if (base_cell.pdcch_cfg.common.max_coreset0_duration.has_value()) {
      param.max_coreset0_duration = base_cell.pdcch_cfg.common.max_coreset0_duration.value();
    } else if ((param.dl_carrier.carrier_bw > bs_channel_bandwidth::MHz50) && (freq_range == frequency_range::FR1)) {
      param.max_coreset0_duration = 1;
    }
    const bool is_tdd = (dplx_mode == duplex_mode::TDD);
    if (is_tdd) {
      param.tdd_ul_dl_cfg_common.emplace(generate_tdd_pattern(param.scs_common, cell.cell.tdd_ul_dl_cfg.value()));
    }

    // Create the configuration from the cell config builder.
    out_cfg.push_back(config_helpers::make_default_du_cell_config(param));
    odu::du_cell_config& out_cell = out_cfg.back();

    // Overwrite the parameters that the cell config builder set as default.
    // > MIB
    out_cell.cell_barred            = base_cell.cell_barred;
    out_cell.intra_freq_reselection = base_cell.intra_freq_reselection;

    // > PLMN/TAC/NCI.
    out_cell.nr_cgi.plmn_id = plmn_identity::parse(base_cell.plmn).value();
    out_cell.nr_cgi.nci     = nr_cell_identity::create(config.gnb_id, base_cell.sector_id.value()).value();
    out_cell.tac            = base_cell.tac;
    out_cell.enabled        = base_cell.enabled;

    // > SSB.
    out_cell.ran.ssb_cfg.ssb_period      = static_cast<ssb_periodicity>(base_cell.ssb_cfg.ssb_period_msec);
    out_cell.ran.ssb_cfg.ssb_block_power = base_cell.ssb_cfg.ssb_block_power;
    out_cell.ran.ssb_cfg.pss_to_sss_epre = base_cell.ssb_cfg.pss_to_sss_epre;

    // > Carrier config.
    out_cell.ran.dl_carrier.nof_ant = base_cell.nof_antennas_dl;
    out_cell.ran.ul_carrier.nof_ant = base_cell.nof_antennas_ul;
    // > System Information.
    fill_si_acquisition_info(out_cell.si, base_cell);
    if (out_cell.si.si_config.has_value()) {
      // Enable otherSI search space.
      out_cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.other_si_search_space_id = to_search_space_id(1);
    }

    // UL common config.
    if (base_cell.ul_common_cfg.p_max.has_value()) {
      out_cell.ran.ul_cfg_common.freq_info_ul.p_max = base_cell.ul_common_cfg.p_max.value();
    }

    // DL common config.
    out_cell.ran.dl_cfg_common.freq_info_dl.scs_carrier_list.back().tx_direct_current_location =
        dc_offset_helper::pack(base_cell.pdsch_cfg.dc_offset,
                               out_cell.ran.dl_cfg_common.freq_info_dl.scs_carrier_list.back().carrier_bandwidth);

    // PRACH config.
    rach_config_common& rach_cfg                           = *out_cell.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common;
    rach_cfg.rach_cfg_generic.prach_config_index           = base_cell.prach_cfg.prach_config_index.value();
    rach_cfg.rach_cfg_generic.ra_resp_window               = base_cell.prach_cfg.ra_resp_window.value();
    rach_cfg.rach_cfg_generic.zero_correlation_zone_config = base_cell.prach_cfg.zero_correlation_zone;
    rach_cfg.rach_cfg_generic.preamble_rx_target_pw        = base_cell.prach_cfg.preamble_rx_target_pw;
    rach_cfg.rach_cfg_generic.preamble_trans_max           = base_cell.prach_cfg.preamble_trans_max;
    rach_cfg.rach_cfg_generic.power_ramping_step_db        = base_cell.prach_cfg.power_ramping_step_db;
    // \c msg1_frequency_start for RACH is one of the parameters that can either be set manually, or need to be
    // recomputed at the end of the manual configuration, as a results of other user parameters passed by the user.
    const bool update_msg1_frequency_start = not base_cell.prach_cfg.prach_frequency_start.has_value();
    if (not update_msg1_frequency_start) {
      // Set manually.
      rach_cfg.rach_cfg_generic.msg1_frequency_start = base_cell.prach_cfg.prach_frequency_start.value();
    }
    rach_cfg.msg3_transform_precoder = cell.cell.pusch_cfg.enable_transform_precoding;
    const bool is_long_prach         = is_long_preamble(
        prach_configuration_get(freq_range, dplx_mode, rach_cfg.rach_cfg_generic.prach_config_index).format);
    // \c is_prach_root_seq_index_l839 and msg1_scs are derived parameters, that depend on the PRACH format. They are
    // originally computed in the base_cell struct, but since we overwrite the PRACH prach_config_index (which
    // determines the PRACH format), we need to recompute both \c is_prach_root_seq_index_l839 and \c msg1_scs.
    rach_cfg.is_prach_root_seq_index_l839 = is_long_prach;
    rach_cfg.msg1_scs                     = is_long_prach ? subcarrier_spacing::invalid : base_cell.common_scs;
    rach_cfg.prach_root_seq_index         = base_cell.prach_cfg.prach_root_sequence_index;
    rach_cfg.total_nof_ra_preambles       = base_cell.prach_cfg.total_nof_ra_preambles;
    rach_cfg.nof_ssb_per_ro               = base_cell.prach_cfg.nof_ssb_per_ro;
    rach_cfg.nof_cb_preambles_per_ssb     = base_cell.prach_cfg.nof_cb_preambles_per_ssb;
    rach_cfg.ra_prio_slice_info_list.resize(base_cell.prach_cfg.ra_prio_slice_info_list.size());
    for (unsigned i = 0, e = base_cell.prach_cfg.ra_prio_slice_info_list.size(); i != e; ++i) {
      const auto& clifield                             = base_cell.prach_cfg.ra_prio_slice_info_list[i];
      rach_cfg.ra_prio_slice_info_list[i].nsag_id_list = clifield.nsag_ids;
      rach_cfg.ra_prio_slice_info_list[i].prio.pwr_ramp_step_hi_prio =
          static_cast<ra_prioritization::power_ramp_step_high_priority>(clifield.power_ramp_step_high_priority);
      if (clifield.scaling_factor_bi.has_value()) {
        rach_cfg.ra_prio_slice_info_list[i].prio.scaling_bi =
            static_cast<ra_prioritization::scaling_factor_bi>(clifield.scaling_factor_bi.value());
      }
    }
    if (base_cell.prach_cfg.two_step.has_value()) {
      const auto& src = *base_cell.prach_cfg.two_step;
      rach_cfg.two_step_rach_cfg.emplace();
      auto& dst                              = *rach_cfg.two_step_rach_cfg;
      dst.cb_preambles_per_ssb_per_shared_ro = src.cb_preambles_per_ssb_per_shared_ro;
      dst.msgA_rsrp_thres                    = rsrp_range{src.msga_rsrp_thres_dbm};
      dst.msgB_response_window_slots         = src.msgb_response_window_slots;
      dst.pusch.td_offset                    = src.td_offset;
      dst.pusch.pusch_td_res_index           = src.pusch_td_res_index;
      dst.pusch.mcs                          = sch_mcs_index{src.mcs};
      dst.pusch.nof_prbs_per_msgA_po         = src.nof_prbs_per_msga_po;
      dst.pusch.prb_start                    = src.prb_start;
      dst.pusch.po_fdm                       = src.po_fdm;
    }

    // PhysicalCellGroup Config parameters.
    if (base_cell.pcg_cfg.p_nr_fr1.has_value()) {
      out_cell.pcg_params.p_nr_fr1 = base_cell.pcg_cfg.p_nr_fr1.value();
    }

    // MAC Cell Group Config parameters.
    out_cell.mcg_params = make_mac_cell_group_params(base_cell);

    // Paging parameters.
    out_cell.ran.dl_cfg_common.pcch_cfg = generate_pcch_config(base_cell);
    out_cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id =
        to_search_space_id(base_cell.paging_cfg.paging_search_space_id);

    // Parameters for PUSCH-ConfigCommon.
    if (not out_cell.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value()) {
      out_cell.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.emplace();
    }
    out_cell.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().msg3_delta_preamble =
        base_cell.pusch_cfg.msg3_delta_preamble;
    out_cell.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().p0_nominal_with_grant =
        base_cell.pusch_cfg.p0_nominal_with_grant;
    out_cell.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().msg3_delta_power =
        base_cell.pusch_cfg.msg3_delta_power;

    // Parameters for PUCCH-ConfigCommon.
    if (not out_cell.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common.has_value()) {
      out_cell.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common.emplace();
    }
    out_cell.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().p0_nominal = base_cell.pucch_cfg.p0_nominal;
    // If not provided, set a default common resource set index depending on the format used for Resource Set 0.
    // The indices are chosen to maximize the number of symbols and minimize the number of cyclic shifts.
    if (pucch_f0f1_format(base_cell.pucch_cfg.formats) == pucch_format::FORMAT_0) {
      out_cell.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common =
          base_cell.pucch_cfg.pucch_resource_common.value_or(0);
    } else {
      out_cell.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common =
          base_cell.pucch_cfg.pucch_resource_common.value_or(11);
    }

    // Common PDCCH config.
    search_space_configuration& ss1_cfg = out_cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces.back();
    ss1_cfg.set_non_ss0_nof_candidates(base_cell.pdcch_cfg.common.ss1_n_candidates);

    // Set builder for initial BWP UE-dedicated parameters.
    // > PDSCH
    out_cell.ran.init_bwp.pdsch.max_harq_procs =
        static_cast<uint8_t>(config.cells_cfg.front().cell.pdsch_cfg.nof_harqs);
    out_cell.ran.init_bwp.pdsch.dl_harq_feedback_disabled = base_cell.pdsch_cfg.harq_feedback_disabled;
    out_cell.ran.init_bwp.pdsch.mcs_table                 = base_cell.pdsch_cfg.mcs_table;
    out_cell.ran.init_bwp.pdsch.additional_positions =
        uint_to_dmrs_additional_positions(base_cell.pdsch_cfg.dmrs_add_pos);
    out_cell.ran.init_bwp.pdsch.interleaving_bundle_size = base_cell.pdsch_cfg.interleaving_bundle_size;
    // > PUSCH
    out_cell.ran.init_bwp.pusch.min_k2                      = base_cell.pusch_cfg.min_k2;
    out_cell.ran.init_bwp.pusch.transform_precoding_enabled = base_cell.pusch_cfg.enable_transform_precoding;
    out_cell.ran.init_bwp.pusch.mcs_table                   = base_cell.pusch_cfg.mcs_table;
    out_cell.ran.init_bwp.pusch.additional_positions =
        uint_to_dmrs_additional_positions(base_cell.pusch_cfg.dmrs_add_pos);
    // Determine the PUSCH transmission maximum number of layers:
    //  - one layer if transform precoding is enabled; or
    //  - selects the most limiting number of layers among the physical layer capability, the number of antennas and
    //    configured maximum rank.
    out_cell.ran.init_bwp.pusch.max_nof_layers =
        cell.cell.pusch_cfg.enable_transform_precoding
            ? 1
            : std::min({cell.cell.nof_antennas_ul, pusch_constants::MAX_NOF_LAYERS, cell.cell.pusch_cfg.max_rank});
    beta_offsets b_offsets{};
    b_offsets.beta_offset_ack_idx_1              = base_cell.pusch_cfg.beta_offset_ack_idx_1;
    b_offsets.beta_offset_ack_idx_2              = base_cell.pusch_cfg.beta_offset_ack_idx_2;
    b_offsets.beta_offset_ack_idx_3              = base_cell.pusch_cfg.beta_offset_ack_idx_3;
    b_offsets.beta_offset_csi_p1_idx_1           = base_cell.pusch_cfg.beta_offset_csi_p1_idx_1;
    b_offsets.beta_offset_csi_p1_idx_2           = base_cell.pusch_cfg.beta_offset_csi_p1_idx_2;
    b_offsets.beta_offset_csi_p2_idx_1           = base_cell.pusch_cfg.beta_offset_csi_p2_idx_1;
    b_offsets.beta_offset_csi_p2_idx_2           = base_cell.pusch_cfg.beta_offset_csi_p2_idx_2;
    out_cell.ran.init_bwp.pusch.uci_beta_offsets = b_offsets;
    out_cell.ran.init_bwp.pusch.p0_pusch_alpha   = float_to_alpha(base_cell.pusch_cfg.path_loss_compensation_factor);
    out_cell.ran.init_bwp.pusch.max_harq_procs   = static_cast<uint8_t>(base_cell.pusch_cfg.nof_harqs);
    out_cell.ran.init_bwp.pusch.ul_harq_mode     = ~base_cell.pusch_cfg.harq_mode_b;
    // > PUCCH
    out_cell.ran.init_bwp.pucch.min_k1    = base_cell.pucch_cfg.min_k1;
    out_cell.ran.init_bwp.pucch.sr_period = static_cast<sr_periodicity>(
        static_cast<unsigned>(get_nof_slots_per_subframe(base_cell.common_scs) * base_cell.pucch_cfg.sr_period_msec));
    // > RACH.
    out_cell.ran.init_bwp.rach.cfra_enabled   = base_cell.prach_cfg.cfra_enabled;
    out_cell.ran.init_bwp.paging.edrx_enabled = base_cell.paging_cfg.edrx_enabled;
    out_cell.ran.init_bwp.rlm.type            = base_cell.rlm_cfg.resource_type;
    // > NTN.
    if (base_cell.ntn_cfg.has_value()) {
      out_cell.ran.ntn_params = make_ntn_cell_params(base_cell.ntn_cfg.value(), base_cell.pusch_cfg.harq_mode_b.any());
    }

    // UE-dedicated PDCCH config.
    freq_resource_bitmap freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
    unsigned             cset1_start_crb = 0;
    if (base_cell.pdcch_cfg.dedicated.coreset1_rb_start.has_value()) {
      cset1_start_crb = base_cell.pdcch_cfg.dedicated.coreset1_rb_start.value();
    }
    const unsigned nof_crbs    = band_helper::get_n_rbs_from_bw(base_cell.channel_bw_mhz, param.scs_common, freq_range);
    unsigned       cset1_l_crb = nof_crbs - cset1_start_crb;
    if (base_cell.pdcch_cfg.dedicated.coreset1_l_crb.has_value()) {
      cset1_l_crb = base_cell.pdcch_cfg.dedicated.coreset1_l_crb.value();
    }
    const unsigned coreset1_nof_resources = cset1_l_crb / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE;
    freq_resources.fill(cset1_start_crb / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE,
                        (cset1_start_crb / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE) + coreset1_nof_resources,
                        true);

    search_space_configuration& ss2_cfg   = out_cell.ran.init_bwp.pdcch_cfg->search_spaces[0];
    coreset_configuration&      cset1_cfg = out_cell.ran.init_bwp.pdcch_cfg->coresets[0];
    cset1_cfg.set_freq_domain_resources(freq_resources);
    cset1_cfg.set_non_coreset0_duration(base_cell.pdcch_cfg.dedicated.coreset1_duration.value_or(
        out_cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->duration()));
    static constexpr std::array<uint8_t, 5> auto_compute_ss2_n_candidates_cfg = {0, 0, 0, 0, 0};
    if (base_cell.pdcch_cfg.dedicated.ss2_n_candidates != auto_compute_ss2_n_candidates_cfg) {
      ss2_cfg.set_non_ss0_nof_candidates(base_cell.pdcch_cfg.dedicated.ss2_n_candidates);
    } else if (base_cell.pdcch_cfg.dedicated.ss2_type != search_space_configuration::type_t::common) {
      ss2_cfg.set_non_ss0_nof_candidates({config_helpers::compute_max_nof_candidates(aggregation_level::n1, cset1_cfg),
                                          config_helpers::compute_max_nof_candidates(aggregation_level::n2, cset1_cfg),
                                          config_helpers::compute_max_nof_candidates(aggregation_level::n4, cset1_cfg),
                                          0,
                                          0});

      // Reduce the number of PDCCH candidates if the total number of monitored PDCCH candidates per slot exceeds the
      // maximum allowed value as per TS 38.213, Table 10.1-2.
      auto reduce_nof_pdcch_candidates = [&ss2_cfg]() {
        constexpr uint8_t min_nof_candidates = 1U;
        // [Implementation-defined] We reduce the number of PDCCH candidates by alternating the aggregation levels
        // n2 and n4.
        if (ss2_cfg.get_nof_candidates()[1] >= ss2_cfg.get_nof_candidates()[2]) {
          if (ss2_cfg.get_nof_candidates()[1] > min_nof_candidates) {
            const uint8_t current_nof_candidates = ss2_cfg.get_nof_candidates()[1];
            const uint8_t updated_nof_candidates = current_nof_candidates == 8 ? 6U : current_nof_candidates - 1U;
            ss2_cfg.set_non_ss0_nof_candidates({0, updated_nof_candidates, ss2_cfg.get_nof_candidates()[2], 0, 0});
          }
        } else if (ss2_cfg.get_nof_candidates()[2] > min_nof_candidates) {
          const uint8_t current_nof_candidates = ss2_cfg.get_nof_candidates()[2];
          const uint8_t updated_nof_candidates = current_nof_candidates == 8 ? 6U : current_nof_candidates - 1U;
          ss2_cfg.set_non_ss0_nof_candidates({0, ss2_cfg.get_nof_candidates()[1], updated_nof_candidates, 0, 0});
        }
      };

      static constexpr uint8_t min_nof_pdcch_candidates = 1;
      while (config_helpers::compute_tot_nof_monitored_pdcch_candidates_per_slot(*out_cell.ran.init_bwp.pdcch_cfg,
                                                                                 out_cell.ran.dl_cfg_common) >
                 max_nof_monitored_pdcch_candidates(param.scs_common) and
             std::accumulate(ss2_cfg.get_nof_candidates().begin(), ss2_cfg.get_nof_candidates().end(), 0U) >
                 min_nof_pdcch_candidates) {
        reduce_nof_pdcch_candidates();
      }
    }

    if (base_cell.pdcch_cfg.dedicated.ss2_type == search_space_configuration::type_t::common) {
      ss2_cfg.set_non_ss0_monitored_dci_formats(search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true});
      // TODO: Handle this implementation defined restriction of max. 4 PDCCH candidates in validator.
      if (base_cell.pdcch_cfg.dedicated.ss2_n_candidates == auto_compute_ss2_n_candidates_cfg) {
        ss2_cfg.set_non_ss0_nof_candidates(
            {0,
             std::min(static_cast<uint8_t>(4U),
                      config_helpers::compute_max_nof_candidates(aggregation_level::n2, cset1_cfg)),
             std::min(static_cast<uint8_t>(4U),
                      config_helpers::compute_max_nof_candidates(aggregation_level::n4, cset1_cfg)),
             0,
             0});
      }
    } else if (not base_cell.pdcch_cfg.dedicated.dci_format_0_1_and_1_1) {
      search_space_configuration& ss_cfg = out_cell.ran.init_bwp.pdcch_cfg->search_spaces[0];
      ss_cfg.set_non_ss0_monitored_dci_formats(search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0);
    }

    // PDSCH-Config - Update PDSCH time domain resource allocations based on partial slot and/or dedicated PDCCH
    // configuration.
    out_cell.ran.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list =
        time_domain_resource_helper::generate_dedicated_pdsch_td_res_list(
            out_cell.ran.tdd_cfg,
            out_cell.ran.dl_cfg_common.init_dl_bwp.generic_params.cp,
            time_domain_resource_helper::calculate_minimum_pdsch_symbol(
                out_cell.ran.dl_cfg_common.init_dl_bwp.pdcch_common, out_cell.ran.init_bwp.pdcch_cfg));

    // Parameters for PUCCH-Config builder (these parameters will be used later on to generate the PUCCH resources).
    pucch_resource_builder_params&   du_pucch_cfg                  = out_cell.ran.init_bwp.pucch.resources;
    const du_high_unit_pucch_config& user_pucch_cfg_pre_processing = base_cell.pucch_cfg;
    du_high_unit_pucch_config        user_pucch_cfg                = user_pucch_cfg_pre_processing;
    // For 5MHz BW or for 10MHz TDD, the default PUCCH configuration would use too many PRBs, we need to reduce them not
    // to waste the useful UL BW.
    if ((param.dl_carrier.carrier_bw < bs_channel_bandwidth::MHz10 or
         (is_tdd and param.dl_carrier.carrier_bw <= bs_channel_bandwidth::MHz10)) and
        user_pucch_cfg_pre_processing == du_high_unit_pucch_config{}) {
      constexpr unsigned res_set_size_5mhz             = 7;
      constexpr unsigned nof_cell_res_set_configs_5mhz = 1;
      constexpr unsigned nof_cell_sr_res_5mhz          = 7;
      constexpr unsigned nof_cell_csi_res_5mhz         = 7;
      user_pucch_cfg.nof_cell_res_set_configs          = nof_cell_res_set_configs_5mhz;
      user_pucch_cfg.res_set_size = std::min(res_set_size_5mhz, user_pucch_cfg_pre_processing.res_set_size);
      user_pucch_cfg.nof_cell_sr_resources =
          std::min(nof_cell_sr_res_5mhz, user_pucch_cfg_pre_processing.nof_cell_sr_resources);
      user_pucch_cfg.nof_cell_csi_resources =
          std::min(nof_cell_csi_res_5mhz, user_pucch_cfg_pre_processing.nof_cell_csi_resources);
      user_pucch_cfg.f1_enable_occ = true;
    }
    du_pucch_cfg.nof_cell_res_set_configs = user_pucch_cfg.nof_cell_res_set_configs;
    du_pucch_cfg.nof_cell_sr_resources    = user_pucch_cfg.nof_cell_sr_resources;
    du_pucch_cfg.nof_cell_csi_resources   = param.csi_rs_enabled ? user_pucch_cfg.nof_cell_csi_resources : 0U;

    if (pucch_f0f1_format(user_pucch_cfg.formats) == pucch_format::FORMAT_0) {
      auto& f0_params = du_pucch_cfg.f0_or_f1_params.emplace<pucch_f0_params>();
      // Subtract 2 PUCCH resources from value: with Format 0, 2 extra resources will be added by the DU resource
      // allocator when the DU create the UE configuration.
      const unsigned extra_res_harq = pucch_f2f3f4_format(user_pucch_cfg.formats) == pucch_format::FORMAT_2 ? 2U : 0U;
      du_pucch_cfg.res_set_size     = user_pucch_cfg.res_set_size - extra_res_harq;
      f0_params.intraslot_freq_hopping = user_pucch_cfg.f0_intraslot_freq_hopping;
    } else {
      auto& f1_params                  = du_pucch_cfg.f0_or_f1_params.emplace<pucch_f1_params>();
      du_pucch_cfg.res_set_size        = user_pucch_cfg.res_set_size;
      f1_params.occ_supported          = user_pucch_cfg.f1_enable_occ;
      f1_params.nof_cyc_shifts         = static_cast<pucch_nof_cyclic_shifts>(user_pucch_cfg.f1_nof_cyclic_shifts);
      f1_params.intraslot_freq_hopping = user_pucch_cfg.f1_intraslot_freq_hopping;
    }

    switch (pucch_f2f3f4_format(user_pucch_cfg.formats)) {
      case pucch_format::FORMAT_2: {
        // The number of symbols per PUCCH resource F2 is not exposed to the DU user interface and set by default to 2.
        constexpr unsigned pucch_f2_nof_symbols = 2U;
        auto&              f2_params            = du_pucch_cfg.f2_or_f3_or_f4_params.emplace<pucch_f2_params>();
        f2_params.max_code_rate                 = user_pucch_cfg.f2_max_code_rate;
        f2_params.max_nof_rbs                   = user_pucch_cfg.f2_max_payload_bits.has_value()
                                                      ? get_pucch_format2_max_nof_prbs(user_pucch_cfg.f2_max_payload_bits.value(),
                                                                     pucch_f2_nof_symbols,
                                                                     to_float(user_pucch_cfg.f2_max_code_rate))
                                                      : user_pucch_cfg.f2_max_nof_rbs;
        f2_params.intraslot_freq_hopping        = user_pucch_cfg.f2_intraslot_freq_hopping;
        f2_params.max_payload_bits              = user_pucch_cfg.f2_max_payload_bits;
      } break;
      case pucch_format::FORMAT_3: {
        // The number of symbols per PUCCH resource is not exposed to the DU user interface; for PUCCH F3, we use all
        // symbols available for PUCCH within a slot.
        const unsigned max_nof_srs_symbols =
            cell.cell.srs_cfg.srs_type_enabled == "periodic" or cell.cell.srs_cfg.srs_type_enabled != "aperiodic"
                ? cell.cell.srs_cfg.max_nof_symbols_per_slot
                : 0U;
        const unsigned max_nof_pucch_symbols = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - max_nof_srs_symbols;
        const unsigned pucch_f3_nof_symbols  = max_nof_pucch_symbols;
        auto&          f3_params             = du_pucch_cfg.f2_or_f3_or_f4_params.emplace<pucch_f3_params>();
        f3_params.max_code_rate              = user_pucch_cfg.f3_max_code_rate;
        f3_params.max_nof_rbs                = user_pucch_cfg.f3_max_payload_bits.has_value()
                                                   ? get_pucch_format3_max_nof_prbs(
                                          user_pucch_cfg.f3_max_payload_bits.value(),
                                          pucch_f3_nof_symbols,
                                          to_float(user_pucch_cfg.f3_max_code_rate),
                                          // Since we are forcing 14 symbols intraslot_freq_hopping doesn't matter.
                                          false,
                                          user_pucch_cfg.f3_additional_dmrs,
                                          user_pucch_cfg.f3_pi2_bpsk)
                                                   : user_pucch_cfg.f3_max_nof_rbs;
        f3_params.intraslot_freq_hopping     = user_pucch_cfg.f3_intraslot_freq_hopping;
        f3_params.max_payload_bits           = user_pucch_cfg.f3_max_payload_bits;
        f3_params.additional_dmrs            = user_pucch_cfg.f3_additional_dmrs;
        f3_params.pi2_bpsk                   = user_pucch_cfg.f3_pi2_bpsk;
      } break;
      case pucch_format::FORMAT_4: {
        auto& f4_params                  = du_pucch_cfg.f2_or_f3_or_f4_params.emplace<pucch_f4_params>();
        f4_params.max_code_rate          = user_pucch_cfg.f4_max_code_rate;
        f4_params.intraslot_freq_hopping = user_pucch_cfg.f4_intraslot_freq_hopping;
        f4_params.additional_dmrs        = user_pucch_cfg.f4_additional_dmrs;
        f4_params.pi2_bpsk               = user_pucch_cfg.f4_pi2_bpsk;
        f4_params.occ_length             = static_cast<pucch_f4_occ_len>(user_pucch_cfg.f4_occ_length);
      } break;
      default:
        break;
    }

    if (!user_pucch_cfg.repetition_sinr_thresholds.empty()) {
      auto& rep           = out_cell.ran.init_bwp.pucch.resources.harq_ack_rep.emplace();
      rep.sinr_thresholds = user_pucch_cfg.repetition_sinr_thresholds;
      rep.factors_per_res.reserve(user_pucch_cfg.repetition_factors.size());
      for (unsigned f : user_pucch_cfg.repetition_factors) {
        rep.factors_per_res.push_back(static_cast<pucch_repetition_factor>(f));
      }
    }

    // Parameters for SRS-Config.
    srs_builder_params&            du_srs_cfg   = out_cell.ran.init_bwp.srs_cfg;
    const du_high_unit_srs_config& user_srs_cfg = base_cell.srs_cfg;
    if (user_srs_cfg.srs_type_enabled == "periodic") {
      du_srs_cfg.srs_type_enabled = srs_type::periodic;
    } else if (user_srs_cfg.srs_type_enabled == "aperiodic") {
      du_srs_cfg.srs_type_enabled = srs_type::aperiodic;
    } else {
      du_srs_cfg.srs_type_enabled = srs_type::disabled;
    }
    const auto srs_period_slots = static_cast<unsigned>(
        std::round(get_nof_slots_per_subframe(base_cell.common_scs) * user_srs_cfg.srs_period_prohibit_time_ms));
    du_srs_cfg.srs_period_prohib_time    = static_cast<srs_periodicity>(srs_period_slots);
    du_srs_cfg.c_srs                     = user_srs_cfg.c_srs;
    du_srs_cfg.max_nof_symbols           = user_srs_cfg.max_nof_symbols_per_slot;
    du_srs_cfg.tx_comb                   = user_srs_cfg.tx_comb == 2 ? tx_comb_size::n2 : tx_comb_size::n4;
    du_srs_cfg.nof_symbols               = static_cast<srs_nof_symbols>(user_srs_cfg.nof_symbols);
    du_srs_cfg.freq_domain_shift         = user_srs_cfg.freq_domain_shift;
    du_srs_cfg.cyclic_shift_reuse_factor = static_cast<nof_cyclic_shifts>(user_srs_cfg.cyclic_shift_reuse_factor);
    du_srs_cfg.sequence_id_reuse_factor  = user_srs_cfg.sequence_id_reuse_factor;
    du_srs_cfg.p0                        = user_srs_cfg.p0;

    // If any dependent parameter needs to be updated, this is the place.
    config_helpers::compute_nof_sr_csi_pucch_res(du_pucch_cfg,
                                                 base_cell.ul_common_cfg.max_pucchs_per_slot,
                                                 base_cell.pucch_cfg.sr_period_msec,
                                                 base_cell.csi_cfg.csi_rs_enabled
                                                     ? std::optional<unsigned>{base_cell.csi_cfg.csi_rs_period_msec}
                                                     : std::nullopt);
    // The maximum number of symbols for cell PUCCH resources is computed based on the SRS configuration, but only if
    // the SRS are periodic. The aperiodic SRS resources are not currently supported and used only for the UE to accept
    // the configuration; therefore, the maximum number of symbols for PUCCH resources is computed only for periodic
    // SRS.
    du_pucch_cfg.max_nof_symbols = config_helpers::compute_max_nof_pucch_symbols(du_srs_cfg);
    if (user_srs_cfg.srs_type_enabled == "periodic" or user_srs_cfg.srs_type_enabled == "aperiodic") {
      if (std::holds_alternative<pucch_f1_params>(du_pucch_cfg.f0_or_f1_params)) {
        auto& f1_params    = std::get<pucch_f1_params>(du_pucch_cfg.f0_or_f1_params);
        f1_params.nof_syms = std::min(du_pucch_cfg.max_nof_symbols.value(), f1_params.nof_syms.value());
      }
      if (std::holds_alternative<pucch_f3_params>(du_pucch_cfg.f2_or_f3_or_f4_params)) {
        auto& f3_params    = std::get<pucch_f3_params>(du_pucch_cfg.f2_or_f3_or_f4_params);
        f3_params.nof_syms = std::min(du_pucch_cfg.max_nof_symbols.value(), f3_params.nof_syms.value());
      } else if (std::holds_alternative<pucch_f4_params>(du_pucch_cfg.f2_or_f3_or_f4_params)) {
        auto& f4_params    = std::get<pucch_f4_params>(du_pucch_cfg.f2_or_f3_or_f4_params);
        f4_params.nof_syms = std::min(du_pucch_cfg.max_nof_symbols.value(), f4_params.nof_syms.value());
      }
      // Add extra PUSCH time-domain resources to enable PUSCH on symbols not used by the SRS.
      config_helpers::recompute_pusch_time_domain_resources(
          out_cell.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list,
          du_srs_cfg,
          out_cell.ran.tdd_cfg);
    }

    // Parameters for csiMeasConfig.
    if (param.csi_rs_enabled) {
      fill_csi_resources(out_cell, base_cell);
    }

    if (update_msg1_frequency_start) {
      rach_cfg.rach_cfg_generic.msg1_frequency_start = config_helpers::compute_prach_frequency_start(
          du_pucch_cfg, out_cell.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(), is_long_prach);
    }

    // Slicing configuration.
    std::vector<std::string> cell_plmns{base_cell.plmn};
    out_cell.rrm_policy_members = generate_du_slicing_rrm_policy_config(
        cell_plmns, base_cell.slice_cfg, nof_crbs, *cell.cell.scheduler_cfg.policy_cfg);

    error_type<std::string> error = is_du_cell_config_valid(out_cfg.back());
    if (!error) {
      report_error("Invalid configuration DU cell detected.\n> {}\n", error.error());
    }
  }

  return out_cfg;
}

static rlc_am_config generate_du_rlc_am_config(const du_high_unit_rlc_am_config& in_cfg)
{
  rlc_am_config out_rlc;
  // AM Config
  //<  TX SN
  if (!from_number(out_rlc.tx.sn_field_length, in_cfg.tx.sn_field_length)) {
    report_error("Invalid RLC AM TX SN: SN={}\n", in_cfg.tx.sn_field_length);
  }
  out_rlc.tx.t_poll_retx      = in_cfg.tx.t_poll_retx;
  out_rlc.tx.max_retx_thresh  = in_cfg.tx.max_retx_thresh;
  out_rlc.tx.poll_pdu         = in_cfg.tx.poll_pdu;
  out_rlc.tx.poll_byte        = in_cfg.tx.poll_byte;
  out_rlc.tx.max_window       = in_cfg.tx.max_window;
  out_rlc.tx.queue_size       = in_cfg.tx.queue_size;
  out_rlc.tx.queue_size_bytes = in_cfg.tx.queue_size_bytes;
  //< RX SN
  if (!from_number(out_rlc.rx.sn_field_length, in_cfg.rx.sn_field_length)) {
    report_error("Invalid RLC AM RX SN: SN={}\n", in_cfg.rx.sn_field_length);
  }
  out_rlc.rx.t_reassembly      = in_cfg.rx.t_reassembly;
  out_rlc.rx.t_status_prohibit = in_cfg.rx.t_status_prohibit;
  if (in_cfg.rx.max_sn_per_status != 0) {
    out_rlc.rx.max_sn_per_status = in_cfg.rx.max_sn_per_status;
  }
  return out_rlc;
}

static std::map<five_qi_t, odu::du_qos_config> generate_du_qos_config(const du_high_unit_config& config)
{
  std::map<five_qi_t, odu::du_qos_config> out_cfg = {};
  if (config.qos_cfg.empty()) {
    out_cfg = config_helpers::make_default_du_qos_config_list(
        config.warn_on_drop, config.metrics.layers_cfg.enable_rlc ? config.metrics.du_report_period : 0);

    for (const auto& cell : config.cells_cfg) {
      if (cell.cell.ntn_cfg.has_value()) {
        ntn_augment_du_qos_config(cell.cell.ntn_cfg.value(), out_cfg);
      }
    }
    return out_cfg;
  }

  for (const auto& qos : config.qos_cfg) {
    if (out_cfg.find(qos.five_qi) != out_cfg.end()) {
      report_error("Duplicate 5QI configuration: {}\n", qos.five_qi);
    }
    // Convert RLC config
    auto& out_rlc = out_cfg[qos.five_qi].rlc;
    if (!from_string(out_rlc.mode, qos.rlc.mode)) {
      report_error("Invalid RLC mode: {}, mode={}\n", qos.five_qi, qos.rlc.mode);
    }

    if (out_rlc.mode == rlc_mode::um_bidir) {
      // UM Config
      //< RX SN
      if (!from_number(out_rlc.um.rx.sn_field_length, qos.rlc.um.rx.sn_field_length)) {
        report_error("Invalid RLC UM RX SN: {}, SN={}\n", qos.five_qi, qos.rlc.um.rx.sn_field_length);
      }
      //< RX t-reassembly
      out_rlc.um.rx.t_reassembly = qos.rlc.um.rx.t_reassembly;
      //< TX SN
      if (!from_number(out_rlc.um.tx.sn_field_length, qos.rlc.um.tx.sn_field_length)) {
        report_error("Invalid RLC UM TX SN: {}, SN={}\n", qos.five_qi, qos.rlc.um.tx.sn_field_length);
      }
      out_rlc.um.tx.queue_size       = qos.rlc.um.tx.queue_size;
      out_rlc.um.tx.queue_size_bytes = qos.rlc.um.tx.queue_size_bytes;
    } else if (out_rlc.mode == rlc_mode::am) {
      // AM Config
      out_rlc.am = generate_du_rlc_am_config(qos.rlc.am);
    }

    out_rlc.metrics_period =
        std::chrono::milliseconds((config.metrics.layers_cfg.enable_rlc) ? config.metrics.du_report_period : 0);

    // Convert F1-U config
    auto& out_f1u = out_cfg[qos.five_qi].f1u;
    //< t-Notify
    out_f1u.ul_t_notif_timer = std::chrono::milliseconds(qos.f1u_du.t_notify);
    out_f1u.rlc_queue_bytes_limit =
        qos.rlc.mode == "am" ? qos.rlc.am.tx.queue_size_bytes : qos.rlc.um.tx.queue_size_bytes;
    out_f1u.warn_on_drop   = config.warn_on_drop;
    out_f1u.ul_buffer_size = qos.f1u_du.ul_buffer_size;

    // Convert triggered UL grant config (optional - presence enables the feature)
    if (qos.mac.triggered_ul_grant.has_value()) {
      auto& out_trig       = out_cfg[qos.five_qi].triggered_ul_grant.emplace();
      out_trig.delay_slots = qos.mac.triggered_ul_grant->delay_slots;
      out_trig.grant_size  = units::bytes{qos.mac.triggered_ul_grant->grant_size};
    }
  }
  return out_cfg;
}

static std::map<srb_id_t, odu::du_srb_config> generate_du_srb_config(const du_high_unit_config& config)
{
  std::map<srb_id_t, odu::du_srb_config> srb_cfg;

  // SRB1
  srb_cfg.insert(std::make_pair(srb_id_t::srb1, odu::du_srb_config{}));
  if (config.srb_cfg.find(srb_id_t::srb1) != config.srb_cfg.end()) {
    auto& out_rlc             = srb_cfg[srb_id_t::srb1].rlc;
    out_rlc.mode              = rlc_mode::am;
    out_rlc.am                = generate_du_rlc_am_config(config.srb_cfg.at(srb_id_t::srb1).rlc);
    out_rlc.am.tx.pdcp_sn_len = pdcp_sn_size::size12bits;
  } else {
    srb_cfg.at(srb_id_t::srb1).rlc = make_default_srb_rlc_config();
  }

  // SRB2
  srb_cfg.insert(std::make_pair(srb_id_t::srb2, odu::du_srb_config{}));
  if (config.srb_cfg.find(srb_id_t::srb2) != config.srb_cfg.end()) {
    auto& out_rlc             = srb_cfg[srb_id_t::srb2].rlc;
    out_rlc.mode              = rlc_mode::am;
    out_rlc.am                = generate_du_rlc_am_config(config.srb_cfg.at(srb_id_t::srb2).rlc);
    out_rlc.am.tx.pdcp_sn_len = pdcp_sn_size::size12bits;
  } else {
    srb_cfg.at(srb_id_t::srb2).rlc = make_default_srb_rlc_config();
  }

  // SRB3
  srb_cfg.insert(std::make_pair(srb_id_t::srb3, odu::du_srb_config{}));
  if (config.srb_cfg.find(srb_id_t::srb3) != config.srb_cfg.end()) {
    auto& out_rlc             = srb_cfg[srb_id_t::srb3].rlc;
    out_rlc.mode              = rlc_mode::am;
    out_rlc.am                = generate_du_rlc_am_config(config.srb_cfg.at(srb_id_t::srb3).rlc);
    out_rlc.am.tx.pdcp_sn_len = pdcp_sn_size::size12bits;
  } else {
    srb_cfg.at(srb_id_t::srb3).rlc = make_default_srb_rlc_config();
  }

  for (const auto& cell : config.cells_cfg) {
    if (cell.cell.ntn_cfg.has_value()) {
      ntn_augment_du_srb_config(cell.cell.ntn_cfg.value(), srb_cfg);
    }
  }

  return srb_cfg;
}

static mac_expert_config generate_mac_expert_config(const du_high_unit_config& config)
{
  mac_expert_config out_cfg = {};
  for (const auto& cell : config.cells_cfg) {
    out_cfg.configs.push_back({.max_consecutive_dl_kos  = cell.cell.pdsch_cfg.max_consecutive_kos,
                               .max_consecutive_ul_kos  = cell.cell.pusch_cfg.max_consecutive_kos,
                               .max_consecutive_csi_dtx = cell.cell.pucch_cfg.max_consecutive_kos});
  }

  return out_cfg;
}

static scheduler_expert_config generate_scheduler_expert_config(const du_high_unit_config& config)
{
  scheduler_expert_config out_cfg = config_helpers::make_default_scheduler_expert_config();

  const du_high_unit_base_cell_config& cell = config.cells_cfg.front().cell;

  // UE parameters.
  const du_high_unit_pdsch_config&     pdsch                = cell.pdsch_cfg;
  const du_high_unit_pdcch_config&     pdcch                = cell.pdcch_cfg;
  const du_high_unit_pusch_config&     pusch                = cell.pusch_cfg;
  const du_high_unit_scheduler_config& app_sched_expert_cfg = cell.scheduler_cfg;
  out_cfg.ue.dl_mcs                                         = {pdsch.min_ue_mcs, pdsch.max_ue_mcs};
  out_cfg.ue.pdsch_rv_sequence.assign(pdsch.rv_sequence.begin(), pdsch.rv_sequence.end());
  out_cfg.ue.dl_harq_la_cqi_drop_threshold     = pdsch.harq_la_cqi_drop_threshold;
  out_cfg.ue.dl_harq_la_ri_drop_threshold      = pdsch.harq_la_ri_drop_threshold;
  out_cfg.ue.max_nof_dl_harq_retxs             = pdsch.max_nof_harq_retxs;
  out_cfg.ue.max_nof_ul_harq_retxs             = pusch.max_nof_harq_retxs;
  out_cfg.ue.dl_harq_retx_timeout              = std::chrono::milliseconds{pdsch.harq_retx_timeout};
  out_cfg.ue.ul_harq_retx_timeout              = std::chrono::milliseconds{pusch.harq_retx_timeout};
  out_cfg.ue.max_pdschs_per_slot               = pdsch.max_pdschs_per_slot;
  out_cfg.ue.max_pdcch_alloc_attempts_per_slot = pdsch.max_pdcch_alloc_attempts_per_slot;
  out_cfg.ue.pdcch_al_cqi_offset               = pdcch.dedicated.al_cqi_offset;
  out_cfg.ue.pdsch_nof_rbs                     = {pdsch.min_rb_size, pdsch.max_rb_size};
  out_cfg.ue.pusch_nof_rbs                     = {pusch.min_rb_size, pusch.max_rb_size};
  out_cfg.ue.olla_dl_target_bler               = pdsch.olla_target_bler;
  out_cfg.ue.olla_cqi_inc                      = pdsch.olla_cqi_inc;
  out_cfg.ue.olla_max_cqi_offset               = pdsch.olla_max_cqi_offset;
  if (cell.ntn_cfg.has_value()) {
    out_cfg.ue.auto_ack_harq = true;
  }
  out_cfg.ue.ul_mcs = {pusch.min_ue_mcs, pusch.max_ue_mcs};
  out_cfg.ue.pusch_rv_sequence.assign(pusch.rv_sequence.begin(), pusch.rv_sequence.end());
  out_cfg.ue.enable_csi_rs_pdsch_multiplexing = pdsch.enable_csi_rs_pdsch_multiplexing;
  out_cfg.ue.initial_ul_dc_offset             = pusch.dc_offset;
  out_cfg.ue.max_puschs_per_slot              = pusch.max_puschs_per_slot;
  out_cfg.ue.olla_ul_target_bler              = pusch.olla_target_bler;
  out_cfg.ue.olla_ul_snr_inc                  = pusch.olla_snr_inc;
  out_cfg.ue.olla_max_ul_snr_offset           = pusch.olla_max_snr_offset;
  out_cfg.ue.pdsch_crb_limits                 = {pdsch.start_rb, pdsch.end_rb};
  out_cfg.ue.pdsch_interleaving_bundle_size   = pdsch.interleaving_bundle_size;
  out_cfg.ue.pusch_crb_limits                 = {pusch.start_rb, pusch.end_rb};
  if (app_sched_expert_cfg.policy_cfg.has_value()) {
    out_cfg.ue.policy_cfg = *app_sched_expert_cfg.policy_cfg;
  }
  out_cfg.ue.ul_power_ctrl.enable_pusch_cl_pw_control        = pusch.enable_closed_loop_pw_control;
  out_cfg.ue.ul_power_ctrl.enable_phr_bw_adaptation          = pusch.enable_phr_bw_adaptation;
  out_cfg.ue.ul_power_ctrl.target_pusch_sinr                 = pusch.target_pusch_sinr;
  out_cfg.ue.ul_power_ctrl.path_loss_for_target_pusch_sinr   = pusch.path_loss_for_target_pusch_sinr;
  out_cfg.ue.ul_power_ctrl.ema_alpha_cl_pw_control_sinr      = pusch.ema_alpha_cl_pw_control_sinr;
  out_cfg.ue.ta_control.ta_cmd_offset_threshold              = cell.ta_cfg.ta_cmd_offset_threshold;
  out_cfg.ue.ta_control.target                               = cell.ta_cfg.ta_target;
  out_cfg.ue.ta_control.measurement_period                   = cell.ta_cfg.ta_measurement_slot_period;
  out_cfg.ue.ta_control.measurement_prohibit_period          = cell.ta_cfg.ta_measurement_slot_prohibit_period;
  out_cfg.ue.ta_control.update_measurement_ul_sinr_threshold = cell.ta_cfg.ta_update_measurement_ul_sinr_threshold;
  out_cfg.ue.ta_control.outlier_detection_zscore_threshold   = cell.ta_cfg.ta_outlier_detection_zscore_threshold;
  out_cfg.ue.pre_policy_rr_ue_group_size                     = app_sched_expert_cfg.nof_preselected_newtx_ues;
  const srs_periodicity srs_prohibit_time                    = static_cast<srs_periodicity>(static_cast<unsigned>(
      static_cast<float>(get_nof_slots_per_subframe(cell.common_scs)) * cell.srs_cfg.srs_period_prohibit_time_ms));
  // This is only set with aperiodic SRS.
  out_cfg.ue.srs_prohibit_time =
      cell.srs_cfg.srs_type_enabled == "aperiodic" ? std::optional<srs_periodicity>(srs_prohibit_time) : std::nullopt;

  // PUCCH and scheduler expert parameters.
  out_cfg.ue.max_ul_grants_per_slot                           = cell.ul_common_cfg.max_ul_grants_per_slot;
  out_cfg.ue.max_pucchs_per_slot                              = cell.ul_common_cfg.max_pucchs_per_slot;
  out_cfg.ue.min_pucch_pusch_prb_distance                     = cell.ul_common_cfg.min_pucch_pusch_prb_distance;
  const du_high_unit_pucch_config& pucch                      = cell.pucch_cfg;
  out_cfg.ue.ul_power_ctrl.enable_pucch_cl_pw_control         = pucch.enable_closed_loop_pw_control;
  out_cfg.ue.ul_power_ctrl.pucch_f0_sinr_target_dB            = pucch.pucch_f0_sinr_target_dB;
  out_cfg.ue.ul_power_ctrl.pucch_f2_sinr_target_dB            = pucch.pucch_f2_sinr_target_dB;
  out_cfg.ue.ul_power_ctrl.pucch_f3_sinr_target_dB            = pucch.pucch_f3_sinr_target_dB;
  out_cfg.ue.ul_power_ctrl.ema_alpha_cl_pw_control_pucch_sinr = pucch.ema_alpha_cl_pw_control_sinr;

  // RA parameters.
  const du_high_unit_rach_config& prach = cell.prach_cfg;

  out_cfg.ra.rar_mcs_index            = pdsch.fixed_rar_mcs;
  out_cfg.ra.max_nof_msg3_harq_retxs  = prach.max_msg3_harq_retx;
  out_cfg.ra.msg3_mcs_index           = prach.fixed_msg3_mcs;
  out_cfg.ra.nof_prach_guardbands_rbs = prach.nof_prach_guardbands_rbs;

  // SI parameters.
  out_cfg.si.sib1_mcs_index    = pdsch.fixed_sib1_mcs;
  out_cfg.si.sib1_dci_aggr_lev = aggregation_level::n4;

  // Logging and tracing.
  out_cfg.log_broadcast_messages       = config.loggers.broadcast_enabled;
  out_cfg.log_high_latency_diagnostics = config.loggers.high_latency_diagnostics_enabled;

  const error_type<std::string> error = is_scheduler_expert_config_valid(out_cfg);
  if (!error) {
    report_error("Invalid scheduler expert configuration detected.\n");
  }

  return out_cfg;
}

static odu::du_test_mode_config generate_test_mode_config(const du_high_unit_config& du_high_unit_cfg)
{
  odu::du_test_mode_config test_cfg;
  if (du_high_unit_cfg.test_mode_cfg.test_ue.rnti == rnti_t::INVALID_RNTI) {
    return test_cfg;
  }

  const auto&                              test_ue = du_high_unit_cfg.test_mode_cfg.test_ue;
  std::optional<std::chrono::milliseconds> attach_detach_duration;
  if (test_ue.attach_detach_duration_ms.has_value()) {
    attach_detach_duration = std::chrono::milliseconds{*test_ue.attach_detach_duration_ms};
  }
  test_cfg.test_ue =
      odu::du_test_mode_config::test_mode_ue_config{test_ue.rnti,
                                                    test_ue.nof_ues,
                                                    test_ue.ue_creation_stagger_slots,
                                                    test_ue.auto_ack_indication_delay,
                                                    test_ue.pdsch_active,
                                                    test_ue.pusch_active,
                                                    test_ue.cqi,
                                                    test_ue.ri,
                                                    test_ue.pmi,
                                                    test_ue.i_1_1,
                                                    test_ue.i_1_2,
                                                    test_ue.i_1_3,
                                                    test_ue.i_2,
                                                    attach_detach_duration,
                                                    std::chrono::milliseconds{test_ue.attach_detach_guard_duration_ms}};

  return test_cfg;
}

void ocudu::generate_du_high_config(odu::du_high_configuration& du_hi_cfg, const du_high_unit_config& du_high_unit_cfg)
{
  // DU-high configuration.
  du_hi_cfg.ran.gnb_du_id          = du_high_unit_cfg.gnb_du_id;
  du_hi_cfg.ran.gnb_du_name        = fmt::format("odu{}", fmt::underlying(du_hi_cfg.ran.gnb_du_id));
  du_hi_cfg.ran.cells              = generate_du_cell_config(du_high_unit_cfg);
  du_hi_cfg.metrics.enable_mac     = du_high_unit_cfg.metrics.layers_cfg.enable_mac;
  du_hi_cfg.metrics.enable_rlc     = du_high_unit_cfg.metrics.layers_cfg.enable_rlc;
  du_hi_cfg.metrics.enable_sched   = du_high_unit_cfg.metrics.layers_cfg.enable_scheduler;
  du_hi_cfg.metrics.enable_du_proc = du_high_unit_cfg.metrics.layers_cfg.enable_du_proc;
  du_hi_cfg.metrics.period         = std::chrono::milliseconds{du_high_unit_cfg.metrics.du_report_period};

  // Validates the derived parameters.
  du_hi_cfg.ran.srbs                  = generate_du_srb_config(du_high_unit_cfg);
  du_hi_cfg.ran.qos                   = generate_du_qos_config(du_high_unit_cfg);
  du_hi_cfg.ran.mac_cfg               = generate_mac_expert_config(du_high_unit_cfg);
  du_hi_cfg.ran.mac_cfg.initial_crnti = to_rnti(0x4601);
  du_hi_cfg.ran.sched_cfg             = generate_scheduler_expert_config(du_high_unit_cfg);

  // Configure test mode
  du_hi_cfg.test_cfg = generate_test_mode_config(du_high_unit_cfg);
}

void ocudu::fill_du_high_worker_manager_config(worker_manager_config&     config,
                                               const du_high_unit_config& unit_cfg,
                                               bool                       is_blocking_mode_enabled)
{
  config.config_affinities.resize(unit_cfg.cells_cfg.size());

  auto& du_hi_cfg = config.du_hi_cfg.emplace();

  du_hi_cfg.ue_data_tasks_queue_size = unit_cfg.expert_execution_cfg.du_queue_cfg.ue_data_executor_queue_size;
  du_hi_cfg.is_rt_mode_enabled       = !is_blocking_mode_enabled;
  du_hi_cfg.nof_cells                = unit_cfg.cells_cfg.size();
  du_hi_cfg.executor_tracing_enable  = unit_cfg.tracer.executor_tracing_enable;

  auto& pcap_cfg = config.pcap_cfg;
  if (unit_cfg.pcaps.f1ap.enabled) {
    pcap_cfg.is_f1ap_enabled = true;
  }
  if (unit_cfg.pcaps.f1u.enabled) {
    pcap_cfg.is_f1u_enabled = true;
  }
  if (unit_cfg.pcaps.mac.enabled) {
    pcap_cfg.is_mac_enabled = true;
  }
  if (unit_cfg.pcaps.rlc.enabled) {
    pcap_cfg.is_rlc_enabled = true;
  }
}
