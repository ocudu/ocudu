// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_manager_impl.h"
#include "cu_up_manager_helpers.h"
#include "routines/cu_up_bearer_context_modification_routine.h"
#include "routines/cu_up_e1_connection_loss_routine.h"
#include "routines/cu_up_test_mode_routines.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/async/execute_on_blocking.h"

using namespace ocudu;
using namespace ocuup;

/// Helper functions
static ue_manager_config generate_ue_manager_config(uint32_t                      max_nof_ues,
                                                    const n3_interface_config&    n3_config,
                                                    const cu_up_test_mode_config& test_mode_config)
{
  return {max_nof_ues, n3_config, test_mode_config};
}

static ue_manager_dependencies generate_ue_manager_dependencies(const cu_up_manager_impl_dependencies& dependencies,
                                                                cu_up_manager_pdcp_interface& cu_up_mngr_pdcp_if,
                                                                ocudulog::basic_logger&       logger)
{
  return {{dependencies.e1aps},
          dependencies.timers,
          dependencies.f1u_gateway,
          dependencies.ngu_session_mngr,
          cu_up_mngr_pdcp_if,
          dependencies.ngu_demux,
          dependencies.n3_teid_allocator,
          dependencies.f1u_teid_allocator,
          dependencies.exec_mapper,
          dependencies.gtpu_pcap,
          logger};
}

cu_up_manager_impl::cu_up_manager_impl(const cu_up_manager_impl_config&       config,
                                       const cu_up_manager_impl_dependencies& dependencies) :
  cu_up_id(config.cu_up_id),
  cu_up_name(config.cu_up_name),
  plmns(config.plmns),
  stop_command(dependencies.stop_command),
  e1aps(dependencies.e1aps),
  qos(config.qos),
  n3_cfg(config.n3_cfg),
  test_mode_cfg(config.test_mode_cfg),
  ngu_demux(dependencies.ngu_demux),
  exec_mapper(dependencies.exec_mapper),
  timers(dependencies.timers),
  cu_up_task_scheduler(dependencies.cu_up_task_scheduler)
{
  /// Create UE manager.
  ue_mng = std::make_unique<ue_manager>(generate_ue_manager_config(config.max_nof_ues, n3_cfg, test_mode_cfg),
                                        generate_ue_manager_dependencies(dependencies, *this, logger));
}

async_task<void> cu_up_manager_impl::stop()
{
  return ue_mng->stop();
}

void cu_up_manager_impl::schedule_cu_up_async_task(async_task<void> task)
{
  cu_up_task_scheduler.schedule(std::move(task));
}

void cu_up_manager_impl::schedule_ue_async_task(cu_up_ue_index_t ue_index, async_task<void> task)
{
  ue_mng->schedule_ue_async_task(ue_index, std::move(task));
}

e1ap_bearer_context_setup_response
cu_up_manager_impl::handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg)
{
  e1ap_bearer_context_setup_response response = {};
  response.ue_index                           = INVALID_CU_UP_UE_INDEX;
  response.success                            = false;

  // 1. Create new UE context.
  ue_context_cfg ue_cfg = {};
  fill_sec_as_config(ue_cfg.security_info, msg.security_info);
  ue_cfg.activity_level                   = msg.activity_notif_level;
  ue_cfg.ue_inactivity_timeout            = msg.ue_inactivity_timer;
  ue_cfg.qos                              = qos;
  ue_cfg.ue_dl_aggregate_maximum_bit_rate = msg.ue_dl_aggregate_maximum_bit_rate;
  ue_context* ue_ctxt                     = ue_mng->add_ue(msg.e1_index, ue_cfg);
  if (!ue_ctxt) {
    logger.error("Could not create UE context");
    return response;
  }
  ue_ctxt->get_logger().log_info("UE created");

  // 2. Handle bearer context setup request.
  for (const auto& pdu_session : msg.pdu_session_res_to_setup_list) {
    if (pdu_session_setup_result result = ue_ctxt->setup_pdu_session(pdu_session); result.success) {
      process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, result);
    } else {
      e1ap_pdu_session_resource_failed_item res_failed_item;

      res_failed_item.pdu_session_id = result.pdu_session_id;
      res_failed_item.cause          = result.cause;

      response.pdu_session_resource_failed_list.emplace(result.pdu_session_id, res_failed_item);
    }
  }

  // 3. Create response.
  response.ue_index = ue_ctxt->get_index();
  response.success  = true;
  return response;
}

async_task<e1ap_bearer_context_modification_response>
cu_up_manager_impl::handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("Could not find UE context");
    return launch_async([](coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx) {
      CORO_BEGIN(ctx);
      e1ap_bearer_context_modification_response res;
      res.success = false;
      CORO_RETURN(res);
    });
  }
  return execute_and_continue_on_blocking(ue_ctxt->ue_exec_mapper->ctrl_executor(),
                                          exec_mapper.ctrl_executor(),
                                          timers,
                                          launch_async<cu_up_bearer_context_modification_routine>(*ue_ctxt, msg));
}

async_task<void>
cu_up_manager_impl::handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (!ue_ctxt) {
    logger.error("ue={}: Discarding E1 Bearer Context Release Command. UE context not found",
                 fmt::underlying(msg.ue_index));
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  ue_ctxt->get_logger().log_debug("Received E1 Bearer Context Release Command");

  // Skip if UE is already flagged for removal; flag it for removal otherwise.
  if (ue_ctxt->remove_pending()) {
    logger.info("ue={}: Skipped scheduling UE removal, UE removal is already pending.", fmt::underlying(msg.ue_index));
    return launch_no_op_task();
  }
  ue_ctxt->request_removal();

  return ue_mng->remove_ue(msg.ue_index);
}

void cu_up_manager_impl::handle_e1ap_connection_drop(cu_up_e1_index_t e1_index)
{
  if (to_underlying(e1_index) >= e1aps.size()) {
    logger.error("e1={}: Could not handle E1 connection drop from unknown E1", fmt::underlying(e1_index));
    return;
  }
  std::reference_wrapper<e1ap_interface> e1ap = e1aps[to_underlying(e1_index)];
  schedule_cu_up_async_task(launch_async<cu_up_e1_connection_loss_routine>(
      cu_up_e1_connection_loss_routine_config{.cu_up_id = cu_up_id, .cu_up_name = cu_up_name, .plmns = plmns},
      cu_up_e1_connection_loss_routine_dependencies{.stop_command = stop_command,
                                                    .e1ap         = e1ap,
                                                    .ue_mng       = *ue_mng,
                                                    .timers       = timers,
                                                    .ctrl_exec    = exec_mapper.ctrl_executor(),
                                                    .logger       = logger}));
}

async_task<void> cu_up_manager_impl::handle_e1_reset(const e1ap_reset& msg)
{
  // Full E1 reset, release all bearer contexts.
  if (msg.type == e1ap_reset::full) {
    return ue_mng->remove_all_ues();
  }

  // Partial E1 reset, release the indicated bearer contexts.
  if (msg.ues.empty()) {
    logger.error("Received partial E1 reset, but no UEs to release");
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  return ue_mng->remove_ues(msg.ues);
}

//
// PDCP control events handling.
//

void cu_up_manager_impl::handle_pdcp_protocol_failure(cu_up_ue_index_t ue_index)
{
  ue_context* ue_ctxt = ue_mng->find_ue(ue_index);
  if (!ue_ctxt) {
    logger.error("ue={}: Could not handle PDCP protocol failure. UE context not found", ue_index);
    return;
  }
  cu_up_e1_index_t e1_index = ue_ctxt->get_e1_index();
  if (to_underlying(e1_index) >= e1aps.size()) {
    logger.error("e1={} ue={}: Could not handle PDCP protocol failure from unknown E1",
                 fmt::underlying(e1_index),
                 fmt::underlying(ue_index));
    return;
  }

  std::reference_wrapper<e1ap_interface> e1ap = e1aps[to_underlying(e1_index)];
  e1ap.get().handle_bearer_context_release_request_required(ue_index);
}

void cu_up_manager_impl::handle_pdcp_integrity_failure(cu_up_ue_index_t ue_index)
{
  ue_context* ue_ctxt = ue_mng->find_ue(ue_index);
  if (!ue_ctxt) {
    logger.error("ue={}: Could not handle PDCP integrity failure. UE context not found", ue_index);
    return;
  }
  cu_up_e1_index_t e1_index = ue_ctxt->get_e1_index();
  if (to_underlying(e1_index) >= e1aps.size()) {
    logger.error("e1={} ue={}: Could not handle PDCP integrity failure from unknown E1",
                 fmt::underlying(e1_index),
                 fmt::underlying(ue_index));
    return;
  }

  std::reference_wrapper<e1ap_interface> e1ap = e1aps[to_underlying(e1_index)];
  e1ap.get().handle_bearer_context_release_request_required(ue_index);
}

void cu_up_manager_impl::handle_pdcp_max_count_reached(cu_up_ue_index_t ue_index)
{
  ue_context* ue_ctxt = ue_mng->find_ue(ue_index);
  if (!ue_ctxt) {
    logger.error("ue={}: Reached PDCP MAX count, but could not find UE context", ue_index);
    return;
  }

  cu_up_e1_index_t e1_index = ue_ctxt->get_e1_index();
  if (to_underlying(e1_index) >= e1aps.size()) {
    logger.error("e1={} ue={}: Could not handle PDCP MAX count reached from unknown E1",
                 fmt::underlying(e1_index),
                 fmt::underlying(ue_index));
    return;
  }

  std::reference_wrapper<e1ap_interface> e1ap = e1aps[to_underlying(e1_index)];
  e1ap.get().handle_bearer_context_release_request_required(ue_index);
}

void cu_up_manager_impl::handle_pdcp_resume_required(cu_up_ue_index_t ue_index)
{
  ue_context* ue_ctxt = ue_mng->find_ue(ue_index);
  if (!ue_ctxt) {
    logger.error("ue={}: Resume was requested, but could not find UE context", ue_index);
    return;
  }

  cu_up_e1_index_t e1_index = ue_ctxt->get_e1_index();
  if (to_underlying(e1_index) >= e1aps.size()) {
    logger.error("e1={} ue={}: Could not handle PDCP resume required from unknown E1",
                 fmt::underlying(e1_index),
                 fmt::underlying(ue_index));
    return;
  }
  std::reference_wrapper<e1ap_interface> e1ap = e1aps[to_underlying(e1_index)];

  if (!ue_ctxt->is_suspended()) {
    logger.warning("ue={}: Resume requested, but bearer context is not suspended", ue_index);
  }

  if (ue_ctxt->resume_pending()) {
    logger.debug("ue={}: Resume already requested. Ignoring more requrests", ue_index);
  }

  e1ap.get().handle_dl_data_notification_required(ue_index);
}

///
/// Test mode helpers.
///
async_task<void> cu_up_manager_impl::enable_test_mode()
{
  return launch_async<cu_up_enable_test_mode_routine>(test_mode_cfg, *this, *ue_mng, ngu_demux);
}

async_task<void> cu_up_manager_impl::disable_test_mode()
{
  return launch_async<cu_up_disable_test_mode_routine>(*this, *ue_mng);
}

async_task<void> cu_up_manager_impl::reestablish_test_mode()
{
  return launch_async<cu_up_reestablish_test_mode_routine>(test_mode_cfg, *this, *ue_mng);
}

void cu_up_manager_impl::trigger_enable_test_mode()
{
  if (test_mode_cfg.attach_detach_period.count() == 0) {
    return;
  }

  test_mode_ue_timer = timers.create_unique_timer(exec_mapper.ctrl_executor());
  test_mode_ue_timer.set(test_mode_cfg.attach_detach_period,
                         [this]() { schedule_cu_up_async_task(enable_test_mode()); });
  test_mode_ue_timer.run();
}

void cu_up_manager_impl::trigger_disable_test_mode()
{
  if (test_mode_cfg.attach_detach_period.count() == 0) {
    return;
  }

  test_mode_ue_timer = timers.create_unique_timer(exec_mapper.ctrl_executor());
  test_mode_ue_timer.set(test_mode_cfg.attach_detach_period,
                         [this]() { schedule_cu_up_async_task(disable_test_mode()); });
  test_mode_ue_timer.run();
}

void cu_up_manager_impl::trigger_reestablish_test_mode()
{
  if (test_mode_cfg.reestablish_period.count() == 0) {
    return;
  }

  test_mode_ue_timer = timers.create_unique_timer(exec_mapper.ctrl_executor());
  test_mode_ue_timer.set(test_mode_cfg.reestablish_period,
                         [this]() { schedule_cu_up_async_task(reestablish_test_mode()); });
  test_mode_ue_timer.run();
}
