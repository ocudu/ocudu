// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/ue_context/ue.h"
#include "lib/scheduler/ue_context/ue_repository.h"
#include "lib/scheduler/ue_scheduling/triggered_ul_grant_scheduler.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/result_test_helpers.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_simulator.h"
#include "ocudu/mac/mac_lc_config.h"
#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include <chrono>
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

constexpr du_cell_index_t CELL_IDX = to_du_cell_index(0);
// Configured triggered-grant delay, in ms. All cells in this fixture use 15 kHz SCS (numerology 0), i.e. 1 slot per
// subframe, so a delay of DELAY_SLOTS ms maps to the same number of slots. The scheduler does the ms->slot conversion.
constexpr uint8_t  DELAY_SLOTS   = 4;
constexpr unsigned GRANT_BYTES   = 600;
constexpr lcid_t   TRIG_LCID     = lcid_t::LCID_MIN_DRB;
constexpr lcid_t   NOT_TRIG_LCID = uint_to_lcid(TRIG_LCID + 1);
constexpr lcg_id_t TRIG_LCG      = uint_to_lcg_id(2);

class trig_ul_sched_test : public scheduler_test_simulator, public ::testing::Test
{
public:
  trig_ul_sched_test() : scheduler_test_simulator(scheduler_test_sim_config{.auto_uci = true, .auto_crc = true})
  {
    add_cell(sched_config_helper::make_default_sched_cell_configuration_request());
  }

  sched_ue_creation_request_message make_ue_req(du_ue_index_t ue_idx, rnti_t crnti, bool with_trigger)
  {
    auto lcid    = with_trigger ? TRIG_LCID : NOT_TRIG_LCID;
    auto req     = sched_config_helper::create_default_sched_ue_creation_request(cell_cfg().params, {lcid});
    req.ue_index = ue_idx;
    req.crnti    = crnti;
    if (with_trigger) {
      auto& drb_lc                          = req.cfg.lc_config_list->back();
      drb_lc.triggered_ul_grant             = mac_lc_config::triggered_ul_grant_cfg{};
      drb_lc.triggered_ul_grant->delay_ms   = std::chrono::milliseconds{DELAY_SLOTS};
      drb_lc.triggered_ul_grant->grant_size = units::bytes{GRANT_BYTES};
    }
    return req;
  }

  void give_dl_data(du_ue_index_t ue_idx, lcid_t lcid)
  {
    push_dl_buffer_state({ue_idx, lcid, GRANT_BYTES * 4, next_slot_rx()});
  }

  void give_ul_bsr(du_ue_index_t ue_idx, rnti_t crnti, unsigned bytes)
  {
    push_bsr(ul_bsr_indication_message{CELL_IDX, ue_idx, crnti, bsr_format::SHORT_BSR, {{TRIG_LCG, bytes}}});
  }
};

// ---- Tests with cell scheduler and triggered grant interaction ---------------------------------------------

TEST_F(trig_ul_sched_test, no_dl_grant_no_pusch)
{
  /*
  Test: UE configured with triggered grant, but BSR/PUSCH is not triggered if there is no PDSCH.
  Precondition:
    UE with triggered ul grant is configured
  Action:
    UE connected, but no DL scheduling
  Postcondition:
    PUSCH is not scheduled
  */

  // Precondition
  const rnti_t rnti = to_rnti(0x4601);
  add_ue(make_ue_req(to_du_ue_index(0), rnti, true), true);

  // No DL data pushed — scheduler has nothing to schedule for this UE's DRB.
  for (unsigned i = 0; i < 50; ++i) {
    run_slot();
    ASSERT_EQ(find_ue_pusch(rnti, *last_sched_result()), nullptr) << "unexpected PUSCH at slot " << i;
  }
}

TEST_F(trig_ul_sched_test, dl_grant_triggers_pusch)
{
  /*
  Test: UE configured with triggered grant, PDSCH triggers BSR and UL grant
  Precondition:
    UE with triggered ul grant is configured
  Action:
    PDSCH scheduling for UE at slot N
  Postcondition:
    PDCCH for UL scheduled at slot N+DELAY_SLOTS, and PUSCH scheduled at N+DELAY_SLOTS+K2
    UL Grant size choosen according to GRANT_BYTES
  */

  // Precondition
  const rnti_t rnti = to_rnti(0x4601);
  add_ue(make_ue_req(to_du_ue_index(0), rnti, true), true);

  // Action
  give_dl_data(to_du_ue_index(0), TRIG_LCID);

  // 1. schedule DL
  slot_point pdsch_slot;
  bool       pdsch_found = run_slot_until(
      [&] {
        if (find_ue_pdsch(rnti, *last_sched_result()) != nullptr) {
          pdsch_slot = last_result_slot();
          return true;
        }
        return false;
      },
      20);
  ASSERT_TRUE(pdsch_found) << "UE did not receive a DL grant within timeout";

  // Postcondition:
  const uint8_t    k2        = cell_cfg().params.init_bwp.pusch.min_k2;
  const slot_point pdcch_due = pdsch_slot + DELAY_SLOTS;
  const slot_point pusch_due = pdcch_due + k2;

  // 1. UL PDCCH scheduled at N+DELAY_SLOTS
  bool pdcch_reached = run_slot_until([&] { return last_result_slot() == pdcch_due; }, DELAY_SLOTS + 2);
  ASSERT_TRUE(pdcch_reached) << fmt::format("did not reach PDCCH slot {}", pdcch_due);
  ASSERT_NE(find_ue_ul_pdcch(rnti), nullptr) << fmt::format("no UL PDCCH at slot {}", pdcch_due);

  // 2. UL PUSCH scheduled at N+DELAY_SLOTS+K2
  bool pusch_reached = run_slot_until([&] { return last_result_slot() == pusch_due; }, k2 + 2);
  ASSERT_TRUE(pusch_reached) << fmt::format("did not reach PUSCH slot {}", pusch_due);
  const ul_sched_info* pusch = find_ue_pusch(rnti, *last_sched_result());
  ASSERT_NE(pusch, nullptr) << fmt::format("no PUSCH at slot {}", pusch_due);

  // 3. Grant has proper size
  EXPECT_GE(pusch->pusch_cfg.tb_size_bytes.value(), GRANT_BYTES) << "PUSCH TBS smaller than triggered grant size";
  EXPECT_LT(pusch->pusch_cfg.tb_size_bytes.value(), GRANT_BYTES + 200) << "PUSCH TBS bigger than triggered grant size";
}

TEST_F(trig_ul_sched_test, only_ue_with_triggered_bearer_gets_pusch)
{
  /*
  Test: Triggered UL grant is only issued to UEs that have a bearer configured with triggered_ul_grant.
  Precondition:
    Two UEs: one with triggered UL grant bearer (TRIG_LCID), one with a plain bearer only.
  Action:
    DL scheduling for both UEs.
  Postcondition:
    Only the triggered UE receives a PUSCH grant.
    The normal UE does not receive a PUSCH grant.
  */

  // Precondition
  const rnti_t trig_rnti  = to_rnti(0x4601);
  const rnti_t plain_rnti = to_rnti(0x4602);
  add_ue(make_ue_req(to_du_ue_index(0), trig_rnti, true), true);
  add_ue(make_ue_req(to_du_ue_index(1), plain_rnti, false), true);

  // Action
  give_dl_data(to_du_ue_index(0), TRIG_LCID);
  give_dl_data(to_du_ue_index(1), NOT_TRIG_LCID);

  // 1. Wait for both UEs to receive a DL grant.
  bool trig_pdsch   = false;
  bool normal_pdsch = false;
  run_slot_until(
      [&] {
        trig_pdsch   = trig_pdsch || find_ue_pdsch(trig_rnti, *last_sched_result()) != nullptr;
        normal_pdsch = normal_pdsch || find_ue_pdsch(plain_rnti, *last_sched_result()) != nullptr;
        return trig_pdsch && normal_pdsch;
      },
      20);
  ASSERT_TRUE(trig_pdsch) << "triggered UE never received a DL grant";
  ASSERT_TRUE(normal_pdsch) << "normal UE never received a DL grant";

  // Postcondition:
  // 1. Triggered UE receives PUSCH.
  const bool trig_pusch = run_slot_until([&] { return find_ue_pusch(trig_rnti, *last_sched_result()) != nullptr; }, 50);
  EXPECT_TRUE(trig_pusch) << "triggered UE did not receive PUSCH";

  // 2. Normal UE does not receive PUSCH — check the current slot and the following 20.
  EXPECT_EQ(find_ue_pusch(plain_rnti, *last_sched_result()), nullptr)
      << "normal UE got unexpected PUSCH at slot " << last_result_slot().system_slot();
  for (unsigned i = 0; i < 20; ++i) {
    run_slot();
    EXPECT_EQ(find_ue_pusch(plain_rnti, *last_sched_result()), nullptr)
        << "normal UE got unexpected PUSCH at slot " << last_result_slot().system_slot();
  }
}

TEST_F(trig_ul_sched_test, no_injection_when_pending_bytes_already_sufficient)
{
  /*
  Test: BSR injection is skipped when the UE already has sufficient UL buffer, and the pending grant
        is cleaned from the queue regardless.
  Precondition:
    UE with triggered UL grant bearer.
  Action:
    PDSCH at slot N registers a pending_grant (target = N+DELAY_SLOTS).
    Natural BSR above GRANT_BYTES reported just before the trigger slot.
    At slot N+DELAY_SLOTS the scheduler sees pending_bytes >= GRANT_BYTES.
  Postcondition:
    BSR injection is skipped — PUSCH TBS reflects the natural BSR, not the reduced GRANT_BYTES.
    Pending grant is cleaned from queue — no PUSCH appears after BSR is drained.
  */

  // Precondition
  const du_ue_index_t ue_idx      = to_du_ue_index(0);
  const rnti_t        rnti        = to_rnti(0x4601);
  const unsigned      natural_bsr = GRANT_BYTES + 100;
  add_ue(make_ue_req(ue_idx, rnti, true), true);

  // Action: register one pending_grant via a small DL packet.
  push_dl_buffer_state({ue_idx, TRIG_LCID, GRANT_BYTES / 2, next_slot_rx()});
  slot_point pdsch_slot;
  bool       pdsch_found = run_slot_until(
      [&] {
        if (find_ue_pdsch(rnti, *last_sched_result()) != nullptr) {
          pdsch_slot = last_result_slot();
          return true;
        }
        return false;
      },
      20);
  ASSERT_TRUE(pdsch_found);

  // Inject BSR just before the trigger slot — BSR event is processed at the start
  // of the trigger slot, before trig_ul_sched checks pending_bytes.
  const slot_point trigger_slot = pdsch_slot + DELAY_SLOTS;
  run_slot_until([&] { return last_result_slot() == trigger_slot - 1; }, DELAY_SLOTS);
  give_ul_bsr(ue_idx, rnti, natural_bsr);
  run_slot();
  ASSERT_EQ(last_result_slot(), trigger_slot);

  // Postcondition:
  const uint8_t    k2        = cell_cfg().params.init_bwp.pusch.min_k2;
  const slot_point pusch_due = trigger_slot + k2;
  run_slot_until([&] { return last_result_slot() == pusch_due; }, k2 + 2);

  const ul_sched_info* pusch = find_ue_pusch(rnti, *last_sched_result());
  ASSERT_NE(pusch, nullptr) << fmt::format("no PUSCH at slot {}", pusch_due);

  // 1. PUSCH TBS reflects natural_bsr — injection was skipped.
  EXPECT_GE(pusch->pusch_cfg.tb_size_bytes.value(), natural_bsr)
      << "PUSCH TBS smaller than natural BSR — injection may have overwritten pending_bytes";

  // 2. Queue is clean — no PUSCH after BSR is drained.
  give_ul_bsr(ue_idx, rnti, 0);
  for (unsigned i = 0; i < 20; ++i) {
    run_slot();
    EXPECT_EQ(find_ue_pusch(rnti, *last_sched_result()), nullptr)
        << "PUSCH at slot " << last_result_slot().system_slot() << " — pending_grant was not cleaned from queue";
  }
}
// ---- Unit tests for triggered_ul_grant_scheduler functions ------------------

class trig_ul_grant_unit_test : public ::testing::Test
{
protected:
  trig_ul_grant_unit_test()
  {
    cfg_mgr.add_cell(sched_config_helper::make_default_sched_cell_configuration_request());
    ue_repo.add_cell(cfg_mgr.get_cell(CELL_IDX), nullptr);
  }

  ue& add_ue_to_repo(du_ue_index_t ue_idx, rnti_t crnti, bool with_trigger)
  {
    auto req =
        sched_config_helper::create_default_sched_ue_creation_request(cfg_mgr.get_cell(CELL_IDX).params, {TRIG_LCID});
    req.ue_index = ue_idx;
    req.crnti    = crnti;
    if (with_trigger) {
      auto& drb_lc                          = req.cfg.lc_config_list->back();
      drb_lc.triggered_ul_grant             = mac_lc_config::triggered_ul_grant_cfg{};
      drb_lc.triggered_ul_grant->delay_ms   = std::chrono::milliseconds(DELAY_SLOTS);
      drb_lc.triggered_ul_grant->grant_size = units::bytes{GRANT_BYTES};
    }
    ue_repo.add_ue(*cfg_mgr.add_ue(req), false, {}, false);
    return *ue_repo.find(ue_idx);
  }

  static sched_result make_dl_grant(du_ue_index_t ue_idx, lcid_t lcid)
  {
    sched_result   res{};
    dl_msg_alloc   grant{};
    dl_msg_tb_info tb{};
    dl_msg_lc_info lc{};
    grant.context.ue_index = ue_idx;
    lc.lcid                = lcid_dl_sch_t{static_cast<uint16_t>(lcid)};
    lc.sched_bytes         = 100;
    tb.lc_chs_to_sched.push_back(lc);
    grant.tb_list.push_back(tb);
    res.dl.ue_grants.push_back(grant);
    return res;
  }

  static void report_bsr(ue& u, du_ue_index_t ue_idx, rnti_t crnti, unsigned bytes)
  {
    ul_bsr_indication_message bsr{};
    bsr.cell_index = CELL_IDX;
    bsr.ue_index   = ue_idx;
    bsr.crnti      = crnti;
    bsr.type       = bsr_format::SHORT_BSR;
    bsr.reported_lcgs.push_back({TRIG_LCG, bytes});
    u.handle_bsr_indication(bsr);
  }

  test_helpers::test_sched_config_manager cfg_mgr{cell_config_builder_params{}};
  ue_repository                           ue_repo{scheduler_ue_expert_config{}};
  triggered_ul_grant_scheduler            ul_trig_sched{ue_repo, CELL_IDX, subcarrier_spacing::kHz15};
  const slot_point                        sl{subcarrier_spacing::kHz15, 0};
};

TEST_F(trig_ul_grant_unit_test, process_dl_results_enqueues_grant_for_triggered_lcid)
{
  /*
  Test: process_dl_results enqueues a pending_grant when a DL grant is issued for a triggered LCID.
  Precondition:
    UE with triggered UL grant bearer configured.
    No pending BSR.
  Action:
    process_dl_results called with a DL grant for TRIG_LCID.
    run_slot called at the target slot (sl + DELAY_SLOTS).
  Postcondition:
    BSR is injected — pending bytes for TRIG_LCG >= GRANT_BYTES.
  */
  ue& u = add_ue_to_repo(to_du_ue_index(0), to_rnti(0x4601), true);
  ASSERT_EQ(u.logical_channels().pending_bytes(TRIG_LCG), 0U);

  ul_trig_sched.process_dl_results(sl, make_dl_grant(to_du_ue_index(0), TRIG_LCID));
  ul_trig_sched.run_slot(sl + DELAY_SLOTS);

  EXPECT_GE(u.logical_channels().pending_bytes(TRIG_LCG), GRANT_BYTES);
}

TEST_F(trig_ul_grant_unit_test, process_dl_results_ignores_non_triggered_lcid)
{
  /*
  Test: process_dl_results does not enqueue a grant when the bearer has no triggered_ul_grant config.
  Precondition:
    UE configured without triggered UL grant bearer.
  Action:
    process_dl_results called with a DL grant for TRIG_LCID.
    run_slot called at the target slot (sl + DELAY_SLOTS).
  Postcondition:
    No BSR is injected — pending bytes for TRIG_LCG remain 0.
  */
  ue& u = add_ue_to_repo(to_du_ue_index(0), to_rnti(0x4601), false);

  ul_trig_sched.process_dl_results(sl, make_dl_grant(to_du_ue_index(0), TRIG_LCID));
  ul_trig_sched.run_slot(sl + DELAY_SLOTS);

  EXPECT_EQ(u.logical_channels().pending_bytes(TRIG_LCG), 0U);
}

TEST_F(trig_ul_grant_unit_test, run_slot_does_not_fire_before_target_slot)
{
  /*
  Test: BSR injection fires exactly at the target slot, not before.
  Precondition:
    UE with triggered UL grant bearer configured.
    Pending grant registered via process_dl_results at slot sl.
  Action:
    run_slot called at sl + DELAY_SLOTS - 1 (one slot before target).
    run_slot called at sl + DELAY_SLOTS (target slot).
  Postcondition:
    At sl + DELAY_SLOTS - 1: pending bytes == 0 (no early injection).
    At sl + DELAY_SLOTS: pending bytes >= GRANT_BYTES (injection fired on time).
  */
  ue& u = add_ue_to_repo(to_du_ue_index(0), to_rnti(0x4601), true);
  ul_trig_sched.process_dl_results(sl, make_dl_grant(to_du_ue_index(0), TRIG_LCID));

  ul_trig_sched.run_slot(sl + DELAY_SLOTS - 1);
  EXPECT_EQ(u.logical_channels().pending_bytes(TRIG_LCG), 0U) << "BSR injected too early";

  ul_trig_sched.run_slot(sl + DELAY_SLOTS);
  EXPECT_GE(u.logical_channels().pending_bytes(TRIG_LCG), GRANT_BYTES) << "BSR not injected at target slot";
}

TEST_F(trig_ul_grant_unit_test, run_slot_skips_injection_when_bsr_already_sufficient)
{
  /*
  Test: BSR injection is skipped when the UE already has sufficient pending bytes at the target slot.
  Precondition:
    UE with triggered UL grant bearer configured.
    Natural BSR of GRANT_BYTES + 1 injected before run_slot.
  Action:
    process_dl_results registers a pending grant.
    run_slot called at the target slot.
  Postcondition:
    Pending bytes remain equal to the natural BSR — injection did not overwrite it.
  */
  ue& u = add_ue_to_repo(to_du_ue_index(0), to_rnti(0x4601), true);

  report_bsr(u, to_du_ue_index(0), to_rnti(0x4601), GRANT_BYTES + 100);
  const unsigned natural_pending = u.logical_channels().pending_bytes(TRIG_LCG);

  ul_trig_sched.process_dl_results(sl, make_dl_grant(to_du_ue_index(0), TRIG_LCID));
  ul_trig_sched.run_slot(sl + DELAY_SLOTS);

  EXPECT_EQ(u.logical_channels().pending_bytes(TRIG_LCG), natural_pending) << "natural BSR was overwritten";
}

TEST_F(trig_ul_grant_unit_test, grant_cleaned_from_queue_after_run_slot)
{
  /*
  Test: A pending grant is removed from the queue after run_slot processes it, preventing re-injection.
  Precondition:
    UE with triggered UL grant bearer configured.
    Pending grant registered and fired at sl + DELAY_SLOTS.
  Action:
    BSR zeroed after injection.
    run_slot called at sl + DELAY_SLOTS + 1 (one slot after the grant fired).
  Postcondition:
    Pending bytes remain 0 — no re-injection occurred, grant was cleaned from the queue.
  */
  ue& u = add_ue_to_repo(to_du_ue_index(0), to_rnti(0x4601), true);
  ul_trig_sched.process_dl_results(sl, make_dl_grant(to_du_ue_index(0), TRIG_LCID));
  ul_trig_sched.run_slot(sl + DELAY_SLOTS);
  ASSERT_GE(u.logical_channels().pending_bytes(TRIG_LCG), GRANT_BYTES);

  // Zero the BSR; if the grant was not cleaned a re-injection would follow.
  report_bsr(u, to_du_ue_index(0), to_rnti(0x4601), 0);

  ul_trig_sched.run_slot(sl + DELAY_SLOTS + 1);
  EXPECT_EQ(u.logical_channels().pending_bytes(TRIG_LCG), 0U) << "grant was not cleaned — re-injection occurred";
}

} // namespace
