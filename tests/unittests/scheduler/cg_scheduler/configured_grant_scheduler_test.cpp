// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit tests for the configured_grant_scheduler. Verifies that the scheduler correctly places periodic
/// CG PUSCH grants in the right slots, and that the scheduler output (symbols, RBs, RNTI) matches the CG
/// configuration.

#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/scheduler/scheduler_result_finder.h"
#include "tests/test_doubles/scheduler/scheduler_test_message_validators.h"
#include "tests/test_doubles/utils/test_rng_seed.h"
#include "tests/unittests/scheduler/test_utils/result_test_helpers.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_simulator.h"
#include "ocudu/ran/configured_grant/cg_configuration.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/ran/resource_allocation/resource_allocation_frequency.h"
#include "ocudu/scheduler/config/cg_builder_params.h"
#include "ocudu/scheduler/resource_grid_util.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {
/// Helper struct that holds parameters for a single CG test scenario.
struct cg_test_params {
  cg_configuration::periodicity_t periodicity    = cg_configuration::periodicity_t::sl40;
  unsigned                        period_slots   = 40;
  unsigned                        mcs            = 5;
  unsigned                        nof_harq_procs = 4;
  /// When false, the cell is built without CG configured at cell level, so the CG scheduler is not instantiated.
  bool cell_cg_enabled = true;
};

/// Default CG VRB allocation set by the config factory (see make_default_cg_config() in
/// serving_cell_config_factory.cpp): start_vrb=10, length_vrb=10. The CG resource manager, which would overwrite these
/// values, is not part of the scheduler config path exercised by this test.
constexpr unsigned default_cg_start_vrb = 10;
constexpr unsigned default_cg_nof_rbs   = 10;

struct cg_duplex_test_params {
  std::string                            name;
  duplex_mode                            mode;
  std::optional<tdd_ul_dl_config_common> tdd_pattern;
  unsigned                               default_cg_offset;
};

/// Formatter for duplex test params, used by gtest. Avoids the fallback raw-byte printer, which reads
/// uninitialized padding bytes.
void PrintTo(const cg_duplex_test_params& value, ::std::ostream* os)
{
  *os << fmt::format("{}, default_cg_offset={}", value.name, value.default_cg_offset);
}

} // namespace

/// Builds cell_config_builder_params from duplex test params.
static cell_config_builder_params make_cell_builder_params(const cg_duplex_test_params& p)
{
  auto params = cell_config_builder_profiles::create(p.mode);
  if (p.tdd_pattern.has_value()) {
    params.tdd_ul_dl_cfg_common = p.tdd_pattern.value();
  }
  return params;
}

/// Base class for all configured_grant_scheduler tests. Provides cell + UE setup with CG enabled and
/// helper methods to find CG grants in the scheduler output.
class configured_grant_scheduler_test : public scheduler_test_simulator, public ::testing::Test
{
protected:
  static constexpr rnti_t ue_crnti = to_rnti(0x4601);
  /// CS-RNTI assigned to the test UE. Matches the temporary value used in cg_res_mng.cpp.
  static constexpr rnti_t cs_rnti = to_rnti(0xe0ef);

  cg_test_params                           cg_params;
  sched_cell_configuration_request_message cell_req;
  unsigned                                 default_cg_offset_;

  explicit configured_grant_scheduler_test(const cg_test_params&             params_        = {},
                                           const cell_config_builder_params& builder_params = {},
                                           unsigned                          default_offset = 0) :
    scheduler_test_simulator(/*tx_rx_delay=*/4, builder_params.scs_common),
    cg_params(params_),
    default_cg_offset_(default_offset)
  {
    cell_req = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    if (cg_params.cell_cg_enabled) {
      cell_req.ran.init_bwp.cg_cfg = cg_builder_params{
          .periodicity        = cg_params.periodicity,
          .mcs                = cg_params.mcs,
          .nof_harq_processes = cg_params.nof_harq_procs,
      };
    }
    add_cell(cell_req);
  }

  /// Adds the test UE to the scheduler with CG configuration.
  /// CG is applied via a reconfiguration (not at creation) to match the DRB-gated CG production flow,
  /// where add_reconf_ue() is the sole entry point for CG scheduler setup.
  void add_cg_ue(du_ue_index_t           ue_idx         = to_du_ue_index(0),
                 std::optional<unsigned> cg_slot_offset = std::nullopt,
                 rnti_t                  crnti          = ue_crnti,
                 rnti_t                  cs_rnti_val    = cs_rnti,
                 std::optional<unsigned> cg_start_vrb   = std::nullopt)
  {
    auto ue_req     = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran);
    ue_req.ue_index = ue_idx;
    ue_req.crnti    = crnti;
    ASSERT_TRUE(ue_req.cfg.cells.has_value() and not ue_req.cfg.cells->empty());
    auto& ue_cell = ue_req.cfg.cells->front();
    ASSERT_TRUE(ue_cell.serv_cell_cfg.ul_config.has_value());
    ASSERT_TRUE(ue_cell.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.has_value());
    ue_cell.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg->rrc_configured_ul_grant_cfg->time_domain_offset =
        cg_slot_offset.value_or(default_cg_offset_);
    if (cg_start_vrb.has_value()) {
      auto& freq = std::get<ra_frequency_type1_configuration>(
          ue_cell.serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg->rrc_configured_ul_grant_cfg->freq_domain_res);
      freq.start_vrb = cg_start_vrb.value();
    }

    // Step 1: Create the UE without CG — add_reconf_ue() is the sole CG entry point, so CG setup
    // is deferred to the reconfiguration below. Wait for the creation event to be processed before
    // issuing the reconfiguration, since handle_ue_reconfiguration_request requires the UE to exist.
    auto ue_req_no_cg = ue_req;
    ue_req_no_cg.cfg.cells->front().serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.reset();
    add_ue(ue_req_no_cg, /*wait_notification=*/true);

    // Step 2: Reconfigure to install the CG config, which triggers add_reconf_ue() in the scheduler.
    sched_ue_reconfiguration_message reconf;
    reconf.ue_index = ue_idx;
    reconf.crnti    = crnti;
    reconf.cfg      = ue_req.cfg;
    reconf.cs_rnti  = cs_rnti_val;
    sched->handle_ue_reconfiguration_request(reconf);

    // Step 3: Confirm the UE applied the new config, so it exits the pending_reconf state. Otherwise, the UE stays in
    // fallback, where HARQ-ACKs are never allocated on CG slots (the UE might not have applied the CG config yet).
    sched->handle_ue_config_applied(ue_idx);
  }

  /// Advances the scheduler until a CG PUSCH for \c search_rnti is seen in the output, or \c max_slots is exhausted.
  const ul_sched_info* run_until_next_cg_pusch(unsigned max_slots = 0, rnti_t search_rnti = cs_rnti)
  {
    if (max_slots == 0) {
      // The CG PUSCH is booked max_ul_slot_alloc_delay slots ahead, so the first grant appears in last_sched_result()
      // max_ul_slot_alloc_delay + (period - 1) steps after UE creation. Add extra margin for safety.
      constexpr unsigned ul_delay = get_max_slot_ul_alloc_delay(/*ntn_cs_koffset=*/0);
      max_slots                   = ul_delay + cg_params.period_slots + 20;
    }
    for (unsigned i = 0; i != max_slots; ++i) {
      run_slot();
      const ul_sched_info* grant = find_ue_pusch(search_rnti, *last_sched_result());
      if (grant != nullptr) {
        return grant;
      }
    }
    return nullptr;
  }

  /// Sends a UE reconfiguration with modified CG parameters.
  void reconf_cg(std::optional<cg_configuration::periodicity_t> new_periodicity = std::nullopt,
                 std::optional<unsigned>                        new_offset      = std::nullopt,
                 std::optional<unsigned>                        new_mcs         = std::nullopt)
  {
    auto ue_req     = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran);
    ue_req.ue_index = to_du_ue_index(0);
    ue_req.crnti    = ue_crnti;
    auto& cg_cfg    = ue_req.cfg.cells->front().serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.value();

    if (new_periodicity.has_value()) {
      cg_cfg.periodicity = new_periodicity.value();
    }
    if (new_mcs.has_value()) {
      cg_cfg.rrc_configured_ul_grant_cfg->mcs = new_mcs.value();
    }
    cg_cfg.rrc_configured_ul_grant_cfg->time_domain_offset = new_offset.value_or(default_cg_offset_);

    sched_ue_reconfiguration_message reconf;
    reconf.ue_index = to_du_ue_index(0);
    reconf.crnti    = ue_crnti;
    reconf.cfg      = ue_req.cfg;
    reconf.cs_rnti  = cs_rnti;
    sched->handle_ue_reconfiguration_request(reconf);

    // Confirm the UE applied the new config, so it exits the pending_reconf state (see add_cg_ue()).
    sched->handle_ue_config_applied(to_du_ue_index(0));
  }

  /// Sends a UE reconfiguration that removes the CG configuration (cg_cfg reset to nullopt).
  void remove_cg_via_reconfig() const
  {
    auto ue_req     = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran);
    ue_req.ue_index = to_du_ue_index(0);
    ue_req.crnti    = ue_crnti;
    ue_req.cfg.cells->front().serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.reset();

    sched_ue_reconfiguration_message reconf;
    reconf.ue_index = to_du_ue_index(0);
    reconf.crnti    = ue_crnti;
    reconf.cfg      = ue_req.cfg;
    reconf.cs_rnti  = cs_rnti;
    sched->handle_ue_reconfiguration_request(reconf);
  }

  /// Sends a CRC indication for a CG PUSCH grant with the given CRC result and SINR.
  void send_cg_crc(const ul_sched_info& grant, slot_point pusch_slot, bool crc_ok, std::optional<float> sinr_dB) const
  {
    ul_crc_indication crc_ind;
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.sl_rx      = pusch_slot;
    ul_crc_pdu_indication pdu{};
    pdu.rnti           = grant.pusch_cfg.rnti;
    pdu.ue_index       = grant.context.ue_index;
    pdu.harq_id        = grant.pusch_cfg.harq_id;
    pdu.tb_crc_success = crc_ok;
    pdu.ul_sinr_dB     = sinr_dB;
    crc_ind.crcs.push_back(pdu);
    sched->handle_crc_indication(crc_ind);
  }
};

/// Parameterised fixture for running the base CG tests across FDD and TDD configurations.
class cg_duplex_test : public configured_grant_scheduler_test,
                       public ::testing::WithParamInterface<cg_duplex_test_params>
{
protected:
  cg_duplex_test() :
    configured_grant_scheduler_test(cg_test_params{},
                                    make_cell_builder_params(GetParam()),
                                    GetParam().default_cg_offset)
  {
  }
};

/// Test: after adding a CG UE, CG PUSCH grants appear and repeat with the configured period.
TEST_P(cg_duplex_test, cg_grants_are_periodic)
{
  add_cg_ue();

  // Find the first CG grant.
  const ul_sched_info* first_grant = run_until_next_cg_pusch();
  ASSERT_NE(first_grant, nullptr) << "No CG PUSCH found within " << cg_params.period_slots + 20 << " slots";
  const slot_point first_slot = last_result_slot();

  // Advance exactly cg_period_slots more and expect the next CG grant.
  for (unsigned i = 0; i != cg_params.period_slots; ++i) {
    run_slot();
  }
  EXPECT_NE(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr)
      << "Expected second CG PUSCH " << cg_params.period_slots << " slots after first";
  EXPECT_EQ(last_result_slot() - first_slot, cg_params.period_slots);

  // Advance one more period and verify a third grant.
  for (unsigned i = 0; i != cg_params.period_slots; ++i) {
    run_slot();
  }
  EXPECT_NE(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr)
      << "Expected third CG PUSCH " << 2 * cg_params.period_slots << " slots after first";
}

/// Test: the CG PUSCH output uses the CS-RNTI (not the C-RNTI).
TEST_P(cg_duplex_test, cg_pusch_rnti_is_cs_rnti)
{
  add_cg_ue();
  const ul_sched_info* grant = run_until_next_cg_pusch();
  ASSERT_NE(grant, nullptr);
  EXPECT_EQ(grant->pusch_cfg.rnti, cs_rnti);
  // Verify the grant is NOT indexed by the C-RNTI.
  EXPECT_NE(grant->pusch_cfg.rnti, ue_crnti);
}

/// Test: the OFDM symbols in the CG PUSCH match the PUSCH time-domain allocation entry used by the CG config.
TEST_P(cg_duplex_test, cg_pusch_symbols_match_td_alloc)
{
  add_cg_ue();
  const ul_sched_info* grant = run_until_next_cg_pusch();
  ASSERT_NE(grant, nullptr);

  // CG uses time_domain_allocation = 0 (entry 0 of the cell's common PUSCH TD alloc list).
  const auto& pusch_td_list = cell_cfg().params.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
  ASSERT_FALSE(pusch_td_list.empty()) << "Common PUSCH TD alloc list is empty";
  EXPECT_EQ(grant->pusch_cfg.symbols, pusch_td_list[0].symbols);
}

/// Test: the VRBs in the CG PUSCH match the configured VRB allocation (start=10, length=10).
TEST_P(cg_duplex_test, cg_pusch_rbs_match_cg_config)
{
  add_cg_ue();
  const ul_sched_info* grant = run_until_next_cg_pusch();
  ASSERT_NE(grant, nullptr);

  // The CG config factory sets vrbs = {default_cg_start_vrb, default_cg_start_vrb + default_cg_nof_rbs}.
  constexpr vrb_interval expected_vrbs{default_cg_start_vrb, default_cg_start_vrb + default_cg_nof_rbs};
  ASSERT_TRUE(grant->pusch_cfg.rbs.is_type1()) << "Expected type-1 (contiguous) VRB allocation";
  EXPECT_EQ(grant->pusch_cfg.rbs.type1(), expected_vrbs);
}

/// Test: the decision context fields are filled correctly for CG PUSCH.
TEST_P(cg_duplex_test, cg_pusch_context_fields_are_correct)
{
  add_cg_ue();
  const ul_sched_info* grant = run_until_next_cg_pusch();
  ASSERT_NE(grant, nullptr);

  // CG always schedules new transmissions (no retransmissions).
  EXPECT_EQ(grant->context.nof_retxs, 0U);
  // The UE index should be valid.
  EXPECT_NE(grant->context.ue_index, INVALID_DU_UE_INDEX);
}

/// Test: after removing a CG UE, no further CG PUSCH grants are produced.
TEST_P(cg_duplex_test, after_ue_removal_no_more_cg_grants)
{
  add_cg_ue();

  // Confirm at least one grant appears before removal.
  ASSERT_NE(run_until_next_cg_pusch(), nullptr) << "Pre-condition: expected at least one CG grant";

  // Remove the UE.
  rem_ue(to_du_ue_index(0));

  // Run 2 periods and verify no more CG grants.
  for (unsigned i = 0; i < 2 * cg_params.period_slots + 10; ++i) {
    run_slot();
    EXPECT_EQ(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr) << "Unexpected CG PUSCH after UE removal";
  }
}

/// Test: after changing the CG period from 40 to 20, the gap between consecutive grants halves.
TEST_P(cg_duplex_test, cg_reconfig_period_change)
{
  auto_crc = true;
  add_cg_ue();

  // Find 2 consecutive grants and verify the gap equals the old period (40).
  const ul_sched_info* g1 = run_until_next_cg_pusch();
  ASSERT_NE(g1, nullptr) << "No first CG PUSCH found";
  const slot_point s1 = last_result_slot();

  const ul_sched_info* g2 = run_until_next_cg_pusch();
  ASSERT_NE(g2, nullptr) << "No second CG PUSCH found";
  const slot_point s2 = last_result_slot();
  EXPECT_EQ(s2 - s1, 40) << "Gap before reconfig should be 40 slots";

  // Reconfigure: change period to 20.
  reconf_cg(cg_configuration::periodicity_t::sl20);

  // Find 2 consecutive grants and verify the gap equals the new period (20).
  const ul_sched_info* g3 = run_until_next_cg_pusch();
  ASSERT_NE(g3, nullptr) << "No CG PUSCH found after reconfig";
  const slot_point s3 = last_result_slot();

  const ul_sched_info* g4 = run_until_next_cg_pusch();
  ASSERT_NE(g4, nullptr) << "No second CG PUSCH found after reconfig";
  const slot_point s4 = last_result_slot();
  EXPECT_EQ(s4 - s3, 20) << "Gap after reconfig should be 20 slots";
}

/// Test: after changing the CG time-domain offset, grants shift to a new slot position while maintaining the same
/// period.
TEST_P(cg_duplex_test, cg_reconfig_offset_change)
{
  auto_crc = true;
  add_cg_ue(to_du_ue_index(0), /*cg_slot_offset=*/7);

  // Find a grant and record the slot.
  const ul_sched_info* g1 = run_until_next_cg_pusch();
  ASSERT_NE(g1, nullptr) << "No CG PUSCH found with offset=7";
  const slot_point sl_old = last_result_slot();

  // Reconfigure: change offset from 5 to 9.
  reconf_cg(std::nullopt, 9);

  // Find 2 consecutive grants and verify period is preserved (40).
  const ul_sched_info* g2 = run_until_next_cg_pusch();
  ASSERT_NE(g2, nullptr) << "No CG PUSCH found after offset reconfig";
  const slot_point sl_new = last_result_slot();

  const ul_sched_info* g3 = run_until_next_cg_pusch();
  ASSERT_NE(g3, nullptr) << "No second CG PUSCH found after offset reconfig";
  const slot_point sl3 = last_result_slot();
  EXPECT_EQ(sl3 - sl_new, 40) << "Period should remain 40 after offset change";

  // Verify the new grant position differs from the old one within the period.
  EXPECT_NE((sl_new - sl_old) % 40, 0U) << "New offset should produce grants at a different position within the period";
}

/// Test: a UE that is created without a CG reconfiguration does not produce CG grants.
TEST_P(cg_duplex_test, ue_with_no_cg_config_produces_no_cg_grants)
{
  // Create a UE without CG configuration (no reconfiguration issued, so add_reconf_ue() is never called).
  auto ue_req     = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran);
  ue_req.ue_index = to_du_ue_index(0);
  ue_req.crnti    = ue_crnti;
  ue_req.cfg.cells->front().serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.reset();
  add_ue(ue_req);

  // Run for 2 periods and verify no PUSCH with cs_rnti = 0xe0ef appears.
  for (unsigned i = 0; i != 2 * cg_params.period_slots + 10; ++i) {
    run_slot();
    EXPECT_EQ(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr);
  }
}

/// Test: when a PUCCH HARQ-ACK falls in the same slot as a CG PUSCH, the UCI is moved to the PUSCH and the PUCCH
/// is removed.
TEST_P(cg_duplex_test, cg_pusch_absorbs_pucch_harq_ack)
{
  auto_uci = true;
  auto_crc = true;
  add_cg_ue();

  bool found = false;
  for (unsigned i = 0; i != static_cast<unsigned>(cg_params.periodicity) * 5; ++i) {
    // Keep DL buffer non-empty so the scheduler generates PDSCHs → PUCCH HARQ-ACK reports.
    push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(0), LCID_SRB1, 10000});
    run_slot();

    const sched_result&  res      = *last_sched_result();
    const ul_sched_info* cg_pusch = find_ue_pusch_with_harq_ack(cs_rnti, res);
    if (cg_pusch == nullptr) {
      continue;
    }
    found = true;
    // The PUSCH UCI should carry at least one HARQ-ACK bit.
    EXPECT_GT(cg_pusch->uci->harq->harq_ack_nof_bits, 0U);
    // The PUCCH with HARQ-ACK for this UE must have been removed after the mux.
    EXPECT_EQ(find_ue_pucch_with_harq_ack(ue_crnti, res), nullptr)
        << "PUCCH HARQ-ACK should be absent after UCI mux onto CG PUSCH";
    break;
  }
  EXPECT_TRUE(found) << "No CG PUSCH with muxed HARQ-ACK UCI was observed within 200 slots";
}

/// Test: CG HARQs are freed by timeout before the next CG occasion for the same HARQ ID, so forced
/// reuse never occurs. With configured_grant_timer=4 and nof_harq_procs=4, the timeout is
/// 4 × period_slots, which equals nof_harq_procs × period_slots — exactly the CG reuse period.
TEST_P(cg_duplex_test, cg_harq_freed_by_timeout_before_reuse)
{
  add_cg_ue();

  // Run for several full CG cycles: nof_harq_procs × period_slots × 3 slots.
  // Each cycle uses all HARQ IDs once; if timeouts work, they are freed before reuse.
  const unsigned total_slots    = cg_params.nof_harq_procs * cg_params.period_slots * 3;
  unsigned       cg_grant_count = 0;

  // Find the first CG grant to establish the starting point.
  const ul_sched_info* first_grant = run_until_next_cg_pusch();
  ASSERT_NE(first_grant, nullptr) << "No CG PUSCH found";
  ++cg_grant_count;

  // Run for the remaining slots and count CG grants.
  for (unsigned i = 0; i < total_slots; ++i) {
    run_slot();
    if (find_ue_pusch(cs_rnti, *last_sched_result()) != nullptr) {
      ++cg_grant_count;
    }
  }

  // Expect at least (total_slots / period_slots) grants — one per period.
  const unsigned expected_min_grants = total_slots / cg_params.period_slots;
  EXPECT_GE(cg_grant_count, expected_min_grants)
      << "CG grants should keep appearing every period; HARQ timeout should free processes before reuse";
}

/// Test: CG PUSCH and BSR-triggered dynamic PUSCH coexist for the same UE without interference, when not within the
/// same slot.
TEST_P(cg_duplex_test, cg_and_dynamic_pusch_coexist_when_in_different_slots)
{
  auto_crc = true;
  add_cg_ue();

  // Find first CG grant.
  ASSERT_NE(run_until_next_cg_pusch(), nullptr) << "Pre-condition: expected at least one CG grant";

  // Push a small BSR right after the CG grant is allocated. The dynamic PUSCH will be booked for a near-future slot
  // that is well before the next CG occasion (~period slots away), avoiding resource collisions between the two grant
  // types.
  push_bsr(ul_bsr_indication_message{
      to_du_cell_index(0), to_du_ue_index(0), ue_crnti, bsr_format::SHORT_BSR, {{uint_to_lcg_id(0), 200}}});

  // Run until a dynamic PUSCH appears (should be within a few slots due to k2 delay).
  bool dynamic_found = false;
  for (unsigned i = 0; i != cg_params.period_slots; ++i) {
    run_slot();
    for (const auto& pusch : last_sched_result()->ul.puschs) {
      if (pusch.pusch_cfg.rnti == ue_crnti and not pusch.pusch_cfg.is_cg) {
        dynamic_found = true;
      }
    }
    if (dynamic_found) {
      break;
    }
  }
  EXPECT_TRUE(dynamic_found) << "BSR-triggered dynamic PUSCH should appear alongside CG grants";

  // Verify CG grants continue appearing for 2 more periods after the BSR-triggered scheduling.
  unsigned           cg_count      = 0;
  constexpr unsigned safety_margin = 10U;
  for (unsigned i = 0; i != 2 * cg_params.period_slots + safety_margin; ++i) {
    run_slot();
    if (find_ue_pusch(cs_rnti, *last_sched_result()) != nullptr) {
      ++cg_count;
    }
  }
  EXPECT_GE(cg_count, 2U) << "CG grants should not be suppressed by dynamic scheduling";
}

/// Test: when a CG PUSCH is allocated in a slot, no dynamic PUSCH grant is allocated in that same slot.
TEST_P(cg_duplex_test, no_dynamic_pusch_in_cg_slot)
{
  auto_crc = true;
  add_cg_ue();

  // Find first CG grant to confirm the CG schedule is active.
  ASSERT_NE(run_until_next_cg_pusch(), nullptr) << "Pre-condition: expected at least one CG grant";

  // Push a large BSR so the dynamic scheduler is eager to grant PUSCH in every available slot.
  push_bsr(ul_bsr_indication_message{
      to_du_cell_index(0), to_du_ue_index(0), ue_crnti, bsr_format::SHORT_BSR, {{uint_to_lcg_id(0), 100000}}});

  // Run for several CG periods and check that no slot contains both a CG and a dynamic PUSCH.
  for (unsigned i = 0; i != 4 * cg_params.period_slots; ++i) {
    run_slot();
    const auto& puschs = last_sched_result()->ul.puschs;
    bool has_cg = std::any_of(puschs.begin(), puschs.end(), [](const ul_sched_info& p) { return p.pusch_cfg.is_cg; });
    bool has_dynamic =
        std::any_of(puschs.begin(), puschs.end(), [](const ul_sched_info& p) { return not p.pusch_cfg.is_cg; });
    EXPECT_FALSE(has_cg and has_dynamic) << "Slot " << last_result_slot().count()
                                         << ": CG and dynamic PUSCH must not coexist in the same slot";
  }
}

/// Test: a reconfiguration that removes the CG config (cg_cfg reset to nullopt) stops CG grants while the UE remains.
TEST_P(cg_duplex_test, cg_removal_via_reconfig_stops_grants)
{
  add_cg_ue();

  // Confirm at least one CG grant appears.
  ASSERT_NE(run_until_next_cg_pusch(), nullptr) << "Pre-condition: expected at least one CG grant";

  // Send a reconfiguration with CG removed (cg_cfg = nullopt).
  remove_cg_via_reconfig();

  // Run 2 periods and verify no more CG grants, while the UE is still present.
  constexpr unsigned safety_margin = 10U;
  for (unsigned i = 0; i != 2 * cg_params.period_slots + safety_margin; ++i) {
    run_slot();
    EXPECT_EQ(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr)
        << "Unexpected CG PUSCH after CG removal via reconfig";
  }
}

/// Test: the TBS in the CG PUSCH is consistent with the grant's own PHY parameters (MCS, VRBs, symbols, DMRS).
/// The scheduler pre-computes the TBS when the UE CG config is added and caches it; this verifies the cached value
/// against an independent recomputation from the grant fields (see is_valid_ul_sched_info).
TEST_P(cg_duplex_test, cg_pusch_tbs_is_consistent_with_grant_params)
{
  add_cg_ue();
  const ul_sched_info* grant = run_until_next_cg_pusch();
  ASSERT_NE(grant, nullptr);

  // The MCS index in the grant must match the configured CG MCS.
  EXPECT_EQ(grant->pusch_cfg.mcs_index.value(), cg_params.mcs);
  EXPECT_GT(grant->pusch_cfg.tb_size_bytes.value(), 0U);

  // Recomputes the TBS from the grant's symbols, DMRS, MCS description and RB count, and
  // checks the effective code rate. A stale or wrongly computed cached TBS would fail this check.
  EXPECT_TRUE(test_helper::is_valid_ul_sched_info(*grant)) << "CG PUSCH TBS/code-rate inconsistent with grant params";
}

/// Test: a reconfiguration that changes the CG MCS is reflected in the next grant, including a recomputed TBS.
TEST_P(cg_duplex_test, cg_reconfig_mcs_change_updates_tbs)
{
  auto_crc = true;
  add_cg_ue();

  const ul_sched_info* g1 = run_until_next_cg_pusch();
  ASSERT_NE(g1, nullptr) << "No CG PUSCH found before MCS reconfig";
  ASSERT_EQ(g1->pusch_cfg.mcs_index.value(), cg_params.mcs);
  const units::bytes tbs_before_mcs_changed = g1->pusch_cfg.tb_size_bytes;

  // Reconfigure: raise the MCS (5 -> 20), which increases the TBS for the same VRB/symbol allocation.
  constexpr unsigned new_mcs = 20;
  reconf_cg(std::nullopt, std::nullopt, new_mcs);

  const ul_sched_info* g2 = run_until_next_cg_pusch();
  ASSERT_NE(g2, nullptr) << "No CG PUSCH found after MCS reconfig";
  EXPECT_EQ(g2->pusch_cfg.mcs_index.value(), new_mcs);
  EXPECT_GT(g2->pusch_cfg.tb_size_bytes.value(), tbs_before_mcs_changed.value())
      << "Cached TBS should be recomputed after the MCS reconfiguration";
  EXPECT_TRUE(test_helper::is_valid_ul_sched_info(*g2)) << "CG PUSCH TBS/code-rate inconsistent after MCS reconfig";
}

/// Test: after CG is removed via reconfiguration, a subsequent reconfiguration that re-installs the CG config
/// makes CG grants resume. Also exercises the CG scheduler add/rem/add bookkeeping (slot wheel and TBS table).
TEST_P(cg_duplex_test, cg_readd_after_removal_via_reconfig_resumes_grants)
{
  add_cg_ue();

  // Confirm at least one CG grant appears.
  ASSERT_NE(run_until_next_cg_pusch(), nullptr) << "Pre-condition: expected at least one CG grant";

  // Remove CG via reconfig and verify grants stop for one period.
  remove_cg_via_reconfig();
  constexpr unsigned safety_margin = 10U;
  for (unsigned i = 0; i != cg_params.period_slots + safety_margin; ++i) {
    run_slot();
    ASSERT_EQ(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr) << "Unexpected CG PUSCH after CG removal";
  }

  // Re-install the CG config via reconfiguration and verify grants resume.
  reconf_cg();
  const ul_sched_info* grant = run_until_next_cg_pusch();
  EXPECT_NE(grant, nullptr) << "CG grants should resume after CG is re-added via reconfig";
}

INSTANTIATE_TEST_SUITE_P(
    cg_duplex,
    cg_duplex_test,
    ::testing::Values(cg_duplex_test_params{"FDD", duplex_mode::FDD, std::nullopt, 0},
                      cg_duplex_test_params{"TDD_6D1S3U",
                                            duplex_mode::TDD,
                                            tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 6, 10, 3, 0}},
                                            8},
                      cg_duplex_test_params{"TDD_3D1S6U",
                                            duplex_mode::TDD,
                                            tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 3, 10, 6, 0}},
                                            5}),
    [](const auto& p) { return p.param.name; });

/// Fixture for multi-UE CG tests. Provides constants for a second CG UE.
class cg_multi_ue_test : public configured_grant_scheduler_test
{
protected:
  static constexpr rnti_t ue2_crnti = to_rnti(0x4602);
  static constexpr rnti_t cs_rnti_2 = to_rnti(0xe0f0);
  /// VRB start for UE2, placed after UE1's default [10, 20).
  static constexpr unsigned ue2_start_vrb = 20;
};

/// Test: two CG UEs with different slot offsets produce independent periodic grants.
TEST_F(cg_multi_ue_test, two_ues_with_different_offsets)
{
  // UE1: default offset (0), default VRBs [10, 20).
  add_cg_ue(to_du_ue_index(0), /*cg_slot_offset=*/std::nullopt, ue_crnti, cs_rnti);
  // UE2: offset 1, VRBs [20, 30), different C-RNTI/CS-RNTI.
  add_cg_ue(to_du_ue_index(1), /*cg_slot_offset=*/1, ue2_crnti, cs_rnti_2, ue2_start_vrb);

  // Find first CG grant for each UE.
  const ul_sched_info* g1_ue1 = run_until_next_cg_pusch(/*max_slots=*/0, cs_rnti);
  ASSERT_NE(g1_ue1, nullptr) << "No CG PUSCH found for UE1";

  const ul_sched_info* g1_ue2 = run_until_next_cg_pusch(/*max_slots=*/0, cs_rnti_2);
  ASSERT_NE(g1_ue2, nullptr) << "No CG PUSCH found for UE2";

  // Advance one period and verify each UE gets its next grant.
  // Safety margin: extra slots beyond one period to account for the UL allocation delay (the CG grant is
  // booked several slots ahead). Without it, if the next grant lands a few slots past the period boundary, the search
  // would stop too early and miss it
  constexpr unsigned   safety_margin = 10U;
  const ul_sched_info* g2_ue1        = run_until_next_cg_pusch(cg_params.period_slots + safety_margin, cs_rnti);
  ASSERT_NE(g2_ue1, nullptr) << "No second CG PUSCH for UE1";

  const ul_sched_info* g2_ue2 = run_until_next_cg_pusch(cg_params.period_slots + safety_margin, cs_rnti_2);
  ASSERT_NE(g2_ue2, nullptr) << "No second CG PUSCH for UE2";

  // Verify correct RNTI assignment.
  EXPECT_EQ(g2_ue1->pusch_cfg.rnti, cs_rnti);
  EXPECT_EQ(g2_ue2->pusch_cfg.rnti, cs_rnti_2);
}

/// Test: two CG UEs sharing the same slot offset coexist with non-overlapping VRBs.
TEST_F(cg_multi_ue_test, two_ues_same_offset_coexist)
{
  // UE1: default offset, VRBs [10, 20).
  add_cg_ue(to_du_ue_index(0), /*cg_slot_offset=*/std::nullopt, ue_crnti, cs_rnti);
  // UE2: same default offset, VRBs [20, 30).
  add_cg_ue(to_du_ue_index(1), /*cg_slot_offset=*/std::nullopt, ue2_crnti, cs_rnti_2, ue2_start_vrb);

  // Run until we find a slot where both UEs have CG grants.
  bool               found         = false;
  constexpr unsigned safety_margin = 20U;
  for (unsigned i = 0; i != 2 * (cg_params.period_slots + safety_margin); ++i) {
    run_slot();
    const ul_sched_info* pusch_ue1 = find_ue_pusch(cs_rnti, *last_sched_result());
    const ul_sched_info* pusch_ue2 = find_ue_pusch(cs_rnti_2, *last_sched_result());
    if (pusch_ue1 != nullptr and pusch_ue2 != nullptr) {
      found = true;
      // Verify VRB ranges don't overlap.
      ASSERT_TRUE(pusch_ue1->pusch_cfg.rbs.is_type1());
      ASSERT_TRUE(pusch_ue2->pusch_cfg.rbs.is_type1());
      const vrb_interval vrbs1 = pusch_ue1->pusch_cfg.rbs.type1();
      const vrb_interval vrbs2 = pusch_ue2->pusch_cfg.rbs.type1();
      EXPECT_FALSE(vrbs1.overlaps(vrbs2)) << "VRBs overlap: UE1=[" << vrbs1.start() << "," << vrbs1.stop() << ") UE2=["
                                          << vrbs2.start() << "," << vrbs2.stop() << ")";
      break;
    }
  }
  EXPECT_TRUE(found) << "No slot found where both UEs have CG PUSCH grants";
}

/// Test: removing one CG UE does not disrupt the other UE's CG grants.
TEST_F(cg_multi_ue_test, removing_one_ue_preserves_other_cg_grants)
{
  // UE1: default offset (0), default VRBs.
  add_cg_ue(to_du_ue_index(0), /*cg_slot_offset=*/std::nullopt, ue_crnti, cs_rnti);
  // UE2: offset 1, different VRBs/RNTIs.
  add_cg_ue(to_du_ue_index(1), /*cg_slot_offset=*/1, ue2_crnti, cs_rnti_2, ue2_start_vrb);

  // Confirm both produce grants.
  ASSERT_NE(run_until_next_cg_pusch(/*max_slots=*/0, cs_rnti), nullptr) << "UE1 should produce CG grants";
  ASSERT_NE(run_until_next_cg_pusch(/*max_slots=*/0, cs_rnti_2), nullptr) << "UE2 should produce CG grants";

  // Remove UE1.
  rem_ue(to_du_ue_index(0));

  // Run 2 more periods and verify UE2 grants continue while UE1 grants stop.
  unsigned           ue2_grant_count = 0;
  constexpr unsigned safety_margin   = 20U;
  for (unsigned i = 0; i != 2 * cg_params.period_slots + safety_margin; ++i) {
    run_slot();
    EXPECT_EQ(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr) << "UE1 CG PUSCH should not appear after removal";
    if (find_ue_pusch(cs_rnti_2, *last_sched_result()) != nullptr) {
      ++ue2_grant_count;
    }
  }
  EXPECT_GE(ue2_grant_count, 2U) << "UE2 should continue producing CG grants after UE1 removal";
}

/// Parameterised fixture for checking that the periodicity is met for different CG period values.
struct cg_period_test_params {
  cg_configuration::periodicity_t periodicity;
  unsigned                        period_slots;
};

/// Formatter for period test params, used by gtest. Avoids the fallback raw-byte printer.
void PrintTo(const cg_period_test_params& value, ::std::ostream* os)
{
  *os << fmt::format("period={} slots", value.period_slots);
}

class cg_period_test : public configured_grant_scheduler_test,
                       public ::testing::WithParamInterface<cg_period_test_params>
{
protected:
  cg_period_test() :
    configured_grant_scheduler_test(cg_test_params{
        .periodicity    = GetParam().periodicity,
        .period_slots   = GetParam().period_slots,
        .mcs            = 5,
        .nof_harq_procs = 8,
    })
  {
  }
};

TEST_P(cg_period_test, period_is_met)
{
  add_cg_ue();

  const ul_sched_info* first_grant = run_until_next_cg_pusch();
  ASSERT_NE(first_grant, nullptr) << "No CG grant found for period=" << cg_params.period_slots;
  const slot_point first_slot = last_result_slot();

  for (unsigned i = 0; i < cg_params.period_slots; ++i) {
    run_slot();
  }
  EXPECT_NE(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr);
  EXPECT_EQ(last_result_slot() - first_slot, cg_params.period_slots);
}

INSTANTIATE_TEST_SUITE_P(cg_periods,
                         cg_period_test,
                         ::testing::Values(cg_period_test_params{cg_configuration::periodicity_t::sl10, 10},
                                           cg_period_test_params{cg_configuration::periodicity_t::sl20, 20},
                                           cg_period_test_params{cg_configuration::periodicity_t::sl40, 40},
                                           cg_period_test_params{cg_configuration::periodicity_t::sl80, 80}));

/// Fixture for CG DTX detection tests. Uses a single HARQ process so that every CG occasion must reuse the same
/// HARQ ID. With configured_grant_timer=4 and period=40, the HARQ timeout is 160 slots (4 periods). If the CRC
/// indication doesn't free the HARQ, no CG PUSCH can be scheduled for the next 3 periods.
class cg_dtx_test : public configured_grant_scheduler_test
{
protected:
  cg_dtx_test() :
    configured_grant_scheduler_test(cg_test_params{
        .periodicity    = cg_configuration::periodicity_t::sl40,
        .period_slots   = 40,
        .mcs            = 5,
        .nof_harq_procs = 1,
    })
  {
  }
};

/// Test: DTX is detected when CRC KO and SINR is below the threshold (-8 dB). With a single HARQ process, the
/// only way the next CG PUSCH appears within one period is if the DTX path freed the HARQ.
TEST_F(cg_dtx_test, cg_dtx_detected_when_crc_ko_and_low_sinr)
{
  add_cg_ue();

  const ul_sched_info* grant = run_until_next_cg_pusch();
  ASSERT_NE(grant, nullptr) << "No CG PUSCH found";

  // Send DTX: CRC KO with SINR well below the -8 dB threshold.
  send_cg_crc(*grant, last_result_slot(), false, -20.0f);

  // With 1 HARQ and configured_grant_timer=4 (timeout = 4 × period = 160 slots), if DTX doesn't free
  // the HARQ, no CG PUSCH would appear for 4 periods. Verify it appears within 1 period.
  const ul_sched_info* next_grant = run_until_next_cg_pusch(cg_params.period_slots + 10);
  EXPECT_NE(next_grant, nullptr) << "CG PUSCH should appear within 1 period after DTX (HARQ should be freed)";
}

/// Test: when CRC KO but SINR is above the threshold, DTX is NOT triggered (treated as NACK). With max_retx=0,
/// the HARQ is freed by NACK handling and the next CG grant appears within one period.
TEST_F(cg_dtx_test, cg_nack_when_crc_ko_and_high_sinr)
{
  add_cg_ue();

  const ul_sched_info* grant = run_until_next_cg_pusch();
  ASSERT_NE(grant, nullptr) << "No CG PUSCH found";

  // Send NACK: CRC KO with SINR above the -8 dB threshold (not DTX).
  send_cg_crc(*grant, last_result_slot(), false, 10.0f);

  // HARQ freed by NACK (max_retx=0) — next CG should appear within 1 period.
  const ul_sched_info* next_grant = run_until_next_cg_pusch(cg_params.period_slots + 10);
  EXPECT_NE(next_grant, nullptr) << "CG PUSCH should appear within 1 period after NACK (HARQ should be freed)";
}

/// Test: when CRC KO but SINR is not reported (nullopt), DTX is NOT triggered. HARQ is freed by NACK handling.
TEST_F(cg_dtx_test, cg_no_dtx_when_sinr_missing)
{
  add_cg_ue();

  const ul_sched_info* grant = run_until_next_cg_pusch();
  ASSERT_NE(grant, nullptr) << "No CG PUSCH found";

  // Send CRC KO with no SINR reported — should NOT trigger DTX, treated as NACK.
  send_cg_crc(*grant, last_result_slot(), false, std::nullopt);

  // HARQ freed by NACK (max_retx=0) — next CG should appear within 1 period.
  const ul_sched_info* next_grant = run_until_next_cg_pusch(cg_params.period_slots + 10);
  EXPECT_NE(next_grant, nullptr) << "CG PUSCH should appear within 1 period when SINR is missing";
}

/// Test: when the CRC is OK (ACK), the HARQ is freed immediately and the next CG grant appears within one period.
/// With a single HARQ process, the next grant can only appear if the ACK freed the HARQ (the timeout would take
/// 4 periods).
TEST_F(cg_dtx_test, cg_ack_frees_harq_for_next_occasion)
{
  add_cg_ue();

  const ul_sched_info* grant = run_until_next_cg_pusch();
  ASSERT_NE(grant, nullptr) << "No CG PUSCH found";

  // Send ACK: CRC OK.
  send_cg_crc(*grant, last_result_slot(), true, 15.0f);

  const ul_sched_info* next_grant = run_until_next_cg_pusch(cg_params.period_slots + 10);
  EXPECT_NE(next_grant, nullptr) << "CG PUSCH should appear within 1 period after ACK (HARQ should be freed)";
}

/// Test: when no CRC indication ever arrives, CG grants keep appearing every period. With 1 HARQ process and
/// period=40, the same HARQ ID is needed again one period later, while the HARQ timeout is 4 periods (160 slots);
/// the HARQ manager must forcibly reuse the still-busy HARQ process (forced-reuse path) instead of stalling.
TEST_F(cg_dtx_test, cg_grants_continue_when_crc_never_arrives)
{
  add_cg_ue();

  const ul_sched_info* first_grant = run_until_next_cg_pusch();
  ASSERT_NE(first_grant, nullptr) << "No CG PUSCH found";
  unsigned cg_grant_count = 1;

  // Never send a CRC indication. Run for 8 periods (2 full HARQ-timeout cycles) and count CG grants.
  const unsigned total_slots = 8 * cg_params.period_slots;
  for (unsigned i = 0; i != total_slots; ++i) {
    run_slot();
    if (find_ue_pusch(cs_rnti, *last_sched_result()) != nullptr) {
      ++cg_grant_count;
    }
  }

  // Expect one grant per period despite the missing CRCs.
  const unsigned expected_min_grants = total_slots / cg_params.period_slots;
  EXPECT_GE(cg_grant_count, expected_min_grants)
      << "CG grants should continue via HARQ forced reuse when the CRC never arrives";
}

/// Test: stress test sending DTX for every CG grant across multiple cycles. With a single HARQ, every CG occasion
/// reuses the same HARQ ID; any free-list corruption would cause later allocations to fail.
TEST_F(cg_dtx_test, cg_dtx_stress_multiple_cycles)
{
  add_cg_ue();

  // With 1 HARQ, each period reuses the same ID. Run for 12 periods (= configured_grant_timer × 3 cycles).
  const unsigned total_slots    = cg_configuration::configured_grant_timer * cg_params.period_slots * 3;
  unsigned       cg_grant_count = 0;

  // Find the first CG grant.
  const ul_sched_info* first_grant = run_until_next_cg_pusch();
  ASSERT_NE(first_grant, nullptr) << "No CG PUSCH found";
  send_cg_crc(*first_grant, last_result_slot(), false, -20.0f);
  ++cg_grant_count;

  // Run for the remaining slots, sending DTX for every CG grant.
  for (unsigned i = 0; i < total_slots; ++i) {
    run_slot();
    const ul_sched_info* grant = find_ue_pusch(cs_rnti, *last_sched_result());
    if (grant != nullptr) {
      send_cg_crc(*grant, last_result_slot(), false, -20.0f);
      ++cg_grant_count;
    }
  }

  // Expect at least one grant per period.
  const unsigned expected_min_grants = total_slots / cg_params.period_slots;
  EXPECT_GE(cg_grant_count, expected_min_grants)
      << "CG grants should keep appearing every period despite continuous DTX";
}

/// Fixture for the CSI mux test. The CG offset is forced to match the CSI report slot offset so that CG
/// PUSCH and periodic CSI PUCCH overlap deterministically.
class cg_csi_mux_test : public configured_grant_scheduler_test
{
protected:
  /// CSI report slot offset assigned by the default cell config (last full UL slot in the TDD period).
  static constexpr unsigned csi_report_slot_offset = 9;
};

/// Test: when a PUCCH CSI report falls in the same slot as a CG PUSCH, the CSI is moved to the PUSCH and the
/// PUCCH is removed.
TEST_F(cg_csi_mux_test, cg_pusch_absorbs_pucch_csi)
{
  auto_uci = true;
  auto_crc = true;
  // Force the CG slot offset to the CSI report offset so that CG PUSCH and CSI PUCCH coincide periodically.
  add_cg_ue(to_du_ue_index(0), csi_report_slot_offset);

  // With CG offset=9 and CSI period/offset=9, overlaps occur periodically.
  bool found = false;
  for (unsigned i = 0; i != 400; ++i) {
    push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(0), LCID_SRB1, 10000});
    run_slot();

    const sched_result&  res      = *last_sched_result();
    const ul_sched_info* cg_pusch = find_ue_pusch(cs_rnti, res);
    if (cg_pusch == nullptr or not cg_pusch->uci.has_value()) {
      continue;
    }
    if (not cg_pusch->uci->csi.has_value() or cg_pusch->uci->csi->csi_part1_nof_bits == 0) {
      continue;
    }
    found = true;
    // The PUSCH UCI should carry at least one CSI Part 1 bit.
    EXPECT_GT(cg_pusch->uci->csi->csi_part1_nof_bits, 0U);
    // The PUCCH carrying CSI for this UE must have been removed after the mux.
    EXPECT_EQ(find_ue_pucch_with_csi(ue_crnti, res.ul.pucchs.unsorted()), nullptr)
        << "PUCCH CSI should be absent after UCI mux onto CG PUSCH";
    break;
  }
  EXPECT_TRUE(found) << "No CG PUSCH with muxed CSI was observed within 400 slots";
}

/// Fixture for the multi-bit HARQ-ACK mux test. Uses a DL-heavy TDD pattern (6D1S3U) so that HARQ-ACKs from
/// multiple PDSCHs fall on the same UL slot, producing UCI payloads with more than one HARQ-ACK bit. The CG offset
/// targets the first UL slot of the TDD period (slot 7), where most of the PDSCH HARQ-ACKs accumulate.
class cg_multi_harq_ack_mux_test : public configured_grant_scheduler_test
{
protected:
  cg_multi_harq_ack_mux_test() :
    configured_grant_scheduler_test(cg_test_params{},
                                    make_cell_builder_params(cg_duplex_test_params{
                                        "TDD_6D1S3U",
                                        duplex_mode::TDD,
                                        tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 6, 10, 3, 0}},
                                        7}),
                                    /*default_offset=*/7)
  {
  }
};

/// Test: a CG PUSCH can absorb more than one HARQ-ACK bit from the PUCCH (no max-1 HARQ-bit constraint for CG).
TEST_F(cg_multi_harq_ack_mux_test, cg_pusch_absorbs_multiple_harq_ack_bits)
{
  auto_uci = true;
  auto_crc = true;
  add_cg_ue();

  bool               found               = false;
  constexpr unsigned nof_periods_to_test = 10U;
  for (unsigned i = 0; i != static_cast<unsigned>(cg_params.periodicity) * nof_periods_to_test; ++i) {
    // Keep DL buffer non-empty so the scheduler generates PDSCHs in every DL slot; with 6 DL slots ACKing on 3 UL
    // slots, several PDSCH HARQ-ACKs accumulate on the CG PUSCH slot.
    push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(0), LCID_SRB1, 10000});
    run_slot();

    const sched_result&  res      = *last_sched_result();
    const ul_sched_info* cg_pusch = find_ue_pusch_with_harq_ack(cs_rnti, res);
    if (cg_pusch == nullptr or cg_pusch->uci->harq->harq_ack_nof_bits < 2) {
      continue;
    }
    found = true;
    // The PUCCH with HARQ-ACK for this UE must have been removed after the mux.
    EXPECT_EQ(find_ue_pucch_with_harq_ack(ue_crnti, res), nullptr)
        << "PUCCH HARQ-ACK should be absent after multi-bit UCI mux onto CG PUSCH";
    break;
  }
  EXPECT_TRUE(found) << "No CG PUSCH carrying >=2 HARQ-ACK bits was observed within 400 slots";
}

/// Fixture for a cell without CG configured at cell level. In this case, the CG scheduler is not instantiated
/// (nullptr) and the UE scheduler must run all its slot/event paths without it.
class cg_disabled_cell_test : public configured_grant_scheduler_test
{
protected:
  cg_disabled_cell_test() : configured_grant_scheduler_test(cg_test_params{.cell_cg_enabled = false}) {}
};

/// Test: a cell built without CG at cell level runs UE creation, reconfiguration and removal without producing
/// any CG grant and without crashing (the CG scheduler is not instantiated).
TEST_F(cg_disabled_cell_test, cell_without_cg_config_runs_and_produces_no_cg_grants)
{
  // Create the UE. Since the cell has no CG configured, the default UE config carries no CG either.
  auto ue_req     = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran);
  ue_req.ue_index = to_du_ue_index(0);
  ue_req.crnti    = ue_crnti;
  ASSERT_FALSE(ue_req.cfg.cells->front().serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.has_value())
      << "Pre-condition: UE config should have no CG when the cell has no CG";
  add_ue(ue_req, /*wait_notification=*/true);

  // Reconfigure the UE (still without CG) — exercises the reconfiguration path with no CG scheduler.
  sched_ue_reconfiguration_message reconf;
  reconf.ue_index = to_du_ue_index(0);
  reconf.crnti    = ue_crnti;
  reconf.cfg      = ue_req.cfg;
  sched->handle_ue_reconfiguration_request(reconf);
  sched->handle_ue_config_applied(to_du_ue_index(0));

  // Run for 2 CG periods and verify no CG PUSCH grant appears.
  constexpr unsigned safety_margin = 10U;
  for (unsigned i = 0; i != 2 * cg_params.period_slots + safety_margin; ++i) {
    run_slot();
    ASSERT_EQ(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr) << "Unexpected CG PUSCH in a cell without CG";
  }

  // Remove the UE — exercises the UE deletion path with no CG scheduler.
  rem_ue(to_du_ue_index(0));
  for (unsigned i = 0; i != 10; ++i) {
    run_slot();
  }
}

/// Fixture for CG behavior of UEs created via F1AP without RACH (e.g. RRC Reestablishment), which start in fallback
/// mode awaiting a C-RNTI MAC CE to complete contention resolution (conres_st = pending_conres_crnti_ce).
class cg_fallback_ue_test : public configured_grant_scheduler_test
{
protected:
  static constexpr du_ue_index_t ue_idx = to_du_ue_index(0);

  /// Creates the UE in fallback via the F1AP path (no UL-CCCH slot, no CFRA), without CG configuration.
  /// Note: scheduler_test_simulator::add_ue() is bypassed, as it auto-completes contention resolution by sending the
  /// C-RNTI CE right after the creation request, whereas this fixture must keep the UE awaiting the C-RNTI CE.
  void add_fallback_crnti_ce_ue()
  {
    auto ue_req               = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran);
    ue_req.ue_index           = ue_idx;
    ue_req.crnti              = ue_crnti;
    ue_req.starts_in_fallback = true;
    ASSERT_EQ(sched_config_helper::to_ue_creation_mode(ue_req), ue_creation_mode::high_layers);
    // Create the UE without CG: the CG config is installed via reconfiguration (see add_cg_ue()).
    ue_req.cfg.cells->front().serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.reset();

    sched->handle_ue_creation_request(ue_req);
    notif.last_ue_index_cfg.reset();
    for (unsigned i = 0; i != 100 and notif.last_ue_index_cfg != ue_idx; ++i) {
      run_slot();
    }
    ASSERT_EQ(notif.last_ue_index_cfg, ue_idx) << "UE creation was not completed";
  }

  /// Sends a reconfiguration that installs the CG configuration and confirms it was applied by the UE.
  void install_cg_via_reconf() const
  {
    auto ue_req     = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran);
    ue_req.ue_index = ue_idx;
    ue_req.crnti    = ue_crnti;
    ue_req.cfg.cells->front()
        .serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg->rrc_configured_ul_grant_cfg->time_domain_offset =
        default_cg_offset_;

    sched_ue_reconfiguration_message reconf;
    reconf.ue_index = ue_idx;
    reconf.crnti    = ue_crnti;
    reconf.cfg      = ue_req.cfg;
    reconf.cs_rnti  = cs_rnti;
    sched->handle_ue_reconfiguration_request(reconf);
    sched->handle_ue_config_applied(ue_idx);
  }
};

/// Test (regression): for an F1AP-created UE awaiting a C-RNTI MAC CE, the CG setup requested via reconfiguration is
/// deferred until the CE is received. No CG grant may appear before the CE, grants must appear after it, and the UE
/// must be removed cleanly afterwards (the deferred CG registration must be caught up at CE reception, or the removal
/// would run for a UE never registered in the CG scheduler).
TEST_F(cg_fallback_ue_test, cg_setup_is_deferred_until_crnti_ce_and_ue_is_removed_cleanly)
{
  add_fallback_crnti_ce_ue();

  // Install the CG config while contention resolution is still pending: the CG scheduler registration is skipped.
  install_cg_via_reconf();

  // While the C-RNTI CE is pending, no CG PUSCH must be scheduled.
  ASSERT_EQ(run_until_next_cg_pusch(), nullptr) << "CG PUSCH scheduled while contention resolution is pending";

  // C-RNTI CE received: contention resolution completes and the deferred CG registration is performed.
  sched->handle_crnti_ce_received(ue_idx);

  // CG grants must now appear with the configured periodicity.
  const ul_sched_info* g1 = run_until_next_cg_pusch();
  ASSERT_NE(g1, nullptr) << "No CG PUSCH scheduled after the C-RNTI CE was received";
  const slot_point sl1 = last_result_slot();

  const ul_sched_info* g2 = run_until_next_cg_pusch();
  ASSERT_NE(g2, nullptr) << "No second CG PUSCH scheduled after the C-RNTI CE was received";
  EXPECT_EQ(last_result_slot() - sl1, static_cast<int>(cg_params.period_slots));

  // Remove the UE and wait for the removal to complete.
  schedule_task(launch_rem_ue_task(ue_idx));
  run_until_all_pending_tasks_completion();
}

/// Test: for an F1AP-created UE awaiting a C-RNTI MAC CE that never arrives, the CG setup requested via
/// reconfiguration stays deferred: no CG grant is ever allocated, and the UE removal completes cleanly (the removal
/// must not attempt to unregister a UE that was never registered in the CG scheduler).
TEST_F(cg_fallback_ue_test, when_crnti_ce_never_arrives_no_cg_grant_is_allocated_and_ue_is_removed_cleanly)
{
  add_fallback_crnti_ce_ue();

  // Install the CG config while contention resolution is still pending: the CG scheduler registration is skipped.
  install_cg_via_reconf();

  // The C-RNTI CE never arrives: no CG PUSCH must be scheduled over several CG periods.
  constexpr unsigned safety_margin = 10U;
  for (unsigned i = 0; i != 3 * cg_params.period_slots + safety_margin; ++i) {
    run_slot();
    ASSERT_EQ(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr)
        << "CG PUSCH scheduled for a UE whose contention resolution never completed";
  }

  // Remove the UE while contention resolution is still pending and wait for the removal to complete.
  schedule_task(launch_rem_ue_task(ue_idx));
  run_until_all_pending_tasks_completion();

  // After the removal, no CG PUSCH must appear either.
  for (unsigned i = 0; i != cg_params.period_slots + safety_margin; ++i) {
    run_slot();
    ASSERT_EQ(find_ue_pusch(cs_rnti, *last_sched_result()), nullptr) << "CG PUSCH scheduled after the UE was removed";
  }
}
