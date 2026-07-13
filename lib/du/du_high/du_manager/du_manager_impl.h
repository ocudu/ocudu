// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_cell_manager.h"
#include "du_manager_context.h"
#include "du_manager_controller_impl.h"
#include "du_ue/du_ue_manager.h"
#include "metrics/du_metrics_aggregator_impl.h"
#include "procedures/du_proc_context_view.h"
#include "ran_resource_management/du_ran_resource_manager_impl.h"
#include "ocudu/du/du_high/du_manager/du_manager.h"
#include "ocudu/du/du_high/du_manager/du_manager_mem_resources.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"
#include "ocudu/rlc/rlc_window_seg_pool_factory.h"

namespace ocudu {
namespace odu {

class du_manager_impl final : public du_manager,
                              public du_manager_mac_event_handler,
                              public du_manager_f1ap_event_handler,
                              public du_manager_pws_handler,
                              public du_configurator
{
public:
  explicit du_manager_impl(const du_manager_params& params_);

  // Controller interface.
  du_manager_controller& get_controller() override { return controller; }

  // MAC event handling interface
  du_manager_mac_event_handler& get_mac_event_handler() override { return *this; }
  void                          handle_ul_ccch_indication(const ul_ccch_indication_message& msg) override;
  void                          handle_crnti_ce_indication(const ul_crnti_ce_indication_message& msg) override;

  // Task scheduling interface.
  du_manager_f1ap_event_handler& get_f1ap_event_handler() override { return *this; }

  // PWS (Write-Replace Warning) handling interface.
  du_manager_pws_handler& get_pws_handler() override { return *this; }
  async_task<std::vector<du_cell_index_t>>
       handle_write_replace_warning(const write_replace_warning_information& req) override;
  void schedule_async_task(async_task<void>&& task) override { main_ctrl_loop.schedule(std::move(task)); }
  void schedule_async_task(du_ue_index_t ue_index, async_task<void>&& task) override
  {
    ue_mng.schedule_async_task(ue_index, std::move(task));
  }

  void handle_f1c_connection_loss() override;

  du_ue_index_t find_unused_du_ue_index() override;

  async_task<void> handle_f1_reset_request(const std::vector<du_ue_index_t>& ues_to_reset) override;
  async_task<gnbcu_config_update_response>
  handle_cu_context_update_request(const gnbcu_config_update_request& request) override;

  async_task<f1ap_ue_context_creation_response>
  handle_ue_context_creation(const f1ap_ue_context_creation_request& request) override;

  async_task<f1ap_ue_context_update_response>
  handle_ue_context_update(const f1ap_ue_context_update_request& request) override;

  async_task<void> handle_ue_delete_request(const f1ap_ue_delete_request& request) override;

  async_task<void> handle_ue_drb_deactivation_request(du_ue_index_t ue_index) override;

  void handle_ue_reestablishment(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) override;

  void handle_ue_config_applied(du_ue_index_t ue_index) override;

  du_configurator& get_operation_configurator() override { return *this; }

  async_task<du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(const du_mac_sched_control_config& reconf) override;

  du_param_config_response             handle_sync_operator_config(const du_param_config_request& req) override;
  async_task<du_param_config_response> handle_operator_config(const du_param_config_request& req,
                                                              task_executor& continuation_exec) override;

  void handle_ntn_param_update(const du_ntn_param_update_request& req) override;

  f1ap_du_positioning_handler& get_positioning_handler() override { return *positioning_handler; }

  du_manager_mac_metric_aggregator& get_metrics_aggregator() override { return metrics; }

private:
  // DU manager configuration that will be visible to all running procedures
  du_manager_params       params;
  ocudulog::basic_logger& logger;

  // Resources owned by the DU manager, available to its procedures and sub-structures.
  du_manager_mem_resources mem_resources;

  // Handler for DU tasks.
  fifo_async_task_scheduler main_ctrl_loop;

  // Components
  du_manager_context                           ctxt;
  du_cell_manager                              cell_mng;
  du_ran_resource_manager_impl                 cell_res_alloc;
  du_manager_metrics_aggregator_impl           metrics;
  du_ue_manager                                ue_mng;
  std::unique_ptr<f1ap_du_positioning_handler> positioning_handler;
  du_proc_context_view                         proc_ctxt;
  /// Handle to control the start and stop of the DU activity.
  du_manager_controller_impl controller;
};

} // namespace odu
} // namespace ocudu
