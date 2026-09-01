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

  void do_run_slot() override { si_sched.run_slot(res_grid, next_slot.hyper_sfn()); }

  si_scheduler si_sched;
};

TEST(no_si_scheduler_test, when_no_si_is_provided_then_nothing_is_scheduled)
{
  si_scheduler_test_environment setup{
      make_sched_configuration_request(si_scheduling_config{units::bytes{0}, {}, {}, 0})};

  const unsigned nof_slots = 100;

  for (unsigned i = 0; i != nof_slots; ++i) {
    setup.run_slot();

    ASSERT_TRUE(setup.res_grid[0].result.dl.bc.sibs.empty());
    ASSERT_TRUE(setup.res_grid[0].result.dl.dl_pdcchs.empty());
  }
}

constexpr units::bytes     DEFAULT_SIB1_PAYLOAD_SIZE{128};
const si_scheduling_config DEFAULT_SI_SCHED_CFG{
    DEFAULT_SIB1_PAYLOAD_SIZE,
    {{si_message_scheduling_config{sib_type_set{sib_type::sib2}, units::bytes{64}, 16}}},
    {},
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
    slot_point current_slot = next_slot.without_hyper_sfn();
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

TEST_F(si_scheduler_test, when_si_is_updated_then_new_msg_len_is_applied_right_after_the_request)
{
  si_scheduling_config new_si_sched_cfg = DEFAULT_SI_SCHED_CFG;
  new_si_sched_cfg.si_messages[0].msg_len += units::bytes{64U};
  // Immediate content (e.g. NTN SIB19): grant sizing is expected to update right after the request.
  new_si_sched_cfg.si_messages[0].sibs = sib_type_set{sib_type::sib19};

  {
    bool           found_before = false;
    const unsigned nof_baseline_test_slots =
        DEFAULT_SI_SCHED_CFG.si_messages[0].period_radio_frames * next_slot.nof_slots_per_frame();
    for (unsigned i = 0; i != nof_baseline_test_slots and not found_before; ++i) {
      run_slot();
      for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
        if (sib.si_indicator != sib_information::other_si) {
          continue;
        }
        ASSERT_EQ(sib.version, 0);
        ASSERT_LT(sib.pdsch_cfg.codewords[0].tb_size_bytes, new_si_sched_cfg.si_messages[0].msg_len)
            << "Baseline transmission is already sized for the new length -- test setup is not exercising a real "
               "before/after transition";
        found_before = true;
      }
    }
    ASSERT_TRUE(found_before) << "SI-message was not scheduled within the expected baseline window";
  }

  const unsigned si_ch_wind_len_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.bcch_cfg.mod_period_coeff) *
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned si_msg_period_slots =
      new_si_sched_cfg.si_messages[0].period_radio_frames * next_slot.nof_slots_per_frame();
  auto compute_si_change_min_count = [&]() {
    const unsigned sfn_mod = (next_slot + res_grid.max_dl_slot_alloc_delay).sfn() % si_ch_wind_len_rfs;
    return (si_ch_wind_len_rfs - sfn_mod) * next_slot.nof_slots_per_frame() - next_slot.slot_index();
  };

  // next_slot starts at a randomized point in time (see generate_random_slot_point), so the SI modification window
  // boundary may be less than one SI-message period away. Skip ahead past the boundary until there is enough room
  // to guarantee at least one SI-message occasion before it -- otherwise "found" below could stay false for a
  // reason unrelated to what this test is checking.
  unsigned si_change_min_count = compute_si_change_min_count();
  while (si_change_min_count <= si_msg_period_slots) {
    run_slot();
    si_change_min_count = compute_si_change_min_count();
  }

  // Update SI scheduling.
  si_sched.handle_si_update_request(si_scheduling_update_request{to_du_cell_index(0), 1, new_si_sched_cfg});

  bool found = false;
  for (unsigned i = 0; i != si_change_min_count; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.si_indicator != sib_information::other_si) {
        continue;
      }
      ASSERT_EQ(sib.version, 0) << "SI version must not have changed yet -- still before the modification window";
      ASSERT_GE(sib.pdsch_cfg.codewords[0].tb_size_bytes, new_si_sched_cfg.si_messages[0].msg_len)
          << "PDSCH grant was not resized for the new content length right after the request";
      found = true;
    }
  }

  ASSERT_TRUE(found) << "SI-message was not scheduled within the expected window";
}

TEST_F(si_scheduler_test, when_non_exempt_si_message_is_updated_then_new_msg_len_only_applies_at_si_change_window)
{
  si_scheduling_config new_si_sched_cfg = DEFAULT_SI_SCHED_CFG;
  new_si_sched_cfg.si_messages[0].msg_len += units::bytes{64U};
  ASSERT_FALSE(new_si_sched_cfg.si_messages[0].is_ntn()) << "This SI-message must be non-exempt";

  const unsigned si_ch_wind_len_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.bcch_cfg.mod_period_coeff) *
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned sfn_mod = (next_slot + res_grid.max_dl_slot_alloc_delay).sfn() % si_ch_wind_len_rfs;
  const unsigned si_change_min_count =
      (si_ch_wind_len_rfs - sfn_mod) * next_slot.nof_slots_per_frame() - next_slot.slot_index();

  // Update SI scheduling.
  si_sched.handle_si_update_request(si_scheduling_update_request{to_du_cell_index(0), 1, new_si_sched_cfg});

  // The new length should not get updated before the SI change modification window.
  for (unsigned i = 0; i != si_change_min_count; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.si_indicator != sib_information::other_si) {
        continue;
      }
      ASSERT_EQ(sib.version, 0) << "SI version must not have changed yet -- still before the modification window";
      ASSERT_LT(sib.pdsch_cfg.codewords[0].tb_size_bytes, new_si_sched_cfg.si_messages[0].msg_len)
          << "Grant was updated before the SI change window for a non-exempt SI-message";
    }
  }

  // The new length will only take effect once the SI version itself bumps, at the SI change modification window.
  const unsigned nof_post_window_test_slots = si_ch_wind_len_rfs * next_slot.nof_slots_per_frame();
  for (unsigned i = 0; i != nof_post_window_test_slots; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.si_indicator != sib_information::other_si or sib.version != 1) {
        continue;
      }
      ASSERT_EQ(sib.version, 1) << "SI version have changed yet, within the modification window";
      ASSERT_GE(sib.pdsch_cfg.codewords[0].tb_size_bytes, new_si_sched_cfg.si_messages[0].msg_len)
          << "Grant was not updated to the new msg_len at the SI change window";
    }
  }
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
    slot_point current_slot = next_slot.without_hyper_sfn();
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

/// \brief Puts an ETWS/CMAS SI epoch in effect and signals one broadcast of it.
///
/// What is broadcast, and for how long each broadcast lasts, is stated by the epoch. Every warning is stamped with the
/// version of the epoch, so all of them start one more broadcast.
static void broadcast_warning(si_scheduler&                                      si_sched,
                              const si_scheduling_config&                        cfg,
                              si_version_type                                    version,
                              std::initializer_list<pws_broadcasting_si_message> broadcasting)
{
  pws_si_scheduling_update_request req{to_du_cell_index(0), version, cfg, {}};
  for (pws_broadcasting_si_message entry : broadcasting) {
    entry.version = version;
    req.broadcasting.push_back(entry);
  }
  si_sched.handle_pws_si_update_request(req);
}

const si_scheduling_config ACTIVATION_REQUIRED_SI_SCHED_CFG{
    DEFAULT_SIB1_PAYLOAD_SIZE,
    {{si_message_scheduling_config{sib_type_set{sib_type::sib7}, units::bytes{64}, 16}}},
    {},
    10};

class si_msg_scheduler_activation_test : public si_scheduler_test_environment, public testing::Test
{
protected:
  si_msg_scheduler_activation_test() :
    si_scheduler_test_environment(make_sched_configuration_request(ACTIVATION_REQUIRED_SI_SCHED_CFG))
  {
  }
};

TEST_F(si_msg_scheduler_activation_test,
       when_pws_broadcast_indication_received_then_all_ues_in_rrc_idle_get_notified_at_least_once)
{
  // As per TS 38.304, ETWS/CMAS-capable UEs in RRC_IDLE/RRC_INACTIVE monitor for the PWS short-message notification
  // only in their own paging occasion, once per DRX cycle. Since the network does not know a UE's UE_ID (hence its
  // exact paging occasion), the notification must be repeated across a full default paging cycle -- not sent once.
  const paging_slot_helper slot_helper(cell_cfg);
  broadcast_warning(si_sched,
                    ACTIVATION_REQUIRED_SI_SCHED_CFG,
                    1,
                    {{sib_type_set{sib_type::sib7}, 1, ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].msg_len}});

  const unsigned drx_cycle_rfs  = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned nof_test_slots = 2 * drx_cycle_rfs * next_slot.nof_slots_per_frame();

  static constexpr unsigned        total_nof_ue_ids = 1024;
  bounded_bitset<total_nof_ue_ids> notified_ue_ids(total_nof_ue_ids);
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    slot_point current_slot = next_slot.without_hyper_sfn();
    run_slot();

    for (const auto& pdcch : res_grid[0].result.dl.dl_pdcchs) {
      if (pdcch.dci.type() != ocudu::dci_dl_rnti_config_type::p_rnti_f1_0) {
        continue;
      }
      const auto& dci = pdcch.dci.as_p_rnti_f1_0();
      if (dci.short_messages_indicator != ocudu::dci_1_0_p_rnti_configuration::payload_info::short_messages) {
        continue;
      }
      // etwsAndCmasIndication bit, as per TS 38.331 Table 6.5-1.
      if ((dci.short_messages & 0x40U) == 0) {
        continue;
      }

      const unsigned paging_frame_offset = cell_cfg.params.dl_cfg_common.pcch_cfg.paging_frame_offset;
      const auto     drx_cycle = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
      const auto     nof_pf_per_drx_cycle = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.nof_pf);
      const auto     nof_po_per_pf        = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.ns);
      const unsigned N                    = drx_cycle / nof_pf_per_drx_cycle;
      const unsigned t_div_n              = drx_cycle / N;
      for (unsigned ue_id = 0; ue_id < total_nof_ue_ids; ++ue_id) {
        // Check for paging frame. (SFN + PF_offset) mod T = (T div N)*(UE_ID mod N). See TS 38.304, clause 7.1.
        const unsigned ue_id_mod_n = ue_id % N;
        if (((current_slot.sfn() + paging_frame_offset) % drx_cycle) != (t_div_n * ue_id_mod_n)) {
          continue;
        }

        // Index (i_s), indicating the index of the PO. i_s = floor (UE_ID/N) mod Ns.
        const unsigned i_s = (ue_id / N) % nof_po_per_pf;
        if (slot_helper.is_paging_slot(current_slot, i_s)) {
          notified_ue_ids.set(ue_id);
        }
      }
    }
  }

  ASSERT_TRUE(notified_ue_ids.all());
}

TEST_F(si_msg_scheduler_activation_test, when_new_pws_broadcast_indication_received_then_it_replaces_the_previous_one)
{
  // Submit an initial request, then immediately supersede it with a second one before any slot is processed.
  // Since the pending request is only consumed on the next run_slot(), only the second (last-committed) request
  // should ever take effect -- verifying that a new request fully replaces any previous one, i.e. the SI-message's
  // active window is sized off the second request's nof_segments (1), not the first's (10).
  const units::bytes msg_len             = ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].msg_len;
  const unsigned     period_radio_frames = ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].period_radio_frames;
  const unsigned     default_paging_cycle_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);

  broadcast_warning(si_sched, ACTIVATION_REQUIRED_SI_SCHED_CFG, 1, {{sib_type_set{sib_type::sib7}, 10, msg_len}});
  broadcast_warning(si_sched, ACTIVATION_REQUIRED_SI_SCHED_CFG, 2, {{sib_type_set{sib_type::sib7}, 1, msg_len}});

  // Run long enough to cover even the FIRST (superseded) request's full active window (default paging cycle plus
  // one full segment cycle), so that if it had incorrectly taken effect instead of the second, its much larger
  // transmission count would be observed here.
  const unsigned first_active_duration_rfs = default_paging_cycle_rfs + 10 * period_radio_frames;
  const unsigned nof_test_slots =
      (first_active_duration_rfs + 2 * period_radio_frames) * next_slot.nof_slots_per_frame();

  unsigned nof_tx = 0;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.si_indicator == sib_information::other_si) {
        ++nof_tx;
      }
    }
  }

  const unsigned second_active_duration_rfs = default_paging_cycle_rfs + 1 * period_radio_frames;
  // +1 window of slack to account for the window's start not necessarily being frame-aligned to the activation
  // instant.
  const unsigned expected_max_nof_tx = second_active_duration_rfs / period_radio_frames + 1;
  ASSERT_LE(nof_tx, expected_max_nof_tx) << "Only the second (last-committed) request should take effect";
}

TEST_F(si_msg_scheduler_activation_test, when_message_is_not_activated_then_it_is_never_scheduled)
{
  const unsigned nof_test_slots =
      2 * ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].period_radio_frames * next_slot.nof_slots_per_frame();
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      ASSERT_NE(sib.si_indicator, sib_information::other_si)
          << "SI-message requiring activation must not be scheduled while dormant";
    }
  }
}

TEST_F(si_msg_scheduler_activation_test,
       when_activation_indication_received_then_it_stays_active_beyond_nof_segments_occasions)
{
  // Regression test: single-round PWS delivery must reach UEs that are only notified (via the P-RNTI short
  // message) near the end of the notification window, so the SI-message must stay active well beyond exactly
  // nof_segments occasions -- for a full default paging cycle plus one extra segment cycle (see
  // si_message_scheduler::activate_si_message).
  const unsigned nof_segments        = 3;
  const unsigned period_radio_frames = ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].period_radio_frames;
  const unsigned default_paging_cycle_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned active_duration_rfs = default_paging_cycle_rfs + nof_segments * period_radio_frames;

  broadcast_warning(
      si_sched,
      ACTIVATION_REQUIRED_SI_SCHED_CFG,
      1,
      {{sib_type_set{sib_type::sib7}, nof_segments, ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].msg_len}});

  // Run long enough to observe the full active window plus a margin, so we can confirm the message eventually
  // goes back to dormant instead of just capturing however many transmissions fit in an arbitrarily-sized window.
  const unsigned nof_test_slots = (active_duration_rfs + 2 * period_radio_frames) * next_slot.nof_slots_per_frame();
  unsigned       nof_tx         = 0;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.si_indicator == sib_information::other_si) {
        ++nof_tx;
      }
    }
  }

  // +1 window of slack to account for the window's start not necessarily being frame-aligned to the activation
  // instant.
  const unsigned expected_max_nof_tx = active_duration_rfs / period_radio_frames + 1;
  ASSERT_GT(nof_tx, nof_segments) << "Active window was not extended beyond the old exact-count behavior";
  ASSERT_LE(nof_tx, expected_max_nof_tx);
}

TEST_F(si_msg_scheduler_activation_test, when_si_change_takes_effect_then_ongoing_pws_broadcast_is_not_stopped)
{
  // Regression test: a warning is exempt from the SI change modification window and must keep being broadcast for
  // the duration it was activated for. An unrelated SI change (e.g. an SSB power update) reaching its modification
  // window used to reset every SI-message context, silently aborting the warning mid-flight.
  const units::bytes activated_msg_len = ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].msg_len + units::bytes{64U};
  // Activated indefinitely, so that anything that stops it can only be the SI change itself, rather than the
  // activation's own deadline elapsing before the modification window is reached.
  broadcast_warning(
      si_sched, ACTIVATION_REQUIRED_SI_SCHED_CFG, 1, {{sib_type_set{sib_type::sib7}, std::nullopt, activated_msg_len}});

  // An unrelated SI change is pushed and left to reach its modification window.
  si_scheduling_config new_si_sched_cfg = ACTIVATION_REQUIRED_SI_SCHED_CFG;
  new_si_sched_cfg.sib1_payload_size += units::bytes{8U};
  si_sched.handle_si_update_request(si_scheduling_update_request{to_du_cell_index(0), 1, new_si_sched_cfg});

  const unsigned si_ch_wind_len_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.bcch_cfg.mod_period_coeff) *
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned nof_test_slots = 3 * si_ch_wind_len_rfs * next_slot.nof_slots_per_frame();

  unsigned nof_tx_after_si_change = 0;
  bool     si_change_applied      = false;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.si_indicator == sib_information::sib1 and sib.version == 1) {
        si_change_applied = true;
      }
      if (si_change_applied and sib.si_indicator == sib_information::other_si) {
        ++nof_tx_after_si_change;
        // The activation-time content length must survive the SI change too.
        ASSERT_GE(sib.pdsch_cfg.codewords[0].tb_size_bytes, activated_msg_len);
      }
    }
  }

  ASSERT_TRUE(si_change_applied) << "The SI change never reached its modification window";
  ASSERT_GT(nof_tx_after_si_change, 0U) << "The on-going PWS broadcast was stopped by an unrelated SI change";
}

TEST_F(si_msg_scheduler_activation_test, when_activation_msg_len_exceeds_static_config_then_pdsch_grant_is_sized_for_it)
{
  // Regression test: real PWS content is only known at activation time and can be much larger than whatever
  // (placeholder/test-mode) content was configured/encoded for this SI-message at cell startup. The scheduler must
  // size the PDSCH grant off the activation-time msg_len, not the static si_message_scheduling_config::msg_len.
  const units::bytes static_msg_len    = ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].msg_len;
  const units::bytes activated_msg_len = static_msg_len + units::bytes{64U};
  broadcast_warning(
      si_sched, ACTIVATION_REQUIRED_SI_SCHED_CFG, 1, {{sib_type_set{sib_type::sib7}, 1, activated_msg_len}});

  const unsigned nof_test_slots =
      2 * ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].period_radio_frames * next_slot.nof_slots_per_frame();
  bool found = false;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.si_indicator == sib_information::other_si) {
        ASSERT_GE(sib.pdsch_cfg.codewords[0].tb_size_bytes, activated_msg_len);
        found = true;
      }
    }
  }
  ASSERT_TRUE(found) << "SI-message was not scheduled within the expected window";
}

// SI-message 1 is given an explicit si_window_position (rather than the natural per-index offset) so that its
// window lands on a slot where the (SSB-periodicity-driven) common search space is actually monitored, matching
// SI-message 0's occasion parity.
const si_scheduling_config MULTI_ACTIVATION_REQUIRED_SI_SCHED_CFG{
    DEFAULT_SIB1_PAYLOAD_SIZE,
    {si_message_scheduling_config{sib_type_set{sib_type::sib7}, units::bytes{64}, 16},
     si_message_scheduling_config{sib_type_set{sib_type::sib8}, units::bytes{64}, 16, 3}},
    {},
    10};

class si_msg_scheduler_multi_activation_test : public si_scheduler_test_environment, public testing::Test
{
protected:
  si_msg_scheduler_multi_activation_test() :
    si_scheduler_test_environment(make_sched_configuration_request(MULTI_ACTIVATION_REQUIRED_SI_SCHED_CFG))
  {
  }
};

TEST_F(si_msg_scheduler_multi_activation_test,
       when_two_distinct_si_messages_are_activated_back_to_back_then_both_get_scheduled)
{
  // Regression test: activating two distinct SI-message indices before any slot is processed must not let the
  // second activation clobber the first in a shared pending-request slot.
  const unsigned     nof_segments        = 3;
  const unsigned     period_radio_frames = MULTI_ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].period_radio_frames;
  const units::bytes msg_len             = MULTI_ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].msg_len;
  const unsigned     default_paging_cycle_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned active_duration_rfs = default_paging_cycle_rfs + nof_segments * period_radio_frames;

  broadcast_warning(
      si_sched,
      MULTI_ACTIVATION_REQUIRED_SI_SCHED_CFG,
      1,
      {{sib_type_set{sib_type::sib7}, nof_segments, msg_len}, {sib_type_set{sib_type::sib8}, nof_segments, msg_len}});

  // Run long enough to observe the full active window (default paging cycle plus one segment cycle) for both
  // SI-messages, not just however many transmissions fit in an arbitrarily-sized window.
  const unsigned nof_test_slots = (active_duration_rfs + 2 * period_radio_frames) * next_slot.nof_slots_per_frame();
  std::array<unsigned, 2> nof_tx{0, 0};
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.si_indicator == sib_information::other_si) {
        ASSERT_TRUE(sib.si_msg_index.has_value());
        ++nof_tx[sib.si_msg_index.value()];
      }
    }
  }

  // +1 window of slack to account for the window's start not necessarily being frame-aligned to the activation
  // instant.
  const unsigned expected_max_nof_tx = active_duration_rfs / period_radio_frames + 1;
  ASSERT_GT(nof_tx[0], nof_segments);
  ASSERT_LE(nof_tx[0], expected_max_nof_tx);
  ASSERT_GT(nof_tx[1], nof_segments);
  ASSERT_LE(nof_tx[1], expected_max_nof_tx);
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

TEST_F(si_msg_scheduler_activation_test, when_pws_epoch_is_applied_then_grants_use_it_until_the_warning_ends)
{
  // The ETWS/CMAS epoch is stamped on the grants for as long as a warning is on air, so that the SIB PDU assembler
  // serves the SIB1 that lists it as broadcasting. Once the warning ends, the grants go back to the normal operation
  // epoch on their own, without the MAC having to push anything.
  const unsigned     nof_segments        = 2;
  const unsigned     period_radio_frames = ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].period_radio_frames;
  const units::bytes msg_len             = ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].msg_len;

  const si_version_type baseline_version = 0;
  const si_version_type pws_version      = 1;

  broadcast_warning(
      si_sched, ACTIVATION_REQUIRED_SI_SCHED_CFG, pws_version, {{sib_type_set{sib_type::sib7}, nof_segments, msg_len}});

  const unsigned default_paging_cycle_rfs =
      static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned active_duration_rfs = default_paging_cycle_rfs + nof_segments * period_radio_frames;
  const unsigned nof_test_slots      = 2 * active_duration_rfs * next_slot.nof_slots_per_frame();

  unsigned nof_etws_grants     = 0;
  unsigned nof_baseline_grants = 0;
  bool     reverted            = false;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.version == pws_version) {
        ++nof_etws_grants;
        ASSERT_FALSE(reverted) << "The ETWS/CMAS epoch came back after the warning had ended";
      } else {
        ASSERT_EQ(sib.version, baseline_version);
        if (nof_etws_grants > 0) {
          reverted = true;
          ++nof_baseline_grants;
        }
      }
    }
  }

  ASSERT_GT(nof_etws_grants, 0U) << "No grant was stamped with the ETWS/CMAS epoch";
  ASSERT_TRUE(reverted) << "The grants never went back to the normal operation epoch";
  ASSERT_GT(nof_baseline_grants, 0U);
}

/// Whether the slot carries the etwsAndCmasIndication short message, as per TS 38.331 Table 6.5-1.
static bool has_pws_short_message(const dl_sched_result& result)
{
  return std::any_of(result.dl_pdcchs.begin(), result.dl_pdcchs.end(), [](const pdcch_dl_information& pdcch) {
    if (pdcch.dci.type() != dci_dl_rnti_config_type::p_rnti_f1_0) {
      return false;
    }
    const auto& dci = pdcch.dci.as_p_rnti_f1_0();
    return dci.short_messages_indicator == dci_1_0_p_rnti_configuration::payload_info::short_messages and
           (dci.short_messages & 0x40U) != 0;
  });
}

TEST_F(si_msg_scheduler_activation_test, when_two_epochs_coalesce_then_the_warning_is_still_notified)
{
  // Regression test: an unrelated System Information change pushes a second epoch before the scheduler read the one
  // that started the warning, so only the latter survives in the pending epoch slot. Since each warning carries the
  // version that started its broadcast, the scheduler must still notify it, rather than wait for a trigger that is
  // never pushed again.
  const units::bytes    msg_len           = ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].msg_len;
  const si_version_type warning_version   = 1;
  const si_version_type si_change_version = 2;

  broadcast_warning(
      si_sched, ACTIVATION_REQUIRED_SI_SCHED_CFG, warning_version, {{sib_type_set{sib_type::sib7}, 1, msg_len}});

  // The System Information change lists the warning it found on air, still stamped with the epoch that started it.
  pws_si_scheduling_update_request si_change{
      to_du_cell_index(0), si_change_version, ACTIVATION_REQUIRED_SI_SCHED_CFG, {}};
  si_change.broadcasting.push_back(
      pws_broadcasting_si_message{sib_type_set{sib_type::sib7}, 1, msg_len, warning_version});
  si_sched.handle_pws_si_update_request(si_change);

  const unsigned drx_cycle_rfs  = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned nof_test_slots = drx_cycle_rfs * next_slot.nof_slots_per_frame();

  bool notified = false;
  for (unsigned i = 0; i != nof_test_slots and not notified; ++i) {
    run_slot();
    notified = has_pws_short_message(res_grid[0].result.dl);
  }

  ASSERT_TRUE(notified) << "The warning was never notified after both epochs coalesced";
}

TEST_F(si_msg_scheduler_multi_activation_test, when_a_second_warning_starts_then_it_does_not_extend_the_first_one)
{
  // Each warning is timed from its own broadcast, so a warning starting late must not keep the ones already on air --
  // hence the SIB1 that lists them as broadcasting -- in effect for its own duration counted from the newcomer.
  const units::bytes msg_len      = MULTI_ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].msg_len;
  const unsigned     period_rfs   = MULTI_ACTIVATION_REQUIRED_SI_SCHED_CFG.si_messages[0].period_radio_frames;
  const unsigned paging_cycle_rfs = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle);
  const unsigned slots_per_frame  = next_slot.nof_slots_per_frame();

  static constexpr unsigned long_nof_segments  = 4;
  static constexpr unsigned short_nof_segments = 1;

  // A long warning starts on the SIB7 SI message. Run until shortly before its broadcast completes.
  broadcast_warning(si_sched,
                    MULTI_ACTIVATION_REQUIRED_SI_SCHED_CFG,
                    1,
                    {{sib_type_set{sib_type::sib7}, long_nof_segments, msg_len}});
  const unsigned first_duration_rfs = paging_cycle_rfs + long_nof_segments * period_rfs;
  for (unsigned i = 0, e = (first_duration_rfs - period_rfs / 2) * slots_per_frame; i != e; ++i) {
    run_slot();
  }

  // A short warning starts on the SIB8 SI message. Only it is stamped with the new epoch.
  pws_si_scheduling_update_request req{to_du_cell_index(0), 2, MULTI_ACTIVATION_REQUIRED_SI_SCHED_CFG, {}};
  req.broadcasting.push_back(pws_broadcasting_si_message{sib_type_set{sib_type::sib7}, long_nof_segments, msg_len, 1});
  req.broadcasting.push_back(pws_broadcasting_si_message{sib_type_set{sib_type::sib8}, short_nof_segments, msg_len, 2});
  si_sched.handle_pws_si_update_request(req);

  // Run past both broadcasts and note when the grants went back to the epoch of the normal operation. The epoch is
  // reverted once its last warning ends, which is the newcomer, so it stays in effect for the newcomer's own duration
  // counted from here, plus however long it takes for the next SIB1 to be scheduled.
  const unsigned        second_duration_rfs = paging_cycle_rfs + short_nof_segments * period_rfs;
  const si_version_type baseline_version    = 0;

  std::optional<unsigned> revert_rfs;
  for (unsigned i = 0, e = 2 * (first_duration_rfs + second_duration_rfs) * slots_per_frame; i != e; ++i) {
    run_slot();
    for (const auto& sib : res_grid[0].result.dl.bc.sibs) {
      if (sib.version == baseline_version and not revert_rfs.has_value()) {
        revert_rfs = i / slots_per_frame;
      }
    }
  }
  ASSERT_TRUE(revert_rfs.has_value()) << "The ETWS/CMAS SI epoch was never reverted";

  // Timing the first warning from the newcomer's broadcast, rather than from its own, would hold the epoch in effect
  // for the whole duration of the first warning counted from here.
  ASSERT_LT(revert_rfs.value(), first_duration_rfs) << "The first warning was kept on air by the second one starting";
}
