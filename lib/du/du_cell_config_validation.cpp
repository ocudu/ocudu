// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_cell_config_validation.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/du/du_update_config_helpers.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/pdcch/pdcch_candidates.h"
#include "ocudu/ran/pdcch/pdcch_type0_css_occasions.h"
#include "ocudu/ran/prach/prach_configuration.h"
#include "ocudu/ran/prach/prach_frequency_mapping.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include "ocudu/ran/ssb/ssb_mapping.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/config/serving_cell_config_validator.h"
#include "ocudu/support/config/validator_helpers.h"

using namespace ocudu;
using namespace odu;

#define CHECK_TRUE(cond, ...)                                                                                          \
  if (not(cond)) {                                                                                                     \
    return make_unexpected(fmt::format(__VA_ARGS__));                                                                  \
  }

#define CHECK_COMPARE_(val, expected_val, comparison, ...)                                                             \
  CHECK_TRUE(                                                                                                          \
      (val comparison expected_val), "Invalid {} ({} " #comparison "{})", fmt::format(__VA_ARGS__), val, expected_val)

#define CHECK_EQ(val, expected_val, ...)                                                                               \
  CHECK_TRUE((val == expected_val), "Invalid {} ({} != {})", fmt::format(__VA_ARGS__), val, expected_val)

#define CHECK_NEQ(val, forbidden_val, ...)                                                                             \
  CHECK_TRUE((val != forbidden_val), "Invalid {} ({} == {})", fmt::format(__VA_ARGS__), val, forbidden_val)

/// Checks if "val" is below or equal to "max_val".
#define CHECK_EQ_OR_BELOW(val, max_val, ...)                                                                           \
  CHECK_TRUE((val <= max_val), "Invalid {} ({} > {})", fmt::format(__VA_ARGS__), val, max_val)

/// Checks if "val" is below "max_val".
#define CHECK_BELOW(val, max_val, ...)                                                                                 \
  CHECK_TRUE((val < max_val), "Invalid {} ({} >= {})", fmt::format(__VA_ARGS__), val, max_val)

/// Checks if "val" is above or equal to "max_val".
#define CHECK_EQ_OR_ABOVE(val, max_val, ...)                                                                           \
  CHECK_TRUE((val >= max_val), "Invalid {} ({} < {})", fmt::format(__VA_ARGS__), val, max_val)

/// Checks if "val" is above "max_val".
#define CHECK_ABOVE(val, max_val, ...)                                                                                 \
  CHECK_TRUE((val > max_val), "Invalid {} ({} <= {})", fmt::format(__VA_ARGS__), val, max_val)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Helper typedef.
using check_outcome = error_type<std::string>;

/// Helper function to verify if a number belongs to the list of possible values of an ASN.1 enumerated type.
template <typename ASN1Enumerated, typename Number>
static bool is_valid_enum_number(Number number)
{
  ASN1Enumerated e;
  return asn1::number_to_enum(e, number);
}

/// Checks whether CORESET#0 table index is valid as per TS38.213, Table 13-{1,...,10}.
static check_outcome is_coreset0_ss0_idx_valid(const du_cell_config& cell_cfg)
{
  const subcarrier_spacing scs_common = cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const auto               cs0_idx    = cell_cfg.ran.dl_cfg_common.init_dl_bwp.pdcch_common.get_coreset0();
  CHECK_TRUE(cs0_idx.has_value(), "CORESET#0 index not found in common PDCCH configuration");
  const auto ss0_idx = cell_cfg.ran.dl_cfg_common.init_dl_bwp.pdcch_common.get_searchspace0();
  CHECK_TRUE(ss0_idx.has_value(), "SearchSpace#0 not found in common SearchSpace list");

  // TODO: Check which table to use.
  // TODO: Add checks on minimum bandwidth.
  if (scs_common == subcarrier_spacing::kHz15 and cell_cfg.ran.ssb_cfg.scs == subcarrier_spacing::kHz15) {
    // As per TS38.213, Table 13-1.
    CHECK_BELOW(cs0_idx.value(), 15, "CORESET#0 index table");
  } else if (scs_common == subcarrier_spacing::kHz30 and cell_cfg.ran.ssb_cfg.scs == subcarrier_spacing::kHz30) {
    // As per TS38.213, Table 13-4.
    CHECK_BELOW(cs0_idx.value(), 16, "CORESET#0 index table");
  }
  // The remaining cases will trigger an assert in the SSB checks.

  // This constraint is implementation-defined and comes from the fact that our PDCCH scheduler only schedules PDCCH
  // starting from the symbol index 0.
  if (scs_common == subcarrier_spacing::kHz15 and cell_cfg.ran.ssb_cfg.scs == subcarrier_spacing::kHz15) {
    // As per TS38.213, Table 13-11.
    CHECK_EQ_OR_BELOW(ss0_idx.value(), 9, "SearchSpaceZero index table");
  }

  return {};
}

/// Checks whether CORESET#0 configuration matches the values specified in TS38.211 Section 7.3.2.2.
static check_outcome is_coreset0_params_valid(const du_cell_config& cell_cfg)
{
  const coreset_configuration& cs_cfg = *cell_cfg.ran.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0;
  CHECK_EQ(fmt::underlying(cs_cfg.get_id()), 0, "CORESET#0 ID");

  // Check if Coreset0 is within the Initial DL BWP CRBs.
  const crb_interval& initial_bwp_crbs = cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.crbs;
  crb_interval        coreset0_crbs    = cs_cfg.coreset0_crbs();
  CHECK_TRUE(initial_bwp_crbs.contains(coreset0_crbs),
             "The CORESET#0 CRBs [{}, {}) falls outside of the initial DL BWP CRBs [{}, {})",
             coreset0_crbs.start(),
             coreset0_crbs.stop(),
             initial_bwp_crbs.start(),
             initial_bwp_crbs.stop());

  return {};
}

static check_outcome is_search_space_valid(const search_space_configuration& ss_cfg)
{
  CHECK_EQ_OR_BELOW(fmt::underlying(ss_cfg.get_id()),
                    fmt::underlying(ocudu::MAX_SEARCH_SPACE_ID),
                    "SearchSpace Id={}",
                    fmt::underlying(ss_cfg.get_id()));
  CHECK_EQ_OR_BELOW(fmt::underlying(ss_cfg.get_coreset_id()),
                    fmt::underlying(ocudu::MAX_CORESET_ID),
                    "SearchSpace#{} CORESET Id={}",
                    fmt::underlying(ss_cfg.get_id()),
                    fmt::underlying(ss_cfg.get_coreset_id()));
  const bool valid_period =
      is_valid_enum_number<asn1::rrc_nr::search_space_s::monitoring_slot_periodicity_and_offset_c_::types>(
          ss_cfg.get_monitoring_slot_periodicity());
  CHECK_TRUE(valid_period,
             "Invalid SearchSpace#{} slot period={}",
             fmt::underlying(ss_cfg.get_id()),
             ss_cfg.get_monitoring_slot_periodicity());
  CHECK_BELOW(ss_cfg.get_monitoring_slot_offset(),
              ss_cfg.get_monitoring_slot_periodicity(),
              "SearchSpace#{} monitoring slot offset",
              fmt::underlying(ss_cfg.get_id()));
  CHECK_EQ_OR_BELOW(ss_cfg.get_duration(),
                    ss_cfg.get_monitoring_slot_periodicity(),
                    "SearchSpace#{} monitoring slot duration",
                    fmt::underlying(ss_cfg.get_id()));
  CHECK_NEQ(ss_cfg.get_duration(), 0, "SearchSpace#{} monitoring slot duration", fmt::underlying(ss_cfg.get_id()));
  return {};
}

static check_outcome check_dl_config_common(const du_cell_config& cell_cfg)
{
  const bwp_downlink_common& bwp = cell_cfg.ran.dl_cfg_common.init_dl_bwp;
  // PDCCH
  if (bwp.pdcch_common.coreset0.has_value()) {
    HANDLE_ERROR(is_coreset0_params_valid(cell_cfg));
  }
  if (bwp.pdcch_common.sib1_search_space_id != ocudu::MAX_NOF_SEARCH_SPACES) {
    CHECK_EQ(fmt::underlying(bwp.pdcch_common.sib1_search_space_id),
             0,
             "SearchSpaceSIB1 must be equal to 0 for initial DL BWP");
  }
  if (bwp.pdcch_common.common_coreset.has_value()) {
    CHECK_NEQ(fmt::underlying(bwp.pdcch_common.common_coreset->get_id()), 0, "Common CORESET");
  }
  for (const search_space_configuration& ss : bwp.pdcch_common.search_spaces) {
    HANDLE_ERROR(is_search_space_valid(ss));
    CHECK_TRUE(ss.is_common_search_space(), "Common SearchSpace#{} type", fmt::underlying(ss.get_id()));
    const auto& dci_format_variant = ss.get_monitored_dci_formats();
    const auto& dci_format         = std::get<search_space_configuration::common_dci_format>(dci_format_variant);
    CHECK_TRUE(dci_format.f0_0_and_f1_0,
               "Common SearchSpace#{} must enable DCI format1_0 and format0_0",
               fmt::underlying(ss.get_id()));
    if (ss.get_coreset_id() == 0) {
      CHECK_TRUE(bwp.pdcch_common.coreset0.has_value(),
                 "Common SearchSpace#{} points to CORESET#0 which is inactive",
                 fmt::underlying(ss.get_id()));
    } else {
      CHECK_TRUE(bwp.pdcch_common.common_coreset.has_value() and
                     ss.get_coreset_id() == bwp.pdcch_common.common_coreset->get_id(),
                 "common SearchSpace#{} points to CORESET#{} which is inactive",
                 fmt::underlying(ss.get_id()),
                 fmt::underlying(ss.get_coreset_id()));
    }
  }
  // PDSCH
  CHECK_TRUE(not bwp.pdsch_common.pdsch_td_alloc_list.empty(), "Empty PDSCH-TimeDomainAllocationList");
  for (const auto& pdsch : bwp.pdsch_common.pdsch_td_alloc_list) {
    CHECK_EQ_OR_BELOW(pdsch.k0, 32, "PDSCH k0");

    // These limits are expressed in TS 38.214 Table 5.1.2.1-1.
    if (pdsch.map_type == sch_mapping_type::typeA) {
      CHECK_EQ_OR_BELOW(pdsch.symbols.start(), pdcch_constants::MAX_CORESET_DURATION, "PDSCH S");
      CHECK_EQ_OR_ABOVE(pdsch.symbols.length(), pdcch_constants::MAX_CORESET_DURATION, "PDSCH L");
      CHECK_EQ_OR_BELOW(pdsch.symbols.length(), get_nsymb_per_slot(bwp.generic_params.cp), "PDSCH L");
    } else {
      // Mapping Type B.
      if (bwp.generic_params.cp == cyclic_prefix::NORMAL) {
        CHECK_EQ_OR_BELOW(pdsch.symbols.start(), 12, "PDSCH S");
        CHECK_TRUE(pdsch.symbols.length() == 2 or pdsch.symbols.length() == 4 or pdsch.symbols.length() == 7,
                   "Invalid PDSCH L (valid values are 2, 4, 7)");
      } else {
        // Extended Cyclic Prefix.
        CHECK_EQ_OR_BELOW(pdsch.symbols.start(), 10, "PDSCH S");
        CHECK_TRUE(pdsch.symbols.length() == 2 or pdsch.symbols.length() == 4 or pdsch.symbols.length() == 6,
                   "Invalid PDSCH L (valid values are 2, 4, 6)");
      }
    }
  }
  return {};
}

static check_outcome check_rlm_config(const du_cell_config& cell_cfg)
{
  const auto serv_cell_cfg = config_helpers::make_default_ue_cell_config(cell_cfg.ran).serv_cell_cfg;
  const auto rlm_cfg       = serv_cell_cfg.init_dl_bwp.rlm_cfg;
  if (not rlm_cfg.has_value()) {
    return {};
  }
  const auto csi_meas_cfg = serv_cell_cfg.csi_meas_cfg;
  for (const auto& rlm_res : rlm_cfg->rlm_resources) {
    CHECK_TRUE(rlm_res.resource_purpose == radio_link_monitoring_config::radio_link_monitoring_rs::purpose::rlf,
               "Radio Link Failure is the only supported Radio Link Monitoring purpose");
    if (not csi_meas_cfg.has_value()) {
      CHECK_TRUE(not std::holds_alternative<nzp_csi_rs_res_id_t>(rlm_res.detection_resource),
                 "RLM resources cannot use CSI-RS resources if CSI is not enabled");
    } else {
      if (std::holds_alternative<nzp_csi_rs_res_id_t>(rlm_res.detection_resource)) {
        const auto&               csi_cfg   = csi_meas_cfg.value();
        const nzp_csi_rs_res_id_t csi_rs_id = std::get<nzp_csi_rs_res_id_t>(rlm_res.detection_resource);
        const auto                csi_res_it =
            std::find_if(csi_cfg.nzp_csi_rs_res_list.begin(),
                         csi_cfg.nzp_csi_rs_res_list.end(),
                         [csi_rs_id](const nzp_csi_rs_resource& csi_r) { return csi_r.res_id == csi_rs_id; });
        CHECK_TRUE(csi_res_it != csi_cfg.nzp_csi_rs_res_list.end(),
                   "RLM resource id={} points at CSI-RS resource id={}, which wasn't found in the CSI configuration",
                   fmt::underlying(rlm_res.res_id),
                   fmt::underlying(csi_rs_id));
        CHECK_TRUE(csi_res_it->res_mapping.cdm == csi_rs_cdm_type::no_CDM,
                   "Only CSI-RS resource with no_CDM can be used for RLM");
        CHECK_TRUE(csi_res_it->res_mapping.nof_ports == 1U,
                   "Only CSI-RS resource configured with 1 port can be used for RLM");
        CHECK_TRUE(csi_res_it->res_mapping.freq_density == csi_rs_freq_density_type::one or
                       csi_res_it->res_mapping.freq_density == csi_rs_freq_density_type::three,
                   "Only CSI-RS resource with frequency density one or three can be used for RLM");
      }
    }

    if (std::holds_alternative<ssb_id_t>(rlm_res.detection_resource)) {
      const ssb_id_t ssb_rs_id = std::get<ssb_id_t>(rlm_res.detection_resource);
      CHECK_TRUE(std::any_of(cell_cfg.ran.ssb_cfg.beam_ids.begin(),
                             cell_cfg.ran.ssb_cfg.beam_ids.end(),
                             [ssb_rs_id](const uint8_t ssb_idx) { return ssb_idx == static_cast<uint8_t>(ssb_rs_id); }),
                 "RLM resource id={} points at SSB index={}, which wasn't found in SSB configuration",
                 fmt::underlying(rlm_res.res_id),
                 ssb_rs_id);
    }
  }

  const uint8_t l_max =
      ssb_get_L_max(cell_cfg.ran.ssb_cfg.scs, cell_cfg.ran.dl_carrier.arfcn_f_ref, cell_cfg.ran.dl_carrier.band);
  // Check the constrains on N_RLM values in Table 5-1, TS 38.213, are met.
  if (l_max == 4U) {
    CHECK_TRUE(rlm_cfg->rlm_resources.size() <= 2, "With SSB L_max = 4, max 2 RLM resources can be configured");
  } else if (l_max == 8U) {
    CHECK_TRUE(rlm_cfg->rlm_resources.size() <= 4, "With SSB L_max = 8, max 4 RLM resources can be configured");
  } else if (l_max == 64U) {
    CHECK_TRUE(rlm_cfg->rlm_resources.size() <= 8, "With SSB L_max = 64, max 8 RLM resources can be configured");
  }

  return {};
}

static check_outcome check_dl_config_dedicated(const du_cell_config& cell_cfg)
{
  const subcarrier_spacing scs_common = cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const auto&              pdcch_cfg  = cell_cfg.ran.init_bwp.pdcch_cfg;
  const auto pdsch_cfg = config_helpers::make_default_ue_cell_config(cell_cfg.ran).serv_cell_cfg.init_dl_bwp.pdsch_cfg;

  // PDCCH
  if (pdcch_cfg.has_value()) {
    for (const search_space_configuration& ss : pdcch_cfg->search_spaces) {
      const bool fallback_dci_format_in_ss2 =
          ss.is_common_search_space() or
          not(std::get<search_space_configuration::ue_specific_dci_format>(ss.get_monitored_dci_formats()) ==
              search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);

      if (fallback_dci_format_in_ss2) {
        CHECK_TRUE(cell_cfg.ran.dl_carrier.nof_ant == 1,
                   "Nof. DL antennas {} cannot be greater than 1 when using fallback DCI format\n",
                   cell_cfg.ran.dl_carrier.nof_ant);

        if (pdsch_cfg.has_value()) {
          CHECK_TRUE(pdsch_cfg->mcs_table != pdsch_mcs_table::qam256,
                     "256QAM MCS table cannot be used for PDSCH with fallback DCI format in SearchSpace#2");
        }
      }

      if (pdsch_cfg.has_value() and pdsch_cfg->mcs_table == pdsch_mcs_table::qam64LowSe) {
        // As per Section 5.1.3.1, TS 38.213 and assuming MCS-C-RNTI is not supported.
        CHECK_TRUE(not ss.is_common_search_space(),
                   "64QAM Low Se MCS table cannot be used for PDSCH with DCI in Common SearchSpace");
      }

      if (pdsch_cfg.has_value() and pdsch_cfg->pdsch_mapping_type_a_dmrs.has_value() and
          pdsch_cfg->pdsch_mapping_type_a_dmrs->additional_positions == dmrs_additional_positions::pos3) {
        CHECK_TRUE(
            cell_cfg.ran.dmrs_typeA_pos == dmrs_typeA_position::pos2,
            "PDSCH dmrs-Additional-Position of pos3 is only supported when dmrs-TypeA-Position is equal to pos2");
      }
    }

    // Checks whether nof. monitored PDCCH candidates per slot for a DL BWP does not exceed maximum allowed value as per
    // TS 38.213, Table 10.1-2.
    const unsigned total_nof_monitored_pdcch_candidates =
        config_helpers::compute_tot_nof_monitored_pdcch_candidates_per_slot(*pdcch_cfg, cell_cfg.ran.dl_cfg_common);
    CHECK_EQ_OR_BELOW(total_nof_monitored_pdcch_candidates,
                      max_nof_monitored_pdcch_candidates(scs_common),
                      "Nof. PDCCH candidates monitored per slot for a DL BWP={} exceeds maximum value={}\n",
                      total_nof_monitored_pdcch_candidates,
                      max_nof_monitored_pdcch_candidates(scs_common));
  }

  check_outcome rlm_validation = check_rlm_config(cell_cfg);
  CHECK_TRUE(rlm_validation.has_value(), "Invalid Radio Link Monitoring config: {}", rlm_validation.error());

  return {};
}

static check_outcome check_ssb_configuration(const du_cell_config& cell_cfg)
{
  const ssb_configuration& ssb_cfg    = cell_cfg.ran.ssb_cfg;
  const subcarrier_spacing scs_common = cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;

  // No mixed numerologies supported (yet).
  CHECK_EQ(fmt::underlying(ssb_cfg.scs),
           fmt::underlying(scs_common),
           "SSB SCS must be equal to SCS common. Mixed numerologies are not supported.");

  // Only FR1 SCS supported (for now).
  if (band_helper::get_freq_range(cell_cfg.ran.dl_carrier.band) == frequency_range::FR1) {
    CHECK_EQ_OR_BELOW(fmt::underlying(ssb_cfg.scs),
                      fmt::underlying(subcarrier_spacing::kHz30),
                      "SSB SCS must be 15kHz or 30kHz for FR1.");
  } else {
    CHECK_EQ(
        fmt::underlying(ssb_cfg.scs), fmt::underlying(subcarrier_spacing::kHz120), "SSB SCS must be 120kHz for FR2.");
  }

  // TODO: remove this when multiple beams are supported.
  CHECK_TRUE(ssb_cfg.ssb_bitmap.test(0) and ssb_cfg.ssb_bitmap.none(1U, ssb_cfg.ssb_bitmap.get_L_max()),
             "Multiple beams not supported for SSB.");

  // Checks that SSB does not get located outside the band.
  if (scs_common == subcarrier_spacing::kHz15) {
    // Check if k_SSB is within limits, according to the SCScommon.
    CHECK_EQ_OR_BELOW(ssb_cfg.k_ssb.value(), 11, "For SCS common 15kHz, k_SSB must be within the range [0, 11].");

    // In the following, we assume the SSB is located inside the Transmission Bandwidth Configuration of the specified
    // band. Refer to TS38.104, Section 5.3.1 for the definition of Transmission Bandwidth Configuration.
    // We assume the Initial DL BWP ranges over the whole Transmission Bandwidth Configuration.
    unsigned nof_crbs               = cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.crbs.length();
    unsigned offset_p_A_upper_bound = ssb_cfg.k_ssb.value() > 0 ? nof_crbs - NOF_SSB_PRBS - 1 : nof_crbs - NOF_SSB_PRBS;
    CHECK_EQ_OR_BELOW(
        ssb_cfg.offset_to_point_A.value(),
        offset_p_A_upper_bound,
        "Offset to PointA must be such that the SSB is located inside the Initial DL BWP, i.e, offset_to_point_A <= {}",
        offset_p_A_upper_bound);

  } else if (scs_common == subcarrier_spacing::kHz30) {
    // Check if k_SSB is within limits, according to the SCScommon.
    CHECK_EQ_OR_BELOW(ssb_cfg.k_ssb.value(), 23, "For SCS common 30kHz, k_SSB must be within the range [0, 23].");
    // Check if k_SSB is an even number, as this is a requirement coming from PHY implementation limitation.
    CHECK_TRUE(ssb_cfg.k_ssb.value() % 2 == 0, "For SCS common 30kHz, k_SSB must be an even number.");

    // In the following, we assume the SSB is located inside the Transmission Bandwidth Configuration of the specified
    // band. Refer to TS38.104, Section 5.3.1 for the definition of Transmission Bandwidth Configuration.
    // We assume the Initial DL BWP ranges over the whole Transmission Bandwidth Configuration.
    unsigned nof_crbs = cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.crbs.length();
    unsigned offset_p_A_upper_bound =
        ssb_cfg.k_ssb.value() > 0 ? (nof_crbs - NOF_SSB_PRBS - 1) * 2 : (nof_crbs - NOF_SSB_PRBS) * 2;
    CHECK_EQ_OR_BELOW(
        ssb_cfg.offset_to_point_A.value(),
        offset_p_A_upper_bound,
        "Offset to PointA must be such that the SSB is located inside the Initial DL BWP, i.e, offset_to_point_A <= {}",
        offset_p_A_upper_bound);
    CHECK_TRUE(ssb_cfg.offset_to_point_A.value() % 2 == 0,
               "With SCScommon 30kHz, Offset to PointA must be an even number",
               offset_p_A_upper_bound);
  }

  ssb_pattern_case ssb_case = band_helper::get_ssb_pattern(cell_cfg.ran.dl_carrier.band, ssb_cfg.scs);
  const uint8_t L_max = ssb_get_L_max(ssb_cfg.scs, cell_cfg.ran.dl_carrier.arfcn_f_ref, cell_cfg.ran.dl_carrier.band);
  CHECK_TRUE(ssb_cfg.ssb_bitmap.get_L_max() == L_max, "Mismatch between SSB bitmap size and L_max");

  // (Only for Lmax = 64) It is assumed in \c inOneGroup, \c ssb-PositionsInBurst, \c ServingCellConfigCommonSIB,
  // TS 38.331 that, if a group of 8-bit bitmaps [n, n+8), with n=0, 8, 16, ...56 has non-zero bits, then these 8
  // bits are common to all non-zero 8-bit bitmaps starting with n=0, 8, 16, ...56.
  if (L_max == 64) {
    constexpr uint8_t      nof_groups_and_bits_per_gr = 8U;
    std::optional<uint8_t> first_non_zero_group;
    for (uint8_t group_idx = 0; group_idx != nof_groups_and_bits_per_gr; ++group_idx) {
      const auto group_8_bits =
          ssb_cfg.ssb_bitmap.extract<uint8_t>(nof_groups_and_bits_per_gr * group_idx, nof_groups_and_bits_per_gr);
      if (group_8_bits != 0U and not first_non_zero_group.has_value()) {
        first_non_zero_group.emplace(group_8_bits);
      }
      CHECK_TRUE(group_8_bits == 0U or group_8_bits == first_non_zero_group.value(),
                 "Invalid SSB bitmap for L_max = 64");
    }
  }

  // Check whether the SSB beam bitmap and L_max are compatible with SSB case and DL band.
  if (ssb_case <= ssb_pattern_case::C) {
    double     cutoff_freq_mhz_case_a_b_c      = band_helper::nr_arfcn_to_freq(CUTOFF_FREQ_ARFCN_CASE_A_B_C) / 1e6;
    double     cutoff_freq_mhz_case_c_unpaired = band_helper::nr_arfcn_to_freq(CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED) / 1e6;
    const bool ssb_case_c_unpaired =
        ssb_case == ssb_pattern_case::C and not band_helper::is_paired_spectrum(cell_cfg.ran.dl_carrier.band);

    const bool    frequency_above_cutoff = ssb_case_c_unpaired
                                               ? cell_cfg.ran.dl_carrier.arfcn_f_ref > CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED
                                               : cell_cfg.ran.dl_carrier.arfcn_f_ref > CUTOFF_FREQ_ARFCN_CASE_A_B_C;
    const uint8_t expected_L_max         = frequency_above_cutoff ? 8U : 4U;

    CHECK_EQ(L_max,
             expected_L_max,
             "For SSB case {} and frequency {} {}MHz, L_max must be {}",
             ssb_case_c_unpaired ? "C (unpaired)" : "A, B, C (paired)",
             frequency_above_cutoff ? ">" : "<=",
             ssb_case_c_unpaired ? cutoff_freq_mhz_case_c_unpaired : cutoff_freq_mhz_case_a_b_c,
             expected_L_max);
  } else if (ssb_case == ssb_pattern_case::D) {
    CHECK_EQ(L_max, 64, "For SSB case D L_max must be 64");
  } else {
    CHECK_TRUE(false, "SSB case E not supported")
  }

  return {};
}

static check_outcome check_ul_config_common(const du_cell_config& cell_cfg)
{
  const bwp_uplink_common& bwp = cell_cfg.ran.ul_cfg_common.init_ul_bwp;
  if (bwp.pusch_cfg_common.has_value()) {
    const pusch_config_common& pusch = cell_cfg.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value();
    CHECK_TRUE(pusch.msg3_delta_power.valid(),
               "msg3_delta_power{} in pucch_config_common not in range [-6, 8]",
               pusch.msg3_delta_power.value());
    CHECK_TRUE(pusch.msg3_delta_power.value() % 2 == 0,
               "The value set {} for msg3_delta_power must be a multiple of 2",
               pusch.msg3_delta_power.value());
  }

  // \ref prach_scheduler for the derivation of this validation.
  const prach_configuration prach_cfg = prach_configuration_get(
      band_helper::get_freq_range(cell_cfg.ran.dl_carrier.band),
      band_helper::get_duplex_mode(cell_cfg.ran.dl_carrier.band),
      cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index);

  // The information we need are not related to whether it is the last PRACH occasion.
  constexpr bool                   is_last_prach_occasion = false;
  const prach_preamble_information info =
      is_long_preamble(prach_cfg.format)
          ? get_prach_preamble_long_info(prach_cfg.format)
          : get_prach_preamble_short_info(
                prach_cfg.format,
                to_ra_subcarrier_spacing(cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.scs),
                is_last_prach_occasion);
  const unsigned prach_nof_prbs =
      prach_frequency_mapping_get(info.scs, cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
  const unsigned prach_prb_end =
      cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start +
      cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_fdm * prach_nof_prbs;
  CHECK_TRUE(
      prach_prb_end <= cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(),
      "With the current PRACH configuration index {}, MSG1 frequency start {} and MSG1 FDM {}, the resulting PRACH "
      "RBs fall outside the BWP.",
      cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index,
      cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start,
      cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_fdm);

  if (cell_cfg.ran.init_bwp.srs_cfg.srs_type_enabled == srs_type::aperiodic and cell_cfg.ran.tdd_cfg.has_value() and
      cell_cfg.ran.tdd_cfg.value().pattern2.has_value()) {
    CHECK_TRUE(cell_cfg.ran.tdd_cfg.value().pattern1.nof_ul_symbols ==
                   cell_cfg.ran.tdd_cfg.value().pattern2.value().nof_ul_symbols,
               "With aperiodic SRS, the TDD pattern 1 and 2 must have the same number of symbols.");
  }

  return {};
}

static check_outcome check_ul_config_dedicated(const du_cell_config& cell_cfg)
{
  const auto&                       pusch_params = cell_cfg.ran.init_bwp.pusch;
  const pusch_mcs_table             mcs_table    = pusch_params.mcs_table;
  const search_space_configuration& ss2          = cell_cfg.ran.init_bwp.pdcch_cfg->search_spaces.back();
  const bool                        fallback_dci_format_in_ss2 =
      ss2.is_common_search_space() or
      not(std::get<search_space_configuration::ue_specific_dci_format>(ss2.get_monitored_dci_formats()) ==
          search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);
  if (fallback_dci_format_in_ss2) {
    CHECK_TRUE(mcs_table != pusch_mcs_table::qam256,
               "256QAM MCS table cannot be used for PUSCH with fallback DCI format in SearchSpace#2");
  }
  if (mcs_table == pusch_mcs_table::qam64LowSe) {
    // As per Section 5.1.3.1, TS 38.213 and assuming MCS-C-RNTI is not supported.
    CHECK_TRUE(not ss2.is_common_search_space(),
               "64QAM Low Se MCS table cannot be used for PDSCH with DCI in Common SearchSpace");
  }
  if (pusch_params.additional_positions == dmrs_additional_positions::pos3) {
    CHECK_TRUE(cell_cfg.ran.dmrs_typeA_pos == dmrs_typeA_position::pos2,
               "PUSCH dmrs-Additional-Position of pos3 is only supported when dmrs-TypeA-Position is equal to pos2");
  }
  if (cell_cfg.ran.tdd_cfg.has_value()) {
    CHECK_TRUE(sr_periodicity_to_slot(cell_cfg.ran.init_bwp.pucch.sr_period) %
                       nof_slots_per_tdd_period(cell_cfg.ran.tdd_cfg.value()) ==
                   0,
               "Scheduling request resource periodicity that is not a submultiple of the TDD "
               "configuration periodicity is not supported.");
  }
  return {};
}

static check_outcome check_tdd_ul_dl_config(const du_cell_config& cell_cfg)
{
  if (not cell_cfg.ran.tdd_cfg.has_value()) {
    return {};
  }

  // See TS 38.214, Table 5.1.2.1-1: Valid S and L combinations.
  static constexpr unsigned pdsch_mapping_typeA_min_L_value = 3;

  const pdcch_config_common&                  common_pdcch_cfg = cell_cfg.ran.dl_cfg_common.init_dl_bwp.pdcch_common;
  const pdcch_config&                         ded_pdcch_cfg    = cell_cfg.ran.init_bwp.pdcch_cfg.value();
  const std::optional<coreset_configuration>& coreset0         = common_pdcch_cfg.coreset0;
  const std::optional<coreset_configuration>& common_coreset   = common_pdcch_cfg.common_coreset;
  const auto                                  ss0_idx          = common_pdcch_cfg.get_searchspace0();
  CHECK_TRUE(ss0_idx.has_value(), "SearchSpace#0 not found in common SearchSpace list");
  CHECK_TRUE(coreset0.has_value(), "CORESET#0 not configured");

  const pdcch_type0_css_occasion_pattern1_description& ss0_occasion =
      pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
          .is_fr2 = false, .ss0_index = ss0_idx.value(), .nof_symb_coreset = coreset0->duration()});

  const auto& tdd_cfg = cell_cfg.ran.tdd_cfg.value();
  CHECK_TRUE(
      (get_nof_slots_per_subframe(tdd_cfg.ref_scs) * NOF_SUBFRAMES_PER_FRAME) % nof_slots_per_tdd_period(tdd_cfg) == 0,
      "TDD configuration periodicity that is not a submultiple of the number of slots in a radio frame is "
      "not supported.");
  const auto pattern1_period_slots    = tdd_cfg.pattern1.dl_ul_tx_period_nof_slots;
  auto       pattern1_additional_slot = tdd_cfg.pattern1.nof_ul_symbols + tdd_cfg.pattern1.nof_dl_symbols > 0 ? 1 : 0;
  pattern1_additional_slot += tdd_cfg.pattern1.nof_ul_symbols + tdd_cfg.pattern1.nof_dl_symbols > 14 ? 1 : 0;
  CHECK_EQ_OR_BELOW(tdd_cfg.pattern1.nof_dl_slots + tdd_cfg.pattern1.nof_ul_slots + pattern1_additional_slot,
                    pattern1_period_slots,
                    "TDD UL DL pattern 1 configuration. UL(slots + symbols) + DL(slots + symbols) configuration "
                    "exceeds TDD pattern period.");

  if (tdd_cfg.pattern2.has_value()) {
    const auto pattern2_period_slots = tdd_cfg.pattern2.value().dl_ul_tx_period_nof_slots;
    auto       pattern2_additional_slot =
        tdd_cfg.pattern2.value().nof_ul_symbols + tdd_cfg.pattern2.value().nof_dl_symbols > 0 ? 1 : 0;
    pattern2_additional_slot +=
        tdd_cfg.pattern2.value().nof_ul_symbols + tdd_cfg.pattern2.value().nof_dl_symbols > 14 ? 1 : 0;
    CHECK_EQ_OR_BELOW(tdd_cfg.pattern2.value().nof_dl_slots + tdd_cfg.pattern2.value().nof_ul_slots +
                          pattern2_additional_slot,
                      pattern2_period_slots,
                      "TDD UL DL pattern 2 configuration. UL(slots + symbols) + DL(slots + symbols) configuration "
                      "exceeds TDD pattern period.");
  }

  // NOTE1: Number of DL PDSCH symbols must be atleast greater than SearchSpace monitoring start symbol index + CORESET
  // duration for PDSCH allocation in partial slot. Otherwise, it can be used only for PDCCH allocations.
  // NOTE2: We don't support multiple monitoring occasions in a slot belonging to a SearchSpace.
  // TODO: Consider SearchSpace periodicity while validating DL symbols in special slots.
  std::optional<unsigned> cs_duration;
  unsigned                ss_start_symbol_idx;
  // SearchSpaces in Common PDCCH configuration.
  for (const search_space_configuration& ss_cfg : common_pdcch_cfg.search_spaces) {
    cs_duration         = {};
    ss_start_symbol_idx = 0;
    if (coreset0.has_value()) {
      if (ss_cfg.get_id() == to_search_space_id(0) and ss_cfg.get_coreset_id() == coreset0->get_id()) {
        cs_duration = coreset0->duration();
        // Consider the starting index of last PDCCH monitoring occasion to account for all SSB beams.
        ss_start_symbol_idx = *std::max_element(ss0_occasion.start_symbol.begin(), ss0_occasion.start_symbol.end());
      } else if (ss_cfg.get_id() != to_search_space_id(0) and ss_cfg.get_coreset_id() == coreset0->get_id()) {
        cs_duration         = coreset0->duration();
        ss_start_symbol_idx = ss_cfg.get_first_symbol_index();
      }
    }
    if (common_coreset.has_value()) {
      if (ss_cfg.get_id() == to_search_space_id(0) and ss_cfg.get_coreset_id() == common_coreset->get_id()) {
        cs_duration = common_coreset->duration();
        // Consider the starting index of last PDCCH monitoring occasion to account for all SSB beams.
        ss_start_symbol_idx = *std::max_element(ss0_occasion.start_symbol.begin(), ss0_occasion.start_symbol.end());
      } else if (ss_cfg.get_id() != to_search_space_id(0) and ss_cfg.get_coreset_id() == common_coreset->get_id()) {
        cs_duration         = common_coreset->duration();
        ss_start_symbol_idx = ss_cfg.get_first_symbol_index();
      }
    }

    if (tdd_cfg.pattern1.nof_dl_symbols > 0) {
      // Ensuring there is enough DL symbols for PDCCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern1.nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value(),
                        "TDD UL DL pattern 1 configuration. DL(symbols) configuration is less than CORESET symbols[{}, "
                        "{}) for SearchSpace#{}.",
                        ss_start_symbol_idx,
                        ss_start_symbol_idx + cs_duration.value(),
                        fmt::underlying(ss_cfg.get_id()));

      // Ensuring there is enough DL symbols for PDSCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern1.nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value() + pdsch_mapping_typeA_min_L_value,
                        "TDD UL DL pattern 1 configuration. DL(symbols) configuration is less than the minimum nof. "
                        "OFDM symbols required for PDSCH allocation of mapping typeA in SearchSpace#{}.",
                        fmt::underlying(ss_cfg.get_id()));
    }

    if (tdd_cfg.pattern2.has_value() and tdd_cfg.pattern2.value().nof_dl_symbols > 0) {
      // Ensuring there is enough DL symbols for PDCCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern2.value().nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value(),
                        "TDD UL DL pattern 2 configuration. DL(symbols) configuration is less than CORESET symbols[{}, "
                        "{}) for SearchSpace#{}.",
                        ss_start_symbol_idx,
                        ss_start_symbol_idx + cs_duration.value(),
                        fmt::underlying(ss_cfg.get_id()));

      // Ensuring there is enough DL symbols for PDSCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern2.value().nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value() + pdsch_mapping_typeA_min_L_value,
                        "TDD UL DL pattern 2 configuration. DL(symbols) configuration is less than the minimum nof. "
                        "OFDM symbols required for PDSCH allocation of mapping typeA in SearchSpace#{}.",
                        fmt::underlying(ss_cfg.get_id()));
    }
  }
  // SearchSpaces in Dedicated PDCCH configuration.
  for (const search_space_configuration& ss_cfg : ded_pdcch_cfg.search_spaces) {
    cs_duration         = {};
    ss_start_symbol_idx = ss_cfg.get_first_symbol_index();
    if (coreset0.has_value()) {
      if (ss_cfg.get_coreset_id() == coreset0->get_id()) {
        cs_duration = coreset0->duration();
      }
    }
    if ((not cs_duration.has_value()) and common_coreset.has_value()) {
      if (ss_cfg.get_coreset_id() == common_coreset->get_id()) {
        cs_duration = common_coreset->duration();
      }
    }

    if (not cs_duration.has_value()) {
      for (const coreset_configuration& cs_cfg : ded_pdcch_cfg.coresets) {
        if (ss_cfg.get_coreset_id() == cs_cfg.get_id()) {
          cs_duration = cs_cfg.duration();
          break;
        }
      }
    }

    CHECK_TRUE(cs_duration.has_value(), "CORESET not configured for SearchSpace#{}", fmt::underlying(ss_cfg.get_id()));

    if (tdd_cfg.pattern1.nof_dl_symbols > 0) {
      // Ensuring there is atleast 1 OFDM symbol for PDSCH.
      CHECK_ABOVE(tdd_cfg.pattern1.nof_dl_symbols,
                  ss_start_symbol_idx + cs_duration.value(),
                  "TDD UL DL pattern 1 configuration. DL(symbols) configuration is less than CORESET symbols[{}, "
                  "{}) for SearchSpace#{}.",
                  ss_start_symbol_idx,
                  ss_start_symbol_idx + cs_duration.value(),
                  fmt::underlying(ss_cfg.get_id()));

      // Ensuring there is enough DL symbols for PDSCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern1.nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value() + pdsch_mapping_typeA_min_L_value,
                        "TDD UL DL pattern 1 configuration. DL(symbols) configuration is less than the minimum nof. "
                        "OFDM symbols required for PDSCH allocation of mapping typeA in SearchSpace#{}.",
                        fmt::underlying(ss_cfg.get_id()));
    }

    if (tdd_cfg.pattern2.has_value() and tdd_cfg.pattern2.value().nof_dl_symbols > 0) {
      // Ensuring there is atleast 1 OFDM symbol for PDSCH.
      CHECK_ABOVE(tdd_cfg.pattern2.value().nof_dl_symbols,
                  ss_start_symbol_idx + cs_duration.value(),
                  "TDD UL DL pattern 2 configuration. DL(symbols) configuration is less than CORESET symbols[{}, "
                  "{}) for SearchSpace#{}.",
                  ss_start_symbol_idx,
                  ss_start_symbol_idx + cs_duration.value(),
                  fmt::underlying(ss_cfg.get_id()));

      // Ensuring there is enough DL symbols for PDSCH.
      CHECK_EQ_OR_ABOVE(tdd_cfg.pattern2.value().nof_dl_symbols,
                        ss_start_symbol_idx + cs_duration.value() + pdsch_mapping_typeA_min_L_value,
                        "TDD UL DL pattern 2 configuration. DL(symbols) configuration is less than the minimum nof. "
                        "OFDM symbols required for PDSCH allocation of mapping typeA in SearchSpace#{}.",
                        fmt::underlying(ss_cfg.get_id()));
    }
  }

  return {};
}

static check_outcome check_prach_config(const du_cell_config& cell_cfg)
{
  CHECK_TRUE(cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common.has_value(),
             "Rach config common not present in UL BWP");

  const rach_config_common& rach_cfg = cell_cfg.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common.value();

  const auto prach_cfg = prach_configuration_get(band_helper::get_freq_range(cell_cfg.ran.dl_carrier.band),
                                                 band_helper::get_duplex_mode(cell_cfg.ran.dl_carrier.band),
                                                 rach_cfg.rach_cfg_generic.prach_config_index);
  CHECK_NEQ(fmt::underlying(prach_cfg.format),
            fmt::underlying(ocudu::prach_format_type::invalid),
            "The PRACH format is invalid");

  // Derive PRACH duration information.
  // The parameter \c is_last_prach_occasion is arbitrarily set to false, as it doesn't affect the PRACH number of PRBs.
  constexpr bool                   is_last_prach_occasion = false;
  const prach_preamble_information info =
      is_long_preamble(prach_cfg.format)
          ? get_prach_preamble_long_info(prach_cfg.format)
          : get_prach_preamble_short_info(
                prach_cfg.format,
                to_ra_subcarrier_spacing(cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.scs),
                is_last_prach_occasion);
  const unsigned prach_nof_prbs =
      prach_frequency_mapping_get(info.scs, cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;

  const uint8_t prach_prb_stop =
      rach_cfg.rach_cfg_generic.msg1_frequency_start + rach_cfg.rach_cfg_generic.msg1_fdm * prach_nof_prbs;

  prb_interval prb_interval_no_pucch = config_helpers::find_largest_prb_interval_without_pucch(
      cell_cfg.ran.init_bwp.pucch.resources, cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());

  // This is to preserve a guardband between the PUCCH and PRACH.
  const unsigned pucch_to_prach_guardband = is_long_preamble(prach_cfg.format) ? 0U : 3U;

  CHECK_TRUE(prach_prb_stop + pucch_to_prach_guardband <= prb_interval_no_pucch.stop(),
             "With the given prach_frequency_start={}, the PRACH opportunities in prbs=[{}, {}) overlap with the PUCCH "
             "resources/guardband in PRBs=[{}, {})",
             rach_cfg.rach_cfg_generic.msg1_frequency_start,
             rach_cfg.rach_cfg_generic.msg1_frequency_start,
             prach_prb_stop,
             prb_interval_no_pucch.stop() - pucch_to_prach_guardband,
             cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());

  return {};
}

/// Validates NTN cell configuration parameters.
static check_outcome check_ntn_config(const du_cell_config& cell_cfg)
{
  if (!cell_cfg.ran.ntn_params.has_value()) {
    return {}; // NTN not configured, skip validation
  }

  const auto& ntn = cell_cfg.ran.ntn_params.value().ntn_cfg;

  // Validate cell_specific_koffset (required for NTN).
  if (ntn.cell_specific_koffset.has_value()) {
    CHECK_EQ_OR_ABOVE(ntn.cell_specific_koffset.value().count(), 1, "cell_specific_koffset");
    CHECK_EQ_OR_BELOW(ntn.cell_specific_koffset.value().count(), 1023, "cell_specific_koffset");
  }

  if (ntn.k_mac.has_value()) {
    CHECK_EQ_OR_ABOVE(ntn.k_mac.value(), 1, "k_mac");
    CHECK_EQ_OR_BELOW(ntn.k_mac.value(), 512, "k_mac");
  }

  if (ntn.ntn_ul_sync_validity_dur.has_value()) {
    const bool valid_sync_dur = is_valid_enum_number<asn1::rrc_nr::ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_e_>(
        ntn.ntn_ul_sync_validity_dur.value());
    CHECK_TRUE(valid_sync_dur, "Invalid ntn_ul_sync_validity_dur value={}", ntn.ntn_ul_sync_validity_dur.value());
  }

  if (ntn.epoch_time.has_value()) {
    CHECK_EQ_OR_BELOW(ntn.epoch_time->sfn, 1023, "epoch_time.sfn");
    CHECK_EQ_OR_BELOW(ntn.epoch_time->subframe_number, 9, "epoch_time.subframe_number");
  }

  return {};
}

check_outcome odu::is_du_cell_config_valid(const du_cell_config& cell_cfg)
{
  CHECK_EQ_OR_BELOW(cell_cfg.ran.pci, MAX_PCI, "cell PCI");
  CHECK_EQ_OR_BELOW(fmt::underlying(cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.scs),
                    fmt::underlying(subcarrier_spacing::kHz120),
                    "SCS common");
  HANDLE_ERROR(is_coreset0_ss0_idx_valid(cell_cfg));
  HANDLE_ERROR(check_dl_config_common(cell_cfg));
  HANDLE_ERROR(check_ul_config_common(cell_cfg));
  HANDLE_ERROR(check_ssb_configuration(cell_cfg));
  HANDLE_ERROR(check_tdd_ul_dl_config(cell_cfg));
  HANDLE_ERROR(config_helpers::pucch_parameters_validator(
      cell_cfg.ran.init_bwp.pucch.resources, cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.crbs.length()));
  HANDLE_ERROR(check_prach_config(cell_cfg));
  const serving_cell_config ue_serv_cell_cfg = config_helpers::make_default_ue_cell_config(cell_cfg.ran).serv_cell_cfg;
  HANDLE_ERROR(
      config_validators::validate_csi_meas_cfg(ue_serv_cell_cfg, cell_cfg.ran.tdd_cfg, cell_cfg.ran.ul_cfg_common));
  HANDLE_ERROR(check_dl_config_dedicated(cell_cfg));
  HANDLE_ERROR(check_ul_config_dedicated(cell_cfg));
  HANDLE_ERROR(check_ntn_config(cell_cfg));
  // TODO: Remaining.
  return {};
}
