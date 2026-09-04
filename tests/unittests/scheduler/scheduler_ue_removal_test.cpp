// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief In this file, we test the correct behaviour of the scheduler when removing UEs.

#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/scheduler/resource_grid_util.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

const unsigned MAX_UCI_SLOT_DELAY = get_max_slot_ul_alloc_delay(0);

// Setup the log spy to intercept error and warning log entries when removing a UE.
ocudu::log_sink_spy& test_spy = []() -> ocudu::log_sink_spy& {
  if (!ocudulog::install_custom_sink(
          ocudu::log_sink_spy::name(),
          std::unique_ptr<ocudu::log_sink_spy>(new ocudu::log_sink_spy(ocudulog::get_default_log_formatter())))) {
    report_fatal_error("Unable to create logger spy");
  }
  auto* spy = static_cast<ocudu::log_sink_spy*>(ocudulog::find_sink(ocudu::log_sink_spy::name()));
  if (spy == nullptr) {
    report_fatal_error("Unable to create logger spy");
  }

  ocudulog::fetch_basic_logger("SCHED", *spy, true);
  return *spy;
}();

class sched_ue_removal_test : public scheduler_test_simulator, public ::testing::Test
{
protected:
  sched_ue_removal_test() { add_cell(sched_config_helper::make_default_sched_cell_configuration_request()); }

  void add_ue(du_ue_index_t ue_index, rnti_t rnti)
  {
    // Create a UE with a DRB active.
    auto ue_cfg = sched_config_helper::create_default_sched_ue_creation_request(cell_cfg(to_du_cell_index(0)).params,
                                                                                {test_lcid_drb});
    ue_cfg.ue_index = ue_index;
    ue_cfg.crnti    = rnti;
    scheduler_test_simulator::add_ue(ue_cfg, true);
  }

  bool is_rnti_scheduled(rnti_t rnti) const
  {
    if (ocudu::find_ue_dl_pdcch(rnti, *last_sched_result()) != nullptr) {
      return true;
    }
    if (ocudu::find_ue_ul_pdcch(rnti, *last_sched_result()) != nullptr) {
      return true;
    }
    if (find_ue_pdsch(rnti, *last_sched_result()) != nullptr) {
      return true;
    }
    if (find_ue_pucch(rnti, *last_sched_result()) != nullptr) {
      return true;
    }
    if (find_ue_pusch(rnti, *last_sched_result()) != nullptr) {
      return true;
    }
    return false;
  }

  static const lcid_t test_lcid_drb = LCID_MIN_DRB;
};

TEST_F(sched_ue_removal_test,
       when_ue_has_no_pending_txs_then_ue_removal_waits_for_all_pending_csi_and_sr_to_be_scheduled)
{
  // Create UE.
  du_ue_index_t ue_index = (du_ue_index_t)test_rng::uniform_int<unsigned>(0, MAX_DU_UE_INDEX);
  rnti_t        rnti     = to_rnti(test_rng::uniform_int<unsigned>(0x4601, to_underlying(rnti_t::MAX_CRNTI)));
  add_ue(ue_index, rnti);
  ASSERT_FALSE(notif.last_ue_index_deleted.has_value());

  // Schedule UE removal.
  rem_ue(ue_index);

  // All pending UCI for this UE should be scheduled within this window.
  for (unsigned i = 0; i != MAX_UCI_SLOT_DELAY; ++i) {
    run_slot();
    ASSERT_FALSE(notif.last_ue_index_deleted.has_value());
  }

  // UE is finally removed.
  run_slot();
  ASSERT_TRUE(notif.last_ue_index_deleted.has_value());
  ASSERT_TRUE(notif.last_ue_index_deleted == ue_index);
  ASSERT_FALSE(this->is_rnti_scheduled(rnti));

  // No more allocations for this UE are possible.
  const unsigned MAX_COUNT = 100;
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    run_slot();
    ASSERT_FALSE(this->is_rnti_scheduled(rnti));
  }
}

TEST_F(sched_ue_removal_test, when_ue_has_pending_harqs_then_scheduler_waits_for_harq_clear_before_deleting_ue)
{
  // Create UE.
  du_ue_index_t ue_index = (du_ue_index_t)test_rng::uniform_int<unsigned>(0, MAX_DU_UE_INDEX);
  rnti_t        rnti     = to_rnti(test_rng::uniform_int<unsigned>(0x4601, to_underlying(rnti_t::MAX_CRNTI)));
  add_ue(ue_index, rnti);

  // Push DL buffer status update for UE DRB.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{ue_index, test_lcid_drb, 10000000});

  // Wait for at least one DL HARQ to be allocated.
  const dl_msg_alloc* alloc      = nullptr;
  const unsigned      TX_TIMEOUT = 10;
  for (unsigned i = 0; i != TX_TIMEOUT; ++i) {
    this->run_slot();
    alloc = find_ue_pdsch(rnti, *last_sched_result());
    if (alloc != nullptr) {
      break;
    }
  }
  ASSERT_NE(alloc, nullptr);

  // Schedule UE removal.
  slot_point_extended rem_slot = next_slot;
  rem_ue(ue_index);

  // Wait for the right slot for ACK.
  const unsigned    ACK_TIMEOUT = 10;
  const pucch_info* pucch       = nullptr;
  for (unsigned count = 0; count != ACK_TIMEOUT; ++count) {
    this->run_slot();
    ASSERT_EQ(find_ue_pdsch(rnti, *last_sched_result()), nullptr)
        << "UE allocated despite having no SRB pending bytes and being marked for removal";

    pucch = find_ue_pucch(rnti, *last_sched_result());
    if (pucch != nullptr and pucch->uci_bits.harq_ack_nof_bits > 0) {
      break;
    }
  }
  ASSERT_NE(pucch, nullptr);
  ASSERT_FALSE(notif.last_ue_index_deleted.has_value());

  // HARQ-ACK(s) should empty the HARQ process.
  uci_indication uci;
  uci.cell_index = to_du_cell_index(0);
  uci.slot_rx    = last_result_slot();
  // Note: There can be more than one PUCCH for the same UE. We need to ACK all of them, otherwise the HARQ is not
  // emptied.
  for (const auto& pucch_alloc : last_sched_result()->ul.pucchs) {
    if (pucch_alloc.crnti == rnti) {
      uci.ucis.push_back(create_uci_pdu_with_harq_ack(ue_index, pucch_alloc));
    }
  }
  this->sched->handle_uci_indication(uci);

  // Wait for all pendign UCI PDUs to be sent to the PHY.
  const unsigned REM_TIMEOUT = MAX_UCI_SLOT_DELAY - (next_slot - rem_slot) + 1;
  for (unsigned i = 0; not notif.last_ue_index_deleted.has_value() and i != REM_TIMEOUT; ++i) {
    run_slot();
  }

  // The UE should be removed at this point.
  ASSERT_EQ(notif.last_ue_index_deleted, ue_index);
  ASSERT_FALSE(this->is_rnti_scheduled(rnti));

  // No more allocations for this RNTI are possible.
  const unsigned MAX_COUNT = 100;
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    run_slot();
    ASSERT_FALSE(this->is_rnti_scheduled(rnti));
  }
}

TEST_F(sched_ue_removal_test, when_ue_is_removed_then_any_pending_uci_does_not_cause_log_warnings)
{
  unsigned initial_nof_warnings = test_spy.get_warning_counter() + test_spy.get_error_counter();

  // Create UE.
  du_ue_index_t ue_index = (du_ue_index_t)test_rng::uniform_int<unsigned>(0, MAX_DU_UE_INDEX);
  rnti_t        rnti     = to_rnti(test_rng::uniform_int<unsigned>(0x4601, to_underlying(rnti_t::MAX_CRNTI)));
  add_ue(ue_index, rnti);
  ASSERT_FALSE(notif.last_ue_index_deleted.has_value());

  // Remove UE.
  rem_ue(ue_index);
  for (unsigned i = 0; i != MAX_UCI_SLOT_DELAY; ++i) {
    run_slot();
    ASSERT_FALSE(notif.last_ue_index_deleted.has_value());
  }
  run_slot();
  ASSERT_TRUE(notif.last_ue_index_deleted == ue_index);

  // CSI arrives to Scheduler for the removed UE.
  uci_indication uci;
  uci.cell_index = to_du_cell_index(0);
  uci.slot_rx    = last_result_slot();
  uci_indication::uci_pdu pdu{};
  pdu.crnti    = rnti;
  pdu.ue_index = ue_index;
  uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu f2{};
  f2.csi.emplace();
  f2.csi->first_tb_wideband_cqi = 5;
  pdu.pdu                       = f2;
  uci.ucis.push_back(pdu);
  this->sched->handle_uci_indication(uci);
  run_slot();

  // No log warnings should be generated.
  ASSERT_EQ(initial_nof_warnings, test_spy.get_warning_counter() + test_spy.get_error_counter());
}

TEST_F(sched_ue_removal_test, when_config_applied_event_is_processed_after_ue_removal_then_scheduler_does_not_crash)
{
  // Create UE with SRB0, SRB1 and a DRB configured.
  du_ue_index_t ue_index = (du_ue_index_t)test_rng::uniform_int<unsigned>(0, MAX_DU_UE_INDEX);
  rnti_t        rnti     = to_rnti(test_rng::uniform_int<unsigned>(0x4601, to_underlying(rnti_t::MAX_CRNTI)));
  add_ue(ue_index, rnti);

  // Queue a UE removal event. When processed, it deactivates the UE (clearing its registered logical channels,
  // including SRB1) and defers the actual removal from the UE repository to a later slot.
  sched->handle_ue_removal_request(ue_index);

  // Queue a "config applied" event for the *same* UE right behind the removal event, before either is drained by
  // run_slot(). This mirrors a UE reconfiguration-applied (or CRNTI CE / CFRA Msg3 ACK) notification that was
  // already in flight when the UE got torn down: it still references the UE's last (non-empty) dedicated config,
  // which no longer matches the just-cleared logical channel map.
  sched->handle_ue_config_applied(ue_index);

  // Processing "ue_rem" clears the logical channel map; processing "ue_cfg_applied" right after must not try to
  // re-register a now-unconfigured LCID (e.g. SRB1) with the inter-slice scheduler.
  ASSERT_NO_FATAL_FAILURE(run_slot());
}

TEST_F(sched_ue_removal_test,
       when_ue_is_being_removed_but_keeps_receiving_sr_indications_then_scheduler_ignores_indications)
{
  // Create UE.
  du_ue_index_t ue_index = (du_ue_index_t)test_rng::uniform_int<unsigned>(0, MAX_DU_UE_INDEX);
  rnti_t        rnti     = to_rnti(test_rng::uniform_int<unsigned>(0x4601, to_underlying(rnti_t::MAX_CRNTI)));
  add_ue(ue_index, rnti);

  // Push BSR update for UE.
  this->push_bsr(ul_bsr_indication_message{
      to_du_cell_index(0), ue_index, rnti, bsr_format::SHORT_BSR, ul_bsr_lcg_report_list{{uint_to_lcg_id(0), 100}}});

  // Wait for at least one UL HARQ to be allocated.
  const ul_sched_info* alloc      = nullptr;
  const unsigned       TX_TIMEOUT = 10;
  for (unsigned i = 0; i != TX_TIMEOUT; ++i) {
    this->run_slot();
    alloc = find_ue_pusch(rnti, *last_sched_result());
    if (alloc != nullptr) {
      break;
    }
  }
  ASSERT_NE(alloc, nullptr);

  // Schedule UE removal.
  rem_ue(ue_index);

  // Keep pushing SRs without ACKing HARQ.
  const unsigned UE_REM_TIMEOUT = 1000;
  for (unsigned count = 0; count != UE_REM_TIMEOUT and not notif.last_ue_index_deleted.has_value(); ++count) {
    this->run_slot();
    ASSERT_EQ(find_ue_pusch(rnti, *last_sched_result()), nullptr) << "UE UL allocated despite being marked for removal";

    const pucch_info* pucch = find_ue_pucch(rnti, *last_sched_result());
    if (pucch != nullptr and
        (pucch->format() == pucch_format::FORMAT_1 and pucch->uci_bits.sr_bits != sr_nof_bits::no_sr)) {
      // UCI indication sets SR indication.
      uci_indication uci;
      uci.cell_index = to_du_cell_index(0);
      uci.slot_rx    = last_result_slot();
      uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu f0;
      f0.sr_detected = true;
      f0.ul_sinr_dB  = 10;
      uci.ucis.push_back(uci_indication::uci_pdu{.ue_index = ue_index, .crnti = rnti, .pdu = f0});
      this->sched->handle_uci_indication(uci);
    }
  }

  ASSERT_TRUE(notif.last_ue_index_deleted.has_value()) << "UE has not been deleted";
  ASSERT_EQ(notif.last_ue_index_deleted, ue_index);
}
