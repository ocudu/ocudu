// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_capability_manager.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/du/du_high/du_test_mode_config.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/pusch/tx_scheme_configuration.h"

using namespace ocudu;
using namespace odu;

static const asn1::rrc_nr::ntn_params_r17_s* get_ntn_params_r17(const asn1::rrc_nr::ue_nr_cap_s& ue_cap)
{
  const asn1::rrc_nr::ue_nr_cap_s* ue_cap_ptr = &ue_cap;
  auto        next  = [](const auto* p) { return p && p->non_crit_ext_present ? &p->non_crit_ext : nullptr; };
  const auto* v1700 = next(next(next(next(next(next(next(next(next(next(ue_cap_ptr))))))))));
  if (!v1700 || !v1700->non_terrestrial_network_r17_present || !v1700->ntn_params_r17_present) {
    return nullptr;
  }
  return &v1700->ntn_params_r17;
}

expected<ue_capability_summary, std::string> ocudu::odu::decode_ue_nr_cap_container(const byte_buffer& ue_cap_container)
{
  asn1::rrc_nr::ue_nr_cap_s ue_cap;
  {
    asn1::cbit_ref bref{ue_cap_container};
    if (ue_cap.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
      return make_unexpected(std::string("Couldn't unpack UE NR Capability RRC container"));
    }
  }

  ue_capability_summary ue_caps;

  // fill UE capability summary.
  if (ue_cap.phy_params.phy_params_fr1_present) {
    ue_caps.pdsch_qam256_supported = ue_cap.phy_params.phy_params_fr1.pdsch_256_qam_fr1_present;
  }
  if (ue_cap.phy_params.phy_params_frx_diff_present) {
    ue_caps.pdsch_qam64lowse_supported = ue_cap.phy_params.phy_params_frx_diff.dl_64_qam_mcs_table_alt_present;
    ue_caps.pusch_qam64lowse_supported = ue_cap.phy_params.phy_params_frx_diff.ul_64_qam_mcs_table_alt_present;
  }
  if (ue_cap.phy_params.phy_params_common_present) {
    ue_caps.pdsch_interleaving_vrb_to_prb_supported =
        ue_cap.phy_params.phy_params_common.interleaving_vrb_to_prb_pdsch_present;
  }
  for (const auto& band : ue_cap.rf_params.supported_band_list_nr) {
    // Create and convert band capability.
    ue_capability_summary::supported_band band_cap;
    band_cap.pusch_qam256_supported = band.pusch_256_qam_present;

    // Emplace the band capability in the map.
    ue_caps.bands.emplace(static_cast<nr_band>(band.band_nr), band_cap);
  }
  if (ue_cap.mac_params_present and ue_cap.mac_params.mac_params_xdd_diff_present) {
    ue_caps.long_drx_cycle_supported  = ue_cap.mac_params.mac_params_xdd_diff.long_drx_cycle_present;
    ue_caps.short_drx_cycle_supported = ue_cap.mac_params.mac_params_xdd_diff.short_drx_cycle_present;
  }

  // Convert advanced UE NR capabilities.
  decode_advanced_ue_nr_caps(ue_caps, ue_cap);

  // Convert advanced UE NR NTN capabilities.
  decode_advanced_ue_nr_ntn_caps(ue_caps, ue_cap);

  return ue_caps;
}

ue_capability_manager::ue_capability_manager(span<const du_cell_config> cell_cfg_list_,
                                             du_drx_resource_manager&   drx_mng_,
                                             ocudulog::basic_logger&    logger_,
                                             const du_test_mode_config& test_mode_) :
  base_cell_cfg_list(cell_cfg_list_), drx_res_mng(drx_mng_), logger(logger_), test_cfg(test_mode_)
{
}

void ue_capability_manager::handle_ue_creation(du_ue_resource_config& ue_res_cfg)
{
  drx_res_mng.handle_ue_creation(ue_res_cfg.cell_group);
}

void ue_capability_manager::update(du_ue_resource_config& ue_res_cfg, const byte_buffer& ue_cap_rat_list)
{
  // Decode new UE capabilities.
  if (not decode_ue_capability_list(ue_cap_rat_list)) {
    // No changes detected in the UE capabilities, and update(...) was called before. In this case, we can do not need
    // to apply any extra changes to the ue_res_cfg that weren't already applied.
    return;
  }

  update_impl(ue_res_cfg);
}

void ue_capability_manager::update(du_ue_resource_config& ue_res_cfg, const ue_capability_summary& summary)
{
  // Store injected UE capabilities.
  ue_caps = summary;

  update_impl(ue_res_cfg);
}

void ue_capability_manager::update_impl(du_ue_resource_config& ue_res_cfg)
{
  update_drx(ue_res_cfg);
}

void ue_capability_manager::release(du_ue_resource_config& ue_res_cfg)
{
  // Deallocate DRX resources.
  drx_res_mng.handle_ue_removal(ue_res_cfg.cell_group);
}

bool ue_capability_manager::decode_ue_capability_list(const byte_buffer& ue_cap_rat_list)
{
  using namespace asn1::rrc_nr;

  if (ue_cap_rat_list.empty()) {
    // No update.
    return false;
  }

  ue_cap_rat_container_list_l asn1_cap_list;
  {
    asn1::cbit_ref bref{ue_cap_rat_list};
    if (asn1::unpack_dyn_seq_of(asn1_cap_list, bref, 0, 8) != asn1::OCUDUASN_SUCCESS) {
      logger.error("Couldn't unpack UE Capability RAT Container List RRC container");
      return false;
    }
  }

  for (const ue_cap_rat_container_s& ue_cap_rat : asn1_cap_list) {
    if (ue_cap_rat.rat_type.value != rat_type_opts::nr) {
      logger.warning("Unsupported RAT type in UE Capability RAT Container List RRC container");
      continue;
    }
    expected<ue_capability_summary, std::string> asn1_cap = decode_ue_nr_cap_container(ue_cap_rat.ue_cap_rat_container);
    if (not asn1_cap.has_value()) {
      logger.warning("{}", asn1_cap.error());
      continue;
    }
    ue_caps = asn1_cap.value();
    return true;
  }

  return false;
}

void ue_capability_manager::update_drx(du_ue_resource_config& ue_res_cfg)
{
  cell_group_config& cell_group = ue_res_cfg.cell_group;

  // If UE capabilities are not available or DRX is disabled, disable DRX
  const bool long_drx_supported = ue_caps.has_value() and ue_caps->long_drx_cycle_supported;

  // Allocate DRX resources if DRX is enabled in the gNB.
  drx_res_mng.handle_ue_cap_update(cell_group, long_drx_supported);
}

void ocudu::odu::decode_advanced_ue_nr_caps(odu::ue_capability_summary&      ue_capability,
                                            const asn1::rrc_nr::ue_nr_cap_s& ue_cap)
{
  for (const auto& band : ue_cap.rf_params.supported_band_list_nr) {
    // Select band.
    auto                                   band_id  = static_cast<nr_band>(band.band_nr);
    ue_capability_summary::supported_band& band_cap = ue_capability.bands.at(band_id);

    // Convert UL-MIMO related parameter.
    tx_scheme_codebook_subset pusch_tx_coherence = tx_scheme_codebook_subset::non_coherent;
    if (band.mimo_params_per_band_present) {
      if (band.mimo_params_per_band.pusch_trans_coherence_present) {
        switch (band.mimo_params_per_band.pusch_trans_coherence) {
          case asn1::rrc_nr::mimo_params_per_band_s::pusch_trans_coherence_opts::non_coherent:
          case asn1::rrc_nr::mimo_params_per_band_s::pusch_trans_coherence_opts::nulltype:
            pusch_tx_coherence = tx_scheme_codebook_subset::non_coherent;
            break;
          case asn1::rrc_nr::mimo_params_per_band_s::pusch_trans_coherence_opts::partial_coherent:
            pusch_tx_coherence = tx_scheme_codebook_subset::partial_and_non_coherent;
            break;
          case asn1::rrc_nr::mimo_params_per_band_s::pusch_trans_coherence_opts::full_coherent:
            pusch_tx_coherence = tx_scheme_codebook_subset::fully_and_partial_and_non_coherent;
            break;
        }
      }
    }
    band_cap.pusch_tx_coherence = pusch_tx_coherence;
  }

  for (const auto& band_combination : ue_cap.rf_params.supported_band_combination_list) {
    // Ignore empty and CA band combinations.
    if (band_combination.band_list.size() != 1) {
      continue;
    }

    // Skip if the band parameters is not for NR.
    const asn1::rrc_nr::band_params_c& band_params = band_combination.band_list[0];
    if (band_params.type() != asn1::rrc_nr::band_params_c::types_opts::nr) {
      continue;
    }

    // Get band identifier. Skip if the band is not in the band list.
    auto band_id = static_cast<nr_band>(band_params.nr().band_nr);
    if (ue_capability.bands.count(band_id) == 0) {
      continue;
    }
    ue_capability_summary::supported_band& band_cap = ue_capability.bands.at(band_id);

    // Select feature set identifier.
    uint16_t feature_set_id = band_combination.feature_set_combination;

    // Skip if the selected feature set combination is empty or not for NR.
    if ((ue_cap.feature_set_combinations.size() <= feature_set_id) ||
        (ue_cap.feature_set_combinations[feature_set_id].size() == 0) ||
        (ue_cap.feature_set_combinations[feature_set_id][0].size() == 0) ||
        (ue_cap.feature_set_combinations[feature_set_id][0][0].type() != asn1::rrc_nr::feature_set_c::types_opts::nr)) {
      continue;
    }

    // Extract feature set identifiers for each direction.
    const auto& feature_set   = ue_cap.feature_set_combinations[feature_set_id][0][0].nr();
    uint16_t    feature_ul_id = feature_set.ul_set_nr;
    uint16_t    feature_dl_id = feature_set.dl_set_nr;

    // Skip if any of the feature identifiers is zero.
    if ((feature_ul_id == 0) || (feature_dl_id == 0)) {
      continue;
    }
    --feature_ul_id;
    --feature_dl_id;

    // Skip if the selected UL feature set combination is empty or not for NR.
    if (ue_cap.feature_sets.feature_sets_ul.size() <= feature_ul_id) {
      continue;
    }

    // Select feature set for UL.
    const auto& feature_set_ul = ue_cap.feature_sets.feature_sets_ul[feature_ul_id];

    // Parse SRS capabilities.
    if (feature_set_ul.supported_srs_res_features_present) {
      switch (feature_set_ul.supported_srs_res_features.max_num_srs_ports_per_res) {
        case asn1::rrc_nr::srs_res_features_s::max_num_srs_ports_per_res_opts::n1:
        case asn1::rrc_nr::srs_res_features_s::max_num_srs_ports_per_res_opts::nulltype:
          band_cap.nof_srs_tx_ports = std::max(band_cap.nof_srs_tx_ports, static_cast<uint8_t>(1U));
          break;
        case asn1::rrc_nr::srs_res_features_s::max_num_srs_ports_per_res_opts::n2:
          band_cap.nof_srs_tx_ports = std::max(band_cap.nof_srs_tx_ports, static_cast<uint8_t>(2U));
          break;
        case asn1::rrc_nr::srs_res_features_s::max_num_srs_ports_per_res_opts::n4:
          band_cap.nof_srs_tx_ports = std::max(band_cap.nof_srs_tx_ports, static_cast<uint8_t>(4U));
          break;
      }
    }

    // Select the UL feature per CC identifier.
    uint16_t feature_ul_per_cc_id = feature_set_ul.feature_set_list_per_ul_cc[0];

    // Skip if the identifier is zero.
    if (feature_ul_per_cc_id == 0) {
      continue;
    }
    --feature_ul_per_cc_id;

    // Extract UL feature set per CC.
    const auto& feature_set_per_cc_ul = ue_cap.feature_sets.feature_sets_ul_per_cc[feature_ul_per_cc_id];

    // Parse maximum number of layers.
    if (feature_set_per_cc_ul.mimo_cb_pusch_present &&
        feature_set_per_cc_ul.mimo_cb_pusch.max_num_mimo_layers_cb_pusch_present) {
      switch (feature_set_per_cc_ul.mimo_cb_pusch.max_num_mimo_layers_cb_pusch) {
        case asn1::rrc_nr::mimo_layers_ul_opts::one_layer:
        case asn1::rrc_nr::mimo_layers_ul_opts::nulltype:
          band_cap.pusch_max_rank = std::max(band_cap.pusch_max_rank, 1U);
          break;
        case asn1::rrc_nr::mimo_layers_ul_opts::two_layers:
          band_cap.pusch_max_rank = std::max(band_cap.pusch_max_rank, 2U);
          break;
        case asn1::rrc_nr::mimo_layers_ul_opts::four_layers:
          band_cap.pusch_max_rank = std::max(band_cap.pusch_max_rank, 4U);
          break;
      }
    }
  }
}

void ocudu::odu::decode_advanced_ue_nr_ntn_caps(ue_capability_summary&           ue_capability,
                                                const asn1::rrc_nr::ue_nr_cap_s& ue_cap)
{
  if (auto* ntn = get_ntn_params_r17(ue_cap)) {
    const auto& ntn_params_r17  = *ntn;
    ue_capability.ntn_supported = true;
    if (ntn_params_r17.mac_params_ntn_r17_present) {
      if (ntn_params_r17.mac_params_ntn_r17.mac_params_common_present) {
        // A UE supporting those features shall also indicate the support of nonTerrestrialNetwork-r17.
        ue_capability.disabled_dl_harq_feedback_supported =
            ntn_params_r17.mac_params_ntn_r17.mac_params_common.harq_feedback_disabled_r17_present;
        ue_capability.ul_harq_mode_b_supported =
            ntn_params_r17.mac_params_ntn_r17.mac_params_common.ul_harq_mode_b_r17_present;
      }
    }
  } else {
    // No NTN support.
    return;
  }

  for (const auto& band : ue_cap.rf_params.supported_band_list_nr) {
    nr_band band_id = static_cast<nr_band>(band.band_nr);

    // The following fields are only applicable for NTN bands. (See Table 5.2.2-1 in TS 38.101-5).
    if (not band_helper::is_ntn_band(band_id)) {
      continue;
    }

    ue_capability_summary::supported_band& band_cap = ue_capability.bands.at(band_id);

    // Support of this feature in NTN bands is mandatory for UE supporting nonTerrestrialNetwork-r17.
    if (not band.ul_pre_compensation_r17_present) {
      band_cap.ul_pre_compensation_supported = false;
      ue_capability.ntn_supported            = false;
      return;
    }
    band_cap.ul_pre_compensation_supported = true;

    if (band.max_harq_process_num_r17_present) {
      if (band.max_harq_process_num_r17 == asn1::rrc_nr::band_nr_s::max_harq_process_num_r17_opts::u16d32) {
        band_cap.max_ul_harq_process_num = 16;
        band_cap.max_dl_harq_process_num = 32;
      } else if (band.max_harq_process_num_r17 == asn1::rrc_nr::band_nr_s::max_harq_process_num_r17_opts::u32d16) {
        band_cap.max_ul_harq_process_num = 32;
        band_cap.max_dl_harq_process_num = 16;
      } else if (band.max_harq_process_num_r17 == asn1::rrc_nr::band_nr_s::max_harq_process_num_r17_opts::u32d32) {
        band_cap.max_ul_harq_process_num = 32;
        band_cap.max_dl_harq_process_num = 32;
      }
    }

    // UE indicating support of this feature shall also indicate support of uplinkPreCompensation-r17 for this band.
    band_cap.ul_ta_reporting_supported = band.ul_ta_report_r17_present && band_cap.ul_pre_compensation_supported;

    // UE indicating support of this feature shall also indicate support of uplinkPreCompensation-r17 and
    // uplink-TA-Reporting-r17 for this band.
    band_cap.ue_specific_k_offset_supported =
        band.ue_specific_k_offset_r17_present && band_cap.ul_ta_reporting_supported;
  }
}
