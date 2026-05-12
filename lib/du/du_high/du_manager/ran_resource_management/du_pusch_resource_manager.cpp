// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_pusch_resource_manager.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/pusch/tx_scheme_configuration.h"
#include "ocudu/support/compiler.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;
using namespace odu;

static void set_pusch_mcs_table(serving_cell_config& cell_cfg, pusch_mcs_table mcs_table)
{
  if (cell_cfg.ul_config.has_value() and cell_cfg.ul_config->init_ul_bwp.pusch_cfg.has_value()) {
    cell_cfg.ul_config->init_ul_bwp.pusch_cfg->mcs_table = mcs_table;
  }
}

static void set_ul_mimo(ue_cell_config&           cell_cfg,
                        unsigned                  max_rank,
                        unsigned                  nof_srs_ports,
                        tx_scheme_codebook_subset codebook_subset)
{
  if (OCUDU_UNLIKELY(!cell_cfg.serv_cell_cfg.ul_config.has_value() ||
                     !cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.pusch_cfg ||
                     !cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg)) {
    return;
  }

  cell_cfg.init_bwp().ul.pusch.tx_cfg = tx_scheme_codebook{.max_rank = max_rank, .codebook_subset = codebook_subset};
  cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.pusch_cfg->tx_cfg =
      tx_scheme_codebook{.max_rank = max_rank, .codebook_subset = codebook_subset};

  cell_cfg.init_bwp().ul.srs.nof_ports = static_cast<srs_config::srs_resource::nof_srs_ports>(nof_srs_ports);
  for (auto& srs_res : cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list) {
    srs_res.nof_ports = static_cast<srs_config::srs_resource::nof_srs_ports>(nof_srs_ports);
  }
}

static void set_max_ul_nof_harqs(serving_cell_config& cell_cfg, unsigned nof_harqs)
{
  if (cell_cfg.ul_config.has_value() and cell_cfg.ul_config->pusch_serv_cell_cfg.has_value()) {
    cell_cfg.ul_config->pusch_serv_cell_cfg->nof_harq_proc =
        static_cast<pusch_serving_cell_config::nof_harq_proc_for_pusch>(nof_harqs);
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

static void set_ul_harq_mode(serving_cell_config& cell_cfg, const harq_ul_mode_mask& ul_harq_mode_mask_)
{
  if (cell_cfg.ul_config.has_value() and cell_cfg.ul_config->pusch_serv_cell_cfg.has_value()) {
    cell_cfg.ul_config->pusch_serv_cell_cfg->ul_harq_mode = ul_harq_mode_mask_;
  }
}

du_pusch_resource_manager::du_pusch_resource_manager(span<const du_cell_config> cell_cfg_list_,
                                                     const du_test_mode_config& test_cfg_) :
  cell_cfg_list(cell_cfg_list_), test_cfg(test_cfg_)
{
}

void du_pusch_resource_manager::alloc_resources(cell_group_config& cell_grp_cfg)
{
  apply_config(cell_grp_cfg, std::nullopt);
}

void du_pusch_resource_manager::update_resources(cell_group_config& cell_grp_cfg, const ue_capability_summary& ue_caps)
{
  apply_config(cell_grp_cfg, ue_caps);
}

void du_pusch_resource_manager::dealloc_resources(cell_group_config& /*cell_grp_cfg*/)
{
  // No pool resources to release.
}

void du_pusch_resource_manager::apply_config(cell_group_config&                          cell_grp_cfg,
                                             const std::optional<ue_capability_summary>& ue_caps)
{
  ue_cell_config&       pcell_cfg = cell_grp_cfg.cells.at(SERVING_PCELL_IDX);
  const du_cell_index_t cell_idx  = pcell_cfg.serv_cell_cfg.cell_index;

  set_pusch_mcs_table(pcell_cfg.serv_cell_cfg, select_mcs_table(cell_idx, ue_caps));
  set_ul_mimo(pcell_cfg,
              select_max_rank(cell_idx, ue_caps),
              select_srs_nof_ports(cell_idx, ue_caps),
              select_tx_codebook_subset(cell_idx, ue_caps));
  set_max_ul_nof_harqs(pcell_cfg.serv_cell_cfg, select_max_nof_harqs(cell_idx, ue_caps));
  set_ul_dci_harq_num_field_size(pcell_cfg.serv_cell_cfg, select_dci_harq_field_size(cell_idx, ue_caps));
  set_ul_harq_mode(pcell_cfg.serv_cell_cfg, select_harq_mode(cell_idx, ue_caps));
}

pusch_mcs_table du_pusch_resource_manager::select_mcs_table(du_cell_index_t                             cell_idx,
                                                            const std::optional<ue_capability_summary>& ue_caps) const
{
  nr_band               band          = cell_cfg_list[cell_idx].ran.ul_carrier.band;
  const pusch_mcs_table app_mcs_table = cell_cfg_list[cell_idx].ran.init_bwp.pusch.mcs_table;

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    return app_mcs_table;
  }
  if (not ue_caps.has_value()) {
    return pusch_mcs_table::qam64;
  }

  if (app_mcs_table == pusch_mcs_table::qam256) {
    if (ue_caps->bands.count(band) != 0) {
      return ue_caps->bands.at(band).pusch_qam256_supported ? pusch_mcs_table::qam256 : pusch_mcs_table::qam64;
    }
    if (std::none_of(ue_caps->bands.begin(), ue_caps->bands.end(), [](const auto& b) {
          return b.second.pusch_qam256_supported;
        })) {
      return pusch_mcs_table::qam64;
    }
  } else if (app_mcs_table == pusch_mcs_table::qam64LowSe) {
    return ue_caps->pusch_qam64lowse_supported ? pusch_mcs_table::qam64LowSe : pusch_mcs_table::qam64;
  }

  return app_mcs_table;
}

tx_scheme_codebook_subset
du_pusch_resource_manager::select_tx_codebook_subset(du_cell_index_t                             cell_idx,
                                                     const std::optional<ue_capability_summary>& ue_caps) const
{
  nr_band band = cell_cfg_list[cell_idx].ran.ul_carrier.band;

  if (not ue_caps.has_value() || ue_caps->bands.count(band) == 0) {
    return ue_capability_summary::default_pusch_tx_coherence;
  }

  return ue_caps->bands.at(band).pusch_tx_coherence;
}

unsigned du_pusch_resource_manager::select_srs_nof_ports(du_cell_index_t                             cell_idx,
                                                         const std::optional<ue_capability_summary>& ue_caps) const
{
  nr_band band = cell_cfg_list[cell_idx].ran.ul_carrier.band;

  if (not ue_caps.has_value() || ue_caps->bands.count(band) == 0) {
    return ue_capability_summary::default_nof_srs_tx_ports;
  }

  return ue_caps->bands.at(band).nof_srs_tx_ports;
}

unsigned du_pusch_resource_manager::select_max_rank(du_cell_index_t                             cell_idx,
                                                    const std::optional<ue_capability_summary>& ue_caps) const
{
  nr_band  band           = cell_cfg_list[cell_idx].ran.ul_carrier.band;
  unsigned pusch_max_rank = cell_cfg_list[cell_idx].ran.init_bwp.pusch.max_nof_layers;

  if (not ue_caps.has_value() || ue_caps->bands.count(band) == 0) {
    return std::min(pusch_max_rank, ue_capability_summary::default_pusch_max_rank);
  }

  return std::min(pusch_max_rank, ue_caps->bands.at(band).pusch_max_rank);
}

unsigned du_pusch_resource_manager::select_max_nof_harqs(du_cell_index_t                             cell_idx,
                                                         const std::optional<ue_capability_summary>& ue_caps) const
{
  unsigned cell_max = cell_cfg_list[cell_idx].ran.init_bwp.pusch.max_harq_procs;

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    return cell_max;
  }

  nr_band band = cell_cfg_list[cell_idx].ran.ul_carrier.band;

  if (not ue_caps.has_value()) {
    return std::min(cell_max, ue_capability_summary::default_max_harq_process_num);
  }

  if (not band_helper::is_ntn_band(band) || ue_caps->bands.count(band) == 0 || not ue_caps->ntn_supported) {
    return std::min(cell_max, ue_capability_summary::default_max_harq_process_num);
  }

  return std::min(cell_max, (unsigned)ue_caps->bands.at(band).max_ul_harq_process_num);
}

unsigned
du_pusch_resource_manager::select_dci_harq_field_size(du_cell_index_t                             cell_idx,
                                                      const std::optional<ue_capability_summary>& ue_caps) const
{
  auto default_size = log2_ceil(ue_capability_summary::default_max_harq_process_num);

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    return default_size;
  }

  unsigned cell_max  = cell_cfg_list[cell_idx].ran.init_bwp.pusch.max_harq_procs;
  auto     cell_size = log2_ceil(cell_max);
  nr_band  band      = cell_cfg_list[cell_idx].ran.ul_carrier.band;

  if (not band_helper::is_ntn_band(band) || not ue_caps.has_value() || ue_caps->bands.count(band) == 0 ||
      not ue_caps->ntn_supported) {
    return default_size;
  }

  auto band_size = log2_ceil((unsigned)ue_caps->bands.at(band).max_ul_harq_process_num);
  return std::max(std::min(cell_size, band_size), default_size);
}

harq_ul_mode_mask du_pusch_resource_manager::select_harq_mode(du_cell_index_t                             cell_idx,
                                                              const std::optional<ue_capability_summary>& ue_caps) const
{
  harq_ul_mode_mask default_mask(MAX_NOF_HARQS);
  default_mask.fill(true);

  harq_ul_mode_mask cell_mask = cell_cfg_list[cell_idx].ran.init_bwp.pusch.ul_harq_mode;

  if (test_cfg.test_ue.has_value() and test_cfg.test_ue->rnti != rnti_t::INVALID_RNTI) {
    return cell_mask;
  }

  if (not ue_caps.has_value() || not ue_caps->ntn_supported) {
    return default_mask;
  }

  return ue_caps->ul_harq_mode_b_supported ? cell_mask : default_mask;
}
