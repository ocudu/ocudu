// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/config/logical_channel_config_pool.h"
#include "lib/scheduler/ue_context/logical_channel_system.h"
#include "lib/scheduler/ue_context/ta_management_system.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(inactive_ta_management_system_test, add_ue_returns_inactive_manager)
{
  scheduler_expert_config expert_cfg = config_helpers::make_default_scheduler_expert_config();
  // Disable TA management.
  expert_cfg.ue.ta_control.ta_cmd_offset_threshold = -1;
  ta_management_system ta_sys(expert_cfg.ue.ta_control);

  ue_logical_channel_repository ue_lc_chs;
  ue_ta_manager                 ta_mgr = ta_sys.add_ue(
      time_alignment_group::id_t{0}, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15, ue_lc_chs.view());

  ASSERT_FALSE(ta_mgr.active()) << "TA manager should be inactive when TA management is disabled";
  auto ta_mgr_moved = std::move(ta_mgr);
  ASSERT_FALSE(ta_mgr_moved.active());
  ASSERT_FALSE(ta_mgr.active());
  ASSERT_EQ(ta_sys.nof_active_ues(), 0U);
}

class base_ta_management_system_test
{
protected:
  base_ta_management_system_test() :
    ul_scs(subcarrier_spacing::kHz30),
    ta_sys(expert_cfg.ue.ta_control),
    next_sl_tx(
        ul_scs,
        test_rng::uniform_int<unsigned>(0, NOF_SFNS * NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(ul_scs) - 1))
  {
  }

  void run_slot()
  {
    next_sl_tx++;
    lc_ch_sys.slot_indication();
    ta_sys.slot_indication(next_sl_tx);
  }

  unsigned add_ue(unsigned ntn_koffset = 0)
  {
    unsigned ue_index;
    if (free_list.empty()) {
      ue_index = ues.size();
    } else {
      ue_index = free_list.back();
      free_list.pop_back();
    }
    auto ret = ues.insert(std::make_pair(ue_index, ue_context{}));
    ocudu_assert(ret.second, "Failed to insert UE in the repository");
    auto& u     = ret.first->second;
    u.ue_lc_chs = lc_ch_sys.create_ue(to_du_ue_index(ue_index), ul_scs, false, cfg_pool.create({}));
    u.ta_mgr    = ta_sys.add_ue(time_alignment_group::id_t{0}, ul_scs, ul_scs, u.ue_lc_chs.view(), ntn_koffset);
    return ue_index;
  }

  void rem_ue(unsigned ue_index)
  {
    ocudu_assert(ues.contains(ue_index), "Invalid UE index");
    ues.erase(ue_index);
    free_list.push_back(ue_index);
  }

  /// Computes the N_TA update i.e. N_TA_new - N_TA_old value in T_C units, which will result in new TA command
  /// (new_ta_cmd) being computed by the TA manager.
  int64_t compute_n_ta_diff_leading_to_new_ta_cmd(uint8_t new_ta_cmd)
  {
    return static_cast<int64_t>(std::round(static_cast<double>(((static_cast<int64_t>(new_ta_cmd) - 31) * 16 * 64)) /
                                           static_cast<double>(pow2(to_numerology_value(ul_scs)))));
  }

  std::optional<dl_msg_lc_info> fetch_ta_cmd_mac_ce_allocation(unsigned ue_index)
  {
    static const lcid_dl_sch_t lcid            = lcid_dl_sch_t::TA_CMD;
    static const unsigned      remaining_bytes = lcid.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + 3;
    dl_msg_lc_info             subpdu{};
    if (ues.at(ue_index).ue_lc_chs.allocate_mac_ce(subpdu, remaining_bytes) > 0) {
      return subpdu;
    }
    return {};
  }

  std::optional<dl_msg_lc_info> run_until_next_ta_cmd(unsigned ue_index, unsigned max_slots = 0)
  {
    max_slots = max_slots == 0 ? expert_cfg.ue.ta_control.measurement_period * 2 : max_slots;
    for (unsigned count = 0; count != max_slots; ++count) {
      run_slot();
      auto ta_cmd_mac_ce_alloc = fetch_ta_cmd_mac_ce_allocation(ue_index);
      if (ta_cmd_mac_ce_alloc.has_value()) {
        return ta_cmd_mac_ce_alloc;
      }
    }
    return std::nullopt;
  }

  struct ue_context {
    ue_logical_channel_repository ue_lc_chs;
    ue_ta_manager                 ta_mgr;
  };

  subcarrier_spacing          ul_scs;
  scheduler_expert_config     expert_cfg = config_helpers::make_default_scheduler_expert_config();
  logical_channel_config_pool cfg_pool;
  logical_channel_system      lc_ch_sys;

  ta_management_system ta_sys;

  slot_point next_sl_tx;

  flat_map<unsigned, ue_context> ues;
  std::vector<unsigned>          free_list;
};

/// Test fixture for the TA management of a single UE.
class single_ue_ta_manager_test : public base_ta_management_system_test, public ::testing::Test
{
protected:
  single_ue_ta_manager_test()
  {
    unsigned offset = add_ue();
    ue_lc_chs       = &ues.at(offset).ue_lc_chs;
    ta_mgr          = &ues.at(offset).ta_mgr;
    run_slot();
  }

  std::optional<dl_msg_lc_info> fetch_ta_cmd_mac_ce_allocation()
  {
    return base_ta_management_system_test::fetch_ta_cmd_mac_ce_allocation(0);
  }

  std::optional<dl_msg_lc_info> run_until_next_ta_cmd(unsigned max_slots = 0)
  {
    return base_ta_management_system_test::run_until_next_ta_cmd(0, max_slots);
  }

  ue_logical_channel_repository* ue_lc_chs;
  ue_ta_manager*                 ta_mgr;
};

TEST_F(single_ue_ta_manager_test, ue_ta_manager_lifetime)
{
  ASSERT_EQ(ta_sys.nof_active_ues(), 1U);
  ASSERT_TRUE(ta_mgr->active());

  auto ta_mgr_moved = std::move(*ta_mgr);
  ASSERT_EQ(ta_sys.nof_active_ues(), 1U);
  ASSERT_TRUE(ta_mgr_moved.active());
  ASSERT_FALSE(ta_mgr->active());

  *ta_mgr = std::move(ta_mgr_moved);
  ASSERT_EQ(ta_sys.nof_active_ues(), 1U);
  ASSERT_TRUE(ta_mgr->active());
  ASSERT_FALSE(ta_mgr_moved.active());

  ta_mgr->reset();
  ASSERT_EQ(ta_sys.nof_active_ues(), 0U);
}

TEST_F(single_ue_ta_manager_test, ta_cmd_is_not_triggered_when_no_ul_n_ta_update_indication_are_reported)
{
  ASSERT_FALSE(run_until_next_ta_cmd().has_value()) << "TA command should not be triggered";
}

TEST_F(single_ue_ta_manager_test, ta_cmd_is_not_triggered_when_reported_ul_n_ta_update_indication_has_low_sinr)
{
  // Enqueue a UL N_TA update indication of low SINR.
  const uint8_t new_ta_cmd = 33;
  const float   ul_sinr    = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold - 10;
  ta_mgr->handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);

  // Ensure MAC CE is not allocated for TA command.
  ASSERT_FALSE(run_until_next_ta_cmd().has_value()) << "TA command should not be triggered";
}

TEST_F(single_ue_ta_manager_test, when_n_ta_update_with_high_snr_then_ta_cmd_is_successfully_triggered)
{
  // Start from a random slot.
  unsigned rand_start_slot = test_rng::uniform_int<unsigned>(0, expert_cfg.ue.ta_control.measurement_period - 1);
  for (unsigned i = 0; i < rand_start_slot; ++i) {
    run_slot();
  }

  // Enqueue a UL N_TA update indication of high SINR.
  const uint8_t new_ta_cmd = 33;
  const float   ul_sinr    = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;
  ta_mgr->handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);

  // Ensure MAC CE is allocated for TA command.
  std::optional<dl_msg_lc_info> ta_cmd_mac_ce_alloc =
      run_until_next_ta_cmd(expert_cfg.ue.ta_control.measurement_period);
  ASSERT_TRUE(ta_cmd_mac_ce_alloc.has_value()) << "Missing TA command CE allocation";
  ASSERT_TRUE(ta_cmd_mac_ce_alloc->lcid == lcid_dl_sch_t::TA_CMD) << "TA command is not be triggered";
  ASSERT_TRUE(std::holds_alternative<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload))
      << "TA command CE payload is absent";
  auto ta_cmd_ce = std::get<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload);
  ASSERT_EQ(ta_cmd_ce.ta_cmd, new_ta_cmd) << "New TA command does not match the expected TA command value";
}

TEST_F(single_ue_ta_manager_test, verify_computed_new_ta_cmd_based_on_multiple_n_ta_diff_reported)
{
  // Expected value. Average of ta_values_reported excluding the outlier 45.
  const unsigned expected_new_ta_cmd = 34;
  const float    ul_sinr             = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;

  // Pass enough TA values until outlier detection is activated.
  // Note: We use i%3 to add some variance in the samples.
  unsigned nof_init_samples = 10;
  for (unsigned i = 0; i < nof_init_samples; ++i) {
    ta_mgr->handle_ul_n_ta_update_indication(
        time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(33 + (i % 3)), ul_sinr);
  }
  ASSERT_TRUE(run_until_next_ta_cmd().has_value()) << "TA command should be triggered";
  ASSERT_FALSE(run_until_next_ta_cmd(expert_cfg.ue.ta_control.measurement_prohibit_period).has_value())
      << "TA command should not be triggered during prohibit period";

  // New measurement window.
  const std::vector<uint8_t> ta_values_reported = {35, 34, 45, 34, 33};
  for (const auto ta : ta_values_reported) {
    ta_mgr->handle_ul_n_ta_update_indication(
        time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(ta), ul_sinr);
  }

  std::optional<dl_msg_lc_info> ta_cmd_mac_ce_alloc =
      run_until_next_ta_cmd(expert_cfg.ue.ta_control.measurement_period);
  ASSERT_TRUE(ta_cmd_mac_ce_alloc.has_value()) << "Missing TA command CE allocation";
  ASSERT_TRUE(ta_cmd_mac_ce_alloc->lcid == lcid_dl_sch_t::TA_CMD) << "TA command is not be triggered";
  ASSERT_TRUE(std::holds_alternative<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload))
      << "TA command CE payload is absent";
  auto ta_cmd_ce = std::get<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload);
  ASSERT_EQ(ta_cmd_ce.ta_cmd, expected_new_ta_cmd) << "New TA command does not match the expected TA command value";
}

TEST_F(single_ue_ta_manager_test, ta_cmd_is_not_triggered_if_ue_is_reset)
{
  // Enqueue a UL N_TA update indication of high SINR.
  const uint8_t new_ta_cmd = 33;
  const float   ul_sinr    = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;
  ta_mgr->handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);

  // Reset the UE from the TA management system.
  ta_mgr->reset();

  // Ensure MAC CE is not allocated for TA command.
  ASSERT_FALSE(run_until_next_ta_cmd().has_value()) << "TA command should not be triggered after UE reset";
}

TEST_F(single_ue_ta_manager_test, n_ta_indications_ignored_during_prohibit_period)
{
  // Enqueue a UL N_TA update indication of high SINR, and wait for respective TA command.
  const uint8_t new_ta_cmd = 33;
  const float   ul_sinr    = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;
  ta_mgr->handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);
  ASSERT_TRUE(run_until_next_ta_cmd(expert_cfg.ue.ta_control.measurement_period).has_value())
      << "Missing TA command CE allocation";

  // Enqueue multiple UL N_TA update indications during prohibit period.
  for (unsigned i = 0; i < expert_cfg.ue.ta_control.measurement_prohibit_period; ++i) {
    ta_mgr->handle_ul_n_ta_update_indication(
        time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);
    run_slot();
    ASSERT_FALSE(fetch_ta_cmd_mac_ce_allocation().has_value())
        << "TA command should not be triggered during prohibit period";
  }

  // TEST CASE: Ensure MAC CE is not allocated for TA command.
  ASSERT_FALSE(run_until_next_ta_cmd().has_value()) << "TA command should not be triggered after UE reset";
}

TEST_F(single_ue_ta_manager_test, n_ta_indications_considered_after_prohibit_period)
{
  // Enqueue a UL N_TA update indication of high SINR.
  const uint8_t new_ta_cmd = 33;
  const float   ul_sinr    = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;
  ta_mgr->handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);

  // Ensure MAC CE is allocated for TA command.
  auto ta_cmd_mac_ce_alloc = run_until_next_ta_cmd(expert_cfg.ue.ta_control.measurement_period);
  ASSERT_TRUE(ta_cmd_mac_ce_alloc.has_value()) << "Missing TA command CE allocation";

  // Run through prohibit period.
  ta_cmd_mac_ce_alloc = run_until_next_ta_cmd(expert_cfg.ue.ta_control.measurement_prohibit_period);
  ASSERT_FALSE(ta_cmd_mac_ce_alloc.has_value()) << "TA command should not be triggered during prohibit period";

  // Enqueue a UL N_TA update indication after prohibit period.
  const uint8_t new_ta_cmd2 = 40;
  ta_mgr->handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd2), ul_sinr);

  // Ensure MAC CE is allocated for TA command.
  ta_cmd_mac_ce_alloc = run_until_next_ta_cmd(expert_cfg.ue.ta_control.measurement_period);
  ASSERT_TRUE(ta_cmd_mac_ce_alloc.has_value()) << "Missing TA command CE allocation";
  auto ta_cmd_ce = std::get<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload);
  ASSERT_EQ(ta_cmd_ce.ta_cmd, new_ta_cmd2) << "New TA command does not match the expected TA command value";
}

/// Test fixture for the TA management of a multiple UEs.
class multi_ue_ta_manager_test : public base_ta_management_system_test, public ::testing::Test
{
protected:
  multi_ue_ta_manager_test() = default;
};

TEST_F(multi_ue_ta_manager_test, n_ta_indications_ignored_until_the_ue_applies_the_ta_cmd)
{
  // The UE applies a TA command k + 1 slots after its PDSCH, as per TS 38.213, Section 4.2. k = 5 at 30 kHz.
  static constexpr unsigned ta_cmd_apply_delay = 6;
  ASSERT_EQ(ul_scs, subcarrier_spacing::kHz30);
  const unsigned ue_index = add_ue();
  run_slot();

  const uint8_t new_ta_cmd = 33;
  const float   ul_sinr    = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;
  ues.at(ue_index).ta_mgr.handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);
  ASSERT_TRUE(run_until_next_ta_cmd(ue_index, expert_cfg.ue.ta_control.measurement_period).has_value())
      << "Missing TA command CE allocation";

  for (unsigned i = 0; i != expert_cfg.ue.ta_control.measurement_prohibit_period + ta_cmd_apply_delay; ++i) {
    ues.at(ue_index).ta_mgr.handle_ul_n_ta_update_indication(
        time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);
    run_slot();
    ASSERT_FALSE(fetch_ta_cmd_mac_ce_allocation(ue_index).has_value())
        << "TA command should not be triggered before the UE applies the previous one";
  }
  ASSERT_FALSE(run_until_next_ta_cmd(ue_index).has_value())
      << "N_TA updates measured before the UE applied the TA command were not discarded";

  // N_TA updates count again once the UE applied the TA command.
  const uint8_t new_ta_cmd2 = 40;
  ues.at(ue_index).ta_mgr.handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd2), ul_sinr);
  const auto ta_cmd_mac_ce_alloc = run_until_next_ta_cmd(ue_index, expert_cfg.ue.ta_control.measurement_period);
  ASSERT_TRUE(ta_cmd_mac_ce_alloc.has_value()) << "Missing TA command CE allocation";
  ASSERT_EQ(std::get<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload).ta_cmd, new_ta_cmd2);
}

TEST_F(multi_ue_ta_manager_test, n_ta_indications_ignored_until_the_ue_applies_the_ta_cmd_past_its_koffset)
{
  // In NTN, K_offset delays the TA command further, as per TS 38.213, Section 4.2.
  const unsigned ntn_koffset = 40;
  const unsigned ue_index    = add_ue(ntn_koffset);
  run_slot();

  const uint8_t new_ta_cmd = 33;
  const float   ul_sinr    = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;
  ues.at(ue_index).ta_mgr.handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);
  ASSERT_TRUE(run_until_next_ta_cmd(ue_index, expert_cfg.ue.ta_control.measurement_period).has_value())
      << "Missing TA command CE allocation";

  for (unsigned i = 0; i != expert_cfg.ue.ta_control.measurement_prohibit_period + ntn_koffset; ++i) {
    ues.at(ue_index).ta_mgr.handle_ul_n_ta_update_indication(
        time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);
    run_slot();
    ASSERT_FALSE(fetch_ta_cmd_mac_ce_allocation(ue_index).has_value())
        << "TA command should not be triggered before the UE applies the previous one";
  }
  ASSERT_FALSE(run_until_next_ta_cmd(ue_index).has_value())
      << "N_TA updates measured before the UE applied the TA command were not discarded";

  // N_TA updates count again once the UE applied the TA command.
  const uint8_t new_ta_cmd2 = 40;
  ues.at(ue_index).ta_mgr.handle_ul_n_ta_update_indication(
      time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd2), ul_sinr);
  const auto ta_cmd_mac_ce_alloc = run_until_next_ta_cmd(ue_index, expert_cfg.ue.ta_control.measurement_period);
  ASSERT_TRUE(ta_cmd_mac_ce_alloc.has_value()) << "Missing TA command CE allocation";
  ASSERT_EQ(std::get<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload).ta_cmd, new_ta_cmd2);
}

TEST_F(multi_ue_ta_manager_test, ta_cmd_is_successfully_triggered_for_multiple_ues)
{
  const unsigned nof_ues = 5;
  for (unsigned i = 0; i < nof_ues; ++i) {
    add_ue();
  }
  run_slot();

  const uint8_t new_ta_cmd = 33;
  const float   ul_sinr    = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;

  // Enqueue a UL N_TA update indication of high SINR for all UEs.
  for (unsigned i = 0; i < nof_ues; ++i) {
    ues.at(i).ta_mgr.handle_ul_n_ta_update_indication(
        time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);
  }

  // Ensure MAC CE is allocated for TA command for all UEs.
  std::vector<unsigned> ues_count(ues.size(), 0);
  unsigned              count_ues = 0;
  unsigned              max_slots = expert_cfg.ue.ta_control.measurement_period * 2;
  for (unsigned count = 0; count != max_slots; ++count) {
    run_slot();
    for (unsigned ue_index = 0; ue_index < ues.size(); ++ue_index) {
      auto ta_cmd_mac_ce_alloc = fetch_ta_cmd_mac_ce_allocation(ue_index);
      if (ta_cmd_mac_ce_alloc.has_value()) {
        if (ues_count[ue_index] == 0) {
          ASSERT_EQ(ues_count[ue_index], 0U) << "Multiple TA commands allocated for UE " << ue_index;
          ++ues_count[ue_index];
          count_ues++;
        }
        ASSERT_EQ(ta_cmd_mac_ce_alloc->lcid, lcid_dl_sch_t::TA_CMD) << "TA command is not be triggered for UE ";
        auto ta_cmd_ce = std::get<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload);
        ASSERT_EQ(ta_cmd_ce.ta_cmd, new_ta_cmd);
      }
    }
  }
  ASSERT_EQ(count_ues, nof_ues) << "Not all UEs received TA command";
}

TEST_F(multi_ue_ta_manager_test, ta_cmd_is_successfully_triggered_for_ues_not_reset)
{
  const unsigned nof_ues = 10;
  for (unsigned i = 0; i < nof_ues; ++i) {
    add_ue();
  }
  run_slot();

  // Enqueue a UL N_TA update indication of high SINR for all UEs.
  const uint8_t new_ta_cmd = 33;
  const float   ul_sinr    = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;
  for (unsigned i = 0; i < nof_ues; ++i) {
    ues.at(i).ta_mgr.handle_ul_n_ta_update_indication(
        time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);
  }

  std::vector<unsigned> reset_ues = {2, 7};
  for (unsigned idx : reset_ues) {
    ues.at(idx).ta_mgr.reset();
  }

  // Ensure MAC CE is allocated for TA command for all non-reset UEs.
  std::vector<unsigned> ues_count(ues.size(), 0);
  unsigned              count_ues = 0;
  unsigned              max_slots = expert_cfg.ue.ta_control.measurement_period * 2;
  for (unsigned count = 0; count != max_slots; ++count) {
    run_slot();
    for (unsigned ue_index = 0; ue_index < ues.size(); ++ue_index) {
      auto ta_cmd_mac_ce_alloc = fetch_ta_cmd_mac_ce_allocation(ue_index);
      if (ta_cmd_mac_ce_alloc.has_value()) {
        ASSERT_FALSE(std::find(reset_ues.begin(), reset_ues.end(), ue_index) != reset_ues.end())
            << "TA command should not be allocated for reset UE " << ue_index;
        if (ues_count[ue_index] == 0) {
          ASSERT_EQ(ues_count[ue_index], 0U) << "Multiple TA commands allocated for UE " << ue_index;
          ++ues_count[ue_index];
          count_ues++;
        }
        ASSERT_EQ(ta_cmd_mac_ce_alloc->lcid, lcid_dl_sch_t::TA_CMD) << "TA command is not be triggered for UE ";
        auto ta_cmd_ce = std::get<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload);
        ASSERT_EQ(ta_cmd_ce.ta_cmd, new_ta_cmd);
      }
    }
  }
  ASSERT_EQ(count_ues, nof_ues - reset_ues.size()) << "Not all UEs received TA command";
}

TEST_F(multi_ue_ta_manager_test, test_random_ue_creation_and_ta_cmd_triggers)
{
  const unsigned                        run_nof_slots = 10000;
  std::uniform_real_distribution<float> randf(0.0f, 1.0f);
  auto          is_triggered = [&randf](float prob) { return randf(test_rng::tls_gen()) < prob; };
  const uint8_t new_ta_cmd   = 33;
  const float   ul_sinr      = expert_cfg.ue.ta_control.update_measurement_ul_sinr_threshold + 10;

  for (unsigned slot = 0; slot < run_nof_slots; ++slot) {
    ASSERT_EQ(ues.size(), ta_sys.nof_active_ues());
    // Randomly add UEs.
    if (is_triggered(0.1)) {
      add_ue();
    }

    // Randomly remove UEs.
    if (not ues.empty() and is_triggered(0.1)) {
      unsigned ue_to_rem = test_rng::uniform_int<unsigned>(0, ues.size() - 1);
      unsigned ue_idx    = ues.keys()[ue_to_rem];
      rem_ue(ue_idx);
    }

    // Randomly enqueue UL N_TA update indications for existing UEs.
    for (auto it = ues.begin(); it != ues.end(); ++it) {
      if (is_triggered(0.3)) {
        it->second.ta_mgr.handle_ul_n_ta_update_indication(
            time_alignment_group::id_t{0}, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);
      }
    }

    // Run slot.
    run_slot();

    // Randomly allocate grants for TA commands.
    for (auto it = ues.begin(); it != ues.end(); ++it) {
      if (is_triggered(0.4)) {
        auto ta_cmd_mac_ce_alloc = fetch_ta_cmd_mac_ce_allocation(it->first);
        if (ta_cmd_mac_ce_alloc.has_value()) {
          ASSERT_EQ(ta_cmd_mac_ce_alloc->lcid, lcid_dl_sch_t::TA_CMD)
              << "TA command is not be triggered for UE " << it->first;
          ASSERT_TRUE(std::holds_alternative<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload))
              << "TA command CE payload is absent for UE " << it->first;
        }
      }
    }
  }

  ASSERT_EQ(ues.size(), ta_sys.nof_active_ues());
}
