// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief In this file, we verify the correct operation of the MAC scheduler, as a whole, at handling UCI indications.
/// The objective here is to mainly cover and verify the correct integration of the scheduler building blocks.

#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/scheduler_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

class sched_uci_indication_test : public ::testing::Test
{
protected:
  sched_uci_indication_test() : sched(create_scheduler(scheduler_config{sched_cfg, notif}))
  {
    add_cell();
    add_ue();

    next_slot = slot_point_extended{subcarrier_spacing::kHz15, 0, 0, 1};
  }

  void add_cell()
  {
    sched_cell_configuration_request_message cell_cfg_msg =
        sched_config_helper::make_default_sched_cell_configuration_request();
    cell_cfg = cfg_mng.add_cell(cell_cfg_msg);
    sched->handle_cell_configuration_request(cell_cfg_msg);
  }

  void add_ue()
  {
    sched_ue_creation_request_message ue_cfg_msg = sched_config_helper::create_default_sched_ue_creation_request(
        sched_config_helper::make_default_sched_cell_configuration_request().ran);
    ue_cfg_msg.crnti = ue_rnti;
    sched->handle_ue_creation_request(ue_cfg_msg);
  }

  void run_slot()
  {
    last_sched_res = &sched->slot_indication(next_slot, to_du_cell_index(0));
    ASSERT_TRUE(last_sched_res->success);
    test_scheduler_result_consistency(*cell_cfg, next_slot.without_hyper_sfn(), *last_sched_res);
    ++next_slot;
  }

  void notify_uci_ind_on_pucch(bool sr_ind, span<const bool> harq_bits)
  {
    uci_indication uci_ind{};
    uci_ind.cell_index = to_du_cell_index(0);
    uci_ind.slot_rx    = next_slot.without_hyper_sfn() - 1;
    uci_ind.ucis.resize(1);
    uci_ind.ucis[0].ue_index = to_du_ue_index(0);
    uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu pdu{.sr_detected = sr_ind};
    pdu.harqs.resize(harq_bits.size());
    for (unsigned i = 0; i != harq_bits.size(); ++i) {
      pdu.harqs[i] = harq_bits[i] ? mac_harq_ack_report_status::ack : mac_harq_ack_report_status::nack;
    }
    uci_ind.ucis[0].pdu = pdu;

    sched->handle_uci_indication(uci_ind);
  }

  void notify_uci_ind_on_pusch(span<const bool> harq_bits)
  {
    uci_indication uci_ind{};
    uci_ind.cell_index = to_du_cell_index(0);
    uci_ind.slot_rx    = next_slot.without_hyper_sfn() - 1;
    uci_ind.ucis.resize(1);
    uci_ind.ucis[0].ue_index = to_du_ue_index(0);
    uci_indication::uci_pdu::uci_pusch_pdu pdu{};
    pdu.harqs.resize(harq_bits.size());
    for (unsigned i = 0; i != harq_bits.size(); ++i) {
      pdu.harqs[i] = harq_bits[i] ? mac_harq_ack_report_status::ack : mac_harq_ack_report_status::nack;
    }
    uci_ind.ucis[0].pdu = pdu;

    sched->handle_uci_indication(uci_ind);
  }

  void notify_dl_buffer_status(lcid_t lcid, unsigned pending_bytes)
  {
    dl_buffer_state_indication_message msg{};
    msg.ue_index = ue_id;
    msg.bs       = pending_bytes;
    msg.lcid     = lcid;
    sched->handle_dl_buffer_state_indication(msg);
  }

  bool ue_pdsch_scheduled() const
  {
    return std::any_of(last_sched_res->dl.ue_grants.begin(),
                       last_sched_res->dl.ue_grants.end(),
                       [ue_rnti = ue_rnti](const auto& pdsch) { return pdsch.pdsch_cfg.rnti == ue_rnti; });
  }

  bool ue_pusch_scheduled() const
  {
    return std::any_of(last_sched_res->ul.puschs.begin(), last_sched_res->ul.puschs.end(), [](const auto& pusch) {
      return pusch.pusch_cfg.rnti == ue_rnti;
    });
  }

  bool ue_pucch_harq_ack_grant_scheduled() const
  {
    span<const pucch_info> pucchs = last_sched_res->ul.pucchs.unsorted();
    return std::any_of(pucchs.begin(), pucchs.end(), [](const pucch_info& pucch) {
      bool is_harq_ack = pucch.uci_bits.harq_ack_nof_bits > 0;
      return pucch.crnti == ue_rnti and is_harq_ack;
    });
  }

  static constexpr du_ue_index_t ue_id   = to_du_ue_index(0);
  static constexpr rnti_t        ue_rnti = to_rnti(0x4601);

  ocudulog::basic_logger&                 logger = ocudulog::fetch_basic_logger("SCHED", true);
  sched_cfg_dummy_notifier                notif;
  scheduler_ue_metrics_dummy_notifier     metric_notif;
  const scheduler_expert_config           sched_cfg = config_helpers::make_default_scheduler_expert_config();
  test_helpers::test_sched_config_manager cfg_mng{sched_cfg};
  const cell_configuration*               cell_cfg = nullptr;
  std::unique_ptr<mac_scheduler>          sched;

  slot_point_extended next_slot;
  const sched_result* last_sched_res = nullptr;
};

TEST_F(sched_uci_indication_test, no_retx_after_harq_ack)
{
  static constexpr unsigned MAX_COUNT = 16;
  notify_dl_buffer_status(LCID_SRB1, 100);

  bool pucch_found = false;
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    run_slot();
    if (ue_pucch_harq_ack_grant_scheduled()) {
      notify_uci_ind_on_pucch(false, std::array<bool, 1>{true});
      pucch_found = true;
      break;
    }
  }
  ASSERT_TRUE(pucch_found);

  // No more DL grants after the ACK.
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    run_slot();
    ASSERT_FALSE(ue_pdsch_scheduled());
    ASSERT_FALSE(ue_pucch_harq_ack_grant_scheduled());
  }
}

TEST_F(sched_uci_indication_test, pusch_scheduled_after_sr_indication)
{
  // Maximum delay between the SR indication being forwarded to the scheduler and the scheduler generating an UL grant.
  static constexpr unsigned MAX_UL_GRANT_DELAY = 8;
  notify_uci_ind_on_pucch(true, {});

  bool pusch_found = false;
  for (unsigned i = 0; i != MAX_UL_GRANT_DELAY; ++i) {
    run_slot();
    ASSERT_FALSE(ue_pdsch_scheduled());
    ASSERT_FALSE(ue_pucch_harq_ack_grant_scheduled());
    if (ue_pusch_scheduled()) {
      pusch_found = true;
      break;
    }
  }
  ASSERT_TRUE(pusch_found);
}

TEST_F(sched_uci_indication_test, uci_ind_on_pusch)
{
  static constexpr unsigned MAX_COUNT = 16;
  // SR request.
  notify_uci_ind_on_pucch(true, {});

  bool pusch_found = false;
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    run_slot();
    if (ue_pusch_scheduled()) {
      notify_uci_ind_on_pusch(std::array<bool, 1>{true});
      pusch_found = true;
      break;
    }
  }

  ASSERT_TRUE(pusch_found);

  // No more DL grants after the ACK.
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    run_slot();
    ASSERT_FALSE(ue_pdsch_scheduled());
    ASSERT_FALSE(ue_pucch_harq_ack_grant_scheduled());
  }
}
