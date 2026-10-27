// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/serving_cell_config_validator.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/csi_rs/csi_rs_config_helpers.h"
#include "ocudu/ran/csi_rs/csi_rs_pattern.h"
#include "ocudu/ran/pdcch/pdcch_candidates.h"
#include "ocudu/ran/srs/srs_bandwidth_configuration.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/sched_consts.h"
#include "ocudu/support/config/validator_helpers.h"
#include "ocudu/support/enum_utils.h"

using namespace ocudu;

#define VERIFY_ID_EXISTS(cond_lambda, id_list, ...)                                                                    \
  if (std::find_if(id_list.begin(), id_list.end(), cond_lambda) == id_list.end()) {                                    \
    return make_unexpected(fmt::format(__VA_ARGS__));                                                                  \
  }

validator_result config_validators::validate_pdcch_cfg(const serving_cell_config& ue_cell_cfg,
                                                       const dl_config_common&    dl_cfg_common)
{
  const auto& init_dl_bwp = ue_cell_cfg.init_dl_bwp;
  if (init_dl_bwp.pdcch_cfg.has_value()) {
    const auto& pdcch_cfg = init_dl_bwp.pdcch_cfg.value();

    VERIFY(has_unique_ids(pdcch_cfg.coresets, [](const coreset_configuration& cs) { return cs.get_id(); }),
           "Duplication of CoresetId");
    VERIFY(has_unique_ids(pdcch_cfg.search_spaces, [](const search_space_configuration& ss) { return ss.get_id(); }),
           "Duplication of SearchSpaceId");

    for (const auto& ss : pdcch_cfg.search_spaces) {
      const bool cset_id_found_in_ded =
          std::find_if(pdcch_cfg.coresets.begin(),
                       pdcch_cfg.coresets.end(),
                       [cs_id = ss.get_coreset_id()](const coreset_configuration& cset_cfg) {
                         return cset_cfg.get_id() == cs_id;
                       }) != pdcch_cfg.coresets.end();
      const bool cst_id_found_in_common =
          dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.has_value()
              ? dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value().get_id() == ss.get_coreset_id()
              : false;
      const bool cst_id_found_in_coreset0 = ss.get_coreset_id() == 0;
      VERIFY(cset_id_found_in_ded or cst_id_found_in_common or cst_id_found_in_coreset0,
             "Coreset Id. {} indexed by SearchSpace Id. {} not found within the configured Coresets",
             fmt::underlying(ss.get_coreset_id()),
             fmt::underlying(ss.get_id()));
    }

    const unsigned total_nof_monitored_pdcch_candidates =
        config_helpers::compute_tot_nof_monitored_pdcch_candidates_per_slot(ue_cell_cfg, dl_cfg_common);
    const auto scs = dl_cfg_common.init_dl_bwp.generic_params.scs;
    VERIFY(total_nof_monitored_pdcch_candidates <= max_nof_monitored_pdcch_candidates(scs),
           "Nof. PDCCH candidates monitored per slot for a DL BWP={} exceeds maximum value={}\n",
           total_nof_monitored_pdcch_candidates,
           max_nof_monitored_pdcch_candidates(scs));
  }

  // TODO: Validate other parameters.
  return {};
}

static validator_result validate_zp_csi_rs(const serving_cell_config& ue_cell_cfg)
{
  if (not ue_cell_cfg.init_dl_bwp.pdsch_cfg.has_value()) {
    return {};
  }
  const auto& pdsch_cfg = ue_cell_cfg.init_dl_bwp.pdsch_cfg.value();

  const auto& zp_list = pdsch_cfg.zp_csi_rs_res_list;
  VERIFY(not zp_list.empty() == pdsch_cfg.p_zp_csi_rs_res.has_value(),
         "Only periodic ZP-CSI-RS-ResourceId is supported");
  VERIFY(not zp_list.empty() == ue_cell_cfg.csi_meas_cfg.has_value(),
         "CSI-MeasConfig must be configured if and only if ZP-CSI-RS is configured");
  if (zp_list.empty()) {
    // No CSI configured.
    return {};
  }
  const csi_meas_config& csi_cfg = ue_cell_cfg.csi_meas_cfg.value();

  // Consistency of ZP-CSI-RS resources and periodic resource set.
  VERIFY(has_unique_ids(zp_list, &zp_csi_rs_resource::id), "Duplication of ZP-CSI-RS-ResourceId");
  VERIFY(pdsch_cfg.p_zp_csi_rs_res->id == (zp_csi_rs_res_set_id_t)0,
         "Only ZP-CSI-RS-ResourceId=0 is allowed for periodic ZP-CSI-RS");
  VERIFY(are_all_unique(pdsch_cfg.p_zp_csi_rs_res->zp_csi_rs_res_list), "Duplication of ZP-CSI-RS-ResourceId");

  // CSI-IM resources should match with ZP-CSI-RS.
  for (const auto& csi_im : csi_cfg.csi_im_res_list) {
    bool found = std::any_of(zp_list.begin(), zp_list.end(), [&csi_im](const zp_csi_rs_resource& zp) {
      return zp.period == csi_im.csi_res_period and zp.offset == csi_im.csi_res_offset and
             zp.res_mapping.freq_band_rbs == csi_im.freq_band_rbs and
             zp.res_mapping.first_ofdm_symbol_in_td == csi_im.symbol_location;
    });
    VERIFY(found,
           "CSI-IM does not overlap with ZP-CSI-RS. CSI-IM: {{period={} offset={} band={} symbol={}}}",
           fmt::underlying(csi_im.csi_res_period),
           csi_im.csi_res_offset,
           csi_im.freq_band_rbs,
           csi_im.symbol_location);
  }

  return {};
}

validator_result config_validators::validate_pdsch_cfg(const serving_cell_config& ue_cell_cfg)
{
  const auto& init_dl_bwp = ue_cell_cfg.init_dl_bwp;
  if (not init_dl_bwp.pdsch_cfg.has_value()) {
    return {};
  }
  const pdsch_config& pdsch_cfg = init_dl_bwp.pdsch_cfg.value();
  const auto&         dl_lst    = pdsch_cfg.pdsch_td_alloc_list;
  for (const auto& pdsch : dl_lst) {
    VERIFY(pdsch.k0 <= SCHEDULER_MAX_K0, "k0={} value exceeds maximum supported k0", pdsch.k0);
  }

  // TODO: Validate other parameters.
  return {};
}

validator_result config_validators::validate_pusch_cfg(const uplink_config& ul_config, bool has_csi)
{
  VERIFY(ul_config.init_ul_bwp.pusch_cfg.has_value(), "Missing configuration for pusch-Config in spCellConfig");

  const auto& pusch_cfg = ul_config.init_ul_bwp.pusch_cfg.value();

  VERIFY(pusch_cfg.uci_cfg.has_value(), "Missing configuration for uci-OnPUSCH in pusch-Config");
  VERIFY(pusch_cfg.uci_cfg.value().beta_offsets_cfg.has_value() and
             std::holds_alternative<uci_on_pusch::beta_offsets_semi_static>(
                 pusch_cfg.uci_cfg.value().beta_offsets_cfg.value()),
         "Missing configuration or wrong configuration for beta-Offsets in uci-OnPUSCH");
  const auto& beta_offsets =
      std::get<uci_on_pusch::beta_offsets_semi_static>(pusch_cfg.uci_cfg.value().beta_offsets_cfg.value());
  VERIFY(beta_offsets.beta_offset_ack_idx_1.has_value() and beta_offsets.beta_offset_ack_idx_2.has_value() and
             beta_offsets.beta_offset_ack_idx_3.has_value(),
         "betaOffsetACK-Index1, betaOffsetACK-Index2 and betaOffsetACK-Index3 in uci-OnPUSCH must be configured");
  if (has_csi) {
    VERIFY(beta_offsets.beta_offset_csi_p1_idx_1.has_value() and beta_offsets.beta_offset_csi_p1_idx_2.has_value(),
           "When CSI is enabled, betaOffsetCSI-Part1-Index1 and betaOffsetCSI-Part1-Index2 in uci-OnPUSCH must be "
           "configured");
  }
  VERIFY(pusch_cfg.pusch_pwr_ctrl.has_value(), "Missing configuration for pusch-PowerControl in pusch-Config");
  VERIFY(not pusch_cfg.pusch_pwr_ctrl.value().is_tpc_accumulation_disabled,
         "TPC accumulation for PUSCH power control is expected to be enabled");

  VERIFY(pusch_cfg.pusch_pwr_ctrl.value().p0_alphasets.size() == 1 and
             pusch_cfg.pusch_pwr_ctrl.value().p0_alphasets.front().p0.has_value(),
         "The P0-alphaSet list for PUSCH power control is expected to have size 1 with p0 value configured");

  return {};
}

validator_result config_validators::validate_srs_cfg(const serving_cell_config& ue_cell_cfg,
                                                     crb_interval               ul_bwp_crbs,
                                                     span<const uint8_t>        dl_data_to_ul_ack)
{
  VERIFY(ue_cell_cfg.ul_config.has_value() and ue_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.has_value(),
         "Missing configuration for uplinkConfig or srs-Config in spCellConfig");

  const auto& srs_cfg = ue_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();

  VERIFY(srs_cfg.srs_res_set_list.size() == 1 and srs_cfg.srs_res_set_list.front().id == srs_config::MIN_SRS_RES_SET_ID,
         "The SRS resource set list is expected to have size 1 and its only set is expected to have ID 0");
  const auto& srs_res_set = srs_cfg.srs_res_set_list.front();
  if (std::holds_alternative<srs_config::srs_resource_set::aperiodic_resource_type>(srs_res_set.res_type)) {
    const auto& aperiodic_set = std::get<srs_config::srs_resource_set::aperiodic_resource_type>(srs_res_set.res_type);
    const auto  min_k1_it     = std::min_element(dl_data_to_ul_ack.begin(), dl_data_to_ul_ack.end());
    VERIFY(min_k1_it != dl_data_to_ul_ack.end(), "Expected min k1 from a list");
    const unsigned min_k1 = *min_k1_it;
    VERIFY(aperiodic_set.slot_offset.has_value() and aperiodic_set.slot_offset.value() > min_k1,
           "The SRS resource set ID {}'s slot_offset should be greater than the max between min_k1 and max k2",
           fmt::underlying(srs_res_set.id));
    VERIFY(aperiodic_set.aperiodic_srs_res_trigger == static_cast<uint8_t>(srs_res_set.id) + 1U,
           "The SRS resource set ID {}'s aperiodic_srs_res_trigger should be equal its ID + 1",
           fmt::underlying(srs_res_set.id));
  }
  VERIFY(srs_cfg.srs_res_list.size() == 1 and srs_cfg.srs_res_list.front().id.ue_res_id == srs_config::MIN_SRS_RES_ID,
         "The SRS resource list is expected to have size 1 and its only resource is expected to have ID 0");
  VERIFY(srs_res_set.srs_res_id_list.size() == 1,
         "The SRS resource list of the SRS resource set ID {} is expected to have size 1",
         fmt::underlying(srs_res_set.id));
  VERIFY(srs_res_set.srs_res_id_list.front() == srs_cfg.srs_res_list.front().id.ue_res_id,
         "The SRS resource set ID {}'s resource should point to the SRS resource ID 0",
         fmt::underlying(srs_res_set.id));
  VERIFY(srs_res_set.srs_res_set_usage == srs_usage::codebook, "Only SRS resource set usage \"codebook\" is supported");

  const auto& srs_res = srs_cfg.srs_res_list.front();
  VERIFY(
      (std::holds_alternative<srs_config::srs_resource_set::aperiodic_resource_type>(srs_res_set.res_type) and
       srs_res.res_type == srs_resource_type::aperiodic) or
          (std::holds_alternative<srs_config::srs_resource_set::periodic_resource_type>(srs_res_set.res_type) and
           srs_res.res_type == srs_resource_type::periodic) or
          (std::holds_alternative<srs_config::srs_resource_set::semi_persistent_resource_type>(srs_res_set.res_type) and
           srs_res.res_type == srs_resource_type::semi_persistent),
      "The SRS resource set and its resource should be of the same type");
  if (srs_res.res_type == srs_resource_type::periodic) {
    VERIFY(srs_res.periodicity_and_offset.has_value(),
           "The SRS resource should have a periodicity and offset when the resource type is periodic");
    VERIFY(srs_res.periodicity_and_offset.value().offset <
               static_cast<unsigned>(srs_res.periodicity_and_offset.value().period),
           "The SRS resource offset should be less than the periodicity");
  }
  VERIFY(srs_res.tx_comb.tx_comb_offset < static_cast<uint8_t>(srs_res.tx_comb.size),
         "The SRS resource txCombOffset should be less than the TX comb size");
  const uint8_t max_tx_comb_cs = srs_res.tx_comb.size == tx_comb_size::n2 ? 7U : 11U;
  VERIFY(srs_res.tx_comb.tx_comb_cyclic_shift <= max_tx_comb_cs,
         "The SRS resource tx_comb_cyclic_shift should be less than or equal to 7 for TX comb size n2 and TX comb size "
         "11 for n4");
  VERIFY(srs_res.res_mapping.rept_factor <= srs_res.res_mapping.nof_symb,
         "The SRS resource repetition factor should be less than or equal to the number of symbols");
  // NOTE: The parameter \c start_pos indicates the distance from the last symbol of the slot. The actual starting
  // OFDM symbol is NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - (srs_res.res_mapping.start_pos + 1).
  // The final symbol =
  //         NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - (srs_res.res_mapping.start_pos + 1) + srs_res.res_mapping.nof_symb
  // needs to be less than or equal to NOF_OFDM_SYM_PER_SLOT_NORMAL_CP.
  VERIFY(static_cast<uint8_t>(srs_res.res_mapping.nof_symb) <= srs_res.res_mapping.start_pos + 1,
         "The SRS resource number of symbols and start position should be such that the SRS resource fits within the "
         "slot symbols");

  VERIFY(srs_res.freq_hop.b_srs == 0,
         "SRS res_id={} invalid configuration: Only B_SRS == 0 is currently supported",
         fmt::underlying(srs_res.id.ue_res_id));
  constexpr unsigned                     B_srs_0    = 0;
  const std::optional<srs_configuration> srs_params = srs_configuration_get(srs_res.freq_hop.c_srs, B_srs_0);
  VERIFY(srs_params.has_value(),
         "No valid configuration found for the SRS res_id={}",
         fmt::underlying(srs_res.id.ue_res_id));
  // [Implementation-defined constraint] We construct the UE configuration with this constraint to simplify the
  // scheduler implementation.
  VERIFY(srs_res.freq_domain_shift >= ul_bwp_crbs.start(),
         "SRS res_id={} invalid configuration: frequency shift smaller than BWP lower RB",
         fmt::underlying(srs_res.id.ue_res_id));
  VERIFY(srs_params.value().m_srs <= ul_bwp_crbs.length(),
         "SRS res_id={} invalid configuration: SRS BW exceeding the BWP number or RBs",
         fmt::underlying(srs_res.id.ue_res_id));
  VERIFY(srs_res.freq_domain_shift + srs_params.value().m_srs <= ul_bwp_crbs.stop(),
         "SRS res_id={} invalid configuration: SRS falls outside the BWP upper RB",
         fmt::underlying(srs_res.id.ue_res_id));

  return {};
}

validator_result
config_validators::validate_nzp_csi_rs_list(span<const nzp_csi_rs_resource>               nzp_csi_rs_res_list,
                                            const std::optional<tdd_ul_dl_config_common>& tdd_cfg_common)
{
  // Check: NZP-CSI-RS Resource List ID uniqueness
  VERIFY(has_unique_ids(nzp_csi_rs_res_list, &nzp_csi_rs_resource::res_id), "Duplication of NZP-CSI-RS-ResourceId");

  // Check: Verify firstOFDMSymbolInTimeDomain2 parameter.
  for (const auto& res : nzp_csi_rs_res_list) {
    const auto&   res_mapping = res.res_mapping;
    const uint8_t row_idx     = csi_rs::get_csi_rs_resource_mapping_row_number(
        res_mapping.nof_ports, res_mapping.freq_density, res_mapping.cdm, res_mapping.fd_alloc);
    // As per Table 7.4.1.5.3-1,Section 38.211, the parameter firstOFDMSymbolInTimeDomain2 for symbol \f$l_1\f$
    // should be given by higher layers for Tables rows 13, 14, 16, 17.
    if (row_idx == 13 or row_idx == 14 or row_idx == 16 or row_idx == 17) {
      VERIFY(res_mapping.first_ofdm_symbol_in_td2.has_value(),
             "Missing parameter firstOFDMSymbolInTimeDomain2 for NZP-CSI-RS Resource Id. {} ",
             fmt::underlying(res.res_id));
    }
  }

  // Check: Verify that NZP-CSI-RS do not collide with each other.
  for (const auto* it = nzp_csi_rs_res_list.begin(); it != nzp_csi_rs_res_list.end(); ++it) {
    for (const auto* it2 = it + 1; it2 != nzp_csi_rs_res_list.end(); ++it2) {
      VERIFY(it->csi_res_period == it2->csi_res_period, "NZP-CSI-RS resources with different periods not supported");

      if (it->csi_res_offset != it2->csi_res_offset) {
        continue;
      }
      // In case that match in slot offset, ensure symbols are different.
      VERIFY(it->res_mapping.first_ofdm_symbol_in_td != it2->res_mapping.first_ofdm_symbol_in_td,
             "NZP-CSI-RS resources with same slot offset and same symbol");
      // TODO: Extend to support CSI-RS in the same symbols, but different frequency resources.
    }
  }

  // Check: Verify if CSI-RS symbols allocation are on DL symbols.
  if (tdd_cfg_common.has_value()) {
    for (const nzp_csi_rs_resource& res : nzp_csi_rs_res_list) {
      // Get the symbol mapping from the NZ-CSI-RS configuration.
      const auto&   res_mapping = res.res_mapping;
      const uint8_t row_idx     = csi_rs::get_csi_rs_resource_mapping_row_number(
          res_mapping.nof_ports, res_mapping.freq_density, res_mapping.cdm, res_mapping.fd_alloc);
      csi_rs_pattern_configuration csi_rs_cfg{.start_rb                 = res_mapping.freq_band_rbs.start(),
                                              .nof_rb                   = res_mapping.freq_band_rbs.length(),
                                              .csi_rs_mapping_table_row = static_cast<unsigned>(row_idx),
                                              .symbol_l0                = res_mapping.first_ofdm_symbol_in_td,
                                              .cdm                      = res_mapping.cdm,
                                              .freq_density             = res_mapping.freq_density};
      // symbol_l1 is only used in some configuration, and might not be provided by the higher layers; in such
      // cases, we set an invalid value for symbol_l1, as a way to let the PHY know this value should not be used.
      csi_rs_cfg.symbol_l1 =
          res_mapping.first_ofdm_symbol_in_td2.has_value() ? res_mapping.first_ofdm_symbol_in_td2.value() : 0;
      csi_rs::convert_freq_domain(
          csi_rs_cfg.freq_allocation_ref_idx, res.res_mapping.fd_alloc, static_cast<unsigned>(row_idx));

      const unsigned tdd_period_slots    = nof_slots_per_tdd_period(tdd_cfg_common.value());
      const unsigned csi_rs_period_slots = to_underlying(res.csi_res_period);

      VERIFY(csi_rs_period_slots % tdd_period_slots == 0,
             "Period={} of NZP-CSI-RS-ResourceId={} is not a multiple of the TDD pattern period={}",
             csi_rs_period_slots,
             fmt::underlying(res.res_id),
             tdd_period_slots);

      const cyclic_prefix     cp{cyclic_prefix::NORMAL};
      const ofdm_symbol_range dl_symbols =
          get_active_tdd_dl_symbols(tdd_cfg_common.value(), res.csi_res_offset % tdd_period_slots, cp);

      VERIFY(not dl_symbols.empty(),
             "NZP-CSI-RS-ResourceId={} with offset={} is scheduled in slot={} with no DL symbols",
             fmt::underlying(res.res_id),
             res.csi_res_offset,
             res.csi_res_offset % tdd_period_slots);

      if (dl_symbols.length() != get_nsymb_per_slot(cp)) {
        // In case of special slot.
        const csi_rs_pattern csi_res_mapping = get_csi_rs_pattern(csi_rs_cfg);
        unsigned             max_symbol_idx  = csi_res_mapping.get_reserved_pattern().symbol_mask.find_highest();
        VERIFY(max_symbol_idx < dl_symbols.stop(),
               "NZP-CSI-RS-ResourceId={}, with slot offset {} and symbol index {}, falls outside DL symbols {} in "
               "special slot.",
               fmt::underlying(res.res_id),
               res.csi_res_offset,
               max_symbol_idx,
               dl_symbols);
      }
    }
  }

  return {};
}

validator_result config_validators::validate_csi_meas_cfg(const serving_cell_config&                    ue_cell_cfg,
                                                          const std::optional<tdd_ul_dl_config_common>& tdd_cfg_common,
                                                          const ul_config_common&                       ul_cfg_common)
{
  HANDLE_ERROR(validate_zp_csi_rs(ue_cell_cfg));

  if (not ue_cell_cfg.csi_meas_cfg.has_value()) {
    return {};
  }
  const csi_meas_config& csi_meas_cfg = ue_cell_cfg.csi_meas_cfg.value();

  HANDLE_ERROR(validate_nzp_csi_rs_list(csi_meas_cfg.nzp_csi_rs_res_list, tdd_cfg_common));

  // Ensure no Resource and ResourceSet ID duplications.
  // > NZP-CSI-RS ResourceSet
  VERIFY(has_unique_ids(csi_meas_cfg.nzp_csi_rs_res_set_list, &nzp_csi_rs_resource_set::res_set_id),
         "Duplication of NZP-CSI-RS-ResourceSetId");
  std::optional<nzp_csi_rs_res_id_t> nzp_id = find_disconnected_id(csi_meas_cfg.nzp_csi_rs_res_set_list,
                                                                   csi_meas_cfg.nzp_csi_rs_res_list,
                                                                   &nzp_csi_rs_resource_set::nzp_csi_rs_res,
                                                                   &nzp_csi_rs_resource::res_id);
  VERIFY(not nzp_id.has_value(),
         "ResourceId={} in NZP-CSI-RS-ResourceSet has no associated NZP-CSI-RS-Resource",
         fmt::underlying(*nzp_id));
  // > CSI-IM-ResourceList
  VERIFY(has_unique_ids(csi_meas_cfg.csi_im_res_list, &csi_im_resource::res_id), "Duplication of CSI-IM-ResourceId");
  // > CSI-IM-ResourceSetList.
  VERIFY(has_unique_ids(csi_meas_cfg.csi_im_res_set_list, &csi_im_resource_set::res_set_id),
         "Duplication of CSI-IM-ResourceSetId");
  std::optional<csi_im_res_id_t> im_id = find_disconnected_id(csi_meas_cfg.csi_im_res_set_list,
                                                              csi_meas_cfg.csi_im_res_list,
                                                              &csi_im_resource_set::csi_ims_resources,
                                                              &csi_im_resource::res_id);
  VERIFY(not im_id.has_value(),
         "ResourceId={} in CSI-IM-ResourceSet has no associated CSI-IM-Resource",
         fmt::underlying(*im_id));
  VERIFY(has_unique_ids(csi_meas_cfg.csi_ssb_res_set_list, &csi_ssb_resource_set::res_set_id),
         "Duplication of CSI-SSB-ResourceSetId");
  VERIFY(has_unique_ids(csi_meas_cfg.csi_res_cfg_list, &csi_resource_config::res_cfg_id),
         "Duplication of CSI-ResourceConfigId");
  VERIFY(has_unique_ids(csi_meas_cfg.csi_report_cfg_list, &csi_report_config::report_cfg_id),
         "Duplication of CSI-ReportConfigId");

  // CSI-ResourceConfig.
  for (const auto& res_cfg : csi_meas_cfg.csi_res_cfg_list) {
    if (std::holds_alternative<csi_resource_config::nzp_csi_rs_ssb>(res_cfg.csi_rs_res_set_list)) {
      const auto& variant_value = std::get<csi_resource_config::nzp_csi_rs_ssb>(res_cfg.csi_rs_res_set_list);
      for (const auto& res_set_id : variant_value.nzp_csi_rs_res_set_list) {
        VERIFY_ID_EXISTS([res_set_id](const nzp_csi_rs_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                         csi_meas_cfg.nzp_csi_rs_res_set_list,
                         "NZP CSI-RS resource set id={} does not exist",
                         fmt::underlying(res_set_id));
      }
      for (const auto& res_set_id : variant_value.csi_ssb_res_set_list) {
        VERIFY_ID_EXISTS([res_set_id](const csi_ssb_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                         csi_meas_cfg.csi_ssb_res_set_list,
                         "CSI SSB resource set id={} does not exist",
                         fmt::underlying(res_set_id));
      }
    } else if (std::holds_alternative<csi_resource_config::csi_im_resource_set_list>(res_cfg.csi_rs_res_set_list)) {
      const auto& variant_value = std::get<csi_resource_config::csi_im_resource_set_list>(res_cfg.csi_rs_res_set_list);
      for (const auto& res_set_id : variant_value) {
        VERIFY_ID_EXISTS([res_set_id](const csi_im_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                         csi_meas_cfg.csi_im_res_set_list,
                         "CSI IM resource set id={} does not exist",
                         fmt::underlying(res_set_id));
      }
    }
    // TODO: BWP-Id and Resource Type validation.
  }
  // CSI-ReportConfig.
  for (const auto& rep_cfg : csi_meas_cfg.csi_report_cfg_list) {
    const auto res_for_channel_meas = rep_cfg.res_for_channel_meas;
    VERIFY_ID_EXISTS(
        [res_for_channel_meas](const csi_resource_config& rhs) { return rhs.res_cfg_id == res_for_channel_meas; },
        csi_meas_cfg.csi_res_cfg_list,
        "CSI Resource Config id={} does not exist",
        fmt::underlying(res_for_channel_meas));

    if (rep_cfg.csi_im_res_for_interference.has_value()) {
      const auto csi_im_res_for_interference = rep_cfg.csi_im_res_for_interference.value();
      VERIFY_ID_EXISTS([csi_im_res_for_interference](
                           const csi_resource_config& rhs) { return rhs.res_cfg_id == csi_im_res_for_interference; },
                       csi_meas_cfg.csi_res_cfg_list,
                       "CSI Recourse Config id={} does not exist",
                       fmt::underlying(csi_im_res_for_interference));
    }

    if (rep_cfg.nzp_csi_rs_res_for_interference.has_value()) {
      const auto nzp_csi_rs_res_for_interference = rep_cfg.nzp_csi_rs_res_for_interference.value();
      VERIFY_ID_EXISTS(
          [nzp_csi_rs_res_for_interference](const csi_resource_config& rhs) {
            return rhs.res_cfg_id == nzp_csi_rs_res_for_interference;
          },
          csi_meas_cfg.csi_res_cfg_list,
          "CSI Recourse Config id={} does not exist",
          fmt::underlying(nzp_csi_rs_res_for_interference));
    }

    if (std::holds_alternative<csi_report_config::aperiodic_report>(rep_cfg.report_cfg_type)) {
      const auto& aperiodic_csi = std::get<csi_report_config::aperiodic_report>(rep_cfg.report_cfg_type);
      if (not ue_cell_cfg.ul_config->init_ul_bwp.pusch_cfg->pusch_td_alloc_list.empty()) {
        VERIFY(ue_cell_cfg.ul_config->init_ul_bwp.pusch_cfg->pusch_td_alloc_list.size() ==
                   aperiodic_csi.report_slot_offset_list.size(),
               "The number of PUSCH time domain allocations ({}) does not match the number of CSI report slot offsets "
               "({})",
               ue_cell_cfg.ul_config->init_ul_bwp.pusch_cfg->pusch_td_alloc_list.size(),
               aperiodic_csi.report_slot_offset_list.size());
      } else {
        VERIFY(ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list.size() ==
                   aperiodic_csi.report_slot_offset_list.size(),
               "The number of PUSCH time domain allocations ({}) does not match the number of CSI report slot offsets "
               "({})",
               ue_cell_cfg.ul_config->init_ul_bwp.pusch_cfg->pusch_td_alloc_list.size(),
               ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list.size());
      }
    }
  }

  // TODO: Validate other parameters.
  return {};
}
