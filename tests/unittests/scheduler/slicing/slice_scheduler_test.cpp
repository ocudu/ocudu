// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/slicing/inter_slice_scheduler.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/config/logical_channel_config_factory.h"
#include "ocudu/scheduler/config/pusch_td_resource_indices.h"
#include <gtest/gtest.h>

using namespace ocudu;

static constexpr logical_channel_config create_logical_channel_config(lcid_t lcid, rrm_policy_member policy = {})
{
  logical_channel_config lc_ch = config_helpers::create_default_logical_channel_config(lcid);
  lc_ch.rrm_policy             = policy;
  return lc_ch;
}

class slice_scheduler_test
{
protected:
  slice_scheduler_test(const std::vector<slice_rrm_policy_config>& rrm_policy_members = {}) :
    test_cfg(
        []() {
          cell_config_builder_params params{};
          params.scs_common             = subcarrier_spacing::kHz30;
          params.dl_carrier.carrier_bw  = bs_channel_bandwidth::MHz100;
          params.dl_carrier.arfcn_f_ref = 520002;
          params.dl_carrier.band        = nr_band::n41;
          return params;
        }(),
        scheduler_expert_config{}),
    cell_cfg([this, rrm_policy_members]() -> const cell_configuration& {
      auto req               = test_cfg.get_default_cell_config_request();
      req.rrm_policy_members = rrm_policy_members;
      return *test_cfg.add_cell(req);
    }()),
    ues(cell_cfg.expert_cfg.ue)
  {
    pusch_td_list_per_slot =
        get_pusch_td_resource_indices_per_slot(cell_cfg.scs_common(),
                                               cell_cfg.params.tdd_cfg,
                                               cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value(),
                                               cell_cfg.dl_data_to_ul_ack);
    logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    ues.add_cell(cell_cfg, nullptr);
  }

  ~slice_scheduler_test() { ocudulog::flush(); }

  void run_slot()
  {
    dummy_alloc.slot_indication(next_slot);
    slice_sched.slot_indication(next_slot, dummy_alloc);
    ++next_slot;
  }

  const ue_configuration* add_ue(const sched_ue_creation_request_message& req)
  {
    const ue_configuration* ue_cfg = test_cfg.add_ue(req);
    ues.add_ue(*ue_cfg, req.starts_in_fallback, req.ul_ccch_slot_rx, req.cfra_enabled);
    slice_sched.add_ue(req.ue_index);
    return ue_cfg;
  }

  ocudulog::basic_logger&                 logger = ocudulog::fetch_basic_logger("TEST");
  test_helpers::test_sched_config_manager test_cfg;
  const cell_configuration&               cell_cfg;

  ue_repository ues;

  cell_resource_allocator                                                            dummy_alloc{cell_cfg};
  inter_slice_scheduler                                                              slice_sched{cell_cfg, ues};
  std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> pusch_td_list_per_slot;

public:
  slot_point next_slot{to_numerology_value(cell_cfg.params.dl_cfg_common.freq_info_dl.scs_carrier_list.back().scs), 0};
};

class default_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    auto req               = test_cfg.get_default_ue_config_request();
    req.ue_index           = ue_idx;
    req.crnti              = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback = false;
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(default_slice_scheduler_test, if_no_rrm_policy_cfg_exists_then_only_default_slices_are_created)
{
  ASSERT_EQ(slice_sched.nof_slices(), 2);
}

TEST_F(default_slice_scheduler_test, when_no_lcid_exists_then_default_slice_is_not_a_candidate)
{
  run_slot();

  const auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());

  // Progress with the slots until there are valid k2 values for UL scheduling. If not, the UL slice candidate list will
  // be trivially empty.
  while (pusch_td_list_per_slot[(next_slot - 1).count() % pusch_td_list_per_slot.size()].empty()) {
    run_slot();
  }

  const auto next_ul_slice = slice_sched.get_next_ul_candidate();
  ASSERT_FALSE(next_ul_slice.has_value());
}

TEST_F(default_slice_scheduler_test, when_lcid_is_part_of_default_slice_then_default_slice_is_valid_candidate)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);

  for (unsigned count = 0, e = 10; count != e; ++count) {
    const bool is_dl_active = has_active_tdd_dl_symbols(cell_cfg.params.tdd_cfg.value(), next_slot.slot_index());
    const bool dl_slot_has_k2_values = not pusch_td_list_per_slot[count].empty();
    run_slot();

    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    if (is_dl_active) {
      ASSERT_TRUE(next_dl_slice.has_value());
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
      ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0)));
      ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_SRB1));
    } else {
      ASSERT_FALSE(next_dl_slice.has_value());
    }

    auto next_ul_slice = slice_sched.get_next_ul_candidate();
    if (dl_slot_has_k2_values) {
      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      ASSERT_TRUE(next_ul_slice->is_candidate(to_du_ue_index(0)));
      ASSERT_TRUE(next_ul_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_SRB1));
    } else {
      ASSERT_FALSE(next_ul_slice.has_value());
    }
  }
}

TEST_F(default_slice_scheduler_test,
       when_candidate_instance_goes_out_of_scope_then_it_stops_being_a_candidate_for_the_same_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), cell_cfg.nof_dl_prbs);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), DEFAULT_DRB_RAN_SLICE_ID);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), cell_cfg.nof_dl_prbs);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(default_slice_scheduler_test, when_candidate_instance_goes_out_of_scope_then_it_can_be_a_candidate_for_next_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);

  run_slot();
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);

  run_slot();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
}

TEST_F(default_slice_scheduler_test, when_grant_gets_allocated_then_number_of_available_rbs_decreases)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();

  constexpr unsigned alloc_rbs = 10;
  const unsigned     rem_rbs   = next_dl_slice->remaining_rbs();
  next_dl_slice->store_grant(alloc_rbs);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), rem_rbs - alloc_rbs);
}

TEST_F(default_slice_scheduler_test, returns_only_dl_pending_bytes_of_bearers_belonging_to_a_slice)
{
  constexpr unsigned      srb_pending_bytes{200};
  constexpr unsigned      drb_pending_bytes{5000};
  constexpr du_ue_index_t ue_idx{to_du_ue_index(0)};

  ASSERT_NE(this->add_ue(ue_idx), nullptr);
  // Push buffer state indication for DRB.
  this->ues[ue_idx].handle_dl_buffer_state_indication(LCID_MIN_DRB, drb_pending_bytes);
  // Push buffer state indication for SRB.
  this->ues[ue_idx].handle_dl_buffer_state_indication(LCID_SRB1, srb_pending_bytes);

  run_slot();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  const slice_ue_repository& srb_slice_ues = next_dl_slice->get_slice_ues();
  ASSERT_EQ(srb_slice_ues[ue_idx].pending_dl_newtx_bytes(), get_mac_sdu_required_bytes(srb_pending_bytes));

  // Default DRB slice is next candidate.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), DEFAULT_DRB_RAN_SLICE_ID);
  const slice_ue_repository& drb_slice_ues = next_dl_slice->get_slice_ues();
  ASSERT_EQ(drb_slice_ues[ue_idx].pending_dl_newtx_bytes(), get_mac_sdu_required_bytes(drb_pending_bytes));
}

TEST_F(default_slice_scheduler_test, returns_only_ul_pending_bytes_of_bearers_belonging_to_a_slice)
{
  // Estimate of the number of bytes required for the upper layer header.
  constexpr unsigned      RLC_HEADER_SIZE_ESTIMATE = 3U;
  constexpr unsigned      srb_pending_bytes{200};
  constexpr unsigned      drb_pending_bytes{5000};
  constexpr du_ue_index_t ue_idx{to_du_ue_index(0)};

  constexpr lcg_id_t srb_lcg_id = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1).lc_group;
  constexpr lcg_id_t drb_lcg_id = config_helpers::create_default_logical_channel_config(lcid_t::LCID_MIN_DRB).lc_group;

  ASSERT_NE(this->add_ue(ue_idx), nullptr);
  // Push UL BSR for DRB.
  ul_bsr_indication_message msg{
      to_du_cell_index(0), ue_idx, to_rnti(0x4601 + static_cast<unsigned>(ue_idx)), bsr_format::SHORT_BSR, {}};
  msg.reported_lcgs.push_back(ul_bsr_lcg_report{drb_lcg_id, drb_pending_bytes});
  this->ues[ue_idx].handle_bsr_indication(msg);
  // Push UL BSR for SRB.
  msg.reported_lcgs.clear();
  msg.reported_lcgs.push_back(ul_bsr_lcg_report{srb_lcg_id, srb_pending_bytes});
  this->ues[ue_idx].handle_bsr_indication(msg);

  for (unsigned count = 0, e = 10; count != e; ++count) {
    run_slot();

    auto next_ul_slice = slice_sched.get_next_ul_candidate();
    if (next_ul_slice.has_value()) {
      // Default SRB slice has very high priority.
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      const slice_ue_repository& srb_slice_ues = next_ul_slice->get_slice_ues();
      ASSERT_EQ(srb_slice_ues[ue_idx].pending_ul_newtx_bytes(), get_mac_sdu_required_bytes(srb_pending_bytes));

      // Default DRB slice is next candidate.
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_ul_slice->id(), DEFAULT_DRB_RAN_SLICE_ID);
      const slice_ue_repository& drb_slice_ues = next_ul_slice->get_slice_ues();
      ASSERT_EQ(drb_slice_ues[ue_idx].pending_ul_newtx_bytes(),
                get_mac_sdu_required_bytes(drb_pending_bytes + RLC_HEADER_SIZE_ESTIMATE));
    }
  }
}

// rb_ratio_slice_scheduler_test

class rb_ratio_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  static constexpr unsigned MIN_SLICE_RB = 10;
  static constexpr unsigned MAX_SLICE_RB = 20;

  static constexpr ran_slice_id_t drb1_slice_id{2};
  static constexpr ran_slice_id_t drb2_slice_id{3};

  rb_ratio_slice_scheduler_test() :
    slice_scheduler_test(
        {{{plmn_identity::test_value(), s_nssai_t{slice_service_type{1}}}, {MIN_SLICE_RB, MAX_SLICE_RB}},
         {{plmn_identity::test_value(), s_nssai_t{slice_service_type{2}}}, {MIN_SLICE_RB, MAX_SLICE_RB}}})
  {
  }

  const ue_configuration* add_ue(du_ue_index_t                                        ue_idx,
                                 const std::initializer_list<logical_channel_config>& lc_cfgs = {})
  {
    constexpr unsigned drb1_idx = 2;
    auto               req      = test_cfg.get_default_ue_config_request();
    req.ue_index                = ue_idx;
    req.crnti                   = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback      = false;
    if (lc_cfgs.size() == 0) {
      (*req.cfg.lc_config_list)[drb1_idx].rrm_policy.plmn_id = plmn_identity::test_value();
      // NOTE: if drb1_slice_id = 2 and drb1_slice_id = 3 have the same priority, the scheduler
      // returns drb1_slice_id = 2 as get_next_dl_candidate, as this was build first in the list of scheduler slices.
      // By setting slice_service_type{2} (mapping to drb3_slice_id) for this UE, we make sure that, when
      // get_next_dl_candidate() returns drb3_slice_id, it returns this because of its higher priority, not because of
      // the order in which the slices were built in the scheduler.
      (*req.cfg.lc_config_list)[drb1_idx].rrm_policy.s_nssai = s_nssai_t{slice_service_type{2}};
    } else {
      *req.cfg.lc_config_list = lc_cfgs;
    }
    return slice_scheduler_test::add_ue(req);
  }

  const ue_configuration* add_two_drb_ue(du_ue_index_t ue_idx)
  {
    rrm_policy_member slice1_nssai{.s_nssai = {slice_service_type{1}}};
    rrm_policy_member slice2_nssai{.s_nssai = {slice_service_type{2}}};
    auto              lc_chs = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB), slice1_nssai),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 1), slice2_nssai)};
    return this->add_ue(ue_idx, lc_chs);
  }
};

TEST_F(rb_ratio_slice_scheduler_test, when_slice_with_min_rb_has_ues_it_is_prioritized_over_empty_slice_with_no_ded_rb)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_MIN_DRB));
}

TEST_F(
    rb_ratio_slice_scheduler_test,
    when_slice_rb_ratios_are_min_bounded_but_there_are_no_other_slices_as_candidates_then_remaining_rbs_is_max_bounded)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_rb_ratios_are_min_bounded_and_there_are_other_slices_as_candidates_then_remaining_rbs_is_min_bounded)
{
  ASSERT_NE(this->add_two_drb_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_with_min_rb_is_partially_scheduled_then_it_is_never_a_candidate_again_for_the_same_slot)
{
  ASSERT_NE(this->add_two_drb_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  // We leave one RB empty (MIN_SLICE_RB - 1).
  next_dl_slice->store_grant(MIN_SLICE_RB - 1);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), 1);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_with_min_rb_is_allocated_until_min_rb_then_it_can_still_be_a_candidate_until_max_rb_is_reached)
{
  ASSERT_NE(this->add_two_drb_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice->store_grant(MIN_SLICE_RB);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), 0);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  // Original slice is selected again, now using maxRB ratio as the remaining RBs.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
  // No more slices to schedule.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_candidates_are_scheduled_in_a_slot_then_their_priority_gets_recomputed_in_next_slot)
{
  ASSERT_NE(this->add_two_drb_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice->store_grant(MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
  next_dl_slice->store_grant(MAX_SLICE_RB - MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());

  // New slot and priorities are reestablished.
  run_slot();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice->store_grant(MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slices_are_saturated_then_slices_should_have_equal_opportunity_to_reach_max_rbs)
{
  std::initializer_list<logical_channel_config> lc_cfgs = {
      config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB0),
      config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1),
      config_helpers::create_default_logical_channel_config(lcid_t::LCID_MIN_DRB),
      config_helpers::create_default_logical_channel_config(uint_to_lcid(lcid_t::LCID_MIN_DRB + 1))};
  ASSERT_NE(this->add_ue(to_du_ue_index(0), lc_cfgs), nullptr);

  unsigned max_nof_slots = 100;
  unsigned slice_3_count = 0;
  unsigned slice_2_count = 0;
  for (unsigned count = 0; count != max_nof_slots; ++count) {
    run_slot();

    // Either default SRB slice (Slice 0) or DRB1 slice (Slice 2) or DRB2 slice (Slice 3) is selected first since both
    // have minRBs > 0.
    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    if (not next_dl_slice.has_value()) {
      continue;
    }
    if (next_dl_slice->id() == drb1_slice_id or next_dl_slice->id() == drb2_slice_id) {
      next_dl_slice->store_grant(MIN_SLICE_RB);
    } else {
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
    }

    // Either Slice 0 or 2 or 3 are then selected.
    next_dl_slice = slice_sched.get_next_dl_candidate();
    if (next_dl_slice->id() == drb1_slice_id) {
      slice_2_count++;
      ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
      ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
    } else if (next_dl_slice->id() == drb2_slice_id) {
      slice_3_count++;
      ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
      ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
    }
  }

  ASSERT_EQ(slice_3_count, slice_2_count) << "Round-robin of slices of same priority failed";
}

// prioritised_slice_scheduler_test

class prioritised_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  static constexpr unsigned       MIN_SLICE_RB = 10;
  static constexpr unsigned       PRIORITY     = 1;
  static constexpr ran_slice_id_t drb1_slice_id{1};
  static constexpr ran_slice_id_t drb2_slice_id{2};
  static constexpr ran_slice_id_t drb3_slice_id{3};

  prioritised_slice_scheduler_test() :
    slice_scheduler_test(
        {{{plmn_identity::test_value(), s_nssai_t{slice_service_type{1}}}, {MIN_SLICE_RB, MAX_NOF_PRBS}, PRIORITY},
         {{plmn_identity::test_value(), s_nssai_t{slice_service_type{2}}}, {0, MAX_NOF_PRBS}, PRIORITY}})
  {
  }

  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    rrm_policy_member slice1_nssai{.s_nssai = {slice_service_type{1}}};
    rrm_policy_member slice2_nssai{.s_nssai = {slice_service_type{2}}};
    auto              req   = test_cfg.get_default_ue_config_request();
    req.ue_index            = ue_idx;
    req.crnti               = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback  = false;
    *req.cfg.lc_config_list = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(LCID_MIN_DRB),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 1), slice1_nssai),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 2), slice2_nssai)};
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(prioritised_slice_scheduler_test,
       when_drb_slice_with_min_rb_has_ues_then_it_is_prioritized_over_empty_slice_without_ded_rbs)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);

  for (unsigned count = 0, e = 10; count != e; ++count) {
    const bool is_pdcch_active = has_active_tdd_dl_symbols(cell_cfg.params.tdd_cfg.value(), next_slot.slot_index());
    const bool dl_slot_has_k2_values = not pusch_td_list_per_slot[count].empty();
    run_slot();

    if (is_pdcch_active) {
      auto next_dl_slice = slice_sched.get_next_dl_candidate();
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      // slice 1 (DRB2) is scheduled before slice 2 (DRB3) or default DRB slice because it has MIN_RB > 0.
      ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb3_slice_id);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
    }
    if (dl_slot_has_k2_values) {
      auto next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_TRUE(next_ul_slice.has_value());
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      // slice 1 (DRB2) is scheduled before slice 2 (DRB3) or default DRB slice because it has MIN_RB > 0.
      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), drb2_slice_id);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), drb3_slice_id);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), drb1_slice_id);
    }
    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    auto next_ul_slice = slice_sched.get_next_ul_candidate();
    ASSERT_FALSE(next_dl_slice.has_value());
    ASSERT_FALSE(next_ul_slice.has_value());
  }
}

class dedicated_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  static constexpr unsigned       DED_SLICE_RB = 10;
  static constexpr unsigned       MIN_SLICE_RB = 20;
  static constexpr unsigned       PRIORITY     = 1;
  static constexpr ran_slice_id_t drb1_slice_id{1};
  static constexpr ran_slice_id_t drb2_slice_id{2};
  static constexpr ran_slice_id_t drb3_slice_id{3};

  dedicated_slice_scheduler_test() :
    slice_scheduler_test(
        {{{plmn_identity::test_value(), s_nssai_t{slice_service_type{1}}},
          {DED_SLICE_RB, MIN_SLICE_RB, MAX_NOF_PRBS},
          PRIORITY},
         {{plmn_identity::test_value(), s_nssai_t{slice_service_type{2}}}, {0, MAX_NOF_PRBS}, PRIORITY}})
  {
  }

  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    rrm_policy_member slice1_nssai{.s_nssai = {slice_service_type{1}}};
    rrm_policy_member slice2_nssai{.s_nssai = {slice_service_type{2}}};
    auto              req   = test_cfg.get_default_ue_config_request();
    req.ue_index            = ue_idx;
    req.crnti               = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback  = false;
    *req.cfg.lc_config_list = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(LCID_MIN_DRB),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 1), slice1_nssai),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 2), slice2_nssai)};
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(dedicated_slice_scheduler_test, when_dedicated_resources_not_filled_then_they_remain_unused)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  constexpr unsigned drb2_grant_rbs = 7;
  constexpr unsigned drb3_grant_rbs = 50;
  const unsigned     max_prbs       = slice_sched.slice_config(static_cast<ran_slice_id_t>(0U)).rbs.max();

  for (unsigned count = 0, e = 10; count != e; ++count) {
    const bool is_pdcch_active = has_active_tdd_dl_symbols(cell_cfg.params.tdd_cfg.value(), next_slot.slot_index());
    const bool dl_slot_has_k2_values = not pusch_td_list_per_slot[count].empty();
    run_slot();

    if (is_pdcch_active) {
      auto next_dl_slice = slice_sched.get_next_dl_candidate();
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
      // Dedicated resources are reserved also for empty slices.
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      // Slice with prioritised and dedicated resources gets scheduled.
      ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
      // DRB2 slice can potentially use all RBs, as it's the only one with dedicated resources and no other slice has
      // been allocated anything so far.
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs);
      next_dl_slice->store_grant(drb2_grant_rbs);
      // Other slices get scheduled.
      next_dl_slice = slice_sched.get_next_dl_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb3_slice_id);
      // DRB3 slice can use max the all RBs minus those dedicated for DRB2.
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - std::max(DED_SLICE_RB, drb2_grant_rbs));
      next_dl_slice->store_grant(drb3_grant_rbs);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
      // DRB1 slice can use max the all RBs left after the dedicated for DRB2 + those allocated to DRB3 slices.
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - std::max(DED_SLICE_RB, drb2_grant_rbs) - drb3_grant_rbs);
    }
    if (dl_slot_has_k2_values) {
      auto next_ul_slice = slice_sched.get_next_ul_candidate();
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      // Dedicated resources are reserved also for empty slices.
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_TRUE(next_ul_slice.has_value());
      // Slice with prioritised and dedicated resources gets scheduled.
      ASSERT_EQ(next_ul_slice->id(), drb2_slice_id);
      // DRB2 slice can potentially use all RBs, as it's the only one with dedicated resources and no other slice has
      // been allocated anything so far.
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs);
      next_ul_slice->store_grant(drb2_grant_rbs);
      // Other slices get scheduled.
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), drb3_slice_id);
      // DRB3 slice can use max the all RBs minus those dedicated for DRB2.
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - std::max(DED_SLICE_RB, drb2_grant_rbs));
      next_ul_slice->store_grant(drb3_grant_rbs);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), drb1_slice_id);
      // DRB1 slice can use max the all RBs left after the dedicated for DRB2 + those allocated to DRB3 slices.
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - std::max(DED_SLICE_RB, drb2_grant_rbs) - drb3_grant_rbs);
    }
    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    auto next_ul_slice = slice_sched.get_next_ul_candidate();
    ASSERT_FALSE(next_dl_slice.has_value());
    ASSERT_FALSE(next_ul_slice.has_value());
  }
}

class dedicated_slice_scheduler_test_2nd : public slice_scheduler_test, public ::testing::Test
{
  // This test is a variation of \ref dedicated_slice_scheduler_test, but the order of the slices is reversed (the slice
  // with ded/min resources is the last one). This test verifies that the scheduler correctly computes the remaining
  // RBs when a slice generates 2 candidates with [0, rb_min] and [rb_min, rb_max], but their priorities is such that
  // the candidates [0, rb_min] and [rb_min, rb_max] are not adjacent when creating the candidate priority queue.
protected:
  static constexpr unsigned       DED_SLICE_RB = 10;
  static constexpr unsigned       MIN_SLICE_RB = DED_SLICE_RB;
  static constexpr unsigned       PRIORITY     = 1;
  static constexpr ran_slice_id_t drb1_slice_id{1};
  static constexpr ran_slice_id_t drb2_slice_id{2};
  static constexpr ran_slice_id_t drb3_slice_id{3};

  dedicated_slice_scheduler_test_2nd() :
    slice_scheduler_test(
        {{{plmn_identity::test_value(), s_nssai_t{slice_service_type{1}}}, {0, MAX_NOF_PRBS}, PRIORITY},
         {{plmn_identity::test_value(), s_nssai_t{slice_service_type{2}}},
          {DED_SLICE_RB, MIN_SLICE_RB, MAX_NOF_PRBS},
          PRIORITY}})
  {
  }

  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    rrm_policy_member slice1_nssai{.s_nssai = {slice_service_type{1}}};
    rrm_policy_member slice2_nssai{.s_nssai = {slice_service_type{2}}};
    auto              req   = test_cfg.get_default_ue_config_request();
    req.ue_index            = ue_idx;
    req.crnti               = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback  = false;
    *req.cfg.lc_config_list = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(LCID_MIN_DRB),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 1), slice1_nssai),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 2), slice2_nssai)};
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(dedicated_slice_scheduler_test_2nd, with_candidates_with_min_lim_remaining_rbs_are_computed_correctly)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  // Allocate the same grant for both drb2_slice_id and drb2_slice_id slices; this way, their priority will be the same
  // and the scheduler will always return drb2_slice_id candidate (which has no min priority) over the drb3_slice_id
  // candidate with rb_lim = [MIN_SLICE_RB, MAX_NOF_PRBS], as drb2_slice_id is created before drb3_slice_id in the
  // scheduler.
  constexpr unsigned drb2_3_grant_rbs = 50;
  const unsigned     max_prbs         = slice_sched.slice_config(static_cast<ran_slice_id_t>(0U)).rbs.max();

  for (unsigned count = 0, e = 10; count != e; ++count) {
    const bool is_pdcch_active = has_active_tdd_dl_symbols(cell_cfg.params.tdd_cfg.value(), next_slot.slot_index());
    const bool dl_slot_has_k2_values = not pusch_td_list_per_slot[count].empty();
    run_slot();

    if (is_pdcch_active) {
      auto next_dl_slice = slice_sched.get_next_dl_candidate();
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
      // Dedicated resources are reserved also for empty slices.
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      // Slice with prioritised and dedicated resources gets scheduled.
      ASSERT_EQ(next_dl_slice->id(), drb3_slice_id);
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs);
      next_dl_slice->store_grant(DED_SLICE_RB);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_dl_slice->store_grant(drb2_3_grant_rbs);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      ASSERT_TRUE(next_dl_slice->id() == drb3_slice_id);
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - DED_SLICE_RB - drb2_3_grant_rbs);
      next_dl_slice->store_grant(drb2_3_grant_rbs - DED_SLICE_RB);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - std::max(DED_SLICE_RB, drb2_3_grant_rbs) - drb2_3_grant_rbs);
    }

    if (dl_slot_has_k2_values) {
      auto next_ul_slice = slice_sched.get_next_ul_candidate();
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      // Dedicated resources are reserved also for empty slices.
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_TRUE(next_ul_slice.has_value());
      // Slice with prioritised and dedicated resources gets scheduled.
      ASSERT_EQ(next_ul_slice->id(), drb3_slice_id);
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs);
      next_ul_slice->store_grant(DED_SLICE_RB);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_ul_slice->id(), drb2_slice_id);
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_ul_slice->store_grant(drb2_3_grant_rbs);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_TRUE(next_ul_slice->id() == drb3_slice_id);
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - DED_SLICE_RB - drb2_3_grant_rbs);
      next_ul_slice->store_grant(drb2_3_grant_rbs - DED_SLICE_RB);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_ul_slice->id(), drb1_slice_id);
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - std::max(DED_SLICE_RB, drb2_3_grant_rbs) - drb2_3_grant_rbs);
    }
    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    auto next_ul_slice = slice_sched.get_next_ul_candidate();
    ASSERT_FALSE(next_dl_slice.has_value());
    ASSERT_FALSE(next_ul_slice.has_value());
  }
}

class dedicated_empty_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  static constexpr unsigned DED_SLICE_RB = 10;
  static constexpr unsigned MIN_SLICE_RB = 20;
  static constexpr unsigned PRIORITY     = 1;
  // Default DRB slice.
  static constexpr ran_slice_id_t drb1_slice_id{1};
  // Non-default DRB slices.
  static constexpr ran_slice_id_t drb2_slice_id{2};
  static constexpr ran_slice_id_t drb3_slice_id{3};

  dedicated_empty_slice_scheduler_test() :
    slice_scheduler_test(
        {{{plmn_identity::test_value(), s_nssai_t{slice_service_type{1}}},
          {DED_SLICE_RB, MIN_SLICE_RB, MAX_NOF_PRBS},
          PRIORITY},
         {{plmn_identity::test_value(), s_nssai_t{slice_service_type{2}}}, {MIN_SLICE_RB, MAX_NOF_PRBS}, PRIORITY}})
  {
  }

  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    rrm_policy_member slice_nssai{.s_nssai = {slice_service_type{2}}};
    auto              req   = test_cfg.get_default_ue_config_request();
    req.ue_index            = ue_idx;
    req.crnti               = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback  = false;
    *req.cfg.lc_config_list = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(LCID_MIN_DRB),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 2), slice_nssai)};
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(dedicated_empty_slice_scheduler_test, when_slice_has_no_ues_its_rbs_will_still_remain_unused)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  const unsigned     max_prbs       = slice_sched.slice_config(static_cast<ran_slice_id_t>(0)).rbs.max();
  constexpr unsigned drb3_grant_rbs = 40;

  for (unsigned count = 0, e = 10; count != e; ++count) {
    const bool is_pdcch_active = has_active_tdd_dl_symbols(cell_cfg.params.tdd_cfg.value(), next_slot.slot_index());
    const bool dl_slot_has_k2_values = not pusch_td_list_per_slot[count].empty();
    run_slot();

    if (is_pdcch_active) {
      auto next_dl_slice = slice_sched.get_next_dl_candidate();
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
      // Dedicated resources are reserved also for empty slices;
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb3_slice_id);
      // DRB3 slice cannot be assigned all RBs, as there DRB2 has dedicated RBs.
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      next_dl_slice->store_grant(drb3_grant_rbs);
      ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
      // DRB1 slice can use max the all RBs left after the dedicated for DRB2 + those allocated to DRB3 slices.
      ASSERT_LE(next_dl_slice->remaining_rbs(), max_prbs - DED_SLICE_RB - drb3_grant_rbs);
    }

    if (dl_slot_has_k2_values) {
      auto next_ul_slice = slice_sched.get_next_ul_candidate();
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_ul_slice->id(), drb3_slice_id);
      // DRB3 slice cannot be assigned all RBs, as there DRB2 has dedicated RBs.
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_ul_slice->store_grant(drb3_grant_rbs);
      next_ul_slice = slice_sched.get_next_ul_candidate();
      // Dedicated resources are reserved also for empty slices.
      ASSERT_EQ(next_ul_slice->id(), drb1_slice_id);
      // DRB1 slice can use max the all RBs left after the dedicated for DRB2 + those allocated to DRB3 slices.
      ASSERT_LE(next_ul_slice->remaining_rbs(), max_prbs - DED_SLICE_RB - drb3_grant_rbs);
    }
  }
}
