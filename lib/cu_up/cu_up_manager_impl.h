// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "adapters/gtpu_adapters.h"
#include "ngu_session_manager.h"
#include "ue_manager.h"
#include "ocudu/cu_up/cu_up_config.h"
#include "ocudu/cu_up/cu_up_manager.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include <memory>

namespace ocudu::ocuup {

/// CU-UP manager implementation configuration.
struct cu_up_manager_impl_config {
  gnb_cu_up_id_t                        cu_up_id;
  std::string                           cu_up_name;
  uint32_t                              max_nof_ues;
  std::string                           plmn;
  std::map<five_qi_t, cu_up_qos_config> qos;
  n3_interface_config                   n3_cfg;
  cu_up_test_mode_config                test_mode_cfg;
};

/// CU-UP manager implementation dependencies.
struct cu_up_manager_impl_dependencies {
  std::atomic<bool>&                                  stop_command;
  std::vector<std::reference_wrapper<e1ap_interface>> e1aps;
  gtpu_demux&                                         ngu_demux;
  ngu_session_manager&                                ngu_session_mngr;
  gtpu_teid_pool&                                     n3_teid_allocator;
  gtpu_teid_pool&                                     f1u_teid_allocator;
  cu_up_executor_mapper&                              exec_mapper;
  f1u_cu_up_gateway&                                  f1u_gateway;
  timer_manager&                                      timers;
  dlt_pcap&                                           gtpu_pcap;
  fifo_async_task_scheduler&                          cu_up_task_scheduler;
};

class cu_up_manager_impl final : public cu_up_manager
{
public:
  cu_up_manager_impl(const cu_up_manager_impl_config& config, const cu_up_manager_impl_dependencies& dependencies);

  async_task<void> stop() override;
  e1ap_bearer_context_setup_response
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override;

  async_task<e1ap_bearer_context_modification_response>
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override;

  async_task<void> handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg) override;

  void handle_e1ap_connection_drop(cu_up_e1_index_t e1_index) override;

  async_task<void> handle_e1_reset(const e1ap_reset& msg) override;

  void schedule_cu_up_async_task(async_task<void> task) override;

  void schedule_ue_async_task(cu_up_ue_index_t ue_index, async_task<void> task) override;

  size_t get_nof_ues() override { return ue_mng->get_nof_ues(); }

  // PDCP event handlers.
  void handle_pdcp_protocol_failure(cu_up_ue_index_t ue_index) override;
  void handle_pdcp_max_count_reached(cu_up_ue_index_t ue_index) override;
  void handle_pdcp_resume_required(cu_up_ue_index_t ue_index) override;

  // Test helpers.
  void trigger_enable_test_mode();
  void trigger_disable_test_mode();
  void trigger_reestablish_test_mode();

private:
  void on_statistics_report_timer_expired();

  async_task<void> enable_test_mode() override;
  async_task<void> disable_test_mode();
  async_task<void> reestablish_test_mode();

  gnb_cu_up_id_t cu_up_id;
  std::string    cu_up_name;
  std::string    plmn;

  std::atomic<bool>&                                  stop_command;
  std::vector<std::reference_wrapper<e1ap_interface>> e1aps;
  std::map<five_qi_t, cu_up_qos_config>               qos;
  const network_interface_config                      net_cfg;
  const n3_interface_config                           n3_cfg;
  const cu_up_test_mode_config                        test_mode_cfg;
  gtpu_demux&                                         ngu_demux;
  cu_up_executor_mapper&                              exec_mapper;
  timer_manager&                                      timers;

  // Logger
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("CU-UP", false);

  // Components
  std::unique_ptr<ue_manager> ue_mng;

  unique_timer statistics_report_timer;

  // Test mode
  unique_timer test_mode_ue_timer;

  fifo_async_task_scheduler& cu_up_task_scheduler;
};

} // namespace ocudu::ocuup
