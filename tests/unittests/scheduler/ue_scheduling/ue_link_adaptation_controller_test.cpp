// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/support/mcs_calculator.h"
#include "lib/scheduler/ue_context/ue_link_adaptation_controller.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

class base_ue_link_adaptation_controller_test
{
public:
  base_ue_link_adaptation_controller_test(std::optional<float> olla_cqi_inc   = std::nullopt,
                                          std::optional<float> max_cqi_offset = std::nullopt,
                                          std::optional<float> target_bler    = std::nullopt) :
    sched_cfg([&]() {
      scheduler_expert_config cfg = config_helpers::make_default_scheduler_expert_config();
      if (olla_cqi_inc.has_value()) {
        cfg.ue.olla_cqi_inc = *olla_cqi_inc;
      }
      if (max_cqi_offset.has_value()) {
        cfg.ue.olla_max_cqi_offset = *max_cqi_offset;
      }
      if (target_bler.has_value()) {
        cfg.ue.olla_dl_target_bler = *target_bler;
      }
      return cfg;
    }())
  {
  }

  scheduler_expert_config                 sched_cfg;
  test_helpers::test_sched_config_manager cfg_mng{sched_cfg};
  const cell_configuration&               cell_cfg{
      *cfg_mng.add_cell(sched_config_helper::make_default_sched_cell_configuration_request())};
  ue_channel_state_manager      ue_channel_state{sched_cfg.ue, 1};
  ue_link_adaptation_controller controller{cell_cfg, ue_channel_state};

  const pdsch_mcs_table dl_mcs_table = pdsch_mcs_table::qam64;
  const pusch_mcs_table ul_mcs_table = pusch_mcs_table::qam64;
};

class ue_link_adaptation_controller_test : public ::testing::Test, public base_ue_link_adaptation_controller_test
{
public:
};

TEST_F(ue_link_adaptation_controller_test, starts_with_no_snr_offset)
{
  ASSERT_EQ(controller.dl_cqi_offset(), 0);
  ASSERT_EQ(controller.ul_snr_offset_db(), 0);
}

TEST_F(ue_link_adaptation_controller_test, acks_increase_offsets)
{
  controller.handle_dl_ack_info(true, sch_mcs_index{5}, dl_mcs_table, sch_mcs_index{5});
  ASSERT_GT(controller.dl_cqi_offset(), 0);

  controller.handle_ul_crc_info(true, sch_mcs_index{5}, pusch_mcs_table::qam64, sch_mcs_index{5}, std::nullopt);
  ASSERT_GT(controller.ul_snr_offset_db(), 0);
}

TEST_F(ue_link_adaptation_controller_test, nacks_increase_offsets)
{
  controller.handle_dl_ack_info(false, sch_mcs_index{5}, dl_mcs_table, sch_mcs_index{5});
  ASSERT_LT(controller.dl_cqi_offset(), 0);

  controller.handle_ul_crc_info(false, sch_mcs_index{5}, pusch_mcs_table::qam64, sch_mcs_index{5}, std::nullopt);
  ASSERT_LT(controller.ul_snr_offset_db(), 0);
}

TEST_F(ue_link_adaptation_controller_test, crc_below_min_pusch_snr_is_ignored_by_ul_olla)
{
  // A CRC whose reported SINR is below the configured minimum must leave the UL OLLA offset untouched.
  controller.handle_ul_crc_info(
      false, sch_mcs_index{5}, pusch_mcs_table::qam64, sch_mcs_index{5}, sched_cfg.ue.olla_ul_min_pusch_snr - 1.0F);
  ASSERT_EQ(controller.ul_snr_offset_db(), 0);

  // A CRC whose reported SINR is not below the minimum is processed as usual.
  controller.handle_ul_crc_info(
      false, sch_mcs_index{5}, pusch_mcs_table::qam64, sch_mcs_index{5}, sched_cfg.ue.olla_ul_min_pusch_snr);
  ASSERT_LT(controller.ul_snr_offset_db(), 0);
}

TEST_F(ue_link_adaptation_controller_test, cqi_0_reports_empty_mcs)
{
  // make offset different than zero.
  controller.handle_dl_ack_info(true, sch_mcs_index{5}, dl_mcs_table, sch_mcs_index{5});

  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{0};
  csi.valid                 = true;
  ue_channel_state.handle_csi_report(csi);

  std::optional<sch_mcs_index> mcs = controller.calculate_dl_mcs(dl_mcs_table);
  ASSERT_FALSE(mcs.has_value());
}

TEST_F(ue_link_adaptation_controller_test, cqi_positive_reports_non_empty_mcs)
{
  // make offset different than zero.
  controller.handle_dl_ack_info(true, sch_mcs_index{5}, dl_mcs_table, sch_mcs_index{5});

  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{test_rng::uniform_int<uint8_t>(1, 15)};
  csi.valid                 = true;
  ue_channel_state.handle_csi_report(csi);

  std::optional<sch_mcs_index> mcs = controller.calculate_dl_mcs(dl_mcs_table);
  ASSERT_TRUE(mcs.has_value());
}

class ue_link_adaptation_controller_mcs_derivation_test : public base_ue_link_adaptation_controller_test,
                                                          public ::testing::Test
{
public:
  ue_link_adaptation_controller_mcs_derivation_test() : base_ue_link_adaptation_controller_test(0.1, 15, 0.49) {}
};

TEST_F(ue_link_adaptation_controller_mcs_derivation_test,
       mcs_increases_with_increasing_offset_in_steps_no_larger_than_1)
{
  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{5};
  csi.valid                 = true;
  ue_channel_state.handle_csi_report(csi);

  const sch_mcs_index mcs_lb = map_cqi_to_mcs(csi.first_tb_wideband_cqi->value(), dl_mcs_table).value();
  const sch_mcs_index mcs_ub = map_cqi_to_mcs(csi.first_tb_wideband_cqi->value() + 1, dl_mcs_table).value();

  // zero offset case.
  sch_mcs_index mcs_prev = controller.calculate_dl_mcs(dl_mcs_table).value();
  ASSERT_EQ(mcs_prev, mcs_lb);

  // MCS increases with offset, in steps of size equal or less than 1.
  sch_mcs_index mcs = mcs_prev;
  while (mcs != mcs_ub) {
    // Increase offset.
    controller.handle_dl_ack_info(true, sch_mcs_index{5}, dl_mcs_table, sch_mcs_index{5});

    mcs = controller.calculate_dl_mcs(dl_mcs_table).value();
    ASSERT_LE(mcs - mcs_prev, 1U);
    ASSERT_LE(mcs, mcs_ub);
    mcs_prev = mcs;
  }

  ASSERT_GT(controller.dl_cqi_offset(), 1.0F) << "MCS(CQI+offset) == MCS(CQI+1) if offset >= 1";
}

TEST(ue_link_adaptation_controller_pucch_rep_test, no_rep_config_always_returns_n1)
{
  scheduler_expert_config                 sched_cfg = config_helpers::make_default_scheduler_expert_config();
  test_helpers::test_sched_config_manager cfg_mng{sched_cfg};
  const cell_configuration&               cell_cfg =
      *cfg_mng.add_cell(sched_config_helper::make_default_sched_cell_configuration_request());
  ue_channel_state_manager      ue_channel_state{sched_cfg.ue, 1};
  ue_link_adaptation_controller controller{cell_cfg, ue_channel_state};

  // No matter the SNR, if there is no repetition configuration, the recommended repetition factor is always n1.
  for (const float snr : {-100.0F, -3.0F, 0.0F, 3.0F, 9.0F, 100.0F}) {
    ue_channel_state.update_pusch_snr(snr);
    ASSERT_EQ(controller.get_recommended_pucch_rep_factor(), pucch_repetition_factor::n1);
  }
}

TEST(ue_link_adaptation_controller_pucch_rep_test, rep_factor_follows_sinr_thresholds)
{
  // SINR thresholds, in order [max SINR for n2, max SINR for n4, max SINR for n8].
  constexpr float n2_thres = 9.0F;
  constexpr float n4_thres = 3.0F;
  constexpr float n8_thres = -3.0F;

  scheduler_expert_config sched_cfg = config_helpers::make_default_scheduler_expert_config();

  sched_cell_configuration_request_message req = sched_config_helper::make_default_sched_cell_configuration_request();
  auto&                                    resources = req.ran.init_bwp.pucch.resources;
  pucch_harq_ack_rep_params                rep_params{};
  rep_params.sinr_thresholds = {n2_thres, n4_thres, n8_thres};
  rep_params.factors_per_res.assign(resources.res_set_size.value(), pucch_repetition_factor::n8);
  resources.harq_ack_rep = rep_params;

  test_helpers::test_sched_config_manager cfg_mng{sched_cfg};
  const cell_configuration&               cell_cfg = *cfg_mng.add_cell(req);
  ue_channel_state_manager                ue_channel_state{sched_cfg.ue, 1};
  ue_link_adaptation_controller           controller{cell_cfg, ue_channel_state};

  // SINR at or above the n2 threshold does not warrant any repetition.
  ue_channel_state.update_pusch_snr(n2_thres + 1.0F);
  ASSERT_EQ(controller.get_recommended_pucch_rep_factor(), pucch_repetition_factor::n1);
  ue_channel_state.update_pusch_snr(n2_thres);
  ASSERT_EQ(controller.get_recommended_pucch_rep_factor(), pucch_repetition_factor::n1);

  // SINR between the n4 and n2 thresholds warrants n2 repetitions.
  ue_channel_state.update_pusch_snr((n2_thres + n4_thres) / 2.0F);
  ASSERT_EQ(controller.get_recommended_pucch_rep_factor(), pucch_repetition_factor::n2);
  ue_channel_state.update_pusch_snr(n4_thres);
  ASSERT_EQ(controller.get_recommended_pucch_rep_factor(), pucch_repetition_factor::n2);

  // SINR between the n8 and n4 thresholds warrants n4 repetitions.
  ue_channel_state.update_pusch_snr((n4_thres + n8_thres) / 2.0F);
  ASSERT_EQ(controller.get_recommended_pucch_rep_factor(), pucch_repetition_factor::n4);
  ue_channel_state.update_pusch_snr(n8_thres);
  ASSERT_EQ(controller.get_recommended_pucch_rep_factor(), pucch_repetition_factor::n4);

  // SINR below the n8 threshold warrants n8 repetitions.
  ue_channel_state.update_pusch_snr(n8_thres - 1.0F);
  ASSERT_EQ(controller.get_recommended_pucch_rep_factor(), pucch_repetition_factor::n8);
}
