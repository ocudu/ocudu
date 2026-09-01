// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/common_scheduling/ra_ue_repository.h"
#include "lib/scheduler/config/cell_configuration.h"
#include "lib/scheduler/config/sched_config_manager.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/indication_generators.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

class ra_ue_repository_test : public ::testing::Test
{
protected:
  ra_ue_repository_test() :
    sched_cfg(config_helpers::make_default_scheduler_expert_config()),
    cfg_mng(scheduler_config{sched_cfg, dummy_notif}),
    cell_cfg(*cfg_mng.add_cell(sched_config_helper::make_default_sched_cell_configuration_request(builder_params))),
    repo(cell_cfg, ocudulog::fetch_basic_logger("SCHED")),
    conres_timer_slots(
        static_cast<unsigned>(cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer.count() *
                              get_nof_slots_per_subframe(cell_cfg.scs_common()))),
    sl_tx(to_numerology_value(cell_cfg.scs_common()), 0)
  {
  }

  scheduler_expert_config    sched_cfg;
  sched_cfg_dummy_notifier   dummy_notif;
  cell_config_builder_params builder_params;
  sched_config_manager       cfg_mng;
  const cell_configuration&  cell_cfg;
  ra_ue_repository           repo;
  const unsigned             conres_timer_slots;
  slot_point                 sl_tx;
};

TEST_F(ra_ue_repository_test, add_creates_entry_findable_by_tc_rnti)
{
  const rnti_t tc_rnti = to_rnti(0x4601);

  ra_ue_context* ctx = repo.add(test_helper::create_preamble(0, tc_rnti), sl_tx, ssb_id_t{0});
  ASSERT_NE(ctx, nullptr);
  EXPECT_EQ(ctx->tc_rnti(), tc_rnti);
  EXPECT_EQ(ctx->prach_slot_rx, sl_tx);
  EXPECT_FALSE(ctx->harq_ent.empty());

  auto it = repo.find(tc_rnti);
  ASSERT_NE(it, repo.end());
  EXPECT_EQ(it->tc_rnti(), tc_rnti);

  EXPECT_EQ(repo.find(to_rnti(0x4602)), repo.end());
}

TEST_F(ra_ue_repository_test, erase_removes_resolved_entry)
{
  const rnti_t tc_rnti = to_rnti(0x4601);
  repo.add(test_helper::create_preamble(0, tc_rnti), sl_tx, ssb_id_t{0});
  ASSERT_NE(repo.find(tc_rnti), repo.end());

  repo.erase(tc_rnti);
  EXPECT_EQ(repo.find(tc_rnti), repo.end());
}

TEST_F(ra_ue_repository_test, slot_indication_erases_entry_after_conres_timeout)
{
  const rnti_t tc_rnti = to_rnti(0x4601);
  repo.add(test_helper::create_preamble(0, tc_rnti), sl_tx, ssb_id_t{0});

  // Must survive every slot_indication() call up to (not including) the ConRes timeout boundary.
  for (unsigned i = 0; i != conres_timer_slots; ++i) {
    repo.slot_indication(sl_tx + i);
    ASSERT_NE(repo.find(tc_rnti), repo.end()) << "entry erased too early, at offset=" << i;
  }

  // ra-ContentionResolutionTimer elapsed: erased on this slot_indication() call.
  repo.slot_indication(sl_tx + conres_timer_slots);
  EXPECT_EQ(repo.find(tc_rnti), repo.end());
}

TEST_F(ra_ue_repository_test, slot_indication_keeps_entry_alive_while_msg3_harq_awaits_ack)
{
  const rnti_t tc_rnti = to_rnti(0x4601);

  ra_ue_context* ctx = repo.add(test_helper::create_preamble(0, tc_rnti), sl_tx, ssb_id_t{0});
  ASSERT_NE(ctx, nullptr);

  std::optional<ul_harq_process_handle> h_ul = ctx->harq_ent.alloc_ul_harq(sl_tx, 4);
  ASSERT_TRUE(h_ul.has_value());

  // Not erased while its Msg3 HARQ still awaits ACK/CRC, even past the ConRes timeout.
  repo.slot_indication(sl_tx + conres_timer_slots);
  ASSERT_NE(repo.find(tc_rnti), repo.end());

  // HARQ resolves (ACKed): next slot_indication() call reclaims the entry.
  h_ul->ul_crc_info(true);
  repo.slot_indication(sl_tx + conres_timer_slots + 1);
  EXPECT_EQ(repo.find(tc_rnti), repo.end());
}

TEST_F(ra_ue_repository_test, add_msgb_pending_entry_is_harqless_and_pending_until_scheduled)
{
  const rnti_t tc_rnti = to_rnti(0x4601);

  // As soon as MsgA CRC=OK is known, an entry is created with no committed MsgB slot yet.
  ra_ue_context* ctx = repo.add_msgb_pending(test_helper::create_preamble(0, tc_rnti), sl_tx, ssb_id_t{0});
  ASSERT_NE(ctx, nullptr);
  EXPECT_TRUE(ctx->harq_ent.empty());
  EXPECT_FALSE(ctx->msgb_slot_tx.has_value());
  EXPECT_FALSE(ctx->msgb_ack_slot_tx.has_value());
  EXPECT_TRUE(ctx->is_msgb_success_rar_pending(sl_tx));
  EXPECT_TRUE(ctx->is_msgb_success_rar_pending(sl_tx + 100));

  // Records both the PDSCH slot and its later ACK slot. Gate releases once the (later) ACK slot passes.
  const slot_point msgb_slot_tx     = sl_tx + 3;
  const slot_point msgb_ack_slot_tx = msgb_slot_tx + 4;
  ASSERT_TRUE(repo.set_msgb_scheduled(tc_rnti, msgb_slot_tx, msgb_ack_slot_tx));
  ASSERT_TRUE(ctx->msgb_slot_tx.has_value());
  ASSERT_TRUE(ctx->msgb_ack_slot_tx.has_value());
  EXPECT_EQ(*ctx->msgb_slot_tx, msgb_slot_tx);
  EXPECT_EQ(*ctx->msgb_ack_slot_tx, msgb_ack_slot_tx);
  EXPECT_TRUE(ctx->is_msgb_success_rar_pending(msgb_slot_tx));
  EXPECT_TRUE(ctx->is_msgb_success_rar_pending(msgb_ack_slot_tx));
  EXPECT_FALSE(ctx->is_msgb_success_rar_pending(msgb_ack_slot_tx + 1));
}

TEST_F(ra_ue_repository_test, add_msgb_pending_entry_is_erased_after_conres_timeout_even_if_never_scheduled)
{
  const rnti_t tc_rnti = to_rnti(0x4601);

  repo.add_msgb_pending(test_helper::create_preamble(0, tc_rnti), sl_tx, ssb_id_t{0});
  ASSERT_NE(repo.find(tc_rnti), repo.end());

  // Reclaimed by the ConRes-timer sweep even if set_msgb_scheduled() never runs.
  repo.slot_indication(sl_tx + conres_timer_slots);
  EXPECT_EQ(repo.find(tc_rnti), repo.end());
}

TEST_F(ra_ue_repository_test, add_msgb_pending_then_scheduled_entry_is_erased_after_conres_timeout)
{
  const rnti_t     tc_rnti      = to_rnti(0x4601);
  const slot_point msgb_slot_tx = sl_tx + 3;

  repo.add_msgb_pending(test_helper::create_preamble(0, tc_rnti), sl_tx, ssb_id_t{0});
  ASSERT_TRUE(repo.set_msgb_scheduled(tc_rnti, msgb_slot_tx, msgb_slot_tx + 4));

  // A HARQ-less successRAR entry is swept the same way as a Msg3-tracking one, based purely on prach_slot_rx.
  repo.slot_indication(sl_tx + conres_timer_slots);
  EXPECT_EQ(repo.find(tc_rnti), repo.end());
}
} // namespace
