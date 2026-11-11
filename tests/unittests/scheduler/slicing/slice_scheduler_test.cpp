// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/config/time_domain_mapper.h"
#include "lib/scheduler/slicing/inter_slice_scheduler.h"
#include "lib/scheduler/ue_context/ue_cell_repository.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/config/logical_channel_config_factory.h"
#include <algorithm>
#include <array>
#include <functional>
#include <gtest/gtest.h>
#include <set>

using namespace ocudu;

static constexpr logical_channel_config create_logical_channel_config(lcid_t lcid, rrm_policy_member policy = {})
{
  logical_channel_config lc_ch = config_helpers::create_default_logical_channel_config(lcid);
  lc_ch.rrm_policy             = policy;
  // Each DRB gets its own LCG, as an LCG belongs to a single slice.
  if (lcid >= LCID_MIN_DRB) {
    lc_ch.lc_group = uint_to_lcg_id(std::min<unsigned>(lcid - LCID_MIN_DRB + 1, MAX_LCG_ID));
  }
  return lc_ch;
}

class slice_scheduler_test
{
protected:
  using cell_cfg_customiser = std::function<void(sched_cell_configuration_request_message&)>;

  slice_scheduler_test(const std::vector<slice_rrm_policy_config>& rrm_policy_members = {},
                       const cell_cfg_customiser&                  customise_cell     = {}) :
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
    cell_cfg([this, rrm_policy_members, customise_cell]() -> const cell_configuration& {
      auto req               = test_cfg.get_default_cell_config_request();
      req.rrm_policy_members = rrm_policy_members;
      if (customise_cell) {
        customise_cell(req);
      }
      return *test_cfg.add_cell(req);
    }()),
    ues(cell_cfg.expert_cfg.ue)
  {
    logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    ues.register_cell(cell_ues);
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
    ues.add_ue(*ue_cfg, {sched_config_helper::to_ue_creation_mode(req), req.ul_ccch_slot_rx});
    slice_sched.add_ue(req.ue_index);
    // Slices without pending data are not candidates.
    push_pending_data(req.ue_index);
    return ue_cfg;
  }

  /// Gives pending DL and UL data to all the bearers of a UE.
  void push_pending_data(du_ue_index_t ue_idx)
  {
    static constexpr unsigned pending_bytes = 100;
    ue&                       u             = ues[ue_idx];
    ul_bsr_indication_message bsr{to_du_cell_index(0), ue_idx, u.crnti, bsr_format::LONG_BSR, {}};
    for (logical_channel_config_ptr lc : u.logical_channels().cfg()) {
      if (lc->lcid == LCID_SRB0) {
        continue;
      }
      u.handle_dl_buffer_state_indication(lc->lcid, pending_bytes);
      if (std::none_of(bsr.reported_lcgs.begin(), bsr.reported_lcgs.end(), [&lc](const ul_bsr_lcg_report& r) {
            return r.lcg_id == lc->lc_group;
          })) {
        bsr.reported_lcgs.push_back(ul_bsr_lcg_report{lc->lc_group, pending_bytes});
      }
    }
    u.handle_bsr_indication(bsr);
  }

  ocudulog::basic_logger&                 logger = ocudulog::fetch_basic_logger("TEST");
  test_helpers::test_sched_config_manager test_cfg;
  const cell_configuration&               cell_cfg;

  ue_cell_repository cell_ues{cell_cfg, nullptr};
  ue_repository      ues;

  cell_resource_allocator dummy_alloc{cell_cfg};
  inter_slice_scheduler   slice_sched{cell_cfg, ues};

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
  while (cell_cfg.init_bwp.ul.td_mapper().common_pusch_td_res_indices((next_slot - 1).count()).empty()) {
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
    const bool dl_slot_has_k2_values = not cell_cfg.init_bwp.ul.td_mapper().common_pusch_td_res_indices(count).empty();
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

TEST_F(default_slice_scheduler_test, when_ue_is_reconfigured_outside_an_rrc_procedure_then_it_stays_in_its_slices)
{
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  ASSERT_NE(this->add_ue(ue_idx), nullptr);

  // Reconfiguration that does not move the UE to fallback.
  auto req                       = test_cfg.get_default_ue_config_request();
  req.cfg.cause                  = sched_ue_config_request::causes::not_rrc_proc;
  const ue_configuration* ue_cfg = test_cfg.update_ue(
      sched_ue_reconfiguration_message{.ue_index = ue_idx, .crnti = to_rnti(0x4601), .cfg = req.cfg});
  ASSERT_NE(ue_cfg, nullptr);
  ues.reconfigure_ue(*ue_cfg, req.cfg.cause);
  slice_sched.reconf_ue(ue_idx);
  ASSERT_FALSE(ues[ue_idx].get_pcell().is_in_fallback_mode());

  // Without waiting for config_applied, the UE is still a candidate of its slices.
  std::optional<dl_ran_slice_candidate> next_dl_slice;
  for (unsigned count = 0; count != 10 and not next_dl_slice.has_value(); ++count) {
    run_slot();
    next_dl_slice = slice_sched.get_next_dl_candidate();
  }
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_TRUE(next_dl_slice->is_candidate(ue_idx, lcid_t::LCID_SRB1));
}

TEST_F(default_slice_scheduler_test, when_slice_ues_have_no_pending_data_then_slice_is_not_a_candidate)
{
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  ASSERT_NE(this->add_ue(ue_idx), nullptr);

  // Flush the pending data of the UE.
  ue& u = ues[ue_idx];
  for (logical_channel_config_ptr lc : u.logical_channels().cfg()) {
    u.handle_dl_buffer_state_indication(lc->lcid, 0);
  }
  ul_bsr_indication_message bsr{to_du_cell_index(0), ue_idx, u.crnti, bsr_format::LONG_BSR, {}};
  for (logical_channel_config_ptr lc : u.logical_channels().cfg()) {
    bsr.reported_lcgs.push_back(ul_bsr_lcg_report{lc->lc_group, 0});
  }
  u.handle_bsr_indication(bsr);

  // Neither DL nor UL candidates while there is no pending data.
  for (unsigned count = 0; count != 10; ++count) {
    run_slot();
    ASSERT_FALSE(slice_sched.get_next_dl_candidate().has_value());
    ASSERT_FALSE(slice_sched.get_next_ul_candidate().has_value());
  }

  // DL data makes the slice of the bearer a DL candidate only.
  u.handle_dl_buffer_state_indication(LCID_SRB1, 100);
  std::optional<dl_ran_slice_candidate> next_dl_slice;
  for (unsigned count = 0; count != 10 and not next_dl_slice.has_value(); ++count) {
    run_slot();
    next_dl_slice = slice_sched.get_next_dl_candidate();
    ASSERT_FALSE(slice_sched.get_next_ul_candidate().has_value());
  }
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  ASSERT_FALSE(slice_sched.get_next_dl_candidate().has_value());
}

TEST_F(default_slice_scheduler_test, when_deactivated_ue_gets_config_applied_then_no_channels_are_re_added_to_slices)
{
  // Regression for issue #573: a stale config-applied on a UE that was already deactivated (bearers cleared) but not
  // yet erased must not attempt to re-map its logical channels to slices.
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  ASSERT_NE(this->add_ue(ue_idx), nullptr);
  run_slot();
  ASSERT_TRUE(slice_sched.get_next_dl_candidate().has_value());

  // Deactivation clears the UE bearers in the logical channel system while leaving the UE (and its dedicated config)
  // in the repository.
  this->ues.deactivate_ue(ue_idx);
  ASSERT_TRUE(this->ues[ue_idx].logical_channels().cfg().logical_channels().empty());

  // Re-driving the slice add path must be a no-op instead of asserting on an unconfigured LCID.
  slice_sched.config_applied(ue_idx);

  // Scheduler remains healthy on the following slots.
  run_slot();
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
      ASSERT_EQ(srb_slice_ues[ue_idx].pending_ul_newtx_bytes(),
                units::bytes{get_mac_sdu_required_bytes(srb_pending_bytes)});

      // Default DRB slice is next candidate.
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_ul_slice->id(), DEFAULT_DRB_RAN_SLICE_ID);
      const slice_ue_repository& drb_slice_ues = next_ul_slice->get_slice_ues();
      ASSERT_EQ(drb_slice_ues[ue_idx].pending_ul_newtx_bytes(),
                units::bytes{get_mac_sdu_required_bytes(drb_pending_bytes + RLC_HEADER_SIZE_ESTIMATE)});
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
    OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-17-3-e");
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

TEST_F(rb_ratio_slice_scheduler_test,
       when_ue_is_reconfigured_outside_an_rrc_procedure_then_its_slices_follow_new_config)
{
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  const lcid_t        drb1   = uint_to_lcid(LCID_MIN_DRB);
  const lcid_t        drb2   = uint_to_lcid(LCID_MIN_DRB + 1);
  ASSERT_NE(this->add_two_drb_ue(ue_idx), nullptr);

  // DRB1 moves to the slice of DRB2, and DRB2 is removed.
  auto req                = test_cfg.get_default_ue_config_request();
  *req.cfg.lc_config_list = std::initializer_list<logical_channel_config>{
      create_logical_channel_config(LCID_SRB0),
      create_logical_channel_config(LCID_SRB1),
      create_logical_channel_config(drb1, {.s_nssai = {slice_service_type{2}}})};
  req.cfg.cause                  = sched_ue_config_request::causes::not_rrc_proc;
  const ue_configuration* ue_cfg = test_cfg.update_ue(
      sched_ue_reconfiguration_message{.ue_index = ue_idx, .crnti = to_rnti(0x4601), .cfg = req.cfg});
  ASSERT_NE(ue_cfg, nullptr);
  ues.reconfigure_ue(*ue_cfg, req.cfg.cause);
  slice_sched.reconf_ue(ue_idx);

  // Without waiting for config_applied, each slice holds the UE bearers of the new config.
  std::map<ran_slice_id_t, std::array<bool, 3>> membership;
  for (unsigned count = 0; count != 10 and membership.empty(); ++count) {
    run_slot();
    for (auto candidate = slice_sched.get_next_dl_candidate(); candidate.has_value();
         candidate      = slice_sched.get_next_dl_candidate()) {
      membership[candidate->id()] = {candidate->is_candidate(ue_idx, LCID_SRB1),
                                     candidate->is_candidate(ue_idx, drb1),
                                     candidate->is_candidate(ue_idx, drb2)};
    }
  }
  ASSERT_EQ(membership.count(SRB_RAN_SLICE_ID), 1);
  ASSERT_EQ(membership.count(drb2_slice_id), 1);
  ASSERT_EQ(membership.at(SRB_RAN_SLICE_ID), (std::array<bool, 3>{true, false, false}));
  // The slice of DRB1 has no UE left, so it is not a candidate.
  ASSERT_EQ(membership.count(drb1_slice_id), 0);
  ASSERT_EQ(membership.at(drb2_slice_id), (std::array<bool, 3>{false, true, false}));
}

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
    OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-17-3-e");
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
    const bool dl_slot_has_k2_values = not cell_cfg.init_bwp.ul.td_mapper().common_pusch_td_res_indices(count).empty();
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
    OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-17-3-e");
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
    const bool dl_slot_has_k2_values = not cell_cfg.init_bwp.ul.td_mapper().common_pusch_td_res_indices(count).empty();
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
    OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-17-3-e");
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
    const bool dl_slot_has_k2_values = not cell_cfg.init_bwp.ul.td_mapper().common_pusch_td_res_indices(count).empty();
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
    OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-17-3-e");
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
    const bool dl_slot_has_k2_values = not cell_cfg.init_bwp.ul.td_mapper().common_pusch_td_res_indices(count).empty();
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

/// Cell where the UEs may apply a Differential Koffset of their own on top of the cell-specific one.
class ntn_koffset_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  /// Cell-specific Koffset.
  static constexpr std::chrono::milliseconds cell_koffset{20};
  /// Differential Koffset the UE is made to apply.
  static constexpr std::chrono::milliseconds ue_differential_koffset{8};
  /// Margin added to the reported T_TA when deriving a UE Koffset.
  static constexpr std::chrono::milliseconds ue_koffset_margin{2};

  ntn_koffset_slice_scheduler_test() :
    slice_scheduler_test({}, [](sched_cell_configuration_request_message& req) {
      req.ran.ntn_params.emplace();
      req.ran.ntn_params->ntn_cfg.cell_specific_koffset = cell_koffset;
      req.ran.ntn_params->ue_koffset_cfg                = ue_koffset_config{ue_koffset_margin};
    })
  {
  }

  /// Adds a UE out of fallback with the given index, and returns its configuration.
  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    auto req               = test_cfg.get_default_ue_config_request();
    req.ue_index           = ue_idx;
    req.crnti              = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback = false;
    return slice_scheduler_test::add_ue(req);
  }

  /// Makes the UE apply \c koffset, so that its uplink reaches a slot earlier than the cell-specific Koffset leads to.
  void apply_differential_koffset(du_ue_index_t ue_idx, std::chrono::milliseconds koffset = ue_differential_koffset)
  {
    ue_koffset_tracker& tracker = ues[ue_idx].koffset_tracker();
    tracker.handle_ce_sent(koffset, next_slot);
    tracker.handle_ce_acked();
    tracker.slot_indication(next_slot, slot_point{});
  }

  /// Returns the Koffset in slots of a UE applying \c koffset.
  unsigned ue_koffset_slots(std::chrono::milliseconds koffset = ue_differential_koffset) const
  {
    return cell_cfg.ntn_cs_koffset - koffset.count() * get_nof_slots_per_subframe(cell_cfg.scs_common());
  }
};

TEST_F(ntn_koffset_slice_scheduler_test, ul_slice_candidate_carries_the_k2_its_slot_was_built_from)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  apply_differential_koffset(to_du_ue_index(0));
  ASSERT_LT(ue_koffset_slots(), cell_cfg.ntn_cs_koffset) << "The UE did not apply a Differential Koffset";

  const span<const pusch_time_domain_resource_allocation> pusch_td_list =
      cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;

  // Slots over which the UL slice candidates are checked.
  static constexpr unsigned nof_slots_to_run = 20;

  unsigned nof_candidates            = 0;
  unsigned nof_ue_koffset_candidates = 0;
  for (unsigned count = 0; count != nof_slots_to_run; ++count) {
    const slot_point pdcch_slot = next_slot;
    run_slot();

    for (auto candidate = slice_sched.get_next_ul_candidate(); candidate.has_value();
         candidate      = slice_sched.get_next_ul_candidate()) {
      // The k2 the candidate reports is one the cell offers, and its slot follows from that k2 and one of the
      // Koffsets in use. Deriving it from the cell-specific Koffset alone gets the UE-specific candidates wrong.
      const unsigned k2    = candidate->get_k2();
      const unsigned delay = candidate->get_slot_tx() - pdcch_slot;
      ASSERT_TRUE(std::any_of(pusch_td_list.begin(),
                              pusch_td_list.end(),
                              [k2](const pusch_time_domain_resource_allocation& td_res) { return td_res.k2 == k2; }))
          << "k2=" << k2 << " of the candidate is not a PUSCH TD resource of the cell";
      ASSERT_EQ(delay, k2 + candidate->get_ntn_koffset())
          << "Candidate scheduled " << delay
          << " slots after its PDCCH does not follow from the k2 and the Koffset "
             "it carries";
      ASSERT_TRUE(candidate->get_ntn_koffset() == cell_cfg.ntn_cs_koffset or
                  candidate->get_ntn_koffset() == ue_koffset_slots())
          << "Candidate carries a Koffset no UE of the cell applies";
      ++nof_candidates;
      nof_ue_koffset_candidates += candidate->get_ntn_koffset() == ue_koffset_slots() ? 1 : 0;
    }
  }
  ASSERT_GT(nof_candidates, 0U) << "No uplink slice candidate was offered";
  ASSERT_GT(nof_ue_koffset_candidates, 0U) << "No candidate was offered for the Koffset the UE applies, so the slot "
                                              "the candidates carry says nothing about their k2";
}

TEST_F(ntn_koffset_slice_scheduler_test, every_koffset_in_use_is_offered_candidates_over_time)
{
  // More distinct Koffsets than MAX_NOF_NTN_KOFFSETS_PER_SLICE holds, so no slot can carry a candidate for all of
  // them. Which ones are left out has to change from slot to slot, or the same UEs are never scheduled.
  static constexpr std::array<std::chrono::milliseconds, 5> ue_koffsets{std::chrono::milliseconds{2},
                                                                        std::chrono::milliseconds{4},
                                                                        std::chrono::milliseconds{6},
                                                                        std::chrono::milliseconds{8},
                                                                        std::chrono::milliseconds{10}};
  ASSERT_GT(ue_koffsets.size() + 1, MAX_NOF_NTN_KOFFSETS_PER_SLICE) << "The Koffsets of this test all fit in a slot";

  for (unsigned ue_count = 0, e = ue_koffsets.size(); ue_count != e; ++ue_count) {
    const du_ue_index_t ue_idx = to_du_ue_index(ue_count);
    ASSERT_NE(this->add_ue(ue_idx), nullptr);
    apply_differential_koffset(ue_idx, ue_koffsets[ue_count]);
  }

  // Slots over which the offered Koffsets are collected.
  static constexpr unsigned nof_slots_to_run = 40;

  std::set<unsigned> koffsets_offered;
  for (unsigned count = 0; count != nof_slots_to_run; ++count) {
    run_slot();
    for (auto candidate = slice_sched.get_next_ul_candidate(); candidate.has_value();
         candidate      = slice_sched.get_next_ul_candidate()) {
      koffsets_offered.insert(candidate->get_ntn_koffset());
    }
  }

  for (const std::chrono::milliseconds koffset : ue_koffsets) {
    ASSERT_EQ(koffsets_offered.count(ue_koffset_slots(koffset)), 1U)
        << "No candidate was ever offered for the Koffset of " << koffset.count() << "ms, so its UE is never scheduled";
  }
}

// dynamic_slice_scheduler_test

class dynamic_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  static constexpr unsigned MIN_SLICE_RB = 10;
  static constexpr unsigned MAX_SLICE_RB = 20;
  static constexpr unsigned DED_SLICE_RB = 5;

  static rrm_policy_member make_member(uint8_t sst)
  {
    return {plmn_identity::test_value(), s_nssai_t{slice_service_type{sst}}};
  }

  static du_cell_slice_reconfig_request::rrm_policy_config make_policy(uint8_t sst, rrm_policy_ratio_rb_limits rbs)
  {
    return {make_member(sst), rbs};
  }

  void create_slice(uint8_t sst, rrm_policy_ratio_rb_limits rbs = {MIN_SLICE_RB, MAX_SLICE_RB})
  {
    du_cell_slice_reconfig_request req;
    req.cell_index = to_du_cell_index(0);
    req.policies_to_create.push_back(make_policy(sst, rbs));
    slice_sched.handle_slice_reconfiguration_request(req);
  }

  void remove_slice(uint8_t sst)
  {
    du_cell_slice_reconfig_request req;
    req.cell_index = to_du_cell_index(0);
    req.policies_to_remove.push_back(make_member(sst));
    slice_sched.handle_slice_reconfiguration_request(req);
  }

  /// Adds a UE with one DRB mapped to the slice of the given SST, or to the default DRB slice if SST is zero.
  const ue_configuration* add_ue(du_ue_index_t ue_idx, uint8_t sst)
  {
    auto req                = test_cfg.get_default_ue_config_request();
    req.ue_index            = ue_idx;
    req.crnti               = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback  = false;
    *req.cfg.lc_config_list = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(LCID_MIN_DRB, sst == 0 ? rrm_policy_member{} : make_member(sst))};
    return slice_scheduler_test::add_ue(req);
  }

  /// Adds a UE with a DRB in the slice of each of the given SSTs.
  const ue_configuration* add_ue_with_two_drbs(du_ue_index_t ue_idx, uint8_t sst1, uint8_t sst2)
  {
    auto req                = test_cfg.get_default_ue_config_request();
    req.ue_index            = ue_idx;
    req.crnti               = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback  = false;
    *req.cfg.lc_config_list = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(LCID_MIN_DRB, make_member(sst1)),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 1), make_member(sst2))};
    return slice_scheduler_test::add_ue(req);
  }

  /// Allocates a UL HARQ of the UE sent for a slice, and NACKs it so that it has a retransmission pending.
  void nack_ul_harq(du_ue_index_t ue_idx, ran_slice_id_t slice_id)
  {
    auto h_ul = ues[ue_idx].get_pcell().harqs.alloc_ul_harq(next_slot, next_slot, 4);
    ASSERT_TRUE(h_ul.has_value());
    ul_harq_alloc_context ctxt;
    ctxt.dci_cfg_type = dci_ul_rnti_config_type::c_rnti_f0_1;
    ctxt.slice_id     = slice_id;
    pusch_information pusch;
    pusch.rnti          = ues[ue_idx].crnti;
    pusch.harq_id       = h_ul->id();
    pusch.new_data      = true;
    pusch.tb_size_bytes = units::bytes{100};
    h_ul->save_grant_params(ctxt, pusch);
    h_ul->ul_crc_info(false);
  }

  /// Whether a UL HARQ of the UE has a retransmission pending.
  bool has_pending_ul_retx(du_ue_index_t ue_idx)
  {
    ue_cell& ue_cc = ues[ue_idx].get_pcell();
    for (unsigned i = 0, e = ue_cc.harqs.nof_ul_harqs(); i != e; ++i) {
      if (auto h = ue_cc.harqs.ul_harq(to_harq_id(i)); h.has_value() and h->has_pending_retx()) {
        return true;
      }
    }
    return false;
  }

  /// Runs slots until one carries a PDCCH, and returns the DL candidates of that slot, in the order they are offered.
  std::vector<dl_ran_slice_candidate> run_dl_slot()
  {
    while (not has_active_tdd_dl_symbols(cell_cfg.params.tdd_cfg.value(), next_slot.slot_index())) {
      run_slot();
    }
    run_slot();
    std::vector<dl_ran_slice_candidate> candidates;
    while (auto candidate = slice_sched.get_next_dl_candidate()) {
      candidates.push_back(std::move(candidate.value()));
    }
    return candidates;
  }

  /// Returns the ID of the slice, other than the SRB one, that has the UE as a candidate.
  std::optional<ran_slice_id_t> drb_slice_of(du_ue_index_t ue_idx)
  {
    for (const dl_ran_slice_candidate& candidate : run_dl_slot()) {
      if (candidate.id() != SRB_RAN_SLICE_ID and candidate.is_candidate(ue_idx, LCID_MIN_DRB)) {
        return candidate.id();
      }
    }
    return std::nullopt;
  }
};

TEST_F(dynamic_slice_scheduler_test, when_a_slice_is_created_then_its_limits_are_capped_to_the_cell_and_it_is_dynamic)
{
  ASSERT_EQ(slice_sched.nof_slices(), 2);

  create_slice(1, {MAX_NOF_PRBS, MAX_NOF_PRBS, MAX_NOF_PRBS});

  ASSERT_EQ(slice_sched.nof_slices(), 3);
  const slice_rrm_policy_config& cfg = slice_sched.slice_config(ran_slice_id_t{2});
  ASSERT_EQ(cfg.rrc_member, make_member(1));
  ASSERT_TRUE(slice_sched.is_dynamic(ran_slice_id_t{2}));
  ASSERT_EQ(cfg.rbs.dedicated(), cell_cfg.nof_dl_prbs);
  ASSERT_EQ(cfg.rbs.min(), cell_cfg.nof_dl_prbs);
  ASSERT_EQ(cfg.rbs.max(), cell_cfg.nof_dl_prbs);
}

TEST_F(dynamic_slice_scheduler_test, when_a_slice_is_created_then_ues_of_its_policy_member_are_served_by_it)
{
  create_slice(1);
  ASSERT_NE(this->add_ue(to_du_ue_index(0), 1), nullptr);
  ASSERT_NE(this->add_ue(to_du_ue_index(1), 0), nullptr);

  ASSERT_EQ(drb_slice_of(to_du_ue_index(0)), ran_slice_id_t{2});
  ASSERT_EQ(drb_slice_of(to_du_ue_index(1)), DEFAULT_DRB_RAN_SLICE_ID);
}

TEST_F(dynamic_slice_scheduler_test, when_the_slice_of_a_policy_member_exists_then_its_creation_is_ignored)
{
  create_slice(1, {MIN_SLICE_RB, MAX_SLICE_RB});
  create_slice(1, {0, MAX_NOF_PRBS});

  ASSERT_EQ(slice_sched.nof_slices(), 3);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{2}).rbs.min(), MIN_SLICE_RB);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{2}).rbs.max(), MAX_SLICE_RB);
}

TEST_F(dynamic_slice_scheduler_test, when_a_slice_is_removed_then_its_ues_are_served_by_the_default_drb_slice)
{
  create_slice(1);
  ASSERT_NE(this->add_ue(to_du_ue_index(0), 1), nullptr);
  ASSERT_EQ(drb_slice_of(to_du_ue_index(0)), ran_slice_id_t{2});

  remove_slice(1);

  ASSERT_EQ(slice_sched.nof_slices(), 2);
  ASSERT_EQ(drb_slice_of(to_du_ue_index(0)), DEFAULT_DRB_RAN_SLICE_ID);
}

TEST_F(dynamic_slice_scheduler_test, when_a_slice_other_than_the_last_is_removed_then_the_other_slices_keep_their_ids)
{
  create_slice(1);
  create_slice(2);
  create_slice(3);
  ASSERT_NE(this->add_ue(to_du_ue_index(0), 1), nullptr);
  ASSERT_NE(this->add_ue(to_du_ue_index(1), 2), nullptr);
  ASSERT_NE(this->add_ue(to_du_ue_index(2), 3), nullptr);
  ASSERT_EQ(drb_slice_of(to_du_ue_index(2)), ran_slice_id_t{4});

  remove_slice(2);

  ASSERT_EQ(slice_sched.nof_slices(), 4);
  ASSERT_EQ(slice_sched.nof_slice_ids(), 5);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{4}).rrc_member, make_member(3));
  ASSERT_EQ(drb_slice_of(to_du_ue_index(0)), ran_slice_id_t{2});
  ASSERT_EQ(drb_slice_of(to_du_ue_index(1)), DEFAULT_DRB_RAN_SLICE_ID);
  ASSERT_EQ(drb_slice_of(to_du_ue_index(2)), ran_slice_id_t{4});
}

TEST_F(dynamic_slice_scheduler_test, when_a_slice_is_removed_then_the_retxs_pending_for_it_are_cancelled)
{
  create_slice(1);
  create_slice(2);
  ASSERT_NE(this->add_ue(to_du_ue_index(0), 1), nullptr);
  ASSERT_NE(this->add_ue(to_du_ue_index(1), 2), nullptr);
  nack_ul_harq(to_du_ue_index(0), ran_slice_id_t{2});
  nack_ul_harq(to_du_ue_index(1), ran_slice_id_t{3});
  ASSERT_TRUE(has_pending_ul_retx(to_du_ue_index(0)));
  ASSERT_TRUE(has_pending_ul_retx(to_du_ue_index(1)));

  remove_slice(1);

  // Only the retransmission of the removed slice is cancelled.
  ASSERT_FALSE(has_pending_ul_retx(to_du_ue_index(0)));
  ASSERT_TRUE(has_pending_ul_retx(to_du_ue_index(1)));
}

TEST_F(dynamic_slice_scheduler_test, when_a_slice_is_removed_then_its_ues_keep_the_bearers_of_the_other_slices)
{
  create_slice(1);
  create_slice(2);
  ASSERT_NE(this->add_ue_with_two_drbs(to_du_ue_index(0), 1, 2), nullptr);

  remove_slice(1);

  // The bearer of the removed slice is served by the default DRB slice, and the other one stays where it was.
  bool drb1_in_default = false;
  bool drb2_in_slice_3 = false;
  for (const dl_ran_slice_candidate& candidate : run_dl_slot()) {
    drb1_in_default |=
        candidate.id() == DEFAULT_DRB_RAN_SLICE_ID and candidate.is_candidate(to_du_ue_index(0), LCID_MIN_DRB);
    drb2_in_slice_3 |= candidate.id() == ran_slice_id_t{3} and
                       candidate.is_candidate(to_du_ue_index(0), uint_to_lcid(LCID_MIN_DRB + 1));
  }
  ASSERT_TRUE(drb1_in_default);
  ASSERT_TRUE(drb2_in_slice_3);
}

TEST_F(dynamic_slice_scheduler_test, when_a_slice_is_removed_then_a_slice_created_next_is_empty)
{
  create_slice(1);
  create_slice(2);
  ASSERT_NE(this->add_ue(to_du_ue_index(0), 2), nullptr);
  remove_slice(2);
  create_slice(3);

  // The slice takes the ID that was freed.
  ASSERT_EQ(slice_sched.nof_slices(), 4);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{3}).rrc_member, make_member(3));
  ASSERT_EQ(drb_slice_of(to_du_ue_index(0)), DEFAULT_DRB_RAN_SLICE_ID);
  for (const dl_ran_slice_candidate& candidate : run_dl_slot()) {
    ASSERT_NE(candidate.id(), ran_slice_id_t{3});
  }
}

TEST_F(dynamic_slice_scheduler_test, when_a_slice_with_dedicated_rbs_is_removed_then_its_rbs_are_released)
{
  create_slice(1, {DED_SLICE_RB, MIN_SLICE_RB, MAX_SLICE_RB});
  ASSERT_NE(this->add_ue(to_du_ue_index(0), 0), nullptr);

  // Dedicated resources are reserved also for an empty slice.
  std::vector<dl_ran_slice_candidate> candidates = run_dl_slot();
  ASSERT_FALSE(candidates.empty());
  ASSERT_EQ(candidates.front().id(), SRB_RAN_SLICE_ID);
  ASSERT_LE(candidates.front().remaining_rbs(), cell_cfg.nof_dl_prbs - DED_SLICE_RB);

  candidates.clear();
  remove_slice(1);

  candidates = run_dl_slot();
  ASSERT_FALSE(candidates.empty());
  ASSERT_EQ(candidates.front().id(), SRB_RAN_SLICE_ID);
  ASSERT_EQ(candidates.front().remaining_rbs(), cell_cfg.nof_dl_prbs);
}

TEST_F(dynamic_slice_scheduler_test, when_the_dedicated_rbs_of_the_slices_exceed_the_cell_then_the_creation_is_ignored)
{
  const unsigned more_than_half = cell_cfg.nof_dl_prbs / 2 + 1;
  create_slice(1, {more_than_half, more_than_half, MAX_NOF_PRBS});
  create_slice(2, {more_than_half, more_than_half, MAX_NOF_PRBS});

  ASSERT_EQ(slice_sched.nof_slices(), 3);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{2}).rrc_member, make_member(1));
}

TEST_F(dynamic_slice_scheduler_test, when_a_slice_is_unknown_then_its_removal_is_ignored)
{
  create_slice(1);

  remove_slice(2);

  ASSERT_EQ(slice_sched.nof_slices(), 3);
}

TEST_F(rb_ratio_slice_scheduler_test, when_a_slice_was_not_created_at_runtime_then_its_removal_is_ignored)
{
  ASSERT_NE(this->add_two_drb_ue(to_du_ue_index(0)), nullptr);
  ASSERT_EQ(slice_sched.nof_slices(), 4);
  ASSERT_FALSE(slice_sched.is_dynamic(drb1_slice_id));

  du_cell_slice_reconfig_request req;
  req.cell_index = to_du_cell_index(0);
  req.policies_to_remove.push_back({plmn_identity::test_value(), s_nssai_t{slice_service_type{1}}});
  slice_sched.handle_slice_reconfiguration_request(req);

  ASSERT_EQ(slice_sched.nof_slices(), 4);
  ASSERT_EQ(slice_sched.slice_config(drb1_slice_id).rrc_member.s_nssai.sst.value(), 1);
  run_slot();
  // The SRB slice is the first candidate, and the slice of the UE bearer the next one.
  auto candidate = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(candidate.has_value());
  ASSERT_EQ(candidate->id(), SRB_RAN_SLICE_ID);
  candidate = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(candidate.has_value());
  // The UE keeps its bearer in the slice.
  ASSERT_EQ(candidate->id(), drb1_slice_id);
  ASSERT_TRUE(candidate->is_candidate(to_du_ue_index(0), uint_to_lcid(LCID_MIN_DRB)));
}
