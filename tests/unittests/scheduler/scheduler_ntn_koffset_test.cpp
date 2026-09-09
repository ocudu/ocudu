// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit tests for the timing of the UE-dedicated UL transmissions in an NTN cell, which the cell-specific
/// Koffset delays with respect to the DCI that schedules them (TS 38.213, Section 4.2).

#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_simulator.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include <functional>
#include <gtest/gtest.h>

using namespace ocudu;

/// Cell-specific Koffset of the NTN cell, in milliseconds, i.e. in slots of 15 kHz.
class scheduler_ntn_koffset_test : public scheduler_test_simulator, public ::testing::TestWithParam<unsigned>
{
protected:
  scheduler_ntn_koffset_test() :
    scheduler_test_simulator(scheduler_test_sim_config{.auto_uci       = true,
                                                       .auto_crc       = true,
                                                       .ntn_cs_koffset = std::chrono::milliseconds{GetParam()}})
  {
    auto cell_req = sched_config_helper::make_default_sched_cell_configuration_request(
        cell_config_builder_profiles::create(duplex_mode::FDD));
    cell_req.ran.ntn_params.emplace();
    cell_req.ran.ntn_params->ntn_cfg.cell_specific_koffset = std::chrono::milliseconds{GetParam()};
    this->add_cell(cell_req);

    auto ue_cfg     = sched_config_helper::create_default_sched_ue_creation_request(cell_req.ran, {ue_drb_lcid});
    ue_cfg.ue_index = ue_idx;
    ue_cfg.crnti    = ue_rnti;
    this->add_ue(ue_cfg);
  }

  unsigned koffset() const { return cell_cfg().ntn_cs_koffset; }

  /// Runs the scheduler until the results of \c target_slot are available, calling \c check_slot on every slot, and
  /// stops at the first slot that fails the check.
  void run_until_slot(slot_point target_slot, const std::function<void(slot_point)>& check_slot)
  {
    while (this->last_result_slot() < target_slot and not ::testing::Test::HasFatalFailure()) {
      this->run_slot();
      check_slot(this->last_result_slot());
    }
  }

  const du_ue_index_t ue_idx      = to_du_ue_index(0);
  const rnti_t        ue_rnti     = to_rnti(0x4601);
  const lcid_t        ue_drb_lcid = LCID_MIN_DRB;
};

TEST_P(scheduler_ntn_koffset_test, pusch_is_scheduled_k2_plus_koffset_slots_after_its_dci)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-TIM-1");

  ASSERT_EQ(koffset(), GetParam());
  this->push_bsr(ul_bsr_indication_message{to_du_cell_index(0),
                                           ue_idx,
                                           ue_rnti,
                                           bsr_format::SHORT_BSR,
                                           ul_bsr_lcg_report_list{ul_bsr_lcg_report{uint_to_lcg_id(0), 100000}}});

  // The first UL DCI of the UE schedules its first PUSCH.
  ASSERT_TRUE(this->run_slot_until([this]() { return this->find_ue_ul_pdcch(ue_rnti) != nullptr; }));
  const slot_point            pdcch_slot = this->last_result_slot();
  const pdcch_ul_information& pdcch      = *this->find_ue_ul_pdcch(ue_rnti);
  const bool                  is_f0_1    = pdcch.dci.type() == dci_ul_rnti_config_type::c_rnti_f0_1;
  const unsigned              time_resource =
      is_f0_1 ? pdcch.dci.as_c_rnti_f0_1().time_resource : pdcch.dci.as_c_rnti_f0_0().time_resource;

  // TS 38.214, Section 6.1.2.1: the PUSCH slot is the DCI slot plus k2 plus Koffset. The DCI carries k2 alone.
  const auto pusch_td_list =
      cell_cfg().init_bwp.ul.td_mapper().pusch_td_resources(is_f0_1 ? dci_ul_format::f0_1 : dci_ul_format::f0_0);
  ASSERT_LT(time_resource, pusch_td_list.size());
  const slot_point expected_pusch_slot = pdcch_slot + pusch_td_list[time_resource].k2 + koffset();

  run_until_slot(expected_pusch_slot, [this, expected_pusch_slot](slot_point sl) {
    const bool has_pusch = find_ue_pusch(ue_rnti, *this->last_sched_result()) != nullptr;
    if (sl < expected_pusch_slot) {
      ASSERT_FALSE(has_pusch) << fmt::format("PUSCH at slot {}, before the Koffset delayed slot", sl);
    } else {
      ASSERT_TRUE(has_pusch) << fmt::format("No PUSCH at slot {}, k2 plus Koffset after its DCI", sl);
    }
  });
}

TEST_P(scheduler_ntn_koffset_test, harq_ack_is_scheduled_k1_plus_koffset_slots_after_its_pdsch)
{
  OCUDU_TEST_REQUIREMENTS("DU-NTN-TIM-1");

  ASSERT_EQ(koffset(), GetParam());
  this->push_dl_buffer_state(dl_buffer_state_indication_message{ue_idx, ue_drb_lcid, 100000});

  // The first DL DCI of the UE schedules its first PDSCH.
  ASSERT_TRUE(this->run_slot_until([this]() { return this->find_ue_dl_pdcch(ue_rnti) != nullptr; }));
  const slot_point            pdcch_slot = this->last_result_slot();
  const pdcch_dl_information& pdcch      = *this->find_ue_dl_pdcch(ue_rnti);
  ASSERT_EQ(pdcch.dci.type(), dci_dl_rnti_config_type::c_rnti_f1_1);
  const dci_1_1_configuration& dci = pdcch.dci.as_c_rnti_f1_1();
  ASSERT_TRUE(dci.pdsch_harq_fb_timing_indicator.has_value());

  // TS 38.213, Section 9.2.3: the HARQ-ACK is reported k1 plus Koffset slots after the PDSCH. The DCI carries k1 alone.
  const auto pdsch_td_list = cell_cfg().init_bwp.dl.td_mapper().pdsch_td_resources(dci_dl_format::f1_1);
  ASSERT_LT(dci.time_resource, pdsch_td_list.size());
  const auto k1_list = cell_cfg().init_bwp.ul.td_mapper().dedicated_k1_candidates();
  ASSERT_LT(dci.pdsch_harq_fb_timing_indicator.value(), k1_list.size());
  const slot_point expected_ack_slot = pdcch_slot + pdsch_td_list[dci.time_resource].k0 +
                                       k1_list[dci.pdsch_harq_fb_timing_indicator.value()] + koffset();

  // No UL data is pending, so the HARQ-ACK is reported on PUCCH.
  const auto has_harq_ack = [this]() {
    for (const pucch_info& pucch : this->last_sched_result()->ul.pucchs) {
      if (pucch.crnti == ue_rnti and pucch.uci_bits.harq_ack_nof_bits > 0) {
        return true;
      }
    }
    return false;
  };
  run_until_slot(expected_ack_slot, [expected_ack_slot, &has_harq_ack](slot_point sl) {
    if (sl < expected_ack_slot) {
      ASSERT_FALSE(has_harq_ack()) << fmt::format("HARQ-ACK at slot {}, before the Koffset delayed slot", sl);
    } else {
      ASSERT_TRUE(has_harq_ack()) << fmt::format("No HARQ-ACK at slot {}, k1 plus Koffset after its PDSCH", sl);
    }
  });
}

// Koffset of a LEO and a GEO cell.
INSTANTIATE_TEST_SUITE_P(scheduler_ntn_koffset_test, scheduler_ntn_koffset_test, ::testing::Values(16U, 240U));
