// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief In this file, we test the correct behaviour of the scheduler when handling UE contention resolution, and
/// TC-RNTI to C-RNTI transitions.

#include "test_utils/indication_generators.h"
#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/duplex_mode.h"
#include <functional>
#include <gtest/gtest.h>

using namespace ocudu;

ocudulog::basic_levels sched_log_level = ocudulog::basic_levels::warning;

static bool is_f1_pucch_with_harq_bits(const pucch_info& pucch, bool is_common, bool has_sr)
{
  return pucch.format() == pucch_format::FORMAT_1 and ((pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) == has_sr) and
         pucch.uci_bits.harq_ack_nof_bits > 0 and (not pucch.resources.second_hop_prb.has_value() != is_common);
}

class base_scheduler_conres_test : public scheduler_test_simulator
{
public:
  base_scheduler_conres_test(duplex_mode duplx_mode) :
    scheduler_test_simulator(4, duplx_mode == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30)
  {
    ocudulog::fetch_basic_logger("SCHED", true).set_level(sched_log_level);
    builder_params = cell_config_builder_profiles::create(duplx_mode);
  }

  const pucch_info* run_until_common_pucch_with_harq(rnti_t rnti_to_find)
  {
    const pucch_info* first_conres_msg4_pucch = nullptr;
    run_slot_until([this, &first_conres_msg4_pucch, rnti_to_find]() {
      return std::any_of(this->last_sched_result()->ul.pucchs.begin(),
                         this->last_sched_result()->ul.pucchs.end(),
                         [&first_conres_msg4_pucch, rnti_to_find](const pucch_info& pucch) mutable {
                           first_conres_msg4_pucch = &pucch;
                           return pucch.crnti == rnti_to_find and is_f1_pucch_with_harq_bits(pucch, true, false);
                         });
    });
    return first_conres_msg4_pucch;
  }

  bool run_until_conres_msg4_scheduled(rnti_t rnti_to_find, lcid_dl_sch_t grant_lcid)
  {
    bool grant_allocated = false;
    this->run_slot_until([this, &grant_allocated, rnti_to_find, grant_lcid]() {
      const pdcch_dl_information* dl_pdcch = find_ue_dl_pdcch(rnti_to_find);
      if (dl_pdcch != nullptr) {
        const dl_msg_alloc* pdsch_alloc = find_ue_pdsch(rnti_to_find, *this->last_sched_result());
        // These conditions must be verified to consider the ConRes correctly scheduled.
        const bool conres_tc_rnti_check =
            grant_lcid != lcid_dl_sch_t::UE_CON_RES_ID or dl_pdcch->dci.type == dci_dl_rnti_config_type::tc_rnti_f1_0;
        if (not conres_tc_rnti_check or pdsch_alloc == nullptr or not pdsch_alloc->pdsch_cfg.codewords[0].new_data or
            pdsch_alloc->tb_list.size() != 1) {
          return false;
        }
        for (const auto& lg_ch : pdsch_alloc->tb_list[0].lc_chs_to_sched) {
          if (lg_ch.lcid == grant_lcid) {
            grant_allocated = true;
            return true;
          }
        }
      }

      return false;
    });
    return grant_allocated;
  }

  bool run_until_pucch_scheduled_and_received(rnti_t rnti_to_find, du_ue_index_t ue_index, bool ack)
  {
    // Check whether PUCCH (i) has been allocated and (ii) on common resources.
    const pucch_info* first_conres_pucch = run_until_common_pucch_with_harq(rnti_to_find);
    if (first_conres_pucch == nullptr) {
      return false;
    }

    // If only the ConRes was scheduled, we need to wait for the PUCCH to be sent and inject an ACK, to confirm the
    // ConRes was received by the UE. Without this, the scheduler won't allocate any other PDSCH grant.
    uci_indication uci =
        test_helper::create_uci_indication(next_slot.without_hyper_sfn() - 1, ue_index, *first_conres_pucch);
    std::get<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(uci.ucis[0].pdu).harqs[0] =
        ack ? mac_harq_ack_report_status::ack : mac_harq_ack_report_status::nack;
    this->sched->handle_uci_indication(uci);

    return true;
  }

  const dl_msg_lc_info* get_msg4_msg_lc_info(rnti_t rnti_to_find, lcid_t msg4_lcid) const
  {
    const dl_msg_alloc* pdsch_alloc = find_ue_pdsch(rnti_to_find, *this->last_sched_result());
    if (pdsch_alloc == nullptr or not pdsch_alloc->pdsch_cfg.codewords[0].new_data or
        pdsch_alloc->tb_list.size() != 1) {
      return nullptr;
    }

    for (const auto& lg_ch : pdsch_alloc->tb_list[0].lc_chs_to_sched) {
      if (lg_ch.lcid == msg4_lcid) {
        if (lg_ch.lcid == LCID_SRB0 or lg_ch.lcid == LCID_SRB1) {
          return &lg_ch;
        }
      }
    }
    return nullptr;
  }

  cell_config_builder_params builder_params{};
};

class base_single_ue_scheduler_conres_test : public base_scheduler_conres_test
{
public:
  base_single_ue_scheduler_conres_test(duplex_mode duplx_mode = duplex_mode::FDD) :
    base_scheduler_conres_test(duplx_mode)
  {
    // Create cell config with space for two PDCCHs in the SearchSpace#1.
    add_cell(sched_config_helper::make_default_sched_cell_configuration_request(builder_params));

    ocudu_assert(not this->cell_cfg(to_du_cell_index(0)).nzp_csi_rs_list.empty(),
                 "This test assumes a setup with NZP CSI-RS enabled");
    ocudu_assert(not this->cell_cfg(to_du_cell_index(0)).zp_csi_rs_list.empty(),
                 "This test assumes a setup with ZP CSI-RS enabled");

    // Create a UE with a DRB active.
    auto ue_cfg =
        sched_config_helper::create_default_sched_ue_creation_request(cell_cfg(to_du_cell_index(0)).params, {});
    ue_cfg.ue_index           = ue_index;
    ue_cfg.crnti              = rnti;
    ue_cfg.starts_in_fallback = true;
    ue_cfg.ul_ccch_slot_rx    = this->next_slot.without_hyper_sfn();
    scheduler_test_simulator::add_ue(ue_cfg, false);
  }

  du_ue_index_t ue_index = to_du_ue_index(0);
  rnti_t        rnti     = to_rnti(0x4601);
};

/// \brief Test to verify that when a PDSCH with ConRes CE is scheduled even when there is no LCID0/LCID1 PDU passed
/// down to the scheduler.
class scheduler_conres_without_pdu_test : public base_single_ue_scheduler_conres_test, public ::testing::Test
{};

TEST_F(scheduler_conres_without_pdu_test, when_conres_ce_is_enqueued_and_no_msg4_is_enqueued_then_pdsch_is_scheduled)
{
  // Ensure the ConRes CE is scheduled without a Msg4 SDU.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_result()) != nullptr; }));
  const dl_msg_alloc* conres_alloc = find_ue_pdsch(rnti, *this->last_sched_result());
  ASSERT_EQ(conres_alloc->tb_list.size(), 1);
  ASSERT_EQ(conres_alloc->tb_list[0].lc_chs_to_sched.size(), 1);
  ASSERT_EQ(conres_alloc->tb_list[0].lc_chs_to_sched[0].lcid, lcid_dl_sch_t::UE_CON_RES_ID);
  ASSERT_EQ(find_ue_dl_pdcch(rnti)->dci.type, dci_dl_rnti_config_type::tc_rnti_f1_0);
  ASSERT_TRUE(this->last_sched_result()->dl.csi_rs.empty());
}

// ------------------------------------------------------------------------------------------------------------------ //

struct conres_test_params {
  lcid_t      msg4_lcid;
  duplex_mode duplx_mode;
};

/// Formatter for test params.
void PrintTo(const conres_test_params& value, ::std::ostream* os)
{
  *os << fmt::format(
      "LCID={}, mode={}", fmt::underlying(value.msg4_lcid), value.duplx_mode == duplex_mode::TDD ? "TDD" : "FDD");
}

/// \brief Test to verify the correct scheduling of the ConRes CE and Msg4 LCID0/1 PDU even when multiple PRACH
/// preambles are detected.
class scheduler_con_res_msg4_test : public base_single_ue_scheduler_conres_test,
                                    public ::testing::TestWithParam<conres_test_params>
{
public:
  scheduler_con_res_msg4_test() : base_single_ue_scheduler_conres_test(GetParam().duplx_mode), params(GetParam()) {}

  /// Enqueues several RACH indications with RNTI different than the created UE.
  void enqueue_random_number_of_rach_indications()
  {
    rach_indication_message rach_ind{to_du_cell_index(0), next_slot_rx(), {{0, 0, {}}}};
    auto                    nof_preambles = test_rng::uniform_int<unsigned>(1, 10);
    for (unsigned i = 0; i != nof_preambles; ++i) {
      rach_ind.occasions[0].preambles.push_back({i, to_rnti(static_cast<uint16_t>(rnti) + 1 + i), phy_time_unit{}});
    }
    this->sched->handle_rach_indication(rach_ind);
  }

  const unsigned msg4_size = 128;

  conres_test_params params;
};

TEST_P(scheduler_con_res_msg4_test,
       when_conres_ce_and_srb_pdu_are_enqueued_then_tc_rnti_is_used_and_multiplexing_with_csi_rs_is_avoided)
{
  // Enqueue several RACH indications, so that RARs that need to be scheduled may fight for RB space with the Msg4.
  enqueue_random_number_of_rach_indications();

  // Enqueue Msg4 in SRB0/SRB1.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  // Wait for ConRes (and optionally Msg4) to be scheduled.
  ASSERT_TRUE(run_until_conres_msg4_scheduled(rnti, lcid_dl_sch_t::UE_CON_RES_ID)) << "ConRes not scheduled";
  const dl_msg_alloc* conres_alloc = find_ue_pdsch(rnti, *this->last_sched_result());
  ASSERT_EQ(conres_alloc->tb_list.size(), 1);
  ASSERT_FALSE(conres_alloc->tb_list[0].lc_chs_to_sched.empty());
  ASSERT_EQ(conres_alloc->tb_list[0].lc_chs_to_sched[0].lcid, lcid_dl_sch_t::UE_CON_RES_ID);
  ASSERT_TRUE(this->last_sched_result()->dl.csi_rs.empty());

  // If the allocation is done in a slot where the SSB is also allocated, then there is only space for ConRes. The Msg4
  // will be sent in a separate PDSCH grant.
  const auto* msg4_lc_info = get_msg4_msg_lc_info(rnti, params.msg4_lcid);
  if (msg4_lc_info == nullptr) {
    // If only the ConRes was scheduled, we need to wait for the PUCCH to be sent and inject an ACK, to confirm the
    // ConRes was received by the UE. Without this, the scheduler won't allocate any other PDSCH grant.
    ASSERT_TRUE(run_until_pucch_scheduled_and_received(rnti, ue_index, /* ACK */ true));
    ASSERT_TRUE(run_until_conres_msg4_scheduled(rnti, GetParam().msg4_lcid)) << "Msg4 not scheduled";
    msg4_lc_info = get_msg4_msg_lc_info(rnti, params.msg4_lcid);
    ASSERT_TRUE(msg4_lc_info != nullptr);
    ASSERT_TRUE(this->last_sched_result()->dl.csi_rs.empty());
  }

  if (params.msg4_lcid == LCID_SRB0) {
    ASSERT_GE(msg4_lc_info->sched_bytes, msg4_size);
  }
}

TEST_P(scheduler_con_res_msg4_test, while_ue_is_in_fallback_then_common_pucch_is_used)
{
  // TODO: Increase the crnti message size, once PUCCH scheduler handles multiple HARQ-ACKs falling in the same slot
  //  in fallback mode.
  static constexpr unsigned crnti_msg_size = 8;

  // Enqueue ConRes CE + Msg4.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  ASSERT_TRUE(run_until_conres_msg4_scheduled(rnti, lcid_dl_sch_t::UE_CON_RES_ID)) << "ConRes not scheduled";
  const dl_msg_alloc* conres_alloc = find_ue_pdsch(rnti, *this->last_sched_result());
  ASSERT_EQ(conres_alloc->tb_list.size(), 1);
  ASSERT_FALSE(conres_alloc->tb_list[0].lc_chs_to_sched.empty());
  ASSERT_EQ(conres_alloc->tb_list[0].lc_chs_to_sched[0].lcid, lcid_dl_sch_t::UE_CON_RES_ID);

  // If the allocation is done in a slot where the SSB is also allocated, then there is only space for ConRes. The Msg4
  // will be sent in a separate PDSCH grant.
  const auto* msg4_lc_info = get_msg4_msg_lc_info(rnti, params.msg4_lcid);
  if (msg4_lc_info == nullptr) {
    // If only the ConRes was scheduled, we need to wait for the PUCCH to be sent and inject an ACK, to confirm the
    // ConRes was received by the UE. Without this, the scheduler won't allocate any other PDSCH grant.
    ASSERT_TRUE(run_until_pucch_scheduled_and_received(rnti, ue_index, /* ACK */ true));
    ASSERT_TRUE(run_until_conres_msg4_scheduled(rnti, GetParam().msg4_lcid)) << "Msg4 not scheduled";
  }

  ASSERT_TRUE(run_until_pucch_scheduled_and_received(rnti, ue_index, /* ACK */ true));

  // Enqueue SRB1 data; with the UE in fallback mode, and after the MSG4 has been delivered, both common and dedicated
  // resources should be used.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, LCID_SRB1, crnti_msg_size});

  // Ensure common resources for PDCCH and PDSCH are used rather than UE-dedicated.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_result()) != nullptr; }));
  const pdcch_dl_information& dl_pdcch = *find_ue_dl_pdcch(rnti);
  ASSERT_EQ(dl_pdcch.dci.type, dci_dl_rnti_config_type::c_rnti_f1_0) << "Invalid format used for UE in fallback mode";
  ASSERT_EQ(dl_pdcch.ctx.coreset_cfg->get_id(), to_coreset_id(0));
  const dl_msg_alloc& pdsch = *find_ue_pdsch(rnti, *this->last_sched_result());
  ASSERT_EQ(pdsch.pdsch_cfg.dci_fmt, dci_dl_format::f1_0);

  // Ensure both common and PUCCH resources are used.
  struct pucch_ptrs {
    const pucch_info* f1_common_ptr = nullptr;
    const pucch_info* f1_ded_ptr    = nullptr;
    const pucch_info* f1_ded_sr_ptr = nullptr;
    const pucch_info* f2_ptr        = nullptr;
  } pucch_res_ptrs;

  ASSERT_TRUE(this->run_slot_until([this, &pucch_res_ptrs]() {
    // Depending on the SR and CSI slots, we can have different combinations of PUCCH grants. There must be at least one
    // PUCCH F1 grant using common resources, and:
    // - No PUCCH F1 ded.
    // - 1 PUCCH F1 ded. with 1 HARQ-ACK bit and NO SR.
    // - 1 PUCCH F1 ded. with 1 HARQ-ACK bit and NO SR and 1 PUCCH F1 ded. with 1 HARQ-ACK bit and SR.
    // - 1 PUCCH F2 ded. with 1 HARQ-ACK bit and CSI, with optional SR.

    const auto& pucchs     = this->last_sched_result()->ul.pucchs;
    unsigned    nof_pucchs = pucchs.size();

    if (nof_pucchs == 1) {
      if (pucchs[0].crnti == rnti and is_f1_pucch_with_harq_bits(pucchs[0], true, false)) {
        pucch_res_ptrs.f1_common_ptr = &pucchs[0];
      }
    } else if (nof_pucchs == 2) {
      for (const auto& pucch : pucchs) {
        if (pucch.crnti != rnti) {
          continue;
        }
        if (is_f1_pucch_with_harq_bits(pucch, true, false)) {
          pucch_res_ptrs.f1_common_ptr = &pucch;
        } else if (is_f1_pucch_with_harq_bits(pucch, false, false)) {
          pucch_res_ptrs.f1_ded_ptr = &pucch;
        } else if (pucch.format() == pucch_format::FORMAT_2 and pucch.uci_bits.harq_ack_nof_bits > 0) {
          pucch_res_ptrs.f2_ptr = &pucch;
        }
      }
    } else if (nof_pucchs == 3) {
      for (const auto& pucch : pucchs) {
        if (pucch.crnti == rnti and pucch.format() == pucch_format::FORMAT_1) {
          if (is_f1_pucch_with_harq_bits(pucch, true, false)) {
            pucch_res_ptrs.f1_common_ptr = &pucch;
          } else if (is_f1_pucch_with_harq_bits(pucch, false, true)) {
            pucch_res_ptrs.f1_ded_sr_ptr = &pucch;
          } else if (is_f1_pucch_with_harq_bits(pucch, false, false)) {
            pucch_res_ptrs.f1_ded_ptr = &pucch;
          }
        }
      }
    }

    return pucch_res_ptrs.f1_common_ptr != nullptr;
  }));

  ASSERT_TRUE(pucch_res_ptrs.f1_common_ptr != nullptr);
  if (pucch_res_ptrs.f1_ded_sr_ptr != nullptr) {
    ASSERT_TRUE(pucch_res_ptrs.f1_ded_ptr != nullptr);
  } else {
    ASSERT_FALSE((pucch_res_ptrs.f1_ded_ptr != nullptr) and (pucch_res_ptrs.f2_ptr != nullptr));
  }
}

TEST_P(scheduler_con_res_msg4_test, while_ue_is_in_fallback_then_common_ss_is_used)
{
  // Enqueue Msg4.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  // Wait for ConRes + Msg4 PDCCH to be scheduled.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_dl_pdcch(rnti) != nullptr; }));

  const pdcch_dl_information&       dl_pdcch          = *find_ue_dl_pdcch(rnti);
  bool                              is_common_ss_used = false;
  const search_space_configuration* ss_used           = nullptr;
  for (const search_space_configuration& ss :
       cell_cfg(to_du_cell_index(0)).params.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
    if (dl_pdcch.ctx.context.ss_id == ss.get_id()) {
      is_common_ss_used = true;
      ss_used           = &ss;
      break;
    }
  }
  ASSERT_TRUE(is_common_ss_used) << "UE in fallback should use common SS";
  // PDCCH monitoring must be active in this slot.
  ASSERT_TRUE(ss_used != nullptr and pdcch_helper::is_pdcch_monitoring_active(next_slot.without_hyper_sfn(), *ss_used))
      << fmt::format(
             "Common SS id={} is not monitored at slot={}", fmt::underlying(ss_used->get_id()), next_slot.slot_index());
}

TEST_P(scheduler_con_res_msg4_test, when_msg4_gets_retxed_then_tc_rnti_is_used_and_csi_rs_avoided)
{
  // Enqueue Msg4.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  // Wait for ConRes + Msg4 PDCCH, PDSCH and PUCCH to be scheduled.
  const pucch_info* first_msg4_pucch = run_until_common_pucch_with_harq(rnti);
  ASSERT_NE(first_msg4_pucch, nullptr)
      << "Failed to schedule ConRes CE and Msg4 PDCCH, PDSCH and PUCCH for UE in fallback mode";

  // Enqueue HARQ NACK for Msg4.
  uci_indication uci =
      test_helper::create_uci_indication(next_slot.without_hyper_sfn() - 1, ue_index, *first_msg4_pucch);
  std::get<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(uci.ucis[0].pdu).harqs[0] =
      mac_harq_ack_report_status::nack;
  this->sched->handle_uci_indication(uci);

  // Msg4 re-transmission should be scheduled.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_result()) != nullptr; }));
  const dl_msg_alloc* msg4_alloc = find_ue_pdsch(rnti, *this->last_sched_result());
  ASSERT_EQ(msg4_alloc->tb_list.size(), 0) << "Retransmissions should not have a TB list";
  ASSERT_EQ(msg4_alloc->pdsch_cfg.dci_fmt, dci_dl_format::f1_0);
  ASSERT_EQ(find_ue_dl_pdcch(rnti)->dci.type, dci_dl_rnti_config_type::tc_rnti_f1_0);
  ASSERT_TRUE(this->last_sched_result()->dl.csi_rs.empty());

  // Ensure the retransmission uses common PUCCH.
  const pucch_info* msg4_retx_pucch = run_until_common_pucch_with_harq(rnti);
  ASSERT_NE(msg4_retx_pucch, nullptr) << "Failed to schedule Msg4 retransmission PUCCH using common resources";
  bool found_common_pucch = false;
  for (const auto& pucch : this->last_sched_result()->ul.pucchs) {
    found_common_pucch |= is_f1_pucch_with_harq_bits(pucch, true, false);
  }
  ASSERT_TRUE(found_common_pucch) << "Failed to schedule Msg4 retransmission PUCCH using common resources";
}

INSTANTIATE_TEST_SUITE_P(scheduler_con_res_msg4_test_with_different_srb,
                         scheduler_con_res_msg4_test,
                         ::testing::Values(conres_test_params{LCID_SRB0, duplex_mode::TDD},
                                           conres_test_params{LCID_SRB0, duplex_mode::FDD},
                                           conres_test_params{LCID_SRB1, duplex_mode::FDD},
                                           conres_test_params{LCID_SRB1, duplex_mode::TDD}));

struct conres_expiry_params {
  subcarrier_spacing max_scs        = subcarrier_spacing::kHz15;
  unsigned           ntn_cs_koffset = 0;
};

/// Formatter for test params.
void PrintTo(const conres_expiry_params& value, ::std::ostream* os)
{
  *os << fmt::format("SCS={}, Cell-Specific-K-Offset={}", to_string(value.max_scs), value.ntn_cs_koffset);
}

class scheduler_conres_expiry_test : public scheduler_test_simulator,
                                     public ::testing::TestWithParam<conres_expiry_params>
{
protected:
  scheduler_conres_expiry_test() :
    scheduler_test_simulator(
        scheduler_test_sim_config{.max_scs        = GetParam().max_scs,
                                  .ntn_cs_koffset = std::chrono::milliseconds(GetParam().ntn_cs_koffset)})
  {
    // Create cell.
    auto cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    if (ntn_cs_koffset != std::chrono::milliseconds{0}) {
      cell_cfg_req.ran.ntn_params.emplace();
      cell_cfg_req.ran.ntn_params->ntn_cfg.cell_specific_koffset = ntn_cs_koffset;
    }
    add_cell(cell_cfg_req);
    nof_rtt_slots    = ntn_cs_koffset.count() * next_slot.nof_slots_per_subframe();
    conres_win_slots = cell_cfg(cell_index).params.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer.count() *
                       next_slot.nof_slots_per_subframe();
  }

  void create_ue(unsigned ue_creation_delay_slots = 0)
  {
    auto ue_cfg     = sched_config_helper::create_default_sched_ue_creation_request(cell_cfg(cell_index).params, {});
    ue_cfg.ue_index = ue_index;
    ue_cfg.crnti    = rnti;
    ue_cfg.starts_in_fallback = true;
    ul_ccch_slot_rx           = next_slot.without_hyper_sfn() - ue_creation_delay_slots;
    ue_cfg.ul_ccch_slot_rx    = ul_ccch_slot_rx;
    scheduler_test_simulator::add_ue(ue_cfg, false);
    conres_expiry_slot = ul_ccch_slot_rx + conres_win_slots + nof_rtt_slots;
  }

  cell_config_builder_params builder_params{cell_config_builder_profiles::create(duplex_mode::TDD)};
  const du_cell_index_t      cell_index = to_du_cell_index(0);
  const du_ue_index_t        ue_index   = to_du_ue_index(0);
  const rnti_t               rnti       = to_rnti(0x4601);
  unsigned                   nof_rtt_slots;
  unsigned                   conres_win_slots;
  slot_point                 ul_ccch_slot_rx;
  slot_point                 conres_expiry_slot;
};

TEST_P(scheduler_conres_expiry_test, when_conres_ce_arrives_after_conres_timer_expires_then_no_pdsch_is_scheduled)
{
  // Create UE with a delay large enough that the ConRes timer is already expired at creation time. The
  // auto-injected ConRes CE should be immediately discarded by the scheduler.
  create_ue(conres_win_slots + nof_rtt_slots + test_rng::uniform_int<unsigned>(0, 10));

  // Ensure the ConRes CE is not scheduled.
  ASSERT_FALSE(this->run_slot_until(
      [this]() { return find_ue_pdsch(rnti, *this->last_sched_result(cell_index)) != nullptr; }, 100));
}

TEST_P(scheduler_conres_expiry_test, when_conres_retx_goes_after_conres_timer_expiry_it_is_not_scheduled)
{
  // Create UE with a delay close to the ConRes window so the first newTx is scheduled near expiry.
  create_ue(conres_win_slots + nof_rtt_slots - 10);
  auto pdsch_is_sched = [this]() { return find_ue_pdsch(rnti, *this->last_sched_result(cell_index)) != nullptr; };

  // Get closer to the conRes expiry slot (0 iterations with the chosen delay).
  while (next_slot.without_hyper_sfn() < conres_expiry_slot - 10) {
    run_slot();
    ASSERT_FALSE(pdsch_is_sched());
  }

  // Wait for newTx with ConRes CE to be scheduled (auto-injected at UE creation).
  ASSERT_TRUE(this->run_slot_until(pdsch_is_sched, 100));

  // Wait for common PUCCH.
  ASSERT_TRUE(this->run_slot_until(
      [this]() { return find_ue_pucch(rnti, *this->last_sched_result(cell_index)) != nullptr; }, 100));

  // Enqueue a NACK after ConRes timer expires.
  uci_indication uci_ind;
  uci_ind.cell_index = cell_index;
  uci_ind.slot_rx    = next_slot.without_hyper_sfn() - 1;
  for (const pucch_info& pucch : this->last_sched_result(cell_index)->ul.pucchs) {
    uci_ind.ucis.push_back(test_helper::create_uci_indication_pdu(ue_index, pucch, mac_harq_ack_report_status::nack));
  }
  while (next_slot.without_hyper_sfn() < conres_expiry_slot) {
    run_slot();
    ASSERT_FALSE(pdsch_is_sched());
  }
  this->sched->handle_uci_indication(uci_ind);

  // No PDSCH should be scheduled, as the ConRes timer has expired.
  ASSERT_FALSE(this->run_slot_until(pdsch_is_sched, 100));
}

TEST_P(scheduler_conres_expiry_test, when_ntn_cell_conres_timer_extended_with_rtt)
{
  // Create UE with a delay equal to conres_win_slots when NTN RTT extension is enabled.
  create_ue(nof_rtt_slots > 0 ? conres_win_slots : 0);
  static constexpr unsigned msg4_size = 128;
  auto pdsch_is_sched = [this]() { return find_ue_pdsch(rnti, *this->last_sched_result(cell_index)) != nullptr; };

  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, LCID_SRB1, msg4_size});

  // ConRes CE was auto-injected at UE creation; PDSCH should be scheduled within the remaining timer window.
  ASSERT_TRUE(this->run_slot_until(pdsch_is_sched, 100));
}

INSTANTIATE_TEST_SUITE_P(scheduler_conres_expiry_test,
                         scheduler_conres_expiry_test,
                         ::testing::Values(conres_expiry_params{subcarrier_spacing::kHz30, 0},
                                           conres_expiry_params{subcarrier_spacing::kHz30, 240},
                                           conres_expiry_params{subcarrier_spacing::kHz30, 480}));

class scheduler_ue_no_config_test : public base_scheduler_conres_test, public ::testing::Test
{
protected:
  scheduler_ue_no_config_test() : base_scheduler_conres_test(duplex_mode::TDD)
  {
    // Create cell.
    auto cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    add_cell(cell_cfg_req);

    // Create a UE without serv cell config.
    auto ue_cfg     = sched_config_helper::create_empty_spcell_cfg_sched_ue_creation_request(cell_cfg_req.ran);
    ue_cfg.ue_index = ue_index;
    ue_cfg.crnti    = rnti;
    ue_cfg.starts_in_fallback = true;
    ue_cfg.ul_ccch_slot_rx    = next_slot.without_hyper_sfn();
    scheduler_test_simulator::add_ue(ue_cfg, false);
  }

  const du_cell_index_t cell_index = to_du_cell_index(0);
  const du_ue_index_t   ue_index   = to_du_ue_index(0);
  const rnti_t          rnti       = to_rnti(0x4601);
  const unsigned        msg4_size  = 128;
};

TEST_F(scheduler_ue_no_config_test, when_ue_has_no_serv_cell_cfg_then_msg4_and_conres_are_still_scheduled)
{
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, LCID_SRB0, msg4_size});

  // Wait for ConRes (and optionally Msg4) to be scheduled.
  ASSERT_TRUE(run_until_conres_msg4_scheduled(rnti, lcid_dl_sch_t::UE_CON_RES_ID)) << "ConRes not scheduled";
  const dl_msg_alloc* conres_alloc = find_ue_pdsch(rnti, *this->last_sched_result());
  ASSERT_EQ(conres_alloc->tb_list.size(), 1);
  ASSERT_FALSE(conres_alloc->tb_list[0].lc_chs_to_sched.empty());
  ASSERT_EQ(conres_alloc->tb_list[0].lc_chs_to_sched[0].lcid, lcid_dl_sch_t::UE_CON_RES_ID);

  // If the allocation is done in a slot where the SSB is also allocated, then there is only space for ConRes. The Msg4
  // will be sent in a separate PDSCH grant.
  const auto* msg4_lc_info = get_msg4_msg_lc_info(rnti, LCID_SRB0);
  if (msg4_lc_info == nullptr) {
    // If only the ConRes was scheduled, we need to wait for the PUCCH to be sent and inject an ACK, to confirm the
    // ConRes was received by the UE. Without this, the scheduler won't allocate any other PDSCH grant.
    ASSERT_TRUE(run_until_pucch_scheduled_and_received(rnti, ue_index, /* ACK */ true));
    ASSERT_TRUE(run_until_conres_msg4_scheduled(rnti, LCID_SRB0)) << "Msg4 not scheduled";
  }

  // Enqueue HARQ NACK for Msg4.
  ASSERT_TRUE(run_until_pucch_scheduled_and_received(rnti, ue_index, /* ACK */ false));

  // Wait for Msg4 re-transmission to be scheduled.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_dl_pdcch(rnti) != nullptr; }));
  const dl_msg_alloc* msg4_retx_alloc = find_ue_pdsch(rnti, *this->last_sched_result());
  ASSERT_FALSE(msg4_retx_alloc->pdsch_cfg.codewords[0].new_data);
}

// ------------------------------------------------------------------------------------------------------------------ //

/// Fixture that wires together a full scheduler instance with a cell configured for CFRA (preamble IDs [60, 64))
/// and a pre-created CFRA UE.
class cfra_scheduler_test : public scheduler_test_simulator, public ::testing::Test
{
  static constexpr unsigned NOF_CB_PREAMBLES = 60;

public:
  cfra_scheduler_test()
  {
    cell_config_builder_params bparams;
    auto                       cell_req = sched_config_helper::make_default_sched_cell_configuration_request(bparams);
    cell_req.ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb = NOF_CB_PREAMBLES;
    add_cell(cell_req);

    auto ue_req = sched_config_helper::create_default_sched_ue_creation_request(
        cell_cfg().params, std::array<lcid_t, 3>{lcid_t::LCID_SRB1, lcid_t::LCID_SRB2, lcid_t::LCID_MIN_DRB});
    ue_req.crnti              = cfra_tc_rnti;
    ue_req.ue_index           = cfra_ue_index;
    ue_req.starts_in_fallback = true;
    ue_req.cfra_enabled       = true;
    ue_req.ul_ccch_slot_rx    = std::nullopt;
    add_ue(ue_req);
  }

  rach_indication_message create_cfra_rach_indication() const
  {
    const unsigned cfra_preamble_id =
        cell_cfg().params.ul_cfg_common.init_ul_bwp.rach_cfg_common->nof_cb_preambles_per_ssb;
    auto preamble = test_helper::create_preamble(cfra_preamble_id, cfra_tc_rnti);
    return test_helper::create_rach_indication(next_slot_rx(), {preamble});
  }

  void send_cfra_crc(bool success)
  {
    ul_crc_indication crc_ind;
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.sl_rx      = last_result_slot();
    auto& pdu          = crc_ind.crcs.emplace_back();
    pdu.rnti           = cfra_tc_rnti;
    pdu.ue_index       = cfra_ue_index;
    pdu.harq_id        = to_harq_id(0);
    pdu.tb_crc_success = success;
    sched->handle_crc_indication(crc_ind);
  }

  const du_ue_index_t cfra_ue_index = to_du_ue_index(0);
  const rnti_t        cfra_tc_rnti  = to_rnti(0x4601);
};

/// Verify that no PUCCH is scheduled for the CFRA UE in the same slot as its Msg3 PUSCH.
///
/// This guards against the regression where uci_sched.add_ue() was called at UE creation, causing a
/// PUCCH+PUSCH conflict before Msg3 was ACKed.
TEST_F(cfra_scheduler_test, no_pucch_in_msg3_slot_while_cfra_pending)
{
  sched->handle_rach_indication(create_cfra_rach_indication());

  bool msg3_found = false;
  ASSERT_TRUE(run_slot_until([this, &msg3_found]() {
    const auto& puschs = last_sched_result()->ul.puschs;
    for (const auto& pusch : puschs) {
      if (pusch.pusch_cfg.rnti == cfra_tc_rnti) {
        // Verify no PUCCH for the same RNTI in the same slot.
        const pucch_info* pucch = find_ue_pucch(cfra_tc_rnti, *last_sched_result());
        EXPECT_EQ(pucch, nullptr) << "PUCCH scheduled for CFRA UE in the same slot as Msg3 PUSCH";
        msg3_found = true;
        return true;
      }
    }
    return false;
  })) << "Msg3 PUSCH was not scheduled within the timeout";

  ASSERT_TRUE(msg3_found);
}

/// Verify that after the Msg3 ACK (cfra_msg3_acked), the scheduler runs stably without any resource conflicts.
///
/// The CFRA UE transitions from pending_cfra to conres_completed, and uci_sched.add_ue is called.
/// Subsequent slots must not trigger any scheduling assertions.
TEST_F(cfra_scheduler_test, scheduler_is_stable_after_msg3_ack)
{
  sched->handle_rach_indication(create_cfra_rach_indication());

  // Run until Msg3 PUSCH is scheduled.
  ASSERT_TRUE(run_slot_until([this]() {
    const auto& puschs = last_sched_result()->ul.puschs;
    return std::any_of(
        puschs.begin(), puschs.end(), [this](const ul_sched_info& p) { return p.pusch_cfg.rnti == cfra_tc_rnti; });
  })) << "Msg3 PUSCH was not scheduled within the timeout";

  // Send CFRA CRC ACK: routes through both RA scheduler (clears Msg3 HARQ) and UE event manager
  // (cfra_msg3_acked → conres_completed → uci_sched.add_ue).
  send_cfra_crc(true);

  // Run several more slots to confirm no assertions fire after UCI scheduling is activated.
  for (unsigned i = 0; i < 30; ++i) {
    run_slot();
  }
}
