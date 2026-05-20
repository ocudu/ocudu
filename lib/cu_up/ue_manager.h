// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ngu_session_manager.h"
#include "ue_manager_interfaces.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/cu_up/cu_up_state.h"
#include "ocudu/f1u/cu_up/f1u_gateway.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include "ocudu/support/timers.h"
#include <unordered_map>

namespace ocudu {

namespace ocuup {

/// UE manager configuration.
struct ue_manager_config {
  uint32_t                      max_nof_ues;
  const n3_interface_config&    n3_config;
  const cu_up_test_mode_config& test_mode_config;
};

/// UE manager dependencies.
struct ue_manager_dependencies {
  std::vector<std::reference_wrapper<e1ap_interface>> e1aps;
  timer_manager&                                      timers;
  f1u_cu_up_gateway&                                  f1u_gw;
  ngu_session_manager&                                ngu_session_mngr;
  cu_up_manager_pdcp_interface&                       cu_up_mngr_pdcp_if;
  gtpu_demux_ctrl&                                    gtpu_rx_demux;
  gtpu_teid_pool&                                     n3_teid_allocator;
  gtpu_teid_pool&                                     f1u_teid_allocator;
  cu_up_executor_mapper&                              exec_pool;
  dlt_pcap&                                           gtpu_pcap;
  ocudulog::basic_logger&                             logger;
};

class ue_manager : public ue_manager_ctrl
{
public:
  explicit ue_manager(const ue_manager_config& config, const ue_manager_dependencies& dependencies);

  using ue_db_t              = std::unordered_map<cu_up_ue_index_t, std::unique_ptr<ue_context>>;
  using ue_task_schedulers_t = std::vector<std::unique_ptr<fifo_async_task_scheduler>>;
  const ue_db_t& get_ues() const { return ue_db; }

  async_task<void> stop() override;
  ue_context*      add_ue(cu_up_e1_index_t e1_index, const ue_context_cfg& cfg) override;
  async_task<void> remove_all_ues() override;
  async_task<void> remove_ue(cu_up_ue_index_t ue_index) override;
  ue_context*      find_ue(cu_up_ue_index_t ue_index) override;
  size_t           get_nof_ues() const override { return ue_db.size(); }
  up_state_t       get_up_state() const override
  {
    up_state_t st;
    for (const std::pair<const cu_up_ue_index_t, std::unique_ptr<ue_context>>& ue : ue_db) {
      st.insert({ue.first, ue.second->get_pdu_session_state()});
    }
    return st;
  }

  void schedule_ue_async_task(cu_up_ue_index_t ue_index, async_task<void> task);

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  cu_up_ue_index_t get_next_ue_index();

  async_task<expected<>> schedule_and_wait_ue_removal(cu_up_ue_index_t ue_index);

  uint32_t                                            max_nof_ues;
  const n3_interface_config&                          n3_config;
  const cu_up_test_mode_config&                       test_mode_config;
  std::vector<std::reference_wrapper<e1ap_interface>> e1aps;
  f1u_cu_up_gateway&                                  f1u_gw;
  ngu_session_manager&                                ngu_session_mngr;
  cu_up_manager_pdcp_interface&                       cu_up_mngr_pdcp_if;
  gtpu_demux_ctrl&                                    gtpu_rx_demux;
  gtpu_teid_pool&                                     n3_teid_allocator;
  gtpu_teid_pool&                                     f1u_teid_allocator;
  cu_up_executor_mapper&                              exec_pool;
  task_executor&                                      ctrl_executor;
  dlt_pcap&                                           gtpu_pcap;
  timer_manager&                                      timers;
  ue_db_t                                             ue_db;
  ue_task_schedulers_t                                ue_task_schedulers;
  ocudulog::basic_logger&                             logger;
};

} // namespace ocuup

} // namespace ocudu
