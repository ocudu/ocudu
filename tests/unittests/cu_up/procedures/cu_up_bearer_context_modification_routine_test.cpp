// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Unit tests for the Bearer Context Modification procedure.

#include "lib/cu_up/routines/cu_up_bearer_context_modification_routine.cpp"
#include "tests/unittests/cu_up/cu_up_test_helpers.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocuup;

class cu_up_bearer_context_modification_procedure_tester : public ::testing::Test
{
public:
  cu_up_bearer_context_modification_procedure_tester() :
    e1ap(cu_up_e1_index_t{0}), task_sched(2048), timers(timer_mng, exec), f1u_gw(f1u_bearer)
  {
  }

protected:
  std::unique_ptr<ue_context> create_ue()
  {
    std::unique_ptr<ue_executor_mapper> exec_mapper = std::make_unique<dummy_ue_executor_mapper>(exec);
    return std::make_unique<ue_context>(ue_index,
                                        ue_cfg,
                                        ngu_cfg,
                                        cu_up_test_mode_config{},
                                        ue_context_dependencies{e1ap,
                                                                std::move(exec_mapper),
                                                                task_sched,
                                                                timers,
                                                                timers,
                                                                timers,
                                                                f1u_gw,
                                                                ngu_session_mngr,
                                                                pdcp_if,
                                                                ngu_allocator,
                                                                f1u_allocator,
                                                                gtpu_demux,
                                                                pcap});
  }

  void start_procedure(ue_context& ue_ctx, const e1ap_bearer_context_modification_request& msg)
  {
    proc = launch_async<cu_up_bearer_context_modification_routine>(ue_ctx, msg);
    proc_launcher.emplace(proc);
  }

  void get_procedure_result(e1ap_bearer_context_modification_response& resp)
  {
    ASSERT_TRUE(proc.ready());
    std::optional<e1ap_bearer_context_modification_response> res = proc_launcher->result;
    ASSERT_TRUE(res.has_value());
    resp = *res;
  }

  cu_up_ue_index_t ue_index{1};

private:
  async_task<e1ap_bearer_context_modification_response>                        proc;
  std::optional<lazy_task_launcher<e1ap_bearer_context_modification_response>> proc_launcher;

  inline_task_executor      exec;
  dummy_e1ap                e1ap;
  fifo_async_task_scheduler task_sched;
  timer_manager             timer_mng;
  timer_factory             timers;

  // UE context dependencies.
  dummy_inner_f1u_bearer             f1u_bearer;
  dummy_f1u_gateway                  f1u_gw;
  dummy_ngu_session_manager          ngu_session_mngr;
  dummy_cu_up_manager_pdcp_interface pdcp_if;
  dummy_gtpu_teid_pool               ngu_allocator;
  dummy_gtpu_teid_pool               f1u_allocator;
  dummy_gtpu_demux_ctrl              gtpu_demux;
  null_dlt_pcap                      pcap;

  // NG-U Configs.
  ngu_interface_config ngu_cfg{GTPU_PORT,
                               std::chrono::milliseconds{0},
                               std::chrono::milliseconds{200},
                               GTPU_DEFAULT_TEID_RELEASE_LINGER_TIME,
                               true,
                               2048,
                               1,
                               false};

  // UE context cfg.
  ue_context_cfg ue_cfg{{}, activity_notification_level_t::ue, std::chrono::seconds{30}, {}, 1000000000, {}};
};

TEST_F(cu_up_bearer_context_modification_procedure_tester, when_suspend_received_bearer_context_is_suspended)
{
  // Create UE context.
  std::unique_ptr<ue_context> ue_ctx = create_ue();

  // Prepare Bearer Context Modification Request message.
  // This will suspend the bearer context.
  e1ap_bearer_context_modification_request msg_suspend = {};
  msg_suspend.ue_index                                 = ue_index;
  msg_suspend.bearer_context_status_change             = e1ap_bearer_context_status_change::suspend;

  // Start Procedure.
  start_procedure(*ue_ctx, msg_suspend);

  // Assert context is supended.
  ASSERT_TRUE(ue_ctx->is_suspended());

  // Assert procedure successed.
  e1ap_bearer_context_modification_response resp1;
  get_procedure_result(resp1);
  ASSERT_TRUE(resp1.success);

  // Prepare Bearer Context Modification Request message.
  // This will resume the bearer context.
  e1ap_bearer_context_modification_request msg = {};
  msg.ue_index                                 = ue_index;
  msg.bearer_context_status_change             = e1ap_bearer_context_status_change::resume;

  // Start Procedure.
  start_procedure(*ue_ctx, msg);

  // Assert context is supended.
  ASSERT_FALSE(ue_ctx->is_suspended());

  // Assert procedure successed.
  e1ap_bearer_context_modification_response resp2;
  get_procedure_result(resp2);
  ASSERT_TRUE(resp2.success);
}

TEST_F(cu_up_bearer_context_modification_procedure_tester, when_duplicate_suspend_received_procedure_fails)
{
  // Create UE context.
  std::unique_ptr<ue_context> ue_ctx = create_ue();

  // Prepare Bearer Context Modification Request message.
  // This will suspend the bearer context.
  e1ap_bearer_context_modification_request msg_suspend1 = {};
  msg_suspend1.ue_index                                 = ue_index;
  msg_suspend1.bearer_context_status_change             = e1ap_bearer_context_status_change::suspend;

  // Start Procedure.
  start_procedure(*ue_ctx, msg_suspend1);

  // Assert context is supended.
  ASSERT_TRUE(ue_ctx->is_suspended());

  // Assert procedure successed.
  e1ap_bearer_context_modification_response resp1;
  get_procedure_result(resp1);
  ASSERT_TRUE(resp1.success);

  // Prepare Bearer Context Modification Request message.
  // This will suspend the bearer context again.
  e1ap_bearer_context_modification_request msg_suspend2 = {};
  msg_suspend2.ue_index                                 = ue_index;
  msg_suspend2.bearer_context_status_change             = e1ap_bearer_context_status_change::suspend;

  // Start Procedure.
  start_procedure(*ue_ctx, msg_suspend2);

  // Assert context is supended.
  ASSERT_TRUE(ue_ctx->is_suspended());

  // Assert procedure successed.
  e1ap_bearer_context_modification_response resp2;
  get_procedure_result(resp2);
  ASSERT_FALSE(resp2.success);
}

TEST_F(cu_up_bearer_context_modification_procedure_tester, when_duplicate_resume_received_procedure_fails)
{
  // Create UE context.
  std::unique_ptr<ue_context> ue_ctx = create_ue();

  // Prepare Bearer Context Modification Request message.
  // This will activate an already active the bearer context.
  e1ap_bearer_context_modification_request msg_resume = {};
  msg_resume.ue_index                                 = ue_index;
  msg_resume.bearer_context_status_change             = e1ap_bearer_context_status_change::resume;

  // Start Procedure.
  start_procedure(*ue_ctx, msg_resume);

  // Assert context is supended.
  ASSERT_FALSE(ue_ctx->is_suspended());

  // Assert procedure successed.
  e1ap_bearer_context_modification_response resp;
  get_procedure_result(resp);
  ASSERT_FALSE(resp.success);
}
