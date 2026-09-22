// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/config/cell_configuration.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
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
    pucch_res_mng.add_cell(to_du_cell_index(0), cell_cfg.params);
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
    const auto& sr_res_list = ue->ue_cell_cfg.serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
    ASSERT_FALSE(sr_res_list.empty());
    auto ue_sr_res_offset_pair = std::make_pair(sr_res_list[0].pucch_res_id.ded().cell_res_id, sr_res_list[0].offset);
    ASSERT_EQ(sr_offsets.count(ue_sr_res_offset_pair), 0);
    sr_offsets.insert(ue_sr_res_offset_pair);

    if (cell_cfg_dedicated.serv_cell_cfg.csi_meas_cfg.has_value()) {
      // Check that the CSI is configured and all UEs have different CSI offsets or PUCCH res id.
      const bool has_csi_cfg = ue->ue_cell_cfg.serv_cell_cfg.csi_meas_cfg.has_value() and
                               not ue->ue_cell_cfg.serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.empty();
      ASSERT_TRUE(has_csi_cfg);
      const auto& csi_res_cfg = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
          ue->ue_cell_cfg.serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.front().report_cfg_type);
      auto ue_csi_res_offset_pair = std::make_pair(
          csi_res_cfg.pucch_csi_res_list.front().pucch_res_id.ded().cell_res_id, csi_res_cfg.report_slot_offset);
      ASSERT_EQ(csi_offsets.count(ue_csi_res_offset_pair), 0);
      csi_offsets.insert(ue_csi_res_offset_pair);
    }

    const auto& ue_pucch_cfg = ue->ue_cell_cfg.serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();
    // Each UE should have 2 PUCCH resource sets configured
    ASSERT_EQ(ue_pucch_cfg.pucch_res_set.size(), 2);
    ASSERT_EQ(ue_pucch_cfg.pucch_res_set[0].resources.size(), pucch_params.res_set_size);
    ASSERT_EQ(ue_pucch_cfg.pucch_res_set[1].resources.size(), pucch_params.res_set_size);
    // Make sure UE has all PUCCH resources with different cell_res_id.
    {
      std::set<unsigned> pucch_res_idxs;
      for (unsigned n = 0; n != ue_pucch_cfg.pucch_res_set[0].resources.size(); ++n) {
        pucch_res_idxs.count(ue_pucch_cfg.pucch_res_list[n].res_id.ded().cell_res_id);
        pucch_res_idxs.insert(ue_pucch_cfg.pucch_res_list[n].res_id.ded().cell_res_id);
      }
    }
  }

  ASSERT_TRUE(true);
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
  const auto     cell_res_list = config_helpers::generate_cell_pucch_res_list(pucch_res_params, bwp_size_rbs);
  ASSERT_TRUE(std::any_of(cell_res_list.begin(), cell_res_list.end(), [](const pucch_resource& res) {
    return res.format() == pucch_format::FORMAT_1 and res.rep_factor != pucch_repetition_factor::n1;
  }));
  ASSERT_TRUE(std::any_of(cell_res_list.begin(), cell_res_list.end(), [](const pucch_resource& res) {
    return res.format() == pucch_format::FORMAT_2 and res.rep_factor != pucch_repetition_factor::n1;
  }));

  const du_cell_index_t rep_cell_idx = to_du_cell_index(1);
  pucch_res_mng.add_cell(rep_cell_idx, cell_params_rep);

  ue_cell_config ue_cfg = ocudu::config_helpers::make_default_ue_cell_config(cell_params_rep, rep_cell_idx);
  ASSERT_TRUE(pucch_res_mng.alloc_resources(ue_cfg));

  const auto& res_list = ue_cfg.serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->pucch_res_list;
  ASSERT_FALSE(res_list.empty());

  // Repetition must start disabled, as the UE capabilities are not yet known.
  for (const pucch_resource& res : res_list) {
    ASSERT_EQ(res.rep_factor, pucch_repetition_factor::n1);
  }

  const nr_band band = cell_params_rep.ul_carrier.band;

  // A UE that does not indicate support for dynamic PUCCH repetition must keep repetition disabled.
  ue_capability_summary no_rep_caps;
  pucch_res_mng.update_resources(ue_cfg, no_rep_caps);
  for (const pucch_resource& res : res_list) {
    ASSERT_EQ(res.rep_factor, pucch_repetition_factor::n1);
  }

  // A UE that indicates full support for dynamic PUCCH repetition must have it enabled for every format.
  ue_capability_summary full_rep_caps;
  full_rep_caps.pucch_repeat_f1_3_4_supported          = true;
  full_rep_caps.slot_based_dyn_pucch_rep_r17_supported = true;
  full_rep_caps.bands.emplace(band, ue_capability_summary::supported_band{.pucch_repeat_f0_2_r17_supported = true});
  pucch_res_mng.update_resources(ue_cfg, full_rep_caps);
  for (const pucch_resource& res : res_list) {
    const auto cell_res_it = std::find_if(
        cell_res_list.begin(), cell_res_list.end(), [&](const pucch_resource& r) { return r.res_id == res.res_id; });
    ASSERT_NE(cell_res_it, cell_res_list.end());
    ASSERT_EQ(res.rep_factor, cell_res_it->rep_factor);
  }

  // A UE that supports repetition for F1/3/4, but not for F0/2, must keep repetition disabled for F0/2.
  ue_capability_summary f1_3_4_only_caps;
  f1_3_4_only_caps.pucch_repeat_f1_3_4_supported          = true;
  f1_3_4_only_caps.slot_based_dyn_pucch_rep_r17_supported = true;
  pucch_res_mng.update_resources(ue_cfg, f1_3_4_only_caps);
  for (const pucch_resource& res : res_list) {
    const bool is_f0_or_f2 = res.format() == pucch_format::FORMAT_0 or res.format() == pucch_format::FORMAT_2;
    if (is_f0_or_f2) {
      // Repetition for F0/2 is not supported by this UE, so it must remain disabled.
      ASSERT_EQ(res.rep_factor, pucch_repetition_factor::n1);
    } else {
      const auto cell_res_it = std::find_if(
          cell_res_list.begin(), cell_res_list.end(), [&](const pucch_resource& r) { return r.res_id == res.res_id; });
      ASSERT_NE(cell_res_it, cell_res_list.end());
      ASSERT_EQ(res.rep_factor, cell_res_it->rep_factor);
    }
  }

  // A UE that supports repetition, but not the dynamic indication, must keep repetition disabled.
  ue_capability_summary format_only_caps;
  format_only_caps.pucch_repeat_f1_3_4_supported = true;
  format_only_caps.bands.emplace(band, ue_capability_summary::supported_band{.pucch_repeat_f0_2_r17_supported = true});
  pucch_res_mng.update_resources(ue_cfg, format_only_caps);
  for (const pucch_resource& res : res_list) {
    ASSERT_EQ(res.rep_factor, pucch_repetition_factor::n1);
  }
}
