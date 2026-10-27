// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/config/cell_configuration.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/config/csi_helper.h"
#include "ocudu/scheduler/config/pucch_default_resource.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/config/serving_cell_config_builder.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/rrm/pucch_resource_manager.h"
#include "ocudu/scheduler/rrm/ue_capability_summary.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include <gtest/gtest.h>

using namespace ocudu;

class pucch_resource_manager_tester : public ::testing::Test
{
protected:
  pucch_resource_manager_tester() :
    cfg_mng{config_helpers::make_default_scheduler_expert_config()},
    cell_cfg(*cfg_mng.add_cell(sched_config_helper::make_default_sched_cell_configuration_request())),
    cell_cfg_dedicated(ocudu::config_helpers::make_default_ue_cell_config(cell_cfg.params))
  {
    cell_rrm = make_cell_rrm_config(cell_cfg.params);
    pucch_res_mng.add_cell(to_du_cell_index(0), cell_cfg.params, cell_rrm);
  }

  struct ue_info {
    unsigned       ue_idx;
    ue_cell_config ue_cell_cfg;
  };

  const ue_info* add_ue()
  {
    ues.push_back(ue_info{ue_cnt++, ocudu::config_helpers::make_default_ue_cell_config(cell_cfg.params)});
    pucch_res_mng.alloc_resources(ues.back().ue_cell_cfg);
    return &ues.back();
  }

  test_helpers::test_sched_config_manager cfg_mng;
  const cell_configuration&               cell_cfg;
  ue_cell_config                          cell_cfg_dedicated;
  pucch_resource_builder_params           pucch_params;
  std::vector<ue_info>                    ues;
  unsigned                                ue_cnt = 0;
  cell_rrm_config                         cell_rrm;
  pucch_resource_manager                  pucch_res_mng{64};
};

TEST_F(pucch_resource_manager_tester, when_ues_are_added_their_cfg_have_different_csi_and_sr)
{
  std::set<std::pair<unsigned, unsigned>> sr_offsets;
  std::set<std::pair<unsigned, unsigned>> csi_offsets;
  const unsigned                          nof_ues = 20;
  for (unsigned i = 0; i != nof_ues; ++i) {
    const ue_info* ue = add_ue();

    ASSERT_NE(ue, nullptr);

    // Check that the SR is configured and all UEs have different SR offsets or PUCCH res id.
    const ue_rrm_config& ue_rrm_cfg = ue->ue_cell_cfg.rrm;
    auto ue_sr_res_offset_pair      = std::make_pair(ue_rrm_cfg.pucch.sr_res_id.value(), ue_rrm_cfg.pucch.sr_offset);
    ASSERT_EQ(sr_offsets.count(ue_sr_res_offset_pair), 0);
    sr_offsets.insert(ue_sr_res_offset_pair);

    if (cell_cfg_dedicated.serv_cell_cfg.csi_meas_cfg.has_value()) {
      // Check that the CSI is configured and all UEs have different CSI offsets or PUCCH res id.
      const bool has_csi_cfg = ue->ue_cell_cfg.serv_cell_cfg.csi_meas_cfg.has_value() and
                               not ue->ue_cell_cfg.serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.empty();
      ASSERT_TRUE(has_csi_cfg);
      const auto& csi_res_cfg = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
          ue->ue_cell_cfg.serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.front().report_cfg_type);
      ASSERT_TRUE(ue_rrm_cfg.csi.periodic_report.has_value());
      auto ue_csi_res_offset_pair =
          std::make_pair(ue_rrm_cfg.csi.periodic_report->res_id.value(), csi_res_cfg.report_slot_offset);
      ASSERT_EQ(csi_offsets.count(ue_csi_res_offset_pair), 0);
      csi_offsets.insert(ue_csi_res_offset_pair);
    }

    // Each UE should have 2 PUCCH resource sets configured, with all PUCCH resources having different ded_id.
    const auto layout = config_helpers::make_ue_pucch_res_layout(cell_rrm.pucch.res, ue_rrm_cfg);
    ASSERT_EQ(layout.res_set_0.size(), pucch_params.res_set_size);
    ASSERT_EQ(layout.res_set_1.size(), pucch_params.res_set_size);
    std::set<unsigned> ded_ids;
    for (pucch_res_id_t res_id : layout.res_list) {
      ded_ids.insert(res_id.ded_id());
    }
    ASSERT_EQ(ded_ids.size(), layout.res_list.size());
  }

  ASSERT_TRUE(true);
}

TEST_F(pucch_resource_manager_tester, when_allocation_fails_then_ue_cell_config_is_not_changed)
{
  const ue_cell_config default_cfg = ocudu::config_helpers::make_default_ue_cell_config(cell_cfg.params);

  // Allocate UEs until the cell has no PUCCH resources left.
  ue_cell_config ue_cfg    = default_cfg;
  bool           allocated = true;
  for (unsigned i = 0; i != MAX_NOF_DU_UES and allocated; ++i) {
    ue_cfg    = default_cfg;
    allocated = pucch_res_mng.alloc_resources(ue_cfg);
  }
  ASSERT_FALSE(allocated);

  ASSERT_EQ(ue_cfg.serv_cell_cfg.csi_meas_cfg, default_cfg.serv_cell_cfg.csi_meas_cfg);
  ASSERT_EQ(ue_cfg.rrm, default_cfg.rrm);
}

TEST_F(pucch_resource_manager_tester, deallocated_resources_return_to_the_pool)
{
  const ue_info* ue = add_ue();

  // The resources of the UE return to the pool when the UE is deallocated.
  const unsigned nof_free_sr_cfgs = pucch_res_mng.get_nof_free_sr_configs(to_du_cell_index(0));
  pucch_res_mng.dealloc_resources(ue->ue_cell_cfg);
  ASSERT_EQ(pucch_res_mng.get_nof_free_sr_configs(to_du_cell_index(0)), nof_free_sr_cfgs + 1);
}

TEST_F(pucch_resource_manager_tester, sr_and_csi_in_different_symbols_are_allocated_if_their_offsets_never_collide)
{
  ran_cell_config cell_params_f0          = cell_cfg.params;
  auto&           pucch_res_params        = cell_params_f0.init_bwp.pucch.resources;
  pucch_res_params.f0_or_f1_params        = pucch_f0_params{};
  pucch_res_params.f2_or_f3_or_f4_params  = pucch_f2_params{.nof_syms = 1U, .max_nof_rbs = 4U};
  pucch_res_params.nof_cell_sr_resources  = 1;
  pucch_res_params.nof_cell_csi_resources = 1;

  // Sanity check: the SR and the CSI resources do not overlap in symbols.
  const unsigned bwp_size_rbs  = cell_params_f0.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  const auto     cell_res_list = config_helpers::generate_cell_pucch_res_list(
      pucch_res_params,
      bwp_size_rbs,
      get_pucch_default_nof_edge_prbs(
          cell_params_f0.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common, bwp_size_rbs));
  const auto find_res = [&](pucch_res_id_t res_id) {
    return std::find_if(
        cell_res_list.begin(), cell_res_list.end(), [&](const pucch_resource& r) { return r.res_id == res_id; });
  };
  const auto sr_res_it  = find_res(pucch_res_params.sr_res_id(pucch_sr_resource_id(0)));
  const auto csi_res_it = find_res(pucch_res_params.csi_res_id(pucch_csi_resource_id(0)));
  ASSERT_NE(sr_res_it, cell_res_list.end());
  ASSERT_NE(csi_res_it, cell_res_list.end());
  ASSERT_FALSE(sr_res_it->syms.overlaps(csi_res_it->syms));

  const du_cell_index_t f0_cell_idx = to_du_cell_index(1);
  const cell_rrm_config f0_cell_rrm = make_cell_rrm_config(cell_params_f0);
  pucch_res_mng.add_cell(f0_cell_idx, cell_params_f0, f0_cell_rrm);

  ue_cell_config ue_cfg = ocudu::config_helpers::make_default_ue_cell_config(cell_params_f0, f0_cell_idx);
  ASSERT_TRUE(pucch_res_mng.alloc_resources(ue_cfg));

  ASSERT_TRUE(ue_cfg.rrm.csi.periodic_report.has_value());
  ASSERT_FALSE(csi_helper::are_sr_and_csi_pucchs_scheduled_together(
      sr_periodicity_to_slot(cell_params_f0.init_bwp.pucch.sr_period),
      ue_cfg.rrm.pucch.sr_offset,
      to_underlying(cell_params_f0.init_bwp.csi->csi_rs_period),
      ue_cfg.rrm.csi.periodic_report->offset));
}

TEST_F(pucch_resource_manager_tester, sr_and_csi_do_not_overlap_the_srs_in_the_special_slot)
{
  // DDDDDDSUUU. The SRS uses all 4 UL symbols of the special slot.
  cell_config_builder_params builder_params = cell_config_builder_profiles::create(duplex_mode::TDD);
  builder_params.tdd_ul_dl_cfg_common       = tdd_ul_dl_config_common{builder_params.scs_common, {10, 6, 8, 3, 4}};
  const unsigned  special_slot_idx          = 6;
  ran_cell_config cell_params = sched_config_helper::make_default_sched_cell_configuration_request(builder_params).ran;
  cell_params.init_bwp.srs_cfg.srs_type_enabled = srs_type::periodic;

  // 2-symbol PUCCH resources in symbols 0 to 11. The resources in symbols 10 and 11 fit the special slot.
  auto& pucch_res                  = cell_params.init_bwp.pucch.resources;
  pucch_res.f0_or_f1_params        = pucch_f0_params{};
  pucch_res.f2_or_f3_or_f4_params  = pucch_f2_params{.max_nof_rbs = 4U};
  pucch_res.max_nof_symbols        = 12U;
  pucch_res.nof_cell_sr_resources  = 12;
  pucch_res.nof_cell_csi_resources = 12;

  const cell_rrm_config srs_cell_rrm = make_cell_rrm_config(cell_params);
  pucch_res_mng.add_cell(to_du_cell_index(1), cell_params, srs_cell_rrm);

  // Fill the cell with UEs. No SR or CSI goes in the special slot.
  for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
    ue_cell_config ue_cfg = config_helpers::make_default_ue_cell_config(cell_params, to_du_cell_index(1));
    if (not pucch_res_mng.alloc_resources(ue_cfg)) {
      ASSERT_GT(i, 0);
      break;
    }
    ASSERT_NE(ue_cfg.rrm.pucch.sr_offset % 10, special_slot_idx);
    ASSERT_NE(ue_cfg.rrm.csi.periodic_report.value().offset % 10, special_slot_idx);
  }
}

TEST_F(pucch_resource_manager_tester, repetition_disabled_until_capabilities_confirm_support)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  // Configure PUCCH HARQ-ACK repetition at cell level, so that the HARQ-ACK resources are generated with a repetition
  // factor greater than n1.
  ran_cell_config cell_params_rep  = cell_cfg.params;
  auto&           pucch_res_params = cell_params_rep.init_bwp.pucch.resources;
  pucch_res_params.harq_ack_rep =
      pucch_harq_ack_rep_params{.sinr_thresholds = {-5.0F, 0.0F},
                                .factors_per_res = std::vector<pucch_repetition_factor>(
                                    pucch_res_params.res_set_size.value(), pucch_repetition_factor::n4)};

  // Sanity check: with this configuration, the cell PUCCH resources of both F1 (Resource Set 0) and F2 (Resource Set 1)
  // do have a repetition factor other than n1.
  const unsigned bwp_size_rbs  = cell_params_rep.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  const auto     cell_res_list = config_helpers::generate_cell_pucch_res_list(
      pucch_res_params,
      bwp_size_rbs,
      get_pucch_default_nof_edge_prbs(
          cell_params_rep.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value().pucch_resource_common, bwp_size_rbs));
  ASSERT_TRUE(std::any_of(cell_res_list.begin(), cell_res_list.end(), [](const pucch_resource& res) {
    return res.format() == pucch_format::FORMAT_1 and res.rep_factor != pucch_repetition_factor::n1;
  }));
  ASSERT_TRUE(std::any_of(cell_res_list.begin(), cell_res_list.end(), [](const pucch_resource& res) {
    return res.format() == pucch_format::FORMAT_2 and res.rep_factor != pucch_repetition_factor::n1;
  }));

  const du_cell_index_t rep_cell_idx = to_du_cell_index(1);
  const cell_rrm_config rep_cell_rrm = make_cell_rrm_config(cell_params_rep);
  pucch_res_mng.add_cell(rep_cell_idx, cell_params_rep, rep_cell_rrm);

  ue_cell_config ue_cfg = ocudu::config_helpers::make_default_ue_cell_config(cell_params_rep, rep_cell_idx);
  ASSERT_TRUE(pucch_res_mng.alloc_resources(ue_cfg));

  // Repetition must start disabled, as the UE capabilities are not yet known.
  ASSERT_FALSE(ue_cfg.rrm.pucch.rep_f1_3_4_enabled);
  ASSERT_FALSE(ue_cfg.rrm.pucch.rep_f0_2_enabled);

  const nr_band band = cell_params_rep.ul_carrier.band;

  // A UE that does not indicate support for dynamic PUCCH repetition must keep repetition disabled.
  ue_capability_summary no_rep_caps;
  pucch_res_mng.update_resources(ue_cfg, no_rep_caps);
  ASSERT_FALSE(ue_cfg.rrm.pucch.rep_f1_3_4_enabled);
  ASSERT_FALSE(ue_cfg.rrm.pucch.rep_f0_2_enabled);

  // A UE that indicates full support for dynamic PUCCH repetition must have it enabled for every format.
  ue_capability_summary full_rep_caps;
  full_rep_caps.pucch_repeat_f1_3_4_supported          = true;
  full_rep_caps.slot_based_dyn_pucch_rep_r17_supported = true;
  full_rep_caps.bands.emplace(band, ue_capability_summary::supported_band{.pucch_repeat_f0_2_r17_supported = true});
  pucch_res_mng.update_resources(ue_cfg, full_rep_caps);
  ASSERT_TRUE(ue_cfg.rrm.pucch.rep_f1_3_4_enabled);
  ASSERT_TRUE(ue_cfg.rrm.pucch.rep_f0_2_enabled);

  // A UE that supports repetition for F1/3/4, but not for F0/2, must keep repetition disabled for F0/2.
  ue_capability_summary f1_3_4_only_caps;
  f1_3_4_only_caps.pucch_repeat_f1_3_4_supported          = true;
  f1_3_4_only_caps.slot_based_dyn_pucch_rep_r17_supported = true;
  pucch_res_mng.update_resources(ue_cfg, f1_3_4_only_caps);
  ASSERT_TRUE(ue_cfg.rrm.pucch.rep_f1_3_4_enabled);
  ASSERT_FALSE(ue_cfg.rrm.pucch.rep_f0_2_enabled);

  // A UE that supports repetition, but not the dynamic indication, must keep repetition disabled.
  ue_capability_summary format_only_caps;
  format_only_caps.pucch_repeat_f1_3_4_supported = true;
  format_only_caps.bands.emplace(band, ue_capability_summary::supported_band{.pucch_repeat_f0_2_r17_supported = true});
  pucch_res_mng.update_resources(ue_cfg, format_only_caps);
  ASSERT_FALSE(ue_cfg.rrm.pucch.rep_f1_3_4_enabled);
  ASSERT_FALSE(ue_cfg.rrm.pucch.rep_f0_2_enabled);
}
