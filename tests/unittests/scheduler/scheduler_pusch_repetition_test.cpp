// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit tests for SINR-triggered Rel-16 PUSCH repetitions.

#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "ocudu/scheduler/rrm/pucch_resource_manager.h"
#include <cstdlib>
#include <gtest/gtest.h>
#include <map>

using namespace ocudu;

/// RV of a PUSCH repetition occasion, as per TS 38.214 Table 6.1.2.1-2 (same fixed cycle as PDSCH).
static uint8_t expected_repetition_rv(uint8_t dci_rv, unsigned occasion_idx)
{
  static constexpr std::array<uint8_t, 4> rv_cycle = {0, 2, 3, 1};
  const auto*                             it       = std::find(rv_cycle.begin(), rv_cycle.end(), dci_rv);
  return rv_cycle[(std::distance(rv_cycle.begin(), it) + occasion_idx) % rv_cycle.size()];
}

class base_pusch_repetition_tester : public scheduler_test_simulator
{
protected:
  static constexpr uint8_t nof_reps = 4;

  base_pusch_repetition_tester(std::optional<float>                                        sinr_rep_threshold,
                               double                                                      initial_ul_sinr,
                               bool                                                        force_rep     = false,
                               bool                                                        auto_feedback = false,
                               cell_config_builder_profiles::tdd_pattern_profile_fr1_30khz tdd_pattern =
                                   cell_config_builder_profiles::tdd_pattern_profile_fr1_30khz::DDDDDDDSUU,
                               std::optional<slot_point> start_slot = std::nullopt) :
    scheduler_test_simulator(scheduler_test_sim_config{.sched_cfg =
                                                           [sinr_rep_threshold, initial_ul_sinr, force_rep]() {
                                                             auto expert_cfg =
                                                                 config_helpers::make_default_scheduler_expert_config();
                                                             expert_cfg.ue.pusch_sinr_rep_threshold =
                                                                 sinr_rep_threshold;
                                                             expert_cfg.ue.pusch_force_rep = force_rep;
                                                             expert_cfg.ue.initial_ul_sinr = initial_ul_sinr;
                                                             // Keep the grant clear of the cell's common PUCCH: an
                                                             // occasion reuses the base PRBs in every slot, and the
                                                             // PUSCH CRB limits sizing it are slot-independent.
                                                             expert_cfg.ue.pusch_nof_rbs    = {1, 20};
                                                             expert_cfg.ue.pusch_crb_limits = {10, 40};
                                                             return expert_cfg;
                                                           }(),
                                                       .max_scs    = subcarrier_spacing::kHz30,
                                                       .auto_uci   = auto_feedback,
                                                       .auto_crc   = auto_feedback,
                                                       .start_slot = start_slot})
  {
    params                      = cell_config_builder_profiles::create(duplex_mode::TDD);
    params.tdd_ul_dl_cfg_common = cell_config_builder_profiles::create_tdd_pattern(tdd_pattern);

    // Add Cell.
    auto cell_req = sched_config_helper::make_default_sched_cell_configuration_request(params);
    this->add_cell(cell_req);
    pucch_cfg_builder.add_cell(to_du_cell_index(0), cell_cfg(to_du_cell_index(0)).params);

    // Add UE with a Rel-16 PUSCH TDRA list that mirrors the common list and appends a repetition entry.
    auto ue_cfg           = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran, {ue_drb_lcid});
    ue_cfg.ue_index       = ue_idx;
    ue_cfg.crnti          = ue_rnti;
    auto&       pusch_cfg = (*ue_cfg.cfg.cells)[0].serv_cell_cfg.ul_config->init_ul_bwp.pusch_cfg.value();
    const auto& common_list = cell_req.ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (const auto& alloc : common_list) {
      pusch_cfg.pusch_td_alloc_list.push_back(alloc);
    }
    rep_time_resource                               = pusch_cfg.pusch_td_alloc_list.size();
    pusch_time_domain_resource_allocation rep_alloc = common_list.front();
    rep_alloc.nof_repetitions                       = nof_reps;
    rep_k2                                          = rep_alloc.k2;
    pusch_cfg.pusch_td_alloc_list.push_back(rep_alloc);
    // Give the UE dedicated PUCCH resources: a HARQ-ACK on the cell's common resources cannot be multiplexed onto a
    // PUSCH, so a bundle overlapping one is rejected before its DAI is ever considered.
    report_fatal_error_if_not(pucch_cfg_builder.alloc_resources((*ue_cfg.cfg.cells)[0]),
                              "Failed to allocate PUCCH resources");
    this->add_ue(ue_cfg);
  }

  bool is_fully_ul(slot_point sl) const { return cell_cfg(to_du_cell_index(0)).is_fully_ul_enabled(sl); }

  /// \brief Slot of the n-th repetition occasion (n=0 returns \c base_slot itself), skipping DL/special slots, as
  /// per Rel-17 available slot counting -- mirrors the production selection logic.
  slot_point nth_occasion_slot(slot_point base_slot, unsigned n) const
  {
    slot_point sl = base_slot;
    for (unsigned count = 0; count != n; ++count) {
      do {
        ++sl;
      } while (not is_fully_ul(sl));
    }
    return sl;
  }

  /// Runs the scheduler for \c nof_slots slots, collecting per-slot copies of the results for the test UE.
  void run_and_collect(unsigned nof_slots)
  {
    for (unsigned i = 0; i != nof_slots; ++i) {
      this->run_slot();
      last_collected_slot     = this->last_result_slot();
      const slot_point    sl  = this->last_result_slot();
      const sched_result* res = this->last_sched_result(to_du_cell_index(0));
      for (const auto& pdcch : res->dl.ul_pdcchs) {
        if (pdcch.ctx.rnti == ue_rnti and pdcch.dci.type() == dci_ul_rnti_config_type::c_rnti_f0_1) {
          dcis[sl].push_back(pdcch.dci.as_c_rnti_f0_1());
        }
      }
      for (const auto& grant : res->ul.puschs) {
        if (grant.pusch_cfg.rnti == ue_rnti) {
          grants[sl].push_back(grant);
        }
      }
    }
  }

  const ul_sched_info* find_grant_with_harq(slot_point sl, unsigned harq_id) const
  {
    auto slot_it = grants.find(sl);
    if (slot_it == grants.end()) {
      return nullptr;
    }
    auto it = std::find_if(slot_it->second.begin(), slot_it->second.end(), [harq_id](const ul_sched_info& g) {
      return static_cast<unsigned>(g.pusch_cfg.harq_id) == harq_id;
    });
    return it != slot_it->second.end() ? &*it : nullptr;
  }

  void push_full_dl_buffer_state()
  {
    this->push_dl_buffer_state(dl_buffer_state_indication_message{
        ue_idx, ue_drb_lcid, std::numeric_limits<unsigned>::max() / 2, this->next_slot_rx()});
  }

  void push_full_bsr()
  {
    ul_bsr_indication_message bsr{
        to_du_cell_index(0),
        ue_idx,
        ue_rnti,
        bsr_format::LONG_BSR,
        ul_bsr_lcg_report_list{{uint_to_lcg_id(2), std::numeric_limits<uint32_t>::max() / 2}}};
    this->push_bsr(bsr);
  }

  const du_ue_index_t ue_idx      = to_du_ue_index(0);
  const rnti_t        ue_rnti     = to_rnti(0x4601);
  const lcid_t        ue_drb_lcid = LCID_MIN_DRB;

  pucch_resource_manager     pucch_cfg_builder{64};
  cell_config_builder_params params;
  unsigned                   rep_time_resource = 0;
  /// PDCCH-to-PUSCH delay (k2) of the repetition TDRA row, i.e. the offset from the PDCCH slot to occasion 0.
  uint8_t    rep_k2 = 0;
  slot_point last_collected_slot;

  std::map<slot_point, std::vector<dci_0_1_configuration>> dcis;
  std::map<slot_point, std::vector<ul_sched_info>>         grants;
};

class scheduler_pusch_repetition_test : public base_pusch_repetition_tester, public ::testing::Test
{
protected:
  // Threshold above the initial SINR (0 dB), so that repetitions are triggered from the first allocation.
  scheduler_pusch_repetition_test() : base_pusch_repetition_tester(10.0F, 0.0) {}
};

TEST_F(scheduler_pusch_repetition_test, when_sinr_below_threshold_then_pusch_repetition_bundles_are_scheduled)
{
  // Enqueue enough bytes for continuous UL tx.
  push_full_bsr();

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(10 * tdd_period);

  // At least one bundle must have been scheduled, and at least one carried through in full -- the cell's periodic
  // SR/CSI PUCCH is scheduled independently and may legitimately claim an occasion's slot.
  unsigned nof_bundles = 0;

  for (const auto& [pdcch_slot, slot_dcis] : dcis) {
    for (const dci_0_1_configuration& dci : slot_dcis) {
      // A UE qualifying for repetitions is scheduled as a bundle wherever one fits, and as a single transmission
      // elsewhere -- too few UL slots left before the next DL ones, for one. Only the bundles are of interest here.
      if (dci.time_resource != rep_time_resource) {
        continue;
      }

      // Occasion 0 (the base grant carrying the DCI) is scheduled k2 slots after the PDCCH.
      const slot_point base_slot     = pdcch_slot + rep_k2;
      const slot_point last_occ_slot = nth_occasion_slot(base_slot, nof_reps - 1);
      // Skip bundles whose repetition window extends beyond the collected slots.
      if (last_occ_slot > last_collected_slot) {
        continue;
      }
      ++nof_bundles;

      // Occasion 0 carries the TB and the total/remaining counters behind the FAPI TTI bundling IE.
      const ul_sched_info* occ0 = find_grant_with_harq(base_slot, dci.harq_process_number);
      ASSERT_NE(occ0, nullptr);
      ASSERT_TRUE(occ0->pusch_cfg.repetitions.has_value());
      ASSERT_EQ(occ0->pusch_cfg.repetitions->nof_repetitions, nof_reps);
      ASSERT_EQ(occ0->pusch_cfg.repetitions->nof_remaining_repetitions, nof_reps - 1);
      ASSERT_EQ(occ0->pusch_cfg.rv_index, dci.redundancy_version);

      // Occasions 1..K-1: next UL-only slots, same TBS/PRBs, cycled RV, decremented FAPI countdown. One overlapping
      // a PUCCH of this UE also carries its UCI (TS 38.213 Section 9.2.5.2); where the periodic SR/CSI opportunities
      // land decides whether that happens here, so only "at most one occasion carries UCI" is asserted.
      unsigned nof_uci_occasions = occ0->uci.has_value() ? 1 : 0;
      for (unsigned i = 1; i != nof_reps; ++i) {
        const slot_point     occ_slot = nth_occasion_slot(base_slot, i);
        const ul_sched_info* occ      = find_grant_with_harq(occ_slot, dci.harq_process_number);
        // A bundle is all or nothing: an incomplete one is cancelled before the DCI goes out.
        ASSERT_NE(occ, nullptr) << "hole at occasion " << i << " of a bundle whose base occasion was scheduled";
        ASSERT_TRUE(is_fully_ul(occ_slot));
        ASSERT_TRUE(occ->pusch_cfg.repetitions.has_value());
        ASSERT_EQ(occ->pusch_cfg.repetitions->nof_repetitions, nof_reps);
        ASSERT_EQ(occ->pusch_cfg.repetitions->nof_remaining_repetitions, nof_reps - 1 - i);
        ASSERT_EQ(occ->pusch_cfg.rv_index, expected_repetition_rv(dci.redundancy_version, i));
        ASSERT_EQ(occ->pusch_cfg.tb_size_bytes, occ0->pusch_cfg.tb_size_bytes);
        ASSERT_EQ(occ->pusch_cfg.rbs.type1(), occ0->pusch_cfg.rbs.type1());
        ASSERT_EQ(occ->pusch_cfg.symbols, occ0->pusch_cfg.symbols);
        nof_uci_occasions += occ->uci.has_value() ? 1 : 0;
      }
      ASSERT_LE(nof_uci_occasions, 1) << "UCI was multiplexed onto more than one occasion of the same bundle";
    }
  }
  ASSERT_GT(nof_bundles, 0) << "No PUSCH repetition bundle was scheduled";

  // The UE is not expected to transmit more than one PUSCH per slot.
  for (const auto& [sl, slot_grants] : grants) {
    ASSERT_LE(slot_grants.size(), 1) << fmt::format("More than one PUSCH for the UE at slot {}", sl);
  }
}

class scheduler_pusch_repetition_high_sinr_test : public base_pusch_repetition_tester, public ::testing::Test
{
protected:
  // SINR above the threshold: repetitions must not be used.
  scheduler_pusch_repetition_high_sinr_test() : base_pusch_repetition_tester(10.0F, 20.0) {}
};

TEST_F(scheduler_pusch_repetition_high_sinr_test, when_sinr_above_threshold_then_no_repetitions_are_scheduled)
{
  push_full_bsr();

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(6 * tdd_period);

  for (const auto& [sl, slot_dcis] : dcis) {
    for (const dci_0_1_configuration& dci : slot_dcis) {
      ASSERT_NE(dci.time_resource, rep_time_resource) << fmt::format("Unexpected repetition bundle at slot {}", sl);
    }
  }
}

class scheduler_pusch_repetition_forced_test : public base_pusch_repetition_tester, public ::testing::Test
{
protected:
  // SINR far above the threshold: without force_rep, repetitions would not be used.
  scheduler_pusch_repetition_forced_test() : base_pusch_repetition_tester(10.0F, 20.0, /*force_rep=*/true) {}
};

TEST_F(scheduler_pusch_repetition_forced_test, when_force_rep_enabled_then_repetitions_are_scheduled_despite_high_sinr)
{
  push_full_bsr();

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(10 * tdd_period);

  unsigned nof_bundles = 0;
  for (const auto& [pdcch_slot, slot_dcis] : dcis) {
    for (const dci_0_1_configuration& dci : slot_dcis) {
      // force_rep asks for a bundle at every allocation; where one does not fit, the grant is downgraded instead.
      if (dci.time_resource != rep_time_resource) {
        continue;
      }
      ++nof_bundles;
    }
  }
  ASSERT_GT(nof_bundles, 0) << "No PUSCH repetition bundle was scheduled despite force_rep being enabled";
}

class scheduler_pusch_repetition_disabled_test : public base_pusch_repetition_tester, public ::testing::Test
{
protected:
  // Unset threshold disables SINR-triggered repetitions regardless of the effective SINR.
  scheduler_pusch_repetition_disabled_test() : base_pusch_repetition_tester(std::nullopt, 0.0) {}
};

TEST_F(scheduler_pusch_repetition_disabled_test, when_threshold_unset_then_no_repetitions_are_scheduled)
{
  push_full_bsr();

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(6 * tdd_period);

  for (const auto& [sl, slot_dcis] : dcis) {
    for (const dci_0_1_configuration& dci : slot_dcis) {
      ASSERT_NE(dci.time_resource, rep_time_resource) << fmt::format("Unexpected repetition bundle at slot {}", sl);
    }
  }
}

/// \brief A TDD pattern whose UL runs are two slots long, so a bundle of four occasions never fits and every grant
/// has to take the single-transmission row the selector picked alongside the repetition one.
class scheduler_pusch_repetition_fallback_test : public base_pusch_repetition_tester, public ::testing::Test
{
protected:
  scheduler_pusch_repetition_fallback_test() :
    base_pusch_repetition_tester(10.0F,
                                 20.0,
                                 /*force_rep=*/true,
                                 /*auto_feedback=*/false,
                                 cell_config_builder_profiles::tdd_pattern_profile_fr1_30khz::DSUU)
  {
  }
};

TEST_F(scheduler_pusch_repetition_fallback_test, when_no_bundle_fits_then_a_single_transmission_is_scheduled)
{
  push_full_bsr();

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(10 * tdd_period);

  // The UE keeps being scheduled: a bundle that cannot be scheduled is downgraded, not deferred indefinitely.
  unsigned nof_grants = 0;
  for (const auto& [pdcch_slot, slot_dcis] : dcis) {
    for (const dci_0_1_configuration& dci : slot_dcis) {
      ASSERT_NE(dci.time_resource, rep_time_resource)
          << fmt::format("Repetition bundle scheduled at slot {} where no bundle fits", pdcch_slot);
      ++nof_grants;
    }
  }
  ASSERT_GT(nof_grants, 0) << "The UE was starved instead of falling back to a single transmission";

  // A downgraded grant is an ordinary PUSCH: no repetition parameters reach the PHY.
  for (const auto& [sl, slot_grants] : grants) {
    for (const ul_sched_info& grant : slot_grants) {
      ASSERT_FALSE(grant.pusch_cfg.repetitions.has_value())
          << fmt::format("Downgraded PUSCH at slot {} still carries repetition parameters", sl);
    }
  }
}

/// \brief Bundles scheduled alongside continuous DL traffic, so that the PDSCH HARQ-ACKs book PUCCHs in the UL slots
/// a bundle spans and its slots have a UL DAI to agree on. Auto UCI/CRC feedback keeps both HARQ pools cycling.
class scheduler_pusch_repetition_dai_test : public base_pusch_repetition_tester, public ::testing::Test
{
protected:
  /// A pattern with three consecutive UL slots keeps the occasions of a bundle close enough together that several of
  /// them fall within the k1 range over which the DL allocations book their HARQ-ACK PUCCHs. Which slots those are
  /// depends on where the periodic SR/CSI opportunities fall, so the simulation starts at a fixed slot.
  scheduler_pusch_repetition_dai_test() :
    base_pusch_repetition_tester(10.0F,
                                 20.0,
                                 /*force_rep=*/true,
                                 /*auto_feedback=*/true,
                                 cell_config_builder_profiles::tdd_pattern_profile_fr1_30khz::DDDDDDSUUU,
                                 slot_point{subcarrier_spacing::kHz30, 30})
  {
  }

  /// HARQ-ACK bits carried by each occasion of the bundle starting at \c base_slot, 0 for the occasions carrying none.
  std::vector<unsigned> harq_ack_bits_per_occasion(slot_point base_slot, unsigned harq_id) const
  {
    std::vector<unsigned> bits;
    for (unsigned i = 0; i != nof_reps; ++i) {
      const ul_sched_info* occ = find_grant_with_harq(nth_occasion_slot(base_slot, i), harq_id);
      bits.push_back(occ != nullptr and occ->uci.has_value() and occ->uci->harq.has_value()
                         ? occ->uci->harq->harq_ack_nof_bits
                         : 0);
    }
    return bits;
  }
};

/// The UE multiplexes in each slot of a bundle the HARQ-ACKs booked for that slot, and the single UL DAI of the
/// bundle's only DCI applies to all of them (TS 38.213, clause 9). So a bundle is only scheduled when its slots agree
/// on that DAI, and the DCI carries the per-slot count rather than the total the grant was sized for.
TEST_F(scheduler_pusch_repetition_dai_test, when_a_bundle_is_scheduled_then_one_ul_dai_describes_all_its_slots)
{
  push_full_bsr();
  push_full_dl_buffer_state();

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(100 * tdd_period);

  unsigned nof_bundles                          = 0;
  unsigned nof_bundles_with_ack                 = 0;
  unsigned nof_bundles_with_ack_on_severalslots = 0;

  for (const auto& [pdcch_slot, slot_dcis] : dcis) {
    for (const dci_0_1_configuration& dci : slot_dcis) {
      if (dci.time_resource != rep_time_resource) {
        continue;
      }
      const slot_point base_slot = pdcch_slot + rep_k2;
      // Skip bundles whose repetition window extends beyond the collected slots.
      if (nth_occasion_slot(base_slot, nof_reps - 1) > last_collected_slot) {
        continue;
      }
      ++nof_bundles;

      std::optional<unsigned> expected_bits;
      unsigned                nof_slots_with_ack = 0;
      for (const unsigned bits : harq_ack_bits_per_occasion(base_slot, dci.harq_process_number)) {
        if (bits == 0) {
          // A slot carrying no HARQ-ACK constrains nothing: the UE reports none there, whatever the DAI says.
          continue;
        }
        ++nof_slots_with_ack;
        if (not expected_bits.has_value()) {
          expected_bits = bits;
          continue;
        }
        // Equal modulo 4, which is all the DAI field conveys.
        ASSERT_EQ((bits - 1) % 4, (*expected_bits - 1) % 4)
            << fmt::format("Slots of the bundle at slot {} disagree on the UL DAI: {} vs {} HARQ-ACK bits",
                           base_slot,
                           bits,
                           *expected_bits);
      }
      if (not expected_bits.has_value()) {
        continue;
      }
      ++nof_bundles_with_ack;
      nof_bundles_with_ack_on_severalslots += nof_slots_with_ack > 1 ? 1 : 0;

      // The DAI describes one slot's codebook, not the sum over the slots the grant was sized for.
      ASSERT_EQ(dci.first_dl_assignment_index, (*expected_bits - 1) % 4) << fmt::format(
          "UL DAI of the bundle at slot {} does not describe its {} HARQ-ACK bits per slot", base_slot, *expected_bits);
    }
  }
  ASSERT_GT(nof_bundles, 0) << "No PUSCH repetition bundle was scheduled";
  ASSERT_GT(nof_bundles_with_ack, 0) << "No bundle carried HARQ-ACK, so the DAI rule is not exercised";
  ASSERT_GT(nof_bundles_with_ack_on_severalslots, 0)
      << "No bundle carried HARQ-ACK in more than one slot, so nothing had to agree on a DAI";
}
