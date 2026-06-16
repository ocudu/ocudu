// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/support/sch_pdu_builder.h"
#include "lib/scheduler/ue_context/ue_repository.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// Test suit to hold context which are required to test a UE cell implementation.
class sched_ue_test : public ::testing::Test
{
protected:
  sched_ue_test() :
    expert_cfg(config_helpers::make_default_scheduler_expert_config()),
    cfg_mng(scheduler_config{expert_cfg, metric_notif}),
    sched_cfg(sched_config_helper::make_default_sched_cell_configuration_request(builder_params)),
    cell_cfg(*cfg_mng.add_cell(sched_cfg)),
    serv_cell_cfg(config_helpers::make_default_ue_cell_config(cell_cfg.params).serv_cell_cfg),
    ue_db(expert_cfg.ue)
  {
    ue_db.add_cell(cell_cfg, nullptr);

    sched_ue_creation_request_message ue_req = sched_config_helper::create_default_sched_ue_creation_request(
        cell_cfg.params, std::array<lcid_t, 3>{lcid_t::LCID_SRB1, lcid_t::LCID_SRB2, lcid_t::LCID_MIN_DRB});
    ue_config_update_event  ev     = cfg_mng.add_ue(ue_req);
    const ue_configuration& ue_cfg = ev.next_config();
    ue_db.add_ue(ue_cfg, ue_req.starts_in_fallback, ue_req.ul_ccch_slot_rx, ue_req.cfra_enabled);
    ev.notify_completion();
  }

  pdsch_config_params get_pdsch_cfg_params(const ue_cell&                               ue_cc,
                                           const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                           dci_dl_rnti_config_type                      dci_type)
  {
    switch (dci_type) {
      case dci_dl_rnti_config_type::c_rnti_f1_0:
        return sched_helper::get_pdsch_config_f1_0_c_rnti(cell_cfg, ue_cc.cfg().pdsch_serving_cell_cfg(), pdsch_td_cfg);
      case dci_dl_rnti_config_type::c_rnti_f1_1:
        return sched_helper::get_pdsch_config_f1_1_c_rnti(cell_cfg,
                                                          *ue_cc.cfg().bwp(to_bwp_id(0)).dl.pdsch().ded(),
                                                          ue_cc.cfg().pdsch_serving_cell_cfg(),
                                                          pdsch_td_cfg,
                                                          ue_cc.channel_state_manager().get_nof_dl_layers());
      default:
        report_fatal_error("Unsupported PDCCH DCI DL format");
    }
  }

  pusch_config_params get_pusch_cfg_params(const ue_cell&                               ue_cc,
                                           const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                           dci_ul_rnti_config_type                      dci_type)
  {
    // In the following, we allocate extra bits to account for the possible UCI overhead. At this point, we don't
    // differentiate between HARQ-ACK bits and CSI bits, which would be necessary to compute the beta-offset values.
    // Here, we only need to allocate some extra space.
    const unsigned uci_bits_overallocation = 20U;
    const bool     is_csi_report_slot      = false;

    switch (dci_type) {
      case dci_ul_rnti_config_type::c_rnti_f0_0:
        return get_pusch_config_f0_0_c_rnti(cell_cfg,
                                            &ue_cc.cfg(),
                                            ue_cc.cfg().bwp(ue_cc.active_bwp_id()).ul.common(),
                                            pusch_td_cfg,
                                            uci_bits_overallocation,
                                            is_csi_report_slot);
      case dci_ul_rnti_config_type::c_rnti_f0_1:
        return get_pusch_config_f0_1_c_rnti(ue_cc.cfg(),
                                            pusch_td_cfg,
                                            ue_cc.channel_state_manager().get_nof_ul_layers(),
                                            uci_bits_overallocation,
                                            is_csi_report_slot);
      default:
        report_fatal_error("Unsupported PDCCH DCI UL format");
    }
  }

  scheduler_expert_config                  expert_cfg;
  sched_cfg_dummy_notifier                 metric_notif;
  cell_config_builder_params               builder_params;
  sched_config_manager                     cfg_mng;
  sched_cell_configuration_request_message sched_cfg;
  const cell_configuration&                cell_cfg;
  serving_cell_config                      serv_cell_cfg;
  ocudulog::basic_logger&                  logger = ocudulog::fetch_basic_logger("SCHED");
  ue_repository                            ue_db;
};

TEST_F(sched_ue_test, when_dl_nof_prb_allocated_increases_estimated_dl_rate_increases)
{
  auto&  ue_cc        = ue_db[to_du_ue_index(0)].get_pcell();
  double current_rate = 0;

  // We keep MCS constant for this test.
  const sch_mcs_index ue_mcs = 28;
  // Assume the DCI type to be format 1_1.
  const dci_dl_rnti_config_type dci_type = dci_dl_rnti_config_type::c_rnti_f1_1;

  for (unsigned nof_prbs = 0; nof_prbs < MAX_NOF_PRBS; ++nof_prbs) {
    for (const auto& pdsch_td_cfg :
         ue_cc.cfg().cell_cfg_common.params.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list) {
      const pdsch_config_params pdsch_cfg      = get_pdsch_cfg_params(ue_cc, pdsch_td_cfg, dci_type);
      const double              estimated_rate = ue_cc.get_estimated_dl_rate(pdsch_cfg, ue_mcs, nof_prbs);
      ASSERT_GE(estimated_rate, current_rate);
      current_rate = estimated_rate;
    }
  }
}

TEST_F(sched_ue_test, when_mcs_increases_estimated_dl_rate_increases)
{
  auto& ue_cc = ue_db[to_du_ue_index(0)].get_pcell();

  // Maximum MCS value for 64QAM MCS table.
  const sch_mcs_index max_mcs = 28;

  double current_rate = 0;

  // We keep nof. PRBs allocated constant for this test.
  const unsigned nof_prbs = 20;
  // Assume the DCI type to be format 1_1.
  const dci_dl_rnti_config_type dci_type = dci_dl_rnti_config_type::c_rnti_f1_1;

  for (sch_mcs_index ue_mcs = 1; ue_mcs < max_mcs; ++ue_mcs) {
    for (const auto& pdsch_td_cfg :
         ue_cc.cfg().cell_cfg_common.params.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list) {
      const pdsch_config_params pdsch_cfg      = get_pdsch_cfg_params(ue_cc, pdsch_td_cfg, dci_type);
      const double              estimated_rate = ue_cc.get_estimated_dl_rate(pdsch_cfg, ue_mcs, nof_prbs);
      // NOTE: In case of 64QAM MCS table MCS 17 has lower spectral density than MCS 16 but the estimated bitrate will
      // remain equal hence its required to use the check greater than or equal below.
      ASSERT_GE(estimated_rate, current_rate);
      current_rate = estimated_rate;
    }
  }
}

TEST_F(sched_ue_test, when_ul_nof_prb_allocated_increases_estimated_ul_rate_increases)
{
  auto&  ue_cc        = ue_db[to_du_ue_index(0)].get_pcell();
  double current_rate = 0;

  // We keep MCS constant for this test.
  const sch_mcs_index ue_mcs = 15;
  // Assume the DCI type to be format 0_1.
  const dci_ul_rnti_config_type dci_type = dci_ul_rnti_config_type::c_rnti_f0_1;

  for (unsigned nof_prbs = 0; nof_prbs < MAX_NOF_PRBS; ++nof_prbs) {
    for (const auto& pusch_td_cfg :
         ue_cc.cfg().cell_cfg_common.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list) {
      const pusch_config_params pusch_cfg      = get_pusch_cfg_params(ue_cc, pusch_td_cfg, dci_type);
      const double              estimated_rate = ue_cc.get_estimated_ul_rate(pusch_cfg, ue_mcs, nof_prbs);
      ASSERT_GE(estimated_rate, current_rate);
      current_rate = estimated_rate;
    }
  }
}

TEST_F(sched_ue_test, when_mcs_increases_estimated_ul_rate_increases)
{
  auto& ue_cc = ue_db[to_du_ue_index(0)].get_pcell();

  // Maximum MCS value for 64QAM MCS table.
  const sch_mcs_index max_mcs = 28;

  double current_rate = 0;

  // We keep nof. PRBs allocated constant for this test.
  const unsigned nof_prbs = 20;
  // Assume the DCI type to be format 0_1.
  const dci_ul_rnti_config_type dci_type = dci_ul_rnti_config_type::c_rnti_f0_1;

  for (sch_mcs_index ue_mcs = 1; ue_mcs < max_mcs; ++ue_mcs) {
    for (const auto& pusch_td_cfg :
         ue_cc.cfg().cell_cfg_common.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list) {
      const pusch_config_params pusch_cfg      = get_pusch_cfg_params(ue_cc, pusch_td_cfg, dci_type);
      const double              estimated_rate = ue_cc.get_estimated_ul_rate(pusch_cfg, ue_mcs, nof_prbs);
      // NOTE: In case of 64QAM MCS table MCS 17 has lower spectral density than MCS 16 but the estimated bitrate will
      // remain equal hence its required to use the check greater than or equal below.
      ASSERT_GE(estimated_rate, current_rate);
      current_rate = estimated_rate;
    }
  }
}

class cfra_ue_repository_test : public ::testing::Test
{
protected:
  cfra_ue_repository_test() :
    expert_cfg(config_helpers::make_default_scheduler_expert_config()),
    cfg_mng(scheduler_config{expert_cfg, metric_notif}),
    sched_cfg(sched_config_helper::make_default_sched_cell_configuration_request(builder_params)),
    cell_cfg(*cfg_mng.add_cell(sched_cfg)),
    ue_db(expert_cfg.ue)
  {
    ue_db.add_cell(cell_cfg, nullptr);
  }

  du_ue_index_t add_ue(bool cfra_enabled, bool starts_in_fallback, rnti_t crnti = to_rnti(0x4601))
  {
    sched_ue_creation_request_message ue_req = sched_config_helper::create_default_sched_ue_creation_request(
        cell_cfg.params, std::array<lcid_t, 3>{lcid_t::LCID_SRB1, lcid_t::LCID_SRB2, lcid_t::LCID_MIN_DRB});
    ue_req.crnti                   = crnti;
    ue_req.starts_in_fallback      = starts_in_fallback;
    ue_req.cfra_enabled            = cfra_enabled;
    ue_req.ul_ccch_slot_rx         = std::nullopt;
    ue_config_update_event  ev     = cfg_mng.add_ue(ue_req);
    const ue_configuration& ue_cfg = ev.next_config();
    du_ue_index_t           ue_idx = ue_req.ue_index;
    ue_db.add_ue(ue_cfg, ue_req.starts_in_fallback, ue_req.ul_ccch_slot_rx, ue_req.cfra_enabled);
    ev.notify_completion();
    return ue_idx;
  }

  scheduler_expert_config                  expert_cfg;
  sched_cfg_dummy_notifier                 metric_notif;
  cell_config_builder_params               builder_params;
  sched_config_manager                     cfg_mng;
  sched_cell_configuration_request_message sched_cfg;
  const cell_configuration&                cell_cfg;
  ue_repository                            ue_db;
};

TEST_F(cfra_ue_repository_test, cfra_ue_starts_in_pending_cfra_state)
{
  du_ue_index_t         ue_idx   = add_ue(true, true);
  const ue_pcell_state& pcell_st = ue_db[ue_idx].get_pcell().get_pcell_state();
  ASSERT_EQ(pcell_st.conres_st, ue_conres_state::pending_cfra);
  ASSERT_TRUE(is_in_fallback(pcell_st.config_st, pcell_st.conres_st));
}

TEST_F(cfra_ue_repository_test, cfra_msg3_acked_transitions_to_conres_completed)
{
  du_ue_index_t ue_idx = add_ue(true, true);
  ASSERT_TRUE(ue_db.cfra_msg3_acked(ue_idx));
  const ue_pcell_state& pcell_st = ue_db[ue_idx].get_pcell().get_pcell_state();
  ASSERT_EQ(pcell_st.conres_st, ue_conres_state::conres_completed);
  ASSERT_FALSE(is_in_fallback(pcell_st.config_st, pcell_st.conres_st));
}

TEST_F(cfra_ue_repository_test, cfra_msg3_acked_is_noop_when_not_in_pending_cfra)
{
  du_ue_index_t ue_idx = add_ue(false, false);
  ASSERT_EQ(ue_db[ue_idx].get_pcell().get_pcell_state().conres_st, ue_conres_state::conres_completed);
  ASSERT_FALSE(ue_db.cfra_msg3_acked(ue_idx));
  ASSERT_EQ(ue_db[ue_idx].get_pcell().get_pcell_state().conres_st, ue_conres_state::conres_completed);
}
