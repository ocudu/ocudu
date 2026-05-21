// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "lib/scheduler/config/cell_configuration.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include <gtest/gtest.h>

using namespace ocudu;

class sched_pucch_res_builder_tester : public ::testing::Test
{
protected:
  sched_pucch_res_builder_tester() :
    cfg_mng{config_helpers::make_default_scheduler_expert_config()},
    cell_cfg(*cfg_mng.add_cell(sched_config_helper::make_default_sched_cell_configuration_request())),
    cell_cfg_dedicated(ocudu::config_helpers::make_default_ue_cell_config(cell_cfg.params))
  {
    pucch_builder.setup(cell_cfg.params);
  }

  struct ue_info {
    unsigned       ue_idx;
    ue_cell_config ue_cell_cfg;
  };

  const ue_info* add_ue()
  {
    ues.push_back(ue_info{ue_cnt++, ocudu::config_helpers::make_default_ue_cell_config(cell_cfg.params)});
    pucch_builder.add_build_new_ue_pucch_cfg(ues.back().ue_cell_cfg);
    return &ues.back();
  }

  test_helpers::test_sched_config_manager cfg_mng;
  const cell_configuration&               cell_cfg;
  ue_cell_config                          cell_cfg_dedicated;
  pucch_resource_builder_params           pucch_params;
  std::vector<ue_info>                    ues;
  unsigned                                ue_cnt = 0;
  pucch_res_builder_test_helper           pucch_builder;
};

TEST_F(sched_pucch_res_builder_tester, when_ues_are_added_their_cfg_have_different_csi_and_sr)
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
