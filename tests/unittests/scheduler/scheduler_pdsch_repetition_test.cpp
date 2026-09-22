// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit tests for CQI-triggered Rel-16 PDSCH repetitions.

#include "test_utils/scheduler_test_simulator.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include <gtest/gtest.h>
#include <map>
#include <set>

using namespace ocudu;

/// RV of a PDSCH repetition occasion, as per TS 38.214 Table 5.1.2.1-2.
static uint8_t expected_repetition_rv(uint8_t dci_rv, unsigned occasion_idx)
{
  static constexpr std::array<uint8_t, 4> rv_cycle = {0, 2, 3, 1};
  const auto*                             it       = std::find(rv_cycle.begin(), rv_cycle.end(), dci_rv);
  return rv_cycle[(std::distance(rv_cycle.begin(), it) + occasion_idx) % rv_cycle.size()];
}

class base_pdsch_repetition_tester : public scheduler_test_simulator
{
protected:
  static constexpr uint8_t nof_reps = 4;

  base_pdsch_repetition_tester(float cqi_rep_threshold, unsigned initial_cqi) :
    scheduler_test_simulator(scheduler_test_sim_config{.sched_cfg =
                                                           [cqi_rep_threshold, initial_cqi]() {
                                                             auto expert_cfg =
                                                                 config_helpers::make_default_scheduler_expert_config();
                                                             expert_cfg.ue.pdsch_cqi_rep_threshold = cqi_rep_threshold;
                                                             expert_cfg.ue.initial_cqi             = initial_cqi;
                                                             return expert_cfg;
                                                           }(),
                                                       .max_scs  = subcarrier_spacing::kHz30,
                                                       .auto_uci = true,
                                                       .auto_crc = true})
  {
    params                      = cell_config_builder_profiles::create(duplex_mode::TDD);
    params.tdd_ul_dl_cfg_common = cell_config_builder_profiles::create_tdd_pattern(
        cell_config_builder_profiles::tdd_pattern_profile_fr1_30khz::DDDDDDDSUU);
    // Disable CSI reporting, so that the wideband CQI stays at the configured initial CQI for the whole test (the
    // simulator auto-UCI would otherwise inject CSI reports with CQI=15).
    params.csi_rs_enabled = false;

    // Add Cell.
    auto cell_req = sched_config_helper::make_default_sched_cell_configuration_request(params);
    std::get<pucch_f2_params>(cell_req.ran.init_bwp.pucch.resources.f2_or_f3_or_f4_params).max_code_rate =
        max_pucch_code_rate::dot_35;
    this->add_cell(cell_req);

    // Add UE with a Rel-16 PDSCH TDRA list that mirrors the common list and appends a repetition entry.
    auto ue_cfg           = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran, {ue_drb_lcid});
    ue_cfg.ue_index       = ue_idx;
    ue_cfg.crnti          = ue_rnti;
    auto&       pdsch_cfg = (*ue_cfg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg.value();
    const auto& common_list = cell_req.ran.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (const auto& alloc : common_list) {
      pdsch_cfg.pdsch_td_alloc_list.push_back(alloc);
    }
    rep_time_resource                               = pdsch_cfg.pdsch_td_alloc_list.size();
    pdsch_time_domain_resource_allocation rep_alloc = common_list.front();
    rep_alloc.rep_number                            = nof_reps;
    pdsch_cfg.pdsch_td_alloc_list.push_back(rep_alloc);
    this->add_ue(ue_cfg);
  }

  bool is_fully_dl(slot_point sl) const { return cell_cfg(to_du_cell_index(0)).is_fully_dl_enabled(sl); }

  bool is_fully_ul(slot_point sl) const { return cell_cfg(to_du_cell_index(0)).is_fully_ul_enabled(sl); }

  /// Runs the scheduler for \c nof_slots slots, collecting per-slot copies of the results for the test UE.
  void run_and_collect(unsigned nof_slots)
  {
    for (unsigned i = 0; i != nof_slots; ++i) {
      this->run_slot();
      last_collected_slot     = this->last_result_slot();
      const slot_point    sl  = this->last_result_slot();
      const sched_result* res = this->last_sched_result(to_du_cell_index(0));
      for (const auto& pdcch : res->dl.dl_pdcchs) {
        if (pdcch.ctx.rnti == ue_rnti and pdcch.dci.type() == dci_dl_rnti_config_type::c_rnti_f1_1) {
          dcis[sl].push_back(pdcch.dci.as_c_rnti_f1_1());
        }
      }
      for (const auto& grant : res->dl.ue_grants) {
        if (grant.pdsch_cfg.rnti == ue_rnti) {
          grants[sl].push_back(grant);
        }
      }
      for (const auto& pucch : res->ul.pucchs) {
        if (pucch.crnti == ue_rnti) {
          pucch_slots.insert(sl);
        }
      }
    }
  }

  const dl_msg_alloc* find_grant_with_harq(slot_point sl, unsigned harq_id) const
  {
    auto slot_it = grants.find(sl);
    if (slot_it == grants.end()) {
      return nullptr;
    }
    auto it = std::find_if(slot_it->second.begin(), slot_it->second.end(), [harq_id](const dl_msg_alloc& g) {
      return static_cast<unsigned>(g.pdsch_cfg.harq_id) == harq_id;
    });
    return it != slot_it->second.end() ? &*it : nullptr;
  }

  const du_ue_index_t ue_idx      = to_du_ue_index(0);
  const rnti_t        ue_rnti     = to_rnti(0x4601);
  const lcid_t        ue_drb_lcid = LCID_MIN_DRB;

  cell_config_builder_params params;
  unsigned                   rep_time_resource = 0;
  slot_point                 last_collected_slot;

  std::map<slot_point, std::vector<dci_1_1_configuration>> dcis;
  std::map<slot_point, std::vector<dl_msg_alloc>>          grants;
  std::set<slot_point>                                     pucch_slots;
};

class scheduler_pdsch_repetition_test : public base_pdsch_repetition_tester, public ::testing::Test
{
protected:
  // Threshold above the initial CQI (3), so that repetitions are triggered from the first allocation.
  scheduler_pdsch_repetition_test() : base_pdsch_repetition_tester(6.0F, 3) {}
};

TEST_F(scheduler_pdsch_repetition_test, when_cqi_below_threshold_then_pdsch_repetition_bundles_are_scheduled)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");

  // Enqueue enough bytes for continuous DL tx.
  dl_buffer_state_indication_message dl_buf_st{ue_idx, ue_drb_lcid, 10000000};
  this->push_dl_buffer_state(dl_buf_st);

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(6 * tdd_period);

  // At least one repetition bundle must have been scheduled.
  unsigned nof_bundles = 0;

  for (const auto& [pdcch_slot, slot_dcis] : dcis) {
    for (const dci_1_1_configuration& dci : slot_dcis) {
      // With the effective CQI below the threshold, the scheduler either allocates a repetition bundle or defers the
      // allocation; it never falls back to a single transmission.
      ASSERT_EQ(dci.time_resource, rep_time_resource)
          << fmt::format("Single-TX PDSCH scheduled at slot {} for a UE qualifying for repetitions", pdcch_slot);
      // Skip bundles whose repetition window extends beyond the collected slots.
      if (pdcch_slot + (nof_reps - 1) > last_collected_slot) {
        continue;
      }
      ++nof_bundles;

      // A bundle is only scheduled when the next slot is also usable (at least 2 slots to the special slot).
      ASSERT_TRUE(is_fully_dl(pdcch_slot)) << fmt::format("Bundle scheduled in non-DL slot {}", pdcch_slot);
      ASSERT_TRUE(is_fully_dl(pdcch_slot + 1))
          << fmt::format("Bundle scheduled at slot {} with less than 2 slots to the special slot", pdcch_slot);

      // Occasion 0 carries the TB (new for a newTx bundle, from the HARQ buffer for a reTx bundle).
      const dl_msg_alloc* occ0 = find_grant_with_harq(pdcch_slot, dci.harq_process_number);
      ASSERT_NE(occ0, nullptr);
      ASSERT_EQ(occ0->pdsch_cfg.codewords[0].new_data, occ0->context.nof_retxs == 0);
      ASSERT_EQ(occ0->pdsch_cfg.codewords[0].rv_index, dci.tb1_redundancy_version);

      // Occasions 1..K-1: transmitted in fully-DL slots with the same TBS/PRBs and the cycled RV; dropped otherwise.
      unsigned last_tx_occasion = 0;
      for (unsigned i = 1; i != nof_reps; ++i) {
        const slot_point    occ_slot = pdcch_slot + i;
        const dl_msg_alloc* occ      = find_grant_with_harq(occ_slot, dci.harq_process_number);
        if (is_fully_dl(occ_slot)) {
          last_tx_occasion = i;
          ASSERT_NE(occ, nullptr) << fmt::format("Missing repetition occasion at slot {}", occ_slot);
          ASSERT_FALSE(occ->pdsch_cfg.codewords[0].new_data);
          ASSERT_EQ(occ->pdsch_cfg.codewords[0].rv_index, expected_repetition_rv(dci.tb1_redundancy_version, i));
          ASSERT_EQ(occ->pdsch_cfg.codewords[0].tb_size_bytes, occ0->pdsch_cfg.codewords[0].tb_size_bytes);
          ASSERT_EQ(occ->pdsch_cfg.rbs.type1(), occ0->pdsch_cfg.rbs.type1());
          ASSERT_EQ(occ->pdsch_cfg.symbols, occ0->pdsch_cfg.symbols);
        } else {
          ASSERT_EQ(occ, nullptr) << fmt::format("Unexpected repetition occasion in non-DL slot {}", occ_slot);
        }
      }

      // The HARQ-ACK PUCCH takes place k1 slots after the last transmitted occasion. A dropped occasion is not
      // received at all (TS 38.213, 11.1), so it cannot be the DL slot the PDSCH reception ends in (TS 38.213, 9.2.3).
      ASSERT_TRUE(dci.pdsch_harq_fb_timing_indicator.has_value());
      const auto dedicated_k1_list = cell_cfg(to_du_cell_index(0)).init_bwp.ul.td_mapper().dedicated_k1_candidates();
      const unsigned   k1          = dedicated_k1_list[dci.pdsch_harq_fb_timing_indicator.value()];
      const slot_point expected_pucch_slot = pdcch_slot + last_tx_occasion + k1;
      if (expected_pucch_slot <= last_collected_slot) {
        ASSERT_EQ(pucch_slots.count(expected_pucch_slot), 1)
            << fmt::format("No PUCCH at slot {} for the bundle scheduled at slot {}", expected_pucch_slot, pdcch_slot);
      }
    }
  }
  ASSERT_GT(nof_bundles, 0) << "No PDSCH repetition bundle was scheduled";

  // The UE is not expected to receive more than one unicast PDSCH per slot.
  for (const auto& [sl, slot_grants] : grants) {
    ASSERT_LE(slot_grants.size(), 1) << fmt::format("More than one PDSCH for the UE at slot {}", sl);
  }

  // Every PDCCH-less PDSCH must be an occasion of a repetition bundle started in a preceding slot.
  for (const auto& [sl, slot_grants] : grants) {
    for (const dl_msg_alloc& grant : slot_grants) {
      if (grant.pdsch_cfg.codewords[0].new_data) {
        continue;
      }
      bool found = false;
      for (unsigned i = 1; i != nof_reps and not found; ++i) {
        auto it = dcis.find(sl - i);
        if (it == dcis.end()) {
          continue;
        }
        found = std::any_of(it->second.begin(), it->second.end(), [&](const dci_1_1_configuration& dci) {
          return dci.time_resource == rep_time_resource and
                 dci.harq_process_number == static_cast<unsigned>(grant.pdsch_cfg.harq_id);
        });
      }
      ASSERT_TRUE(found) << fmt::format("PDCCH-less PDSCH at slot {} does not belong to any repetition bundle", sl);
    }
  }
}

TEST_F(scheduler_pdsch_repetition_test, when_harq_is_nacked_then_retx_is_scheduled_as_repetition_bundle)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");

  // Report NACK for all HARQ-ACK bits, forcing HARQ reTxs.
  this->register_uci_handler([](uci_indication& uci) {
    for (auto& pdu : uci.ucis) {
      if (auto* f1 = std::get_if<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(&pdu.pdu)) {
        std::fill(f1->harqs.begin(), f1->harqs.end(), mac_harq_ack_report_status::nack);
      } else if (auto* f2 = std::get_if<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(&pdu.pdu)) {
        std::fill(f2->harqs.begin(), f2->harqs.end(), mac_harq_ack_report_status::nack);
      }
    }
  });

  dl_buffer_state_indication_message dl_buf_st{ue_idx, ue_drb_lcid, 10000000};
  this->push_dl_buffer_state(dl_buf_st);

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(8 * tdd_period);

  // Every allocation for the UE (newTx and reTx alike) must be a repetition bundle; reTx bundles follow the RV cycle
  // starting at the RV signalled in the DCI.
  unsigned nof_retx_bundles = 0;
  for (const auto& [pdcch_slot, slot_dcis] : dcis) {
    for (const dci_1_1_configuration& dci : slot_dcis) {
      ASSERT_EQ(dci.time_resource, rep_time_resource)
          << fmt::format("Single-TX PDSCH scheduled at slot {} for a UE qualifying for repetitions", pdcch_slot);
      if (pdcch_slot + (nof_reps - 1) > last_collected_slot) {
        continue;
      }
      const dl_msg_alloc* occ0 = find_grant_with_harq(pdcch_slot, dci.harq_process_number);
      ASSERT_NE(occ0, nullptr);
      ASSERT_EQ(occ0->pdsch_cfg.codewords[0].new_data, occ0->context.nof_retxs == 0);
      ASSERT_EQ(occ0->pdsch_cfg.codewords[0].rv_index, dci.tb1_redundancy_version);
      if (occ0->context.nof_retxs > 0) {
        ++nof_retx_bundles;
      }
      for (unsigned i = 1; i != nof_reps; ++i) {
        const slot_point    occ_slot = pdcch_slot + i;
        const dl_msg_alloc* occ      = find_grant_with_harq(occ_slot, dci.harq_process_number);
        if (is_fully_dl(occ_slot)) {
          ASSERT_NE(occ, nullptr) << fmt::format("Missing repetition occasion at slot {}", occ_slot);
          ASSERT_FALSE(occ->pdsch_cfg.codewords[0].new_data);
          ASSERT_EQ(occ->pdsch_cfg.codewords[0].rv_index, expected_repetition_rv(dci.tb1_redundancy_version, i));
          ASSERT_EQ(occ->pdsch_cfg.codewords[0].tb_size_bytes, occ0->pdsch_cfg.codewords[0].tb_size_bytes);
        } else {
          ASSERT_EQ(occ, nullptr) << fmt::format("Unexpected repetition occasion in non-DL slot {}", occ_slot);
        }
      }
    }
  }
  ASSERT_GT(nof_retx_bundles, 0) << "No reTx repetition bundle was scheduled";
}

// Bundles whose nominal last occasion falls in a fully-UL slot. Such an occasion is not received at all (TS 38.213,
// 11.1), so the HARQ-ACK is counted from the last transmitted occasion (TS 38.213, 9.2.3). Counting it from the
// nominal one instead asked the UCI allocator for the k1 candidates of a UL slot, whose per-slot k1 list is empty:
// that both read past the end of the empty candidate range and left the bundle unallocated.
TEST_F(scheduler_pdsch_repetition_test, when_trailing_occasions_fall_in_ul_slots_then_harq_ack_follows_last_tx_occasion)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);

  // Bursty traffic whose phase advances by one slot per iteration, so that bundles start at every position of the TDD
  // pattern over the run, including the last DL slot whose trailing occasions fall in the special and UL slots.
  for (unsigned i = 0; i != 2 * tdd_period; ++i) {
    dl_buffer_state_indication_message dl_buf_st{ue_idx, ue_drb_lcid, 1000};
    this->push_dl_buffer_state(dl_buf_st);
    run_and_collect(tdd_period + 1);
  }

  unsigned nof_ul_tail_bundles = 0;
  for (const auto& [pdcch_slot, slot_dcis] : dcis) {
    for (const dci_1_1_configuration& dci : slot_dcis) {
      if (dci.time_resource != rep_time_resource or pdcch_slot + (nof_reps - 1) > last_collected_slot) {
        continue;
      }
      // Only bundles whose nominal last occasion falls in a fully-UL slot are of interest here.
      if (not is_fully_ul(pdcch_slot + (nof_reps - 1))) {
        continue;
      }
      ++nof_ul_tail_bundles;

      unsigned last_tx_occasion = 0;
      for (unsigned i = 1; i != nof_reps; ++i) {
        if (is_fully_dl(pdcch_slot + i)) {
          last_tx_occasion = i;
        }
      }
      ASSERT_TRUE(dci.pdsch_harq_fb_timing_indicator.has_value());
      const auto dedicated_k1_list = cell_cfg(to_du_cell_index(0)).init_bwp.ul.td_mapper().dedicated_k1_candidates();
      const unsigned   k1          = dedicated_k1_list[dci.pdsch_harq_fb_timing_indicator.value()];
      const slot_point expected_pucch_slot = pdcch_slot + last_tx_occasion + k1;
      if (expected_pucch_slot <= last_collected_slot) {
        ASSERT_EQ(pucch_slots.count(expected_pucch_slot), 1)
            << fmt::format("No PUCCH at slot {} for the bundle scheduled at slot {}", expected_pucch_slot, pdcch_slot);
      }
    }
  }
  ASSERT_GT(nof_ul_tail_bundles, 0) << "No bundle whose nominal last occasion falls in a UL slot was scheduled";
}

class scheduler_pdsch_repetition_high_cqi_test : public base_pdsch_repetition_tester, public ::testing::Test
{
protected:
  // CQI above the threshold: repetitions must not be used.
  scheduler_pdsch_repetition_high_cqi_test() : base_pdsch_repetition_tester(6.0F, 12) {}
};

TEST_F(scheduler_pdsch_repetition_high_cqi_test, when_cqi_above_threshold_then_no_repetitions_are_scheduled)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");

  dl_buffer_state_indication_message dl_buf_st{ue_idx, ue_drb_lcid, 10000000};
  this->push_dl_buffer_state(dl_buf_st);

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(6 * tdd_period);

  for (const auto& [sl, slot_dcis] : dcis) {
    for (const dci_1_1_configuration& dci : slot_dcis) {
      ASSERT_NE(dci.time_resource, rep_time_resource) << fmt::format("Unexpected repetition bundle at slot {}", sl);
    }
  }
  for (const auto& [sl, slot_grants] : grants) {
    for (const dl_msg_alloc& grant : slot_grants) {
      ASSERT_TRUE(grant.pdsch_cfg.codewords[0].new_data)
          << fmt::format("Unexpected PDCCH-less PDSCH occasion at slot {}", sl);
    }
  }
}

class scheduler_pdsch_repetition_disabled_test : public base_pdsch_repetition_tester, public ::testing::Test
{
protected:
  // Threshold 0 disables CQI-triggered repetitions regardless of the reported CQI.
  scheduler_pdsch_repetition_disabled_test() : base_pdsch_repetition_tester(0.0F, 3) {}
};

TEST_F(scheduler_pdsch_repetition_disabled_test, when_threshold_is_zero_then_no_repetitions_are_scheduled)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");

  dl_buffer_state_indication_message dl_buf_st{ue_idx, ue_drb_lcid, 10000000};
  this->push_dl_buffer_state(dl_buf_st);

  const unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).params.tdd_cfg);
  run_and_collect(6 * tdd_period);

  for (const auto& [sl, slot_dcis] : dcis) {
    for (const dci_1_1_configuration& dci : slot_dcis) {
      ASSERT_NE(dci.time_resource, rep_time_resource) << fmt::format("Unexpected repetition bundle at slot {}", sl);
    }
  }
  for (const auto& [sl, slot_grants] : grants) {
    for (const dl_msg_alloc& grant : slot_grants) {
      ASSERT_TRUE(grant.pdsch_cfg.codewords[0].new_data)
          << fmt::format("Unexpected PDCCH-less PDSCH occasion at slot {}", sl);
    }
  }
}
