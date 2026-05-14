// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/ue_context/ue_repository.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/scheduler/config/logical_channel_config_factory.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

class ue_configuration_test : public ::testing::Test
{
protected:
  scheduler_expert_config                  sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cell_configuration_request_message msg = sched_config_helper::make_default_sched_cell_configuration_request();
  sched_ue_creation_request_message        ue_create_msg =
      sched_config_helper::create_default_sched_ue_creation_request(msg.ran);

  const cell_configuration& add_cell()
  {
    cell_configuration& cell_cfg = cfg_pool.add_cell(sched_cfg, msg);
    cell_cfg_db.emplace(msg.cell_index, &cell_cfg);
    return cell_cfg;
  }

  du_cell_group_config_pool                      cfg_pool;
  cell_common_configuration_list                 cell_cfg_db;
  std::vector<std::unique_ptr<ue_configuration>> ue_cfg_pool;
  ue_repository                                  ue_db{sched_cfg.ue};

  ue& add_ue(const sched_ue_creation_request_message& ue_req)
  {
    ue_cfg_pool.push_back(
        std::make_unique<ue_configuration>(ue_req.ue_index, ue_req.crnti, cell_cfg_db, cfg_pool.add_ue(ue_req)));
    ue_db.add_ue(*ue_cfg_pool.back(), ue_req.starts_in_fallback, ue_req.ul_ccch_slot_rx, ue_req.cfra_enabled);
    return ue_db[ue_req.ue_index];
  }
};

TEST_F(ue_configuration_test, configuration_valid_on_creation)
{
  const cell_configuration& cell_cfg = add_cell();
  ue_cell_configuration ue_cfg{to_rnti(0x4601), cell_cfg, cfg_pool.add_ue(ue_create_msg).cells[cell_cfg.cell_index]};

  // Test Common Config.
  ASSERT_TRUE(ue_cfg.find_bwp(to_bwp_id(0)) != nullptr);
  ASSERT_TRUE(ue_cfg.bwp(to_bwp_id(0)).dl.cfg() == cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params);
  ASSERT_TRUE(ue_cfg.coreset(to_coreset_id(0)).id() ==
              cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->get_id());
  ASSERT_EQ(0, fmt::underlying(ue_cfg.search_space(to_search_space_id(0)).cfg->get_id()));
  ASSERT_TRUE(*ue_cfg.search_space(to_search_space_id(0)).cfg ==
              cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0]);
  ASSERT_EQ(1, fmt::underlying(ue_cfg.search_space(to_search_space_id(1)).cfg->get_id()));
  ASSERT_TRUE(*ue_cfg.search_space(to_search_space_id(1)).cfg ==
              cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1]);

  // Test Dedicated Config.
  ASSERT_TRUE(ue_cfg.find_coreset(to_coreset_id(2)) == nullptr);
  ASSERT_EQ(2, fmt::underlying(ue_cfg.search_space(to_search_space_id(2)).cfg->get_id()));
  ASSERT_TRUE(*ue_cfg.search_space(to_search_space_id(2)).cfg ==
              (*ue_create_msg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]);
  ASSERT_TRUE(ue_cfg.find_search_space(to_search_space_id(3)) == nullptr);
}

TEST_F(ue_configuration_test, configuration_valid_on_reconfiguration)
{
  const cell_configuration& cell_cfg = cfg_pool.add_cell(sched_cfg, msg);
  ue_cell_configuration ue_cfg{to_rnti(0x4601), cell_cfg, cfg_pool.add_ue(ue_create_msg).cells[to_du_cell_index(0)]};
  ASSERT_EQ(ue_cfg.init_bwp().dl.pdsch().ded()->mcs_table, pdsch_mcs_table::qam256);

  sched_ue_reconfiguration_message recfg_req;
  recfg_req.ue_index = ue_create_msg.ue_index;
  recfg_req.crnti    = ue_create_msg.crnti;
  recfg_req.cfg.cells.emplace();
  recfg_req.cfg.cells.value().push_back(ue_create_msg.cfg.cells->at(0));
  recfg_req.cfg.cells.value()[0].bwps             = ue_create_msg.cfg.cells->at(0).bwps;
  serving_cell_config& ue_cell_reconf             = recfg_req.cfg.cells.value()[0].serv_cell_cfg;
  ue_cell_reconf.init_dl_bwp.pdsch_cfg->mcs_table = pdsch_mcs_table::qam64;
  ue_cfg.reconfigure(cfg_pool.reconf_ue(recfg_req).cells[to_du_cell_index(0)]);

  ASSERT_EQ(ue_cfg.init_bwp().dl.pdsch().ded()->mcs_table, pdsch_mcs_table::qam64);
}

TEST_F(ue_configuration_test, when_reconfiguration_is_received_then_ue_updates_logical_channel_states)
{
  // Test Preamble.
  const cell_configuration& cell_cfg = add_cell();
  ue_db.add_cell(cell_cfg, nullptr);
  auto& u = add_ue(ue_create_msg);

  // Pass Reconfiguration to UE with an new Logical Channel.
  sched_ue_reconfiguration_message recfg{};
  recfg.ue_index = ue_create_msg.ue_index;
  recfg.crnti    = ue_create_msg.crnti;
  recfg.cfg      = ue_create_msg.cfg;
  recfg.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(uint_to_lcid(4)));
  ue_configuration ue_ded_cfg2{*u.ue_cfg_dedicated()};
  ue_ded_cfg2.update(cell_cfg_db, cfg_pool.reconf_ue(recfg));
  ue_db.reconfigure_ue(ue_ded_cfg2, sched_ue_config_request::causes::other_rrc_proc);

  // Confirm that the UE is in fallback.
  ASSERT_TRUE(u.get_pcell().is_in_fallback_mode());
  ASSERT_TRUE(u.get_pcell().get_pcell_state().config_st == ue_config_state::pending_reconf);

  // While in fallback, DL buffer status that are not for SRB0/SRB1, do not get represented in pending bytes.
  ASSERT_FALSE(u.logical_channels().has_dl_pending_bytes());
  for (const auto& lc : *recfg.cfg.lc_config_list) {
    u.handle_dl_buffer_state_indication(lc.lcid, 10);
    if (lc.lcid <= LCID_SRB1) {
      ASSERT_TRUE(u.logical_channels().has_dl_pending_bytes());
    } else {
      ASSERT_FALSE(u.logical_channels().has_dl_pending_bytes());
    }
    u.handle_dl_buffer_state_indication(lc.lcid, 0);
  }

  // Confirm that UE config applied config.
  ue_db.ue_config_applied(ue_create_msg.ue_index);

  // Verify that DL buffer state indications affect newly active logical channels.
  for (const auto& lc : *recfg.cfg.lc_config_list) {
    if (lc.lcid == uint_to_lcid(0)) {
      // LCID0 is a special case.
      continue;
    }
    u.handle_dl_buffer_state_indication(lc.lcid, 10);
    ASSERT_TRUE(u.logical_channels().has_dl_pending_bytes());
    u.handle_dl_buffer_state_indication(lc.lcid, 0);
    ASSERT_FALSE(u.logical_channels().has_dl_pending_bytes());
  }

  // Verify that inactive logical channels do not affect pending bytes.
  u.handle_dl_buffer_state_indication(uint_to_lcid(6), 10);
  ASSERT_FALSE(u.logical_channels().has_dl_pending_bytes());
}

TEST_F(ue_configuration_test, search_spaces_pdcch_candidate_lists_does_not_surpass_limit)
{
  cell_config_builder_params params{};
  params.scs_common                      = subcarrier_spacing::kHz30;
  params.dl_carrier.arfcn_f_ref          = 520002;
  params.dl_carrier.band                 = nr_band::n41;
  params.dl_carrier.carrier_bw           = bs_channel_bandwidth::MHz50;
  msg                                    = sched_config_helper::make_default_sched_cell_configuration_request(params);
  auto&                        pdcch_cfg = *msg.ran.init_bwp.pdcch_cfg;
  const coreset_configuration& cset_cfg  = pdcch_cfg.coresets[0];
  search_space_configuration&  ss_cfg    = pdcch_cfg.search_spaces[0];
  ss_cfg.set_non_ss0_nof_candidates({config_helpers::compute_max_nof_candidates(aggregation_level::n1, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n2, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n4, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n8, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n16, cset_cfg)});
  ue_create_msg = sched_config_helper::create_default_sched_ue_creation_request(msg.ran);

  const cell_configuration& cell_cfg = add_cell();
  rnti_t crnti = to_rnti(test_rng::uniform_int<uint16_t>(to_value(rnti_t::MIN_CRNTI), to_value(rnti_t::MAX_CRNTI)));
  ue_cell_configuration ue_cfg{crnti, cell_cfg, cfg_pool.add_ue(ue_create_msg).cells[cell_cfg.cell_index]};

  const sched_bwp_config& bwp            = ue_cfg.bwp(to_bwp_id(0));
  const unsigned          max_candidates = max_nof_monitored_pdcch_candidates(bwp.dl.cfg().scs);

  unsigned       sfn = test_rng::uniform_int<unsigned>(0, 1023);
  const unsigned slots_to_test =
      msg.ran.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0].get_monitoring_slot_periodicity();
  slot_point start_slot{params.scs_common, sfn, 0}, end_slot = start_slot + slots_to_test;
  for (slot_point pdcch_slot = start_slot; pdcch_slot != end_slot; ++pdcch_slot) {
    unsigned pdcch_candidates_count = 0;
    for (unsigned l = 0; l != NOF_AGGREGATION_LEVELS; ++l) {
      const aggregation_level aggr_lvl = aggregation_index_to_level(l);

      for (const sched_search_space_config* ss : bwp.dl.pdcch().search_spaces()) {
        ASSERT_GE(ss->cfg().get_nof_candidates()[l],
                  ue_cfg.search_space(ss->id()).get_pdcch_candidates(aggr_lvl, pdcch_slot).size())
            << "The generated candidates cannot exceed the number of candidates passed in the SearchSpace config";

        pdcch_candidates_count += ue_cfg.search_space(ss->id()).get_pdcch_candidates(aggr_lvl, pdcch_slot).size();
      }
    }

    // The number of PDCCH candidates in each SearchSpace must not exceed the max number of PDCCH candidates for the
    // given numerology as per TS 38.213 Table 10.1-2.
    ASSERT_LE(pdcch_candidates_count, max_candidates);
  }
}
