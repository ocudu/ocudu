// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/common_scheduling/si_scheduler.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/support/paging_helpers.h"
#include "sub_scheduler_test_environment.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/ran/pdcch/dci_packing.h"
#include "ocudu/scheduler/result/dci_info.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

sched_cell_configuration_request_message make_sched_configuration_request(const si_scheduling_config& si_sched_cfg)
{
  sched_cell_configuration_request_message msg = sched_config_helper::make_default_sched_cell_configuration_request();
  msg.si_scheduling                            = si_sched_cfg;
  return msg;
}

class si_scheduler_test_environment : public sub_scheduler_test_environment
{
public:
  si_scheduler_test_environment(const sched_cell_configuration_request_message& msg) :
    sub_scheduler_test_environment(msg), si_sched(cell_cfg, *pdcch_alloc, msg)
  {
    run_slot();
  }
  ~si_scheduler_test_environment() override { flush_events(); }

  void do_run_slot() override { si_sched.run_slot(res_grid); }

  si_scheduler si_sched;
};

TEST(no_si_scheduler_test, when_no_si_is_provided_then_nothing_is_scheduled)
{
  si_scheduler_test_environment setup{make_sched_configuration_request(si_scheduling_config{units::bytes{0}, {}, 0})};

  const unsigned nof_slots = 100;

  for (unsigned i = 0; i != nof_slots; ++i) {
    setup.run_slot();

    ASSERT_TRUE(setup.res_grid[0].result.dl.bc.sibs.empty());
    ASSERT_TRUE(setup.res_grid[0].result.dl.dl_pdcchs.empty());
  }
}

constexpr units::bytes     DEFAULT_SIB1_PAYLOAD_SIZE{128};
const si_scheduling_config DEFAULT_SI_SCHED_CFG{DEFAULT_SIB1_PAYLOAD_SIZE,
                                                {{si_message_scheduling_config{units::bytes{64}, 16}}},
                                                10};

class si_scheduler_test : public si_scheduler_test_environment, public testing::Test
{
protected:
  si_scheduler_test() : si_scheduler_test_environment(make_sched_configuration_request(DEFAULT_SI_SCHED_CFG)) {}
};

TEST_F(si_scheduler_test, when_sib1_is_cfg_then_sib1_gets_scheduled)
{
  const unsigned nof_test_slots =
      DEFAULT_SI_SCHED_CFG.si_messages[0].period_radio_frames * next_slot.nof_slots_per_frame();

  std::array<bool, 2> si_scheduled{false, false};
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();

    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      ASSERT_EQ(sib.version, 0);
      if (sib.si_indicator == sib_information::sib1) {
        ASSERT_GE(sib.pdsch_cfg.codewords[0].tb_size_bytes, DEFAULT_SIB1_PAYLOAD_SIZE);
        si_scheduled[0] = true;
      } else {
        ASSERT_GE(sib.pdsch_cfg.codewords[0].tb_size_bytes, DEFAULT_SI_SCHED_CFG.si_messages[0].msg_len);
        ASSERT_EQ(sib.si_msg_index.value(), 0);
        si_scheduled[1] = true;
      }
    }
  }

  std::array<bool, 2> expected{true, true};
  ASSERT_EQ(si_scheduled, expected);
}

TEST_F(si_scheduler_test, when_si_is_updated_then_new_version_is_applied_at_si_change_window_boundary)
{
  const units::bytes   new_sib1_len     = DEFAULT_SIB1_PAYLOAD_SIZE + units::bytes{64U};
  si_scheduling_config new_si_sched_cfg = DEFAULT_SI_SCHED_CFG;
  new_si_sched_cfg.sib1_payload_size    = new_sib1_len;
  new_si_sched_cfg.si_messages[0].msg_len += units::bytes{64U};

  const unsigned si_ch_wind_len_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.bcch_cfg.mod_period_coeff) *
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned sfn_mod = (next_slot + res_grid.max_dl_slot_alloc_delay).sfn() % si_ch_wind_len_rfs;
  const unsigned si_change_min_count =
      (si_ch_wind_len_rfs - sfn_mod) * next_slot.nof_slots_per_frame() - next_slot.slot_index();

  // Update SI scheduling.
  si_sched.handle_si_update_request(si_scheduling_update_request{to_du_cell_index(0), 1, new_si_sched_cfg});

  const unsigned          nof_test_slots = 2 * si_ch_wind_len_rfs * next_slot.nof_slots_per_frame();
  unsigned                last_version   = 0;
  std::optional<unsigned> window_version;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    slot_point current_slot = next_slot;
    run_slot();

    if (current_slot.sfn() % si_ch_wind_len_rfs == 0 and current_slot.slot_index() == 0) {
      // Detected SI change window start.
      window_version = std::nullopt;
      test_logger.info("New window starting at {}", current_slot);
    }

    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (window_version.has_value()) {
        ASSERT_EQ(sib.version, window_version.value())
            << "SI version cannot change in the middle of a SI change window";
      } else {
        window_version = sib.version;
        ASSERT_GE(sib.version, last_version) << "SI version cannot decrease";
        last_version = sib.version;

        if (sib.version == 1) {
          ASSERT_GE(i, si_change_min_count) << "SI change applied too soon";
        }
      }

      unsigned tbs = sib.pdsch_cfg.codewords[0].tb_size_bytes.value();
      if (sib.version == 1) {
        ASSERT_GE(tbs,
                  sib.si_indicator == sib_information::sib1 ? new_sib1_len.value()
                                                            : new_si_sched_cfg.si_messages[0].msg_len.value())
            << "New SI payload length not applied";
      } else {
        ASSERT_EQ(sib.version, 0);
        ASSERT_GE(tbs,
                  sib.si_indicator == sib_information::sib1 ? DEFAULT_SIB1_PAYLOAD_SIZE.value()
                                                            : DEFAULT_SI_SCHED_CFG.si_messages[0].msg_len.value());
      }
    }
  }

  ASSERT_EQ(last_version, 1);
}

TEST_F(si_scheduler_test, when_si_is_updated_all_ues_in_rrc_idle_get_notified_exactly_once)
{
  const paging_slot_helper slot_helper(cell_cfg);
  const units::bytes       new_sib1_len     = DEFAULT_SIB1_PAYLOAD_SIZE + units::bytes{64U};
  si_scheduling_config     new_si_sched_cfg = DEFAULT_SI_SCHED_CFG;
  new_si_sched_cfg.sib1_payload_size        = new_sib1_len;
  new_si_sched_cfg.si_messages[0].msg_len += units::bytes{64U};

  // Update SI scheduling.
  si_sched.handle_si_update_request(si_scheduling_update_request{to_du_cell_index(0), 1, new_si_sched_cfg});

  const unsigned si_ch_wind_len_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.bcch_cfg.mod_period_coeff) *
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);

  const unsigned                   nof_test_slots      = 2 * si_ch_wind_len_rfs * next_slot.nof_slots_per_frame();
  bool                             new_version_applied = false;
  static constexpr unsigned        total_nof_ue_ids    = 1024;
  bounded_bitset<total_nof_ue_ids> notified_ue_ids(total_nof_ue_ids);
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    slot_point current_slot = next_slot;
    run_slot();

    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.version == 1) {
        new_version_applied = true;
      }
    }

    for (const auto& pdcch : res_grid[0].result.dl.dl_pdcchs) {
      if (pdcch.dci.type() != ocudu::dci_dl_rnti_config_type::p_rnti_f1_0) {
        continue;
      }

      const auto& dci = pdcch.dci.as_p_rnti_f1_0();
      if (dci.short_messages_indicator != ocudu::dci_1_0_p_rnti_configuration::payload_info::short_messages) {
        continue;
      }
      ASSERT_EQ(dci.short_messages, 0x80);

      // Notifications shouldn't be sent after the new version is applied.
      ASSERT_FALSE(new_version_applied);

      const unsigned paging_frame_offset = cell_cfg.params.dl_cfg_common.pcch_cfg.paging_frame_offset;
      const auto     drx_cycle = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
      const auto     nof_pf_per_drx_cycle = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.nof_pf);
      const auto     nof_po_per_pf        = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.ns);
      const unsigned N                    = drx_cycle / nof_pf_per_drx_cycle;
      const unsigned t_div_n              = drx_cycle / N;
      for (unsigned ue_id = 0; ue_id < total_nof_ue_ids; ++ue_id) {
        // Check for paging frame.
        // (SFN + PF_offset) mod T = (T div N)*(UE_ID mod N). See TS 38.304, clause 7.1.
        const unsigned ue_id_mod_n = ue_id % N;
        if (((current_slot.sfn() + paging_frame_offset) % drx_cycle) != (t_div_n * ue_id_mod_n)) {
          continue;
        }

        // Index (i_s), indicating the index of the PO.
        // i_s = floor (UE_ID/N) mod Ns.
        const unsigned i_s = (ue_id / N) % nof_po_per_pf;
        if (slot_helper.is_paging_slot(current_slot, i_s)) {
          ASSERT_FALSE(notified_ue_ids.test(ue_id));
          notified_ue_ids.set(ue_id);
          test_logger.debug("UE ID {} notified at {}", ue_id, current_slot);
        }
      }
    }
  }

  ASSERT_TRUE(notified_ue_ids.all());
}

class si_msg_scheduler_tdra_test : public si_scheduler_test_environment, public testing::Test
{
protected:
  static constexpr ofdm_symbol_range expected_symbols{ofdm_symbol_range::start_and_len(2, 9)};

  si_msg_scheduler_tdra_test() :
    si_scheduler_test_environment([]() {
      // Default A table entry 0: {k0=0, typeA, symbols=[2,14)} i.e. S=2, L=12.
      // Custom entry 0:          {k0=0, typeA, symbols=[2,11)} i.e. S=2, L=9.
      // Custom PDSCH resource TD list should be used when configured.
      auto req = make_sched_configuration_request(DEFAULT_SI_SCHED_CFG);
      req.ran.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list = {
          {0, sch_mapping_type::typeA, ofdm_symbol_range::start_and_len(2, 9)}};
      return req;
    }())
  {
  }
};

TEST_F(si_msg_scheduler_tdra_test, when_custom_pdsch_td_alloc_list_configured_then_si_msg_pdsch_uses_configured_symbols)
{
  bool           other_si_found = false;
  const unsigned nof_test_slots =
      2 * DEFAULT_SI_SCHED_CFG.si_messages[0].period_radio_frames * next_slot.nof_slots_per_frame();
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();
    for (const auto& sib : last_sched_result().dl.bc.sibs) {
      if (sib.si_indicator == sib_information::other_si) {
        EXPECT_EQ(sib.pdsch_cfg.symbols, expected_symbols);
        other_si_found = true;
      }
    }
  }
  ASSERT_TRUE(other_si_found) << "SI message was not scheduled within the expected window";
}

} // namespace
