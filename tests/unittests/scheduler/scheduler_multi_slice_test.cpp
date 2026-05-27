// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_simulator.h"
#include "ocudu/adt/ranges/transform.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"
#include <gtest/gtest.h>

using namespace ocudu;

struct multi_slice_test_params {
  std::vector<slice_rrm_policy_config> slices;
};

static s_nssai_t get_nssai(unsigned sst, unsigned sd)
{
  return s_nssai_t{slice_service_type{(uint8_t)sst}, slice_differentiator::create(sd).value()};
}

static rrm_policy_member get_rrm_policy(unsigned sst, unsigned sd)
{
  return rrm_policy_member{plmn_identity::test_value(), get_nssai(sst, sd)};
}

class base_multi_slice_scheduler_tester : public scheduler_test_simulator
{
protected:
  base_multi_slice_scheduler_tester(const multi_slice_test_params& params_) :
    scheduler_test_simulator(4, subcarrier_spacing::kHz30), params(params_)
  {
    // Add Cell.
    auto sched_cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    sched_cell_cfg_req.rrm_policy_members = params.slices;
    this->add_cell(sched_cell_cfg_req);
  }

  rnti_t add_ue(const std::vector<std::pair<lcid_t, s_nssai_t>>& lcid_to_cfg)
  {
    auto                get_lcid  = [](const auto& e) { return e.first; };
    auto                only_lcid = views::transform(lcid_to_cfg, get_lcid);
    std::vector<lcid_t> lcid_list(only_lcid.begin(), only_lcid.end());
    auto                ue_cfg =
        sched_config_helper::create_default_sched_ue_creation_request(cell_cfg(to_du_cell_index(0)).params, lcid_list);
    for (unsigned i = 0; i < lcid_list.size(); i++) {
      auto it                = std::find_if(ue_cfg.cfg.lc_config_list->begin(),
                             ue_cfg.cfg.lc_config_list->end(),
                             [lcid = lcid_to_cfg[i].first](const auto& l) { return l.lcid == lcid; });
      it->rrm_policy.s_nssai = lcid_to_cfg[i].second;
    }
    ue_cfg.crnti    = to_rnti(0x4601 + ue_count);
    ue_cfg.ue_index = to_du_ue_index(ue_count);
    scheduler_test_simulator::add_ue(ue_cfg);

    // Set high CQI.
    uci_indication uci;
    uci.slot_rx    = next_slot_rx();
    uci.cell_index = to_du_cell_index(0);
    uci.ucis.resize(1);
    uci.ucis[0].crnti    = ue_cfg.crnti;
    uci.ucis[0].ue_index = ue_cfg.ue_index;
    uci.ucis[0].pdu      = uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu{
             .csi = csi_report_data{.first_tb_wideband_cqi = 15, .valid = true}};
    this->sched->handle_uci_indication(uci);

    ue_count++;
    return ue_cfg.crnti;
  }

  multi_slice_test_params    params;
  cell_config_builder_params builder_params = cell_config_builder_profiles::create(duplex_mode::TDD);

  unsigned ue_count = 0;
};

class single_slice_limited_max_rbs_scheduler_test : public base_multi_slice_scheduler_tester, public ::testing::Test
{
protected:
  static constexpr unsigned max_slice_rbs = 10;

  single_slice_limited_max_rbs_scheduler_test() :
    base_multi_slice_scheduler_tester(
        multi_slice_test_params{{slice_rrm_policy_config{get_rrm_policy(1, 1), {0, max_slice_rbs}}}})
  {
  }
};

TEST_F(single_slice_limited_max_rbs_scheduler_test, single_ue_limited_to_max_rbs)
{
  // Create UE and fill its buffer.
  rnti_t rnti = this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(0), LCID_MIN_DRB, 500});

  ASSERT_TRUE(this->run_slot_until(
      [this, rnti]() { return find_ue_pdsch(rnti, this->last_sched_result()->dl.ue_grants) != nullptr; }));
  const dl_msg_alloc* msg = find_ue_pdsch(rnti, this->last_sched_result()->dl.ue_grants);

  ASSERT_TRUE(msg->pdsch_cfg.rbs.type1().length() <= max_slice_rbs);
}

TEST_F(single_slice_limited_max_rbs_scheduler_test, multi_ue_limited_to_max_rbs)
{
  // Create UE and fill its buffer.
  unsigned            nof_ues = test_rng::uniform_int<unsigned>(2, 10);
  unsigned            dl_bo   = test_rng::uniform_int<unsigned>(1, 50);
  std::vector<rnti_t> rntis;
  for (unsigned i = 0; i < nof_ues; i++) {
    rntis.push_back(this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))}));
    this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(i), LCID_MIN_DRB, dl_bo});
  }

  ASSERT_TRUE(this->run_slot_until(
      [&]() { return find_ue_pdsch(rntis.front(), this->last_sched_result()->dl.ue_grants) != nullptr; }));
  unsigned nof_rbs = 0;
  for (const dl_msg_alloc& msg : this->last_sched_result()->dl.ue_grants) {
    nof_rbs += msg.pdsch_cfg.rbs.type1().length();
  }

  ASSERT_TRUE(nof_rbs <= max_slice_rbs);
}

TEST_F(single_slice_limited_max_rbs_scheduler_test, multi_ue_limited_to_max_rbs_reconf)
{
  // Create UE and fill its buffer.
  unsigned            nof_ues = test_rng::uniform_int<unsigned>(2, 10);
  unsigned            dl_bo   = test_rng::uniform_int<unsigned>(10, 50);
  std::vector<rnti_t> rntis;
  unsigned            nof_rbs = 0;
  for (unsigned i = 0; i < nof_ues; i++) {
    rntis.push_back(this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))}));
  }

  auto run_scheduler = [this, nof_ues, &rntis, &nof_rbs, dl_bo]() {
    for (unsigned i = 0; i < nof_ues; i++) {
      this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(i), LCID_MIN_DRB, dl_bo});
    }

    ASSERT_TRUE(this->run_slot_until(
        [&]() { return find_ue_pdsch(rntis.front(), this->last_sched_result()->dl.ue_grants) != nullptr; }));

    for (const dl_msg_alloc& msg : this->last_sched_result()->dl.ue_grants) {
      nof_rbs += msg.pdsch_cfg.rbs.type1().length();
    }
  };

  // Run scheduler with the first set of parameters.
  run_scheduler();
  ASSERT_TRUE(nof_rbs <= max_slice_rbs);

  // Apply reconfiguration here.
  constexpr unsigned             new_max_rbs = 8;
  constexpr unsigned             new_min_rbs = 2;
  du_cell_slice_reconfig_request slice_reconfig_req;
  slice_reconfig_req.cell_index = cell_cfg().cell_index;
  du_cell_slice_reconfig_request::rrm_policy_config rrm_policy_cfg{.rrc_member = get_rrm_policy(1, 1),
                                                                   .rbs        = {new_min_rbs, new_max_rbs}};
  slice_reconfig_req.rrm_policies.emplace_back(rrm_policy_cfg);
  sched->handle_slice_reconfiguration_request(slice_reconfig_req);

  test_logger.info("Slice reconfiguration applied");

  // Re-run scheduler with the new set of parameters.
  nof_rbs = 0;
  run_scheduler();
  ASSERT_TRUE(nof_rbs <= new_max_rbs);
}

class multi_slice_with_prio_slice_scheduler_test : public base_multi_slice_scheduler_tester, public ::testing::Test
{
protected:
  static constexpr unsigned slice1_min_rbs = 10;
  static constexpr unsigned slice1_max_rbs = 20;

  multi_slice_with_prio_slice_scheduler_test() :
    base_multi_slice_scheduler_tester(
        multi_slice_test_params{{slice_rrm_policy_config{get_rrm_policy(1, 1), {slice1_min_rbs, slice1_max_rbs}},
                                 slice_rrm_policy_config{get_rrm_policy(1, 2), {0, MAX_NOF_PRBS}}}})
  {
  }
};

TEST_F(multi_slice_with_prio_slice_scheduler_test, multi_ue_limited_to_max_rbs)
{
  // Create 3 UEs and fill their buffers.
  this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(0), LCID_MIN_DRB, 100000});
  this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(1), LCID_MIN_DRB, 100000});
  this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 2))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(2), LCID_MIN_DRB, 100000});

  unsigned                nof_checks = 4;
  std::array<unsigned, 3> rnti_sum_rbs{0};
  for (unsigned i = 0; i < nof_checks; i++) {
    ASSERT_TRUE(this->run_slot_until([&]() { return not this->last_sched_result()->dl.ue_grants.empty(); }));

    std::array<unsigned, 3> rnti_rbs{0};
    for (const dl_msg_alloc& msg : this->last_sched_result()->dl.ue_grants) {
      unsigned idx = static_cast<unsigned>(msg.pdsch_cfg.rnti) - 0x4601;
      rnti_rbs[idx] += msg.pdsch_cfg.rbs.type1().length();
    }
    ASSERT_GE(rnti_rbs[0] + rnti_rbs[1], slice1_min_rbs);
    ASSERT_LE(rnti_rbs[0] + rnti_rbs[1], slice1_max_rbs);
    ASSERT_GT(rnti_rbs[2], 0);

    for (unsigned j = 0; j != rnti_rbs.size(); j++) {
      rnti_sum_rbs[j] += rnti_rbs[j];
    }
  }

  ASSERT_TRUE(std::all_of(rnti_sum_rbs.begin(), rnti_sum_rbs.end(), [](unsigned n) { return n > 0; }));
}

TEST_F(multi_slice_with_prio_slice_scheduler_test, multi_ue_limited_to_max_rbs_reconf)
{
  // Create 3 UEs and fill their buffers.
  this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(0), LCID_MIN_DRB, 100000});
  this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(1), LCID_MIN_DRB, 100000});
  this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 2))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(2), LCID_MIN_DRB, 100000});

  auto run_scheduler_assess_rbs =
      [this](unsigned max_rbs_sl1, unsigned min_rbs_sl1, unsigned max_rbs_sl2, unsigned min_rbs_sl2) {
        unsigned                nof_checks = 4;
        std::array<unsigned, 3> rnti_sum_rbs{0};
        for (unsigned i = 0; i < nof_checks; i++) {
          ASSERT_TRUE(this->run_slot_until([&]() { return not this->last_sched_result()->dl.ue_grants.empty(); }));

          std::array<unsigned, 3> rnti_rbs{0};
          for (const dl_msg_alloc& msg : this->last_sched_result()->dl.ue_grants) {
            unsigned idx = static_cast<unsigned>(msg.pdsch_cfg.rnti) - 0x4601;
            rnti_rbs[idx] += msg.pdsch_cfg.rbs.type1().length();
          }
          ASSERT_GE(rnti_rbs[0] + rnti_rbs[1], min_rbs_sl1);
          ASSERT_LE(rnti_rbs[0] + rnti_rbs[1], max_rbs_sl1);
          ASSERT_GT(rnti_rbs[2], min_rbs_sl2);
          ASSERT_LE(rnti_rbs[2], max_rbs_sl2);

          for (unsigned j = 0; j != rnti_rbs.size(); j++) {
            rnti_sum_rbs[j] += rnti_rbs[j];
          }
        }

        ASSERT_TRUE(std::all_of(rnti_sum_rbs.begin(), rnti_sum_rbs.end(), [](unsigned n) { return n > 0; }));
      };

  // Run scheduler with the first set of parameters.
  run_scheduler_assess_rbs(slice1_max_rbs, slice1_min_rbs, MAX_NOF_PRBS, 0);

  // Apply reconfiguration here.
  constexpr unsigned             new_max_slice1_rbs = 15;
  constexpr unsigned             new_min_slice1_rbs = 5;
  constexpr unsigned             new_max_slice2_rbs = 20;
  du_cell_slice_reconfig_request slice_reconfig_req;
  slice_reconfig_req.cell_index = cell_cfg().cell_index;
  du_cell_slice_reconfig_request::rrm_policy_config rrm_policy_cfg_sl_1{
      .rrc_member = get_rrm_policy(1, 1), .rbs = {new_min_slice1_rbs, new_max_slice1_rbs}};
  slice_reconfig_req.rrm_policies.emplace_back(rrm_policy_cfg_sl_1);
  du_cell_slice_reconfig_request::rrm_policy_config rrm_policy_cfg_sl_2{.rrc_member = get_rrm_policy(1, 2),
                                                                        .rbs        = {0, new_max_slice2_rbs}};
  slice_reconfig_req.rrm_policies.emplace_back(rrm_policy_cfg_sl_2);
  sched->handle_slice_reconfiguration_request(slice_reconfig_req);

  test_logger.info("Slice reconfiguration applied");

  // Re-run scheduler with the new set of parameters.
  run_scheduler_assess_rbs(new_max_slice1_rbs, new_min_slice1_rbs, new_max_slice2_rbs, 0);
}

// Sanity check: single UE with max RB slice gets UL grants.
TEST_F(single_slice_limited_max_rbs_scheduler_test, single_ue_ul_gets_scheduled)
{
  rnti_t                    rnti = this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))});
  ul_bsr_indication_message bsr{to_du_cell_index(0),
                                to_du_ue_index(0),
                                rnti,
                                bsr_format::SHORT_BSR,
                                {ul_bsr_lcg_report{uint_to_lcg_id(2), 10000000}}};
  this->push_bsr(bsr);
  auto_crc = true;
  auto_uci = true;

  unsigned ul_rbs = 0;
  for (unsigned i = 0; i < 200; ++i) {
    run_slot();
    for (const ul_sched_info& pusch : last_sched_result()->ul.puschs) {
      if (pusch.pusch_cfg.rnti == rnti) {
        ul_rbs += pusch.pusch_cfg.rbs.type1().length();
      }
    }
  }
  ASSERT_GT(ul_rbs, 0U) << "Single UE never got UL grant";
}

class multi_slice_dedicated_ul_rbs_test : public scheduler_test_simulator, public ::testing::Test
{
protected:
  // sst=2, sd=42 → 25% of 51 PRBs = 13 RBs.
  // sst=1, sd=1  → 75% of 51 PRBs = 38 RBs.
  static constexpr unsigned slice_25_rbs = 13;
  static constexpr unsigned slice_75_rbs = 38;

  static s_nssai_t nssai_25() { return get_nssai(2, 42); }
  static s_nssai_t nssai_75() { return get_nssai(1, 1); }

  multi_slice_dedicated_ul_rbs_test() : scheduler_test_simulator(4, subcarrier_spacing::kHz30)
  {
    auto_crc = true;
    auto_uci = true;

    auto params                 = cell_config_builder_profiles::create(duplex_mode::TDD);
    params.tdd_ul_dl_cfg_common = cell_config_builder_profiles::create_tdd_pattern(
        cell_config_builder_profiles::tdd_pattern_profile_fr1_30khz::DDDSU);

    auto cell_req               = sched_config_helper::make_default_sched_cell_configuration_request(params);
    cell_req.rrm_policy_members = {
        slice_rrm_policy_config{get_rrm_policy(2, 42), {slice_25_rbs, slice_25_rbs, slice_25_rbs}},
        slice_rrm_policy_config{get_rrm_policy(1, 1), {slice_75_rbs, slice_75_rbs, slice_75_rbs}}};
    this->add_cell(cell_req);
  }

  rnti_t add_ue_to_slice(unsigned ue_idx, s_nssai_t nssai)
  {
    auto ue_cfg = sched_config_helper::create_default_sched_ue_creation_request(cell_cfg(to_du_cell_index(0)).params,
                                                                                {LCID_MIN_DRB});
    ue_cfg.ue_index                                         = to_du_ue_index(ue_idx);
    ue_cfg.crnti                                            = to_rnti(0x4601 + ue_idx);
    ue_cfg.cfg.lc_config_list.value()[2].rrm_policy.s_nssai = nssai;
    scheduler_test_simulator::add_ue(ue_cfg);
    return ue_cfg.crnti;
  }

  void push_bsr_for_ue(du_ue_index_t ue_idx, rnti_t rnti)
  {
    ul_bsr_indication_message bsr{
        to_du_cell_index(0), ue_idx, rnti, bsr_format::SHORT_BSR, {ul_bsr_lcg_report{uint_to_lcg_id(2), 10000000}}};
    this->push_bsr(bsr);
  }
};

// Tests that UL dedicated PRB ratios are respected when two slices together cover 100% of the cell bandwidth.
// Regression test for: slicing: dedicated UL proportion not following config and DL (issue #383).
// Slice config mirrors the original bug report:
//   sst=2, sd=42 → 25% dedicated/min/max PRBs
//   sst=1, sd=1  → 75% dedicated/min/max PRBs
// TDD DDDSU pattern, 30kHz SCS, 20MHz (51 PRBs). Both DL and UL traffic present.
TEST_F(multi_slice_dedicated_ul_rbs_test, ul_dedicated_rbs_ratio_is_respected)
{
  // UE0 in the 75% slice (sst=1,sd=1), UE1 in the 25% slice (sst=2,sd=42).
  rnti_t rnti_75 = this->add_ue_to_slice(0, nssai_75());
  rnti_t rnti_25 = this->add_ue_to_slice(1, nssai_25());

  // Push large UL and DL buffers — matches real-world scenario from bug report.
  push_bsr_for_ue(to_du_ue_index(0), rnti_75);
  push_bsr_for_ue(to_du_ue_index(1), rnti_25);
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(0), LCID_MIN_DRB, 10000000});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(1), LCID_MIN_DRB, 10000000});

  static constexpr unsigned nof_test_slots = 2000;
  unsigned                  ul_rbs_75      = 0;
  unsigned                  ul_rbs_25      = 0;
  unsigned                  ul_rbs_total   = 0;
  unsigned                  ul_slots       = 0;
  unsigned                  dl_rbs_75      = 0;
  unsigned                  dl_rbs_25      = 0;

  for (unsigned i = 0; i < nof_test_slots; ++i) {
    run_slot();
    const sched_result* res = last_sched_result();

    // UL: count per-slice PRBs and total PRBs in each UL slot.
    if (not res->ul.puschs.empty()) {
      unsigned slot_ul_total = 0;
      for (const ul_sched_info& pusch : res->ul.puschs) {
        unsigned nof_rbs = pusch.pusch_cfg.rbs.type1().length();
        slot_ul_total += nof_rbs;
        if (pusch.pusch_cfg.rnti == rnti_75) {
          ul_rbs_75 += nof_rbs;
        } else if (pusch.pusch_cfg.rnti == rnti_25) {
          ul_rbs_25 += nof_rbs;
        }
      }
      ul_rbs_total += slot_ul_total;
      ++ul_slots;
    }

    // DL: count per-slice PRBs.
    for (const dl_msg_alloc& pdsch : res->dl.ue_grants) {
      unsigned nof_rbs = pdsch.pdsch_cfg.rbs.type1().length();
      if (pdsch.pdsch_cfg.rnti == rnti_75) {
        dl_rbs_75 += nof_rbs;
      } else if (pdsch.pdsch_cfg.rnti == rnti_25) {
        dl_rbs_25 += nof_rbs;
      }
    }
  }

  const double avg_ul_prbs_per_slot = ul_slots > 0 ? static_cast<double>(ul_rbs_total) / ul_slots : 0.0;
  const auto   cell_ul_prbs         = static_cast<double>(slice_75_rbs + slice_25_rbs);

  test_logger.info("UL RBs: 75%={} 25%={} total={} ul_slots={} avg_prbs/ul_slot={:.1f} (cell={})",
                   ul_rbs_75,
                   ul_rbs_25,
                   ul_rbs_total,
                   ul_slots,
                   avg_ul_prbs_per_slot,
                   cell_ul_prbs);
  test_logger.info("DL RBs: 75%={} 25%={}", dl_rbs_75, dl_rbs_25);

  ASSERT_GT(ul_rbs_75, 0U) << "75% slice UE was never scheduled in UL";
  ASSERT_GT(ul_rbs_25, 0U) << "25% slice UE was never scheduled in UL";
  ASSERT_GT(dl_rbs_75, 0U) << "75% slice UE was never scheduled in DL";
  ASSERT_GT(dl_rbs_25, 0U) << "25% slice UE was never scheduled in DL";

  static constexpr double expected_ratio = static_cast<double>(slice_75_rbs) / slice_25_rbs;
  static constexpr double tolerance      = 0.15;

  // DL ratio must be correct — this is a sanity check; DL slicing works in the bug report.
  if (dl_rbs_25 > 0) {
    double dl_ratio = static_cast<double>(dl_rbs_75) / dl_rbs_25;
    EXPECT_NEAR(dl_ratio, expected_ratio, expected_ratio * tolerance)
        << "DL RB ratio wrong: 75%=" << dl_rbs_75 << " 25%=" << dl_rbs_25;
  }

  // UL ratio must match configured dedicated PRB shares.
  double ul_ratio = static_cast<double>(ul_rbs_75) / ul_rbs_25;
  EXPECT_NEAR(ul_ratio, expected_ratio, expected_ratio * tolerance)
      << "UL RB ratio wrong: 75%=" << ul_rbs_75 << " 25%=" << ul_rbs_25;

  // Total UL PRBs per slot must equal the sum of both slices' dedicated allocations.
  // When both UEs have data, the scheduler should fully utilise the UL slot.
  EXPECT_NEAR(avg_ul_prbs_per_slot, cell_ul_prbs, cell_ul_prbs * tolerance)
      << "UL slot underutilised: avg=" << avg_ul_prbs_per_slot << " expected=" << cell_ul_prbs;
}
