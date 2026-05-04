// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_capability_manager.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/du/du_high/du_test_mode_config.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/pusch/tx_scheme_configuration.h"
#include "ocudu/scheduler/scheduler_configurator.h"

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

// Configure dedicated UE configuration to set MCS ant CQI tables.
static void set_pdsch_mcs_table(serving_cell_config& cell_cfg, pdsch_mcs_table mcs_table)
{
  cqi_table_t cqi_table;
  // Set CQI table according to the MCS table used for PDSCH.
  switch (mcs_table) {
    case pdsch_mcs_table::qam64:
      cqi_table = cqi_table_t::table1;
      break;
    case pdsch_mcs_table::qam256:
      cqi_table = cqi_table_t::table2;
      break;
    case pdsch_mcs_table::qam64LowSe:
      cqi_table = cqi_table_t::table3;
      break;
    default:
      report_error("Invalid MCS table={}\n", fmt::underlying(mcs_table));
  }

  // Set MCS index table for PDSCH. See TS 38.214, Table 5.1.3.1-[1-3].
  if (cell_cfg.init_dl_bwp.pdsch_cfg.has_value()) {
    cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = mcs_table;
  }

  if (cell_cfg.csi_meas_cfg.has_value()) {
    for (auto& csi_report_cfg : cell_cfg.csi_meas_cfg.value().csi_report_cfg_list) {
      // Set CQI table. See TS 38.214, Table 5.2.2.1-[1-4].
      csi_report_cfg.cqi_table = cqi_table;
    }
  }
}

// Configure dedicated UE configuration to set PUSCH MCS.
static void set_pusch_mcs_table(serving_cell_config& cell_cfg, pusch_mcs_table mcs_table)
{
  // Set MCS index table for PUSCH. See TS 38.214, Table 5.1.3.1-[1-3].
  if (cell_cfg.ul_config.has_value() and cell_cfg.ul_config->init_ul_bwp.pusch_cfg.has_value()) {
    cell_cfg.ul_config->init_ul_bwp.pusch_cfg->mcs_table = mcs_table;
  }
}

// Configure dedicated UE configuration to set UL-MIMO related parameters.
static void set_ul_mimo(ue_cell_config&           cell_cfg,
                        unsigned                  max_rank,
                        unsigned                  nof_srs_ports,
                        tx_scheme_codebook_subset codebook_subset)
{
  // Skip if the UL configuration is not present.
  if (OCUDU_UNLIKELY(!cell_cfg.serv_cell_cfg.ul_config.has_value() ||
                     !cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.pusch_cfg ||
                     !cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg)) {
    return;
  }

  // Prepare codebook transmission parameters.
  cell_cfg.init_bwp().ul.pusch.tx_cfg = tx_scheme_codebook{.max_rank = max_rank, .codebook_subset = codebook_subset};
  cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.pusch_cfg->tx_cfg =
      tx_scheme_codebook{.max_rank = max_rank, .codebook_subset = codebook_subset};

  // Force the number of ports for all SRS resources to the maximum the UE supports.
  cell_cfg.init_bwp().ul.srs.nof_ports = static_cast<srs_config::srs_resource::nof_srs_ports>(nof_srs_ports);
  for (auto& srs_res : cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list) {
    srs_res.nof_ports = static_cast<srs_config::srs_resource::nof_srs_ports>(nof_srs_ports);
  }
}

static void set_pdsch_interleaving(serving_cell_config& cell_cfg, vrb_to_prb::mapping_type interleaving_opt)
{
  // Set \c vrb-ToPRB-Interleaver for PDSCH, as per TS 38.331, \c PDSCH-Config.
  if (cell_cfg.init_dl_bwp.pdsch_cfg.has_value()) {
    cell_cfg.init_dl_bwp.pdsch_cfg->vrb_to_prb_interleaving = interleaving_opt;
  }
}

static void set_max_dl_nof_harqs(serving_cell_config& cell_cfg, unsigned nof_harqs)
{
  if (cell_cfg.pdsch_serv_cell_cfg.has_value()) {
    cell_cfg.pdsch_serv_cell_cfg->nof_harq_proc =
        static_cast<pdsch_serving_cell_config::nof_harq_proc_for_pdsch>(nof_harqs);
  }
}

static void set_max_ul_nof_harqs(serving_cell_config& cell_cfg, unsigned nof_harqs)
{
  if (cell_cfg.ul_config.has_value() and cell_cfg.ul_config->pusch_serv_cell_cfg.has_value()) {
    cell_cfg.ul_config->pusch_serv_cell_cfg->nof_harq_proc =
        static_cast<pusch_serving_cell_config::nof_harq_proc_for_pusch>(nof_harqs);
  }
}

static void set_dl_dci_harq_num_field_size(serving_cell_config& cell_cfg, unsigned field_size)
{
  if (cell_cfg.init_dl_bwp.pdsch_cfg.has_value()) {
    cell_cfg.init_dl_bwp.pdsch_cfg->harq_process_num_size_dci_1_1 =
        static_cast<pdsch_config::harq_process_num_dci_1_1_size>(field_size);
    cell_cfg.init_dl_bwp.pdsch_cfg->harq_process_num_size_dci_1_2 =
        static_cast<pdsch_config::harq_process_num_dci_1_2_size>(field_size);
  }
}

static void set_ul_dci_harq_num_field_size(serving_cell_config& cell_cfg, unsigned field_size)
{
  if (cell_cfg.ul_config.has_value() and cell_cfg.ul_config->init_ul_bwp.pusch_cfg.has_value()) {
    cell_cfg.ul_config->init_ul_bwp.pusch_cfg->harq_process_num_size_dci_0_1 =
        static_cast<pusch_config::harq_process_num_dci_0_1_size>(field_size);
    cell_cfg.ul_config->init_ul_bwp.pusch_cfg->harq_process_num_size_dci_0_2 =
        static_cast<pusch_config::harq_process_num_dci_0_2_size>(field_size);
  }
}

static void set_ul_harq_mode(serving_cell_config& cell_cfg, const harq_ul_mode_mask& ul_harq_mode_mask)
{
  if (cell_cfg.ul_config.has_value() and cell_cfg.ul_config->pusch_serv_cell_cfg.has_value()) {
    cell_cfg.ul_config->pusch_serv_cell_cfg->ul_harq_mode = ul_harq_mode_mask;
  }
}

static void set_dl_harq_feedback_disabled(serving_cell_config&                  cell_cfg,
                                          const harq_dl_feedback_disabled_mask& disabled_harq_feedback_mask)
{
  if (cell_cfg.pdsch_serv_cell_cfg.has_value()) {
    cell_cfg.pdsch_serv_cell_cfg->dl_harq_feedback_disabled = disabled_harq_feedback_mask;
  }
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
  du_cell_index_t cell_idx  = to_du_cell_index(0);
  ue_cell_config& pcell_cfg = ue_res_cfg.cell_group.cells.at(SERVING_PCELL_IDX);

  // Set default MCS tables and disable UL MIMO.
  set_pdsch_mcs_table(pcell_cfg.serv_cell_cfg, select_pdsch_mcs_table(cell_idx));
  set_pusch_mcs_table(pcell_cfg.serv_cell_cfg, select_pusch_mcs_table(cell_idx));
  set_pdsch_interleaving(pcell_cfg.serv_cell_cfg, select_pdsch_interleaving(cell_idx));
  set_ul_mimo(
      pcell_cfg, select_pusch_max_rank(cell_idx), select_srs_nof_ports(cell_idx), select_tx_codebook_subset(cell_idx));
  set_max_dl_nof_harqs(pcell_cfg.serv_cell_cfg, select_max_dl_nof_harqs(cell_idx));
  set_max_ul_nof_harqs(pcell_cfg.serv_cell_cfg, select_max_ul_nof_harqs(cell_idx));
  set_dl_dci_harq_num_field_size(pcell_cfg.serv_cell_cfg, select_dl_dci_harq_num_field_size(cell_idx));
  set_ul_dci_harq_num_field_size(pcell_cfg.serv_cell_cfg, select_ul_dci_harq_num_field_size(cell_idx));
  set_dl_harq_feedback_disabled(pcell_cfg.serv_cell_cfg, select_disabled_dl_harq_feedback(cell_idx));
  set_ul_harq_mode(pcell_cfg.serv_cell_cfg, select_ul_harq_mode(cell_idx));

  // Initialize UE with DRX disabled.
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
  du_cell_index_t cell_idx  = to_du_cell_index(0);
  ue_cell_config& pcell_cfg = ue_res_cfg.cell_group.cells.at(SERVING_PCELL_IDX);

  // Enable 256QAM for PDSCH, if supported.
  set_pdsch_mcs_table(pcell_cfg.serv_cell_cfg, select_pdsch_mcs_table(cell_idx));

  // Enable 256QAM for PUSCH, if supported.
  set_pusch_mcs_table(pcell_cfg.serv_cell_cfg, select_pusch_mcs_table(cell_idx));

  // Enable VRB-to-PRB interleaving for PDSCH, if supported.
  set_pdsch_interleaving(pcell_cfg.serv_cell_cfg, select_pdsch_interleaving(cell_idx));

  // Setup UL MIMO parameters.
  set_ul_mimo(
      pcell_cfg, select_pusch_max_rank(cell_idx), select_srs_nof_ports(cell_idx), select_tx_codebook_subset(cell_idx));

  // Set max DL/UL nof HARQs.
  set_max_dl_nof_harqs(pcell_cfg.serv_cell_cfg, select_max_dl_nof_harqs(cell_idx));
  set_max_ul_nof_harqs(pcell_cfg.serv_cell_cfg, select_max_ul_nof_harqs(cell_idx));

  // Set DL/UL DCI HARQ Process Number size.
  set_dl_dci_harq_num_field_size(pcell_cfg.serv_cell_cfg, select_dl_dci_harq_num_field_size(cell_idx));
  set_ul_dci_harq_num_field_size(pcell_cfg.serv_cell_cfg, select_ul_dci_harq_num_field_size(cell_idx));

  // Set DL HARQ feedback disabled.
  set_dl_harq_feedback_disabled(pcell_cfg.serv_cell_cfg, select_disabled_dl_harq_feedback(cell_idx));

  // Set UL HARQ Mode B enabled.
  set_ul_harq_mode(pcell_cfg.serv_cell_cfg, select_ul_harq_mode(cell_idx));

  // Setup DRX config.
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

pdsch_mcs_table ue_capability_manager::select_pdsch_mcs_table(du_cell_index_t cell_idx) const
{
  const pdsch_mcs_table app_mcs_table = base_cell_cfg_list[cell_idx].ran.init_bwp.pdsch.mcs_table;
  if (ue_caps.has_value()) {
    if (app_mcs_table == pdsch_mcs_table::qam256 and ue_caps->pdsch_qam256_supported) {
      return pdsch_mcs_table::qam256;
    }
    if (app_mcs_table == pdsch_mcs_table::qam64LowSe and ue_caps->pdsch_qam64lowse_supported) {
      return pdsch_mcs_table::qam64LowSe;
    }
  } else if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    // Has no capabilities but the UE is in test mode.
    return app_mcs_table;
  }

  // Default to QAM64 if no base cell PDSCH config of if the UE capabilities are not available.
  return pdsch_mcs_table::qam64;
}

pusch_mcs_table ue_capability_manager::select_pusch_mcs_table(du_cell_index_t cell_idx) const
{
  nr_band               band          = base_cell_cfg_list[cell_idx].ran.ul_carrier.band;
  const pusch_mcs_table app_mcs_table = base_cell_cfg_list[cell_idx].ran.init_bwp.pusch.mcs_table;

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    // In case of test mode, we do not need to rely on capabilities.
    return app_mcs_table;
  }
  if (not ue_caps.has_value()) {
    // UE capabilities have not been decoded yet. Default to QAM64.
    return pusch_mcs_table::qam64;
  }

  if (app_mcs_table == pusch_mcs_table::qam256) {
    // If the band capability is present, select the MCS table from this band.
    if (ue_caps->bands.count(band)) {
      return ue_caps->bands.at(band).pusch_qam256_supported ? pusch_mcs_table::qam256 : pusch_mcs_table::qam64;
    }

    // In case the preferred MCS table is 256QAM, but the UE does not support it, we default to QAM64.
    if (std::none_of(ue_caps->bands.begin(), ue_caps->bands.end(), [](const auto& b) {
          return b.second.pusch_qam256_supported;
        })) {
      return pusch_mcs_table::qam64;
    }
  } else if (app_mcs_table == pusch_mcs_table::qam64LowSe) {
    return ue_caps.value().pusch_qam64lowse_supported ? pusch_mcs_table::qam64LowSe : pusch_mcs_table::qam64;
  }

  return app_mcs_table;
}

vrb_to_prb::mapping_type ue_capability_manager::select_pdsch_interleaving(du_cell_index_t cell_idx) const
{
  const vrb_to_prb::mapping_type app_pdsch_interleaving =
      base_cell_cfg_list[cell_idx].ran.init_bwp.pdsch.interleaving_bundle_size;
  if (ue_caps.has_value()) {
    if (ue_caps->pdsch_interleaving_vrb_to_prb_supported) {
      // If the PDSCH interleaving is enabled in the application configuration and supported by the UE, return it.
      return app_pdsch_interleaving;
    }
  } else if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    // Has no capabilities but the UE is in test mode.
    return app_pdsch_interleaving;
  }

  // Default to \c non-interleaved if no base cell PDSCH config or if the UE capabilities are not available.
  return vrb_to_prb::mapping_type::non_interleaved;
}

tx_scheme_codebook_subset ue_capability_manager::select_tx_codebook_subset(du_cell_index_t cell_idx) const
{
  nr_band band = base_cell_cfg_list[cell_idx].ran.ul_carrier.band;

  // If UE capabilities or the band are not available, return default value.
  if (not ue_caps.has_value() || ue_caps->bands.count(band) == 0) {
    return ue_capability_summary::default_pusch_tx_coherence;
  }

  return ue_caps->bands.at(band).pusch_tx_coherence;
}

unsigned ue_capability_manager::select_srs_nof_ports(du_cell_index_t cell_idx) const
{
  nr_band band = base_cell_cfg_list[cell_idx].ran.ul_carrier.band;

  // If UE capabilities or the band are not available, return default value.
  if (not ue_caps.has_value() || ue_caps->bands.count(band) == 0) {
    return ue_capability_summary::default_nof_srs_tx_ports;
  }

  return ue_caps->bands.at(band).nof_srs_tx_ports;
}

unsigned ue_capability_manager::select_pusch_max_rank(du_cell_index_t cell_idx) const
{
  nr_band band = base_cell_cfg_list[cell_idx].ran.ul_carrier.band;

  // Configured maximum number of layers.
  unsigned pusch_max_rank = base_cell_cfg_list[cell_idx].ran.init_bwp.pusch.max_nof_layers;

  // If UE capabilities or the band are not available, return default value.
  if (not ue_caps.has_value() || ue_caps->bands.count(band) == 0) {
    return std::min(pusch_max_rank, ue_capability_summary::default_pusch_max_rank);
  }

  return std::min(pusch_max_rank, ue_caps->bands.at(band).pusch_max_rank);
}

unsigned ue_capability_manager::select_max_dl_nof_harqs(du_cell_index_t cell_idx) const
{
  // Configured maximum number of DL HARQs.
  unsigned cell_max_nof_dl_harq_proc = base_cell_cfg_list[cell_idx].ran.init_bwp.pdsch.max_harq_procs;

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    // In case of test mode, we do not need to rely on capabilities.
    return cell_max_nof_dl_harq_proc;
  }

  nr_band band = base_cell_cfg_list[cell_idx].ran.dl_carrier.band;

  // UE capabilities not yet decoded, so return at most 16.
  // Extended HARQ processes (up to 32 for NTN) require explicit UE capability confirmation.
  if (not ue_caps.has_value()) {
    return std::min(cell_max_nof_dl_harq_proc, ue_capability_summary::default_max_harq_process_num);
  }

  // UE capabilities available but band info not available, or UE does not support NTN, return at most 16.
  if (not band_helper::is_ntn_band(band) || ue_caps->bands.count(band) == 0 || not ue_caps->ntn_supported) {
    return std::min(cell_max_nof_dl_harq_proc, ue_capability_summary::default_max_harq_process_num);
  }

  // UE capabilities available and NTN supported.
  return std::min(cell_max_nof_dl_harq_proc, (unsigned)ue_caps->bands.at(band).max_dl_harq_process_num);
}

unsigned ue_capability_manager::select_max_ul_nof_harqs(du_cell_index_t cell_idx) const
{
  // Configured maximum number of UL HARQs.
  unsigned cell_max_nof_ul_harq_proc = base_cell_cfg_list[cell_idx].ran.init_bwp.pusch.max_harq_procs;

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    // In case of test mode, we do not need to rely on capabilities.
    return cell_max_nof_ul_harq_proc;
  }

  nr_band band = base_cell_cfg_list[cell_idx].ran.ul_carrier.band;

  // UE capabilities not yet decoded, so return at most 16.
  // Extended HARQ processes (up to 32 for NTN) require explicit UE capability confirmation.
  if (not ue_caps.has_value()) {
    return std::min(cell_max_nof_ul_harq_proc, ue_capability_summary::default_max_harq_process_num);
  }

  // UE capabilities available but band info not available, or UE does not support NTN, return at most 16.
  if (not band_helper::is_ntn_band(band) || ue_caps->bands.count(band) == 0 || not ue_caps->ntn_supported) {
    return std::min(cell_max_nof_ul_harq_proc, ue_capability_summary::default_max_harq_process_num);
  }

  // UE capabilities available and NTN supported.
  return std::min(cell_max_nof_ul_harq_proc, (unsigned)ue_caps->bands.at(band).max_ul_harq_process_num);
}

unsigned ue_capability_manager::select_dl_dci_harq_num_field_size(du_cell_index_t cell_idx) const
{
  auto default_dci_size = log2_ceil(ue_capability_summary::default_max_harq_process_num);

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    // In case of test mode, we do not need to rely on capabilities.
    return default_dci_size;
  }

  // Configured maximum number of DL HARQs.
  unsigned cell_max_nof_dl_harq_proc = base_cell_cfg_list[cell_idx].ran.init_bwp.pdsch.max_harq_procs;
  auto     cell_dci_size             = log2_ceil(cell_max_nof_dl_harq_proc);

  nr_band band = base_cell_cfg_list[cell_idx].ran.dl_carrier.band;

  // UE capabilities have not been decoded yet, band info no available, or UE does not support NTN.
  if (not band_helper::is_ntn_band(band) || not ue_caps.has_value() || ue_caps->bands.count(band) == 0 ||
      not ue_caps->ntn_supported) {
    // return 4
    return default_dci_size;
  }

  auto band_dci_size = log2_ceil((unsigned)ue_caps->bands.at(band).max_dl_harq_process_num);
  // return 4 or 5
  return std::max(std::min(cell_dci_size, band_dci_size), default_dci_size);
}

unsigned ue_capability_manager::select_ul_dci_harq_num_field_size(du_cell_index_t cell_idx) const
{
  auto default_dci_size = log2_ceil(ue_capability_summary::default_max_harq_process_num);

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    // In case of test mode, we do not need to rely on capabilities.
    return default_dci_size;
  }

  // Configured maximum number of UL HARQs.
  unsigned cell_max_nof_ul_harq_proc = base_cell_cfg_list[cell_idx].ran.init_bwp.pusch.max_harq_procs;
  auto     cell_dci_size             = log2_ceil(cell_max_nof_ul_harq_proc);

  nr_band band = base_cell_cfg_list[cell_idx].ran.ul_carrier.band;

  // UE capabilities have not been decoded yet, band info no available, or UE does not support NTN.
  if (not band_helper::is_ntn_band(band) || not ue_caps.has_value() || ue_caps->bands.count(band) == 0 ||
      not ue_caps->ntn_supported) {
    // return 4
    return default_dci_size;
  }

  auto band_dci_size = log2_ceil((unsigned)ue_caps->bands.at(band).max_ul_harq_process_num);
  // return 4 or 5
  return std::max(std::min(cell_dci_size, band_dci_size), default_dci_size);
}

harq_dl_feedback_disabled_mask ue_capability_manager::select_disabled_dl_harq_feedback(du_cell_index_t cell_idx) const
{
  // The bit(s) set to zero identify HARQ processes with enabled DL HARQ feedback.
  harq_dl_feedback_disabled_mask default_harq_feedback_disabled(MAX_NOF_HARQS);
  default_harq_feedback_disabled.reset();

  // Configured disabled DL HARQ feedback.
  harq_dl_feedback_disabled_mask cell_harq_feedback_disabled =
      base_cell_cfg_list[cell_idx].ran.init_bwp.pdsch.dl_harq_feedback_disabled;

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    // In case of test mode, we do not need to rely on capabilities.
    return cell_harq_feedback_disabled;
  }

  const nr_band band = base_cell_cfg_list[cell_idx].ran.dl_carrier.band;
  // Not NTN band, UE capabilities have not been decoded yet, band info no available, or UE does not support NTN.
  if (not band_helper::is_ntn_band(band) || not ue_caps.has_value() || ue_caps->bands.count(band) == 0 ||
      not ue_caps->ntn_supported) {
    // DL HARQ Feedback cannot be disabled.
    return default_harq_feedback_disabled;
  }

  if (ue_caps->disabled_dl_harq_feedback_supported) {
    return cell_harq_feedback_disabled;
  }
  return default_harq_feedback_disabled;
}

harq_ul_mode_mask ue_capability_manager::select_ul_harq_mode(du_cell_index_t cell_idx) const
{
  // A bit set to one identifies a HARQ process in modeA and a bit set to zero identifies a HARQ process in modeB.
  harq_ul_mode_mask default_ul_harq_mode_mask(MAX_NOF_HARQS);
  default_ul_harq_mode_mask.fill(true);

  // Configured disabled UL HARQ mode B.
  harq_ul_mode_mask cell_ul_harq_mode_mask = base_cell_cfg_list[cell_idx].ran.init_bwp.pusch.ul_harq_mode;

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    // In case of test mode, we do not need to rely on capabilities.
    return cell_ul_harq_mode_mask;
  }

  // UE capabilities have not been decoded yet, but NTN band.
  if (not ue_caps.has_value() || not ue_caps->ntn_supported) {
    // If the UE does not support NTN capabilities, HARQ mode B cannot be enabled.
    return default_ul_harq_mode_mask;
  }

  if (ue_caps->ul_harq_mode_b_supported) {
    return cell_ul_harq_mode_mask;
  }
  return default_ul_harq_mode_mask;
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
