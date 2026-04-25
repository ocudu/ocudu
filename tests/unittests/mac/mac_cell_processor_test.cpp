// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_dl/mac_cell_processor.h"
#include "mac_test_helpers.h"
#include "ocudu/mac/mac_cell_timing_context.h"
#include "ocudu/mac/phy_cell_operation_controller.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/async/eager_async_task.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;

class base_mac_cell_processor_test
{
public:
  base_mac_cell_processor_test() :
    mac_cell(test_helpers::make_default_mac_cell_config(builder_params),
             sched_adapter,
             sfn_time_adapter,
             rnti_table,
             phy_notifier,
             task_worker,
             task_worker,
             task_worker,
             pcap,
             timers,
             {timer_ctrl.add_cell(to_du_cell_index(0))})
  {
    next_slot = {builder_params.scs_common, 0};
  }

  bool is_pdsch_scheduled() const
  {
    const auto& dl_res = *phy_notifier.last_sched_res->dl_res;
    return not dl_res.rar_grants.empty() or not dl_res.bc.sibs.empty() or not dl_res.ue_grants.empty() or
           not dl_res.paging_grants.empty();
  }

  test_helpers::dummy_mac_scheduler_adapter    sched_adapter;
  test_helpers::dummy_mac_sfn_time_adapter     sfn_time_adapter;
  du_rnti_table                                rnti_table;
  test_helpers::dummy_mac_cell_result_notifier phy_notifier;
  manual_task_worker                           task_worker{128};
  null_mac_pcap                                pcap;
  timer_manager                                timers;
  test_helpers::dummy_mac_clock_controller     timer_ctrl{timers};
  cell_config_builder_params                   builder_params;
  mac_cell_processor                           mac_cell;

  slot_point_extended next_slot;
};

struct test_params {
  unsigned nof_sib_allocated;
  unsigned nof_rar_allocated;
  unsigned nof_ue_allocated;
};

class mac_cell_processor_tester : public base_mac_cell_processor_test, public ::testing::TestWithParam<test_params>
{
protected:
  mac_cell_processor_tester()
  {
    auto params = GetParam();

    // Creates the next sched result based on test parameters.
    sched_adapter.next_sched_result.success           = true;
    sched_adapter.next_sched_result.dl.nof_dl_symbols = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
    sched_adapter.next_sched_result.dl.bc.sibs.resize(params.nof_sib_allocated);
    for (auto& sib_grant : sched_adapter.next_sched_result.dl.bc.sibs) {
      sib_grant.version        = 0;
      sib_grant.pdsch_cfg.rnti = rnti_t::SI_RNTI;
      sib_grant.pdsch_cfg.codewords.resize(1);
      sib_grant.pdsch_cfg.codewords[0].tb_size_bytes = units::bytes{128};
    }
    sched_adapter.next_sched_result.dl.rar_grants.resize(params.nof_rar_allocated);
    for (auto& rar_grant : sched_adapter.next_sched_result.dl.rar_grants) {
      rar_grant.pdsch_cfg.rnti = to_rnti(0x1);
      rar_grant.pdsch_cfg.codewords.resize(1);
      rar_grant.pdsch_cfg.codewords[0].tb_size_bytes = units::bytes{128};
      rar_grant.grants.resize(1);
    }
    sched_adapter.next_sched_result.dl.ue_grants.resize(params.nof_ue_allocated);
    for (auto& ue_grant : sched_adapter.next_sched_result.dl.ue_grants) {
      ue_grant.pdsch_cfg.rnti = to_rnti(0x4601);
      ue_grant.pdsch_cfg.codewords.resize(1);
      ue_grant.pdsch_cfg.codewords[0].tb_size_bytes = units::bytes{128};
    }

    // Start cell.
    auto                     t = mac_cell.start();
    lazy_task_launcher<void> launcher{t};
    this->mac_cell.handle_slot_indication(mac_cell_timing_context{next_slot++});
    task_worker.run_pending_tasks();
    report_error_if_not(launcher.ready(), "Unable to start cell");
    phy_notifier = {};
  }
};

TEST_P(mac_cell_processor_tester, when_cell_is_active_then_slot_indication_triggers_scheduling)
{
  // Scheduler was notified of cell start.
  ASSERT_TRUE(sched_adapter.active);

  auto slot_tp = std::chrono::system_clock::now();
  ASSERT_FALSE(phy_notifier.is_complete);
  mac_cell.handle_slot_indication({next_slot++, slot_tp});

  ASSERT_TRUE(phy_notifier.last_sched_res.has_value());
  ASSERT_EQ(phy_notifier.last_dl_data_res.has_value(), is_pdsch_scheduled());
  ASSERT_TRUE(phy_notifier.is_complete);

  auto test_params = GetParam();
  ASSERT_EQ(phy_notifier.last_sched_res->dl_res->bc.sibs.size(), test_params.nof_sib_allocated);
  ASSERT_EQ(phy_notifier.last_sched_res->dl_res->rar_grants.size(), test_params.nof_rar_allocated);
  ASSERT_EQ(phy_notifier.last_sched_res->dl_res->ue_grants.size(), test_params.nof_ue_allocated);
}

TEST_P(mac_cell_processor_tester, when_cell_is_deactivated_then_scheduler_gets_notified)
{
  auto slot_tp = std::chrono::system_clock::now();

  ASSERT_FALSE(phy_notifier.is_complete);

  async_task<void>         t = mac_cell.stop();
  lazy_task_launcher<void> launcher{t};
  this->task_worker.run_pending_tasks();
  ASSERT_TRUE(t.ready());

  // Scheduler was notified of cell stop.
  ASSERT_FALSE(sched_adapter.active);

  // Cell continues to process slot indications.
  phy_notifier.is_complete = false;
  phy_notifier.last_sched_res.reset();
  phy_notifier.last_ul_res.reset();
  phy_notifier.last_dl_data_res.reset();
  mac_cell.handle_slot_indication({next_slot++, slot_tp});
  ASSERT_TRUE(phy_notifier.is_complete);
}

INSTANTIATE_TEST_SUITE_P(
    nof_pdschs_allocated,
    mac_cell_processor_tester,
    testing::Values(test_params{0, 0, 0}, test_params{1, 0, 0}, test_params{0, 1, 0}, test_params{0, 0, 1}));

class mac_cell_processor_ue_update_test : public base_mac_cell_processor_test, public ::testing::Test
{
protected:
  mac_cell_processor_ue_update_test()
  {
    // Creates the next sched result based on test parameters.
    sched_adapter.next_sched_result.success = true;

    // Start cell.
    auto                     t = mac_cell.start();
    lazy_task_launcher<void> launcher{t};
    this->mac_cell.handle_slot_indication(mac_cell_timing_context{next_slot++});
    task_worker.run_pending_tasks();
    report_error_if_not(launcher.ready(), "Unable to start cell");
  }

  mac_ue_create_request make_ue_create_request(du_ue_index_t ue_index)
  {
    mac_ue_create_request request;
    request.cell_index   = to_du_cell_index(0);
    request.ue_index     = ue_index;
    request.crnti        = to_rnti(0x4601 + static_cast<unsigned>(ue_index));
    request.rlf_notifier = &rlf_notifier;
    request.bearers.resize(3);
    for (unsigned i = 0; i < request.bearers.size(); ++i) {
      request.bearers[i].lcid = uint_to_lcid(i);
    }
    request.ul_ccch_msg = &msg3_buffer;
    return request;
  }

  mac_ue_delete_request make_ue_delete_request(du_ue_index_t ue_index)
  {
    mac_ue_delete_request request;
    request.cell_index = to_du_cell_index(0);
    request.ue_index   = ue_index;
    request.rnti       = to_rnti(0x4601 + static_cast<unsigned>(ue_index));
    return request;
  }

  test_helpers::dummy_ue_rlf_notifier rlf_notifier;
  byte_buffer                         msg3_buffer = byte_buffer::create({0x1, 0x2, 0x3, 0x4, 0x5, 0x6}).value();
};

TEST_F(mac_cell_processor_ue_update_test, ues_created_and_removed_in_batches)
{
  unsigned nof_batches       = 1000;
  unsigned nof_ues_per_batch = 32;
  unsigned nof_ues_constant  = 10;

  for (unsigned uidx = 0; uidx != nof_ues_constant; ++uidx) {
    // Create a new UE.
    du_ue_index_t            ue_index = to_du_ue_index(uidx);
    auto                     t        = mac_cell.add_ue(make_ue_create_request(ue_index));
    lazy_task_launcher<bool> launcher{t};
    task_worker.run_pending_tasks();
    report_error_if_not(launcher.ready(), "Unable to add UE");
    ASSERT_TRUE(launcher.result.value());
  }

  for (unsigned batch = 0; batch != nof_batches; ++batch) {
    // Create a new batch of UEs.
    for (unsigned uidx = 0; uidx != nof_ues_per_batch; ++uidx) {
      // Create a new UE.
      du_ue_index_t            ue_index = to_du_ue_index(nof_ues_constant + uidx);
      mac_ue_create_request    request  = make_ue_create_request(ue_index);
      auto                     t        = mac_cell.add_ue(request);
      lazy_task_launcher<bool> launcher{t};
      task_worker.run_pending_tasks();
      report_error_if_not(launcher.ready(), "Unable to add UE");
      ASSERT_TRUE(launcher.result.value());
    }

    // Remove batch of UEs.
    for (unsigned uidx = 0; uidx != nof_ues_per_batch; ++uidx) {
      // Check that the UE was created.
      du_ue_index_t            ue_index = to_du_ue_index(nof_ues_constant + uidx);
      auto                     t        = mac_cell.remove_ue(make_ue_delete_request(ue_index));
      lazy_task_launcher<void> launcher{t};
      task_worker.run_pending_tasks();
      report_error_if_not(launcher.ready(), "Unable to add UE");
    }
  }
}

/// FAPI cell lifecycle init bypass.
///
/// In monolithic OCUDU, mac_cell_processor::start() is invoked inside DU.start() during initial
/// cell bring-up — at a moment when the FAPI executors that would respond to a START.request are
/// not yet pumping their queues. Awaiting the FAPI START transaction would deadlock for the full
/// 5-second timeout. To avoid this, the very first activation per cell skips the FAPI await; all
/// subsequent activations (runtime cell unlock, runtime add) go through the full FAPI path so the
/// spec-correct lifecycle is honored everywhere it is operationally meaningful.
///
/// These tests pin that contract: the controller is NOT invoked on the first start(), but IS
/// invoked on subsequent starts and on every stop().

namespace {

/// Spy that records every invocation of start()/stop() and returns success.
class phy_cell_operation_controller_spy : public phy_cell_operation_controller
{
  unsigned start_count = 0;
  unsigned stop_count  = 0;

public:
  async_task<bool> start() override
  {
    ++start_count;
    return launch_async([](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  async_task<bool> stop() override
  {
    ++stop_count;
    return launch_async([](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  unsigned get_start_count() const { return start_count; }
  unsigned get_stop_count() const { return stop_count; }
};

} // namespace

class mac_cell_processor_init_bypass_fixture : public ::testing::Test
{
protected:
  test_helpers::dummy_mac_scheduler_adapter    sched_adapter;
  test_helpers::dummy_mac_sfn_time_adapter     sfn_time_adapter;
  du_rnti_table                                rnti_table;
  test_helpers::dummy_mac_cell_result_notifier phy_notifier;
  manual_task_worker                           task_worker{128};
  null_mac_pcap                                pcap;
  timer_manager                                timers;
  test_helpers::dummy_mac_clock_controller     timer_ctrl{timers};
  cell_config_builder_params                   builder_params;
  phy_cell_operation_controller_spy            phy_spy;

  std::unique_ptr<mac_cell_processor> mac_cell;

  mac_cell_processor_init_bypass_fixture()
  {
    mac_cell_config_dependencies deps{timer_ctrl.add_cell(to_du_cell_index(0))};
    deps.phy_cell_op_controller = &phy_spy;
    mac_cell = std::make_unique<mac_cell_processor>(test_helpers::make_default_mac_cell_config(builder_params),
                                                    sched_adapter,
                                                    sfn_time_adapter,
                                                    rnti_table,
                                                    phy_notifier,
                                                    task_worker,
                                                    task_worker,
                                                    task_worker,
                                                    pcap,
                                                    timers,
                                                    std::move(deps));
  }

  void run_async_void(async_task<void>& t)
  {
    lazy_task_launcher<void> launcher{t};
    task_worker.run_pending_tasks();
    ASSERT_TRUE(t.ready());
  }
};

TEST_F(mac_cell_processor_init_bypass_fixture, first_start_does_not_invoke_phy_controller)
{
  ASSERT_EQ(phy_spy.get_start_count(), 0u);

  auto t = mac_cell->start();
  run_async_void(t);

  // Init bypass: first activation must NOT call the FAPI controller — it would
  // deadlock during DU.start() because executors are not yet pumping.
  EXPECT_EQ(phy_spy.get_start_count(), 0u);
}

TEST_F(mac_cell_processor_init_bypass_fixture, second_start_after_stop_invokes_phy_controller)
{
  // First (init) activation — bypassed.
  auto t1 = mac_cell->start();
  run_async_void(t1);
  ASSERT_EQ(phy_spy.get_start_count(), 0u);

  // Stop — always uses the FAPI path.
  auto t2 = mac_cell->stop();
  run_async_void(t2);
  ASSERT_EQ(phy_spy.get_stop_count(), 1u);

  // Subsequent start (runtime unlock equivalent) — full FAPI START path.
  auto t3 = mac_cell->start();
  run_async_void(t3);
  EXPECT_EQ(phy_spy.get_start_count(), 1u);
}

TEST_F(mac_cell_processor_init_bypass_fixture, stop_always_invokes_phy_controller)
{
  auto t1 = mac_cell->start();
  run_async_void(t1);

  auto t2 = mac_cell->stop();
  run_async_void(t2);
  EXPECT_EQ(phy_spy.get_stop_count(), 1u);

  // Re-start, re-stop — stop is always on the FAPI path regardless of init state.
  auto t3 = mac_cell->start();
  run_async_void(t3);
  auto t4 = mac_cell->stop();
  run_async_void(t4);
  EXPECT_EQ(phy_spy.get_stop_count(), 2u);
}

TEST_F(mac_cell_processor_init_bypass_fixture, repeated_runtime_cycles_use_full_fapi_path)
{
  // Burn through the init bypass.
  auto t0 = mac_cell->start();
  run_async_void(t0);

  // Repeated runtime cycles — every start and stop hits the controller.
  for (unsigned i = 1; i <= 3; ++i) {
    auto ts = mac_cell->stop();
    run_async_void(ts);
    auto tr = mac_cell->start();
    run_async_void(tr);
    EXPECT_EQ(phy_spy.get_start_count(), i);
    EXPECT_EQ(phy_spy.get_stop_count(), i);
  }
}
