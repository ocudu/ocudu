// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/ran/pdcch/pdcch_candidates.h"
#include "ocudu/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "ocudu/ran/prach/prach_helper.h"
#include "ocudu/ran/sib/sib_helper.h"
#include "ocudu/ran/ssb/ssb_helper.h"
#include "ocudu/ran/ssb/ssb_mapping.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"

using namespace ocudu;

config_helpers::cell_config_builder_params_extended::cell_config_builder_params_extended(
    const cell_config_builder_params& source) :
  cell_config_builder_params(source)
{
  auto_derive_params();

  cell_nof_crbs =
      band_helper::get_n_rbs_from_bw(dl_carrier.carrier_bw, scs_common, band_helper::get_freq_range(dl_carrier.band));

  // If TDD band and TDD pattern has not been specified, generate a default one.
  if (not tdd_ul_dl_cfg_common.has_value() and band_helper::get_duplex_mode(dl_carrier.band) == duplex_mode::TDD) {
    tdd_ul_dl_cfg_common.emplace();
    tdd_ul_dl_cfg_common->ref_scs                            = scs_common;
    tdd_ul_dl_cfg_common->pattern1.dl_ul_tx_period_nof_slots = 10;
    tdd_ul_dl_cfg_common->pattern1.nof_dl_slots              = 6;
    tdd_ul_dl_cfg_common->pattern1.nof_dl_symbols            = 8;
    tdd_ul_dl_cfg_common->pattern1.nof_ul_slots              = 3;
    tdd_ul_dl_cfg_common->pattern1.nof_ul_symbols            = 0;
  } else if (tdd_ul_dl_cfg_common.has_value() and band_helper::get_duplex_mode(dl_carrier.band) != duplex_mode::TDD) {
    report_error("TDD pattern has been set for non-TDD band\n");
  }

  // Compute and store final SSB position based on (selected) values.
  ssb_arfcn = band_helper::get_ssb_arfcn(dl_carrier.arfcn_f_ref,
                                         dl_carrier.band,
                                         cell_nof_crbs,
                                         scs_common,
                                         *scs_ssb,
                                         offset_to_point_a.value(),
                                         k_ssb.value());
  ocudu_assert(ssb_arfcn.has_value(), "Unable to derive SSB location correctly");
}

template <bool IsDl>
static carrier_configuration
make_default_carrier_configuration(const config_helpers::cell_config_builder_params_extended& params)
{
  carrier_configuration cfg = params.dl_carrier;
  if constexpr (not IsDl) {
    cfg.arfcn_f_ref = band_helper::get_ul_arfcn_from_dl_arfcn(params.dl_carrier.arfcn_f_ref, cfg.band);
  }
  const min_channel_bandwidth min_channel_bw = band_helper::get_min_channel_bw(cfg.band, params.scs_common);
  ocudu_assert(bs_channel_bandwidth_to_MHz(cfg.carrier_bw) >= min_channel_bandwidth_to_MHz(min_channel_bw),
               "Carrier BW {}Mhz must be greater than or equal to minimum channel BW {}Mhz",
               bs_channel_bandwidth_to_MHz(cfg.carrier_bw),
               min_channel_bandwidth_to_MHz(min_channel_bw));
  return cfg;
}

static bwp_configuration make_default_init_bwp(const config_helpers::cell_config_builder_params_extended& params)
{
  bwp_configuration cfg{};
  cfg.scs  = params.scs_common;
  cfg.crbs = {0, params.cell_nof_crbs};
  cfg.cp   = cyclic_prefix::NORMAL;
  return cfg;
}

/// Generates a default CORESET#0 configuration. The default CORESET#0 table index value used is equal to 6.
/// \remark See TS 38.213, Table 13-1.
static coreset_configuration
make_default_coreset0_config(const config_helpers::cell_config_builder_params_extended& params)
{
  return coreset_configuration{params.dl_carrier.band,
                               *params.scs_ssb,
                               params.scs_common,
                               *params.cs0_index,
                               *params.k_ssb,
                               *params.offset_to_point_a,
                               params.pci};
}

static search_space_configuration
make_default_search_space_zero_config(const config_helpers::cell_config_builder_params_extended& params)
{
  return search_space_configuration{
      params.dl_carrier.band, params.scs_common, *params.scs_ssb, *params.cs0_index, params.ss0_index};
}

static search_space_configuration
make_default_ue_search_space_config(const config_helpers::cell_config_builder_params_extended& params)
{
  search_space_configuration cfg = make_default_common_search_space_config(params);
  cfg.set_non_ss0_coreset_id(to_coreset_id(1));
  cfg.set_non_ss0_id(to_search_space_id(2));
  cfg.set_non_ss0_monitored_dci_formats(search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);
  return cfg;
}

static pdcch_config make_default_pdcch_config(const config_helpers::cell_config_builder_params_extended& params)
{
  pdcch_config pdcch_cfg{};
  // >> Add CORESET#1.
  pdcch_cfg.coresets.push_back(make_default_coreset_config(params));
  // >> Add SearchSpace#2.
  pdcch_cfg.search_spaces.push_back(make_default_ue_search_space_config(params));
  pdcch_cfg.search_spaces[0].set_non_ss0_nof_candidates(
      {0,
       config_helpers::compute_max_nof_candidates(aggregation_level::n2, pdcch_cfg.coresets[0]),
       config_helpers::compute_max_nof_candidates(aggregation_level::n4, pdcch_cfg.coresets[0]),
       0,
       0});

  return pdcch_cfg;
}

static dl_config_common make_default_dl_config_common(const config_helpers::cell_config_builder_params_extended& params)
{
  dl_config_common cfg{};

  // Configure FrequencyInfoDL.
  cfg.freq_info_dl.freq_band_list.emplace_back();
  cfg.freq_info_dl.freq_band_list.back().band = params.dl_carrier.band;
  cfg.freq_info_dl.offset_to_point_a          = params.offset_to_point_a->value();
  cfg.freq_info_dl.scs_carrier_list.emplace_back();
  cfg.freq_info_dl.scs_carrier_list.back().scs               = params.scs_common;
  cfg.freq_info_dl.scs_carrier_list.back().offset_to_carrier = 0;
  cfg.freq_info_dl.scs_carrier_list.back().carrier_bandwidth = params.cell_nof_crbs;

  cfg.freq_info_dl.absolute_frequency_ssb = params.ssb_arfcn.value();

  // \c params.dl_f_ref_arfcn refers to the ARFCN of the DL f_ref, as per TS 38.104, Section 5.4.2.1.
  const double dl_absolute_freq_point_a = band_helper::get_abs_freq_point_a_from_f_ref(
      band_helper::nr_arfcn_to_freq(params.dl_carrier.arfcn_f_ref), params.cell_nof_crbs, params.scs_common);
  // \c absolute_freq_point_a needs to be expressed as in ARFCN, as per \c absoluteFrequencyPointA definition in
  // TS 38.211, Section 4.4.4.2.
  cfg.freq_info_dl.absolute_freq_point_a = band_helper::freq_to_nr_arfcn(dl_absolute_freq_point_a);

  // Configure initial DL BWP.
  cfg.init_dl_bwp.generic_params = make_default_init_bwp(params);
  cfg.init_dl_bwp.pdcch_common.coreset0.emplace(make_default_coreset0_config(params));
  cfg.init_dl_bwp.pdcch_common.search_spaces.push_back(make_default_search_space_zero_config(params));
  cfg.init_dl_bwp.pdcch_common.search_spaces.push_back(make_default_common_search_space_config(params));
  cfg.init_dl_bwp.pdcch_common.sib1_search_space_id   = to_search_space_id(0);
  cfg.init_dl_bwp.pdcch_common.paging_search_space_id = to_search_space_id(1);
  cfg.init_dl_bwp.pdcch_common.ra_search_space_id     = to_search_space_id(1);
  cfg.init_dl_bwp.pdsch_common.pdsch_td_alloc_list = time_domain_resource_helper::generate_dedicated_pdsch_td_res_list(
      params.tdd_ul_dl_cfg_common,
      cfg.init_dl_bwp.generic_params.cp,
      time_domain_resource_helper::calculate_minimum_pdsch_symbol(cfg.init_dl_bwp.pdcch_common,
                                                                  make_default_pdcch_config(params)));

  // Configure PCCH.
  cfg.pcch_cfg.default_paging_cycle = paging_cycle::rf128;
  cfg.pcch_cfg.nof_pf               = pcch_config::nof_pf_per_drx_cycle::oneT;
  cfg.pcch_cfg.paging_frame_offset  = 0;
  cfg.pcch_cfg.ns                   = pcch_config::nof_po_per_pf::one;

  return cfg;
}

static ul_config_common make_default_ul_config_common(const config_helpers::cell_config_builder_params_extended& params)
{
  ul_config_common cfg{};
  // This is the ARFCN of the UL f_ref, as per TS 38.104, Section 5.4.2.1.
  const arfcn_t ul_arfcn =
      band_helper::get_ul_arfcn_from_dl_arfcn(params.dl_carrier.arfcn_f_ref, params.dl_carrier.band);
  const double ul_absolute_freq_point_a = band_helper::get_abs_freq_point_a_from_f_ref(
      band_helper::nr_arfcn_to_freq(ul_arfcn), params.cell_nof_crbs, params.scs_common);
  // \c absolute_freq_point_a needs to be expressed as in ARFCN, as per \c absoluteFrequencyPointA definition in
  // TS 38.211, Section 4.4.4.2.
  cfg.freq_info_ul.absolute_freq_point_a = band_helper::freq_to_nr_arfcn(ul_absolute_freq_point_a);
  cfg.freq_info_ul.scs_carrier_list.resize(1);
  cfg.freq_info_ul.scs_carrier_list[0].scs               = params.scs_common;
  cfg.freq_info_ul.scs_carrier_list[0].offset_to_carrier = 0;
  cfg.freq_info_ul.scs_carrier_list[0].carrier_bandwidth = params.cell_nof_crbs;
  cfg.freq_info_ul.freq_band_list.emplace_back();
  cfg.freq_info_ul.freq_band_list.back().band = params.dl_carrier.band;
  cfg.init_ul_bwp.generic_params              = make_default_init_bwp(params);
  cfg.init_ul_bwp.rach_cfg_common.emplace();
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.zero_correlation_zone_config = 0;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index           = 16;
  if (band_helper::get_duplex_mode(params.dl_carrier.band) == duplex_mode::TDD) {
    std::optional<uint8_t> idx_found = prach_helper::find_valid_prach_config_index(
        params.scs_common,
        cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.zero_correlation_zone_config,
        *params.tdd_ul_dl_cfg_common);
    ocudu_assert(idx_found.has_value(), "Unable to find a PRACH config index for the given TDD pattern");
    cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index = idx_found.value();
  }

  // Although this is not specified in the TS, from our tests, the UE expects Msg1-SCS to be given when using short
  // PRACH Preambles formats. With long formats, we can set Msg1-SCS as \c invalid, in which case the UE derives the
  // PRACH SCS from \c prach-ConfigurationIndex in RACH-ConfigGeneric.
  const frequency_range freq_range = band_helper::get_freq_range(params.dl_carrier.band);
  const duplex_mode     duplex     = band_helper::get_duplex_mode(params.dl_carrier.band);
  cfg.init_ul_bwp.rach_cfg_common->msg1_scs =
      is_long_preamble(prach_configuration_get(
                           freq_range, duplex, cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)
                           .format)
          ? subcarrier_spacing::invalid
          : params.scs_common;
  cfg.init_ul_bwp.rach_cfg_common->restricted_set = restricted_set_config::UNRESTRICTED;
  // Set l839 for long preamble formats, l139 for short preamble formats, as per Tables 6.3.3.1-1 and 6.3.3.1-2,
  // TS 38.211.
  cfg.init_ul_bwp.rach_cfg_common->is_prach_root_seq_index_l839 = is_long_preamble(
      prach_configuration_get(freq_range, duplex, cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)
          .format);
  cfg.init_ul_bwp.rach_cfg_common->prach_root_seq_index      = 1;
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_fdm = 1;
  // Add +3 PRBS to the MSG1 frequency start, which act as a guardband between the PUCCH and PRACH.
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start = 6;

  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window = 10u << to_numerology_value(params.scs_common);
  cfg.init_ul_bwp.rach_cfg_common->rach_cfg_generic.preamble_rx_target_pw = -100;

  cfg.init_ul_bwp.pusch_cfg_common.emplace();
  // - [Implementation-defined] Ensure k2 value which is less than or equal to minimum value of k1(s) exist in the
  // first entry of list. This way PDSCH(s) are scheduled before PUSCH and all DL slots are filled with PDSCH and all
  // UL slots are filled with PUSCH under heavy load. It also ensures that correct DAI value goes in the UL PDCCH of
  // DCI Format 0_1.
  cfg.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list =
      time_domain_resource_helper::generate_dedicated_pusch_td_res_list(
          params.tdd_ul_dl_cfg_common, cfg.init_ul_bwp.generic_params.cp, params.min_k2);

  cfg.init_ul_bwp.pucch_cfg_common.emplace();
  cfg.init_ul_bwp.pucch_cfg_common->pucch_resource_common        = 11;
  cfg.init_ul_bwp.pucch_cfg_common->group_hopping                = pucch_group_hopping::NEITHER;
  cfg.init_ul_bwp.pucch_cfg_common->p0_nominal                   = -90;
  cfg.init_ul_bwp.pusch_cfg_common.value().p0_nominal_with_grant = -76;
  cfg.init_ul_bwp.pusch_cfg_common.value().msg3_delta_preamble   = 6;
  cfg.init_ul_bwp.pusch_cfg_common.value().msg3_delta_power      = 8;

  return cfg;
}

static csi_helper::csi_meas_config_builder_params
make_default_csi_meas_builder_params(const config_helpers::cell_config_builder_params_extended& params)
{
  // Parameters used to generate list of CSI resources.
  csi_helper::csi_meas_config_builder_params csi_params{};
  csi_params.pci                      = params.pci;
  csi_params.nof_rbs                  = params.cell_nof_crbs;
  csi_params.nof_ports                = params.dl_carrier.nof_ant;
  csi_params.max_nof_layers           = *params.max_nof_layers;
  csi_params.mcs_table                = pdsch_mcs_table::qam64;
  csi_params.csi_params.csi_rs_period = csi_helper::get_max_csi_rs_period(params.scs_common);

  if (band_helper::get_duplex_mode(params.dl_carrier.band) == duplex_mode::TDD) {
    // Set a default CSI report slot offset that falls in an UL slot.
    const auto& tdd_pattern = *params.tdd_ul_dl_cfg_common;

    const unsigned max_csi_symbol = *std::max_element(csi_params.csi_params.tracking_csi_ofdm_symbol_indices.begin(),
                                                      csi_params.csi_params.tracking_csi_ofdm_symbol_indices.end());

    const ssb_configuration ssb_cfg = make_default_ssb_config(params);
    const auto ssb_slots = ssb_helper::get_occupied_slot_offsets(ssb_cfg, params.dl_carrier.band, tdd_pattern.ref_scs);
    const auto sib1_occ  = sib_helper::get_occupied_slot_offsets(
        ssb_cfg, params.dl_carrier.band, tdd_pattern.ref_scs, params.ss0_index, params.cs0_index->value());
    if (not csi_helper::derive_valid_csi_rs_slot_offsets(csi_params.csi_params,
                                                         std::nullopt,
                                                         std::nullopt,
                                                         {},
                                                         tdd_pattern,
                                                         max_csi_symbol,
                                                         ssb_cfg.ssb_period,
                                                         ssb_slots,
                                                         sib1_occ.window_period_slots,
                                                         sib1_occ.slot_offsets)) {
      report_fatal_error("Failed to find valid csi-MeasConfig");
    }

    for (unsigned i = 0; i != nof_slots_per_tdd_period(tdd_pattern); ++i) {
      // TODO: Support reports in the special slot.
      if (is_tdd_full_ul_slot(tdd_pattern, i)) {
        csi_params.csi_params.csi_report_slot_offset = i;
      }
    }
  } else {
    csi_params.csi_params.meas_csi_slot_offsets = {2U};
  }

  return csi_params;
}

/// Builds a BWP config from a cell config builder parameters.
static bwp_builder_params
make_default_bwp_builder_params(const config_helpers::cell_config_builder_params_extended& params)
{
  bwp_builder_params bwp;
  bwp.pdcch_cfg            = make_default_pdcch_config(params);
  bwp.pdsch.max_nof_layers = params.max_nof_layers;
  bwp.pusch.min_k2         = params.min_k2;
  bwp.pucch.min_k1         = params.min_k1;
  bwp.pucch.sr_period      = static_cast<sr_periodicity>(get_nof_slots_per_subframe(params.scs_common) * 40U);
  if (params.dl_carrier.nof_ant == 4) {
    std::get<pucch_f2_params>(bwp.pucch.resources.f2_or_f3_or_f4_params).max_code_rate = max_pucch_code_rate::dot_35;
  }
  if (params.csi_rs_enabled) {
    bwp.csi = make_default_csi_meas_builder_params(params).csi_params;
  }
  return bwp;
}

carrier_configuration
config_helpers::make_default_dl_carrier_configuration(const cell_config_builder_params_extended& params)
{
  return make_default_carrier_configuration<true>(params);
}

carrier_configuration
config_helpers::make_default_ul_carrier_configuration(const cell_config_builder_params_extended& params)
{
  return make_default_carrier_configuration<false>(params);
}

ssb_configuration
config_helpers::make_default_ssb_config(const config_helpers::cell_config_builder_params_extended& params)
{
  ssb_configuration cfg{};

  cfg.scs               = *params.scs_ssb;
  cfg.offset_to_point_A = *params.offset_to_point_a;
  cfg.ssb_period        = ssb_periodicity::ms10;
  cfg.k_ssb             = *params.k_ssb;

  // Set SSB idx 0 to 1.
  cfg.ssb_bitmap.set(0);
  cfg.ssb_bitmap.set_L_max(ssb_get_L_max(*params.scs_ssb, params.dl_carrier.arfcn_f_ref, params.dl_carrier.band));
  // Set SSB beam ID 0 (corresponding to SSB idx 0) to 0.
  cfg.beam_ids = {0};

  // The values we assign to these parameters are implementation-defined.
  cfg.ssb_block_power = -16;
  cfg.pss_to_sss_epre = ssb_pss_to_sss_epre::dB_0;

  return cfg;
}

ran_cell_config config_helpers::make_default_ran_cell_config(const cell_config_builder_params_extended& params)
{
  ran_cell_config cfg;
  cfg.pci           = params.pci;
  cfg.dl_carrier    = make_default_dl_carrier_configuration(params);
  cfg.ul_carrier    = make_default_ul_carrier_configuration(params);
  cfg.dl_cfg_common = make_default_dl_config_common(params);
  cfg.ul_cfg_common = make_default_ul_config_common(params);
  cfg.ssb_cfg       = make_default_ssb_config(params);
  // The CORESET duration of 3 symbols is only permitted if dmrs-typeA-Position is set to 3. Refer TS 38.211, 7.3.2.2.
  cfg.dmrs_typeA_pos = cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.value().duration() >= 3U
                           ? dmrs_typeA_position::pos3
                           : dmrs_typeA_position::pos2;
  cfg.tdd_cfg        = params.tdd_ul_dl_cfg_common;
  cfg.init_bwp       = make_default_bwp_builder_params(params);
  return cfg;
}

csi_helper::csi_meas_config_builder_params
config_helpers::make_csi_meas_config_builder_params(const ran_cell_config& cell_cfg)
{
  ocudu_assert(cell_cfg.init_bwp.csi.has_value(), "CSI parameters are required to build CSI resources");

  csi_helper::csi_meas_config_builder_params csi_params;
  csi_params.pci            = cell_cfg.pci;
  csi_params.nof_rbs        = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  csi_params.nof_ports      = cell_cfg.dl_carrier.nof_ant;
  csi_params.max_nof_layers = cell_cfg.init_bwp.pdsch.max_nof_layers.value_or(csi_params.nof_ports);
  csi_params.mcs_table      = cell_cfg.init_bwp.pdsch.mcs_table;
  csi_params.csi_params     = cell_cfg.init_bwp.csi.value();
  return csi_params;
}

coreset_configuration
config_helpers::make_default_coreset_config(const config_helpers::cell_config_builder_params_extended& params,
                                            coreset_id                                                 cs_id)
{
  // PRBs spanning the maximnum number of CRBs possible.
  freq_resource_bitmap freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
  const unsigned       coreset_nof_resources = params.cell_nof_crbs / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE;
  freq_resources.fill(0, coreset_nof_resources, true);

  // Number of symbols equal to CORESET#0 duration.
  const pdcch_type0_css_coreset_description desc = pdcch_type0_css_coreset_get(
      params.dl_carrier.band, *params.scs_ssb, params.scs_common, params.cs0_index->value(), params.k_ssb->value());

  return coreset_configuration{cs_id,
                               freq_resources,
                               desc.nof_symb_coreset,
                               std::nullopt,
                               coreset_configuration::precoder_granularity_type::same_as_reg_bundle};
}

search_space_configuration config_helpers::make_default_common_search_space_config(
    const config_helpers::cell_config_builder_params_extended& params)
{
  search_space_configuration::monitoring_symbols_within_slot_t monitoring_symbols_within_slot(
      NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
  monitoring_symbols_within_slot.set(0, true);
  return search_space_configuration{to_search_space_id(1),
                                    to_coreset_id(0),
                                    {0, 0, 1, 0, 0},
                                    search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true},
                                    1,
                                    0,
                                    params.scs_common,
                                    1,
                                    monitoring_symbols_within_slot};
}

uint8_t config_helpers::compute_max_nof_candidates(aggregation_level aggr_lvl, const coreset_configuration& cs_cfg)
{
  // 1 REG = 1 RB and 1 symbol.
  // 1 CCE = 6 {PRB, symbol}. e.g. 3 PRBs over 2 symbols or 6 PRBs over 1 symbol, etc.
  // Example: 3 Frequency domain resources, 2 symbol duration contains 6 CCEs.
  const unsigned max_coreset_cces   = cs_cfg.freq_domain_resources().count() * cs_cfg.duration();
  unsigned       max_nof_candidates = max_coreset_cces / to_nof_cces(aggr_lvl);
  // See TS 38.331, SearchSpace IE.
  // aggregationLevelX - ENUMERATED {n0, n1, n2, n3, n4, n5, n6, n8}.
  if (max_nof_candidates == 7) {
    max_nof_candidates = 6;
  }
  return max_nof_candidates > PDCCH_MAX_NOF_CANDIDATES_SS ? PDCCH_MAX_NOF_CANDIDATES_SS : max_nof_candidates;
}
