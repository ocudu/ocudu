// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_cell_manager.h"
#include "du_ue.h"
#include "du_ue_controller_impl.h"
#include "du_ue_manager_repository.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/du/du_high/du_manager/du_manager.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"
#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include <unordered_map>

namespace ocudu {
namespace odu {

class du_procedure_metrics_collector;

/// \brief This entity orchestrates the addition/reconfiguration/removal of UE contexts in the DU.
class du_ue_manager final : public du_ue_manager_repository
{
public:
  explicit du_ue_manager(du_manager_params&              cfg_,
                         du_ran_resource_manager&        cell_res_alloc,
                         const du_cell_manager&          cell_mng,
                         du_procedure_metrics_collector& metrics);

  du_ue_index_t find_unused_du_ue_index();

  /// \brief Handle the creation of a new UE context when a UL-CCCH is received.
  void handle_ue_create_request(const ul_ccch_indication_message& msg);

  /// \brief Handle the creation of a new UE context by F1AP request.
  async_task<f1ap_ue_context_creation_response> handle_ue_create_request(const f1ap_ue_context_creation_request& req);

  /// \brief Handle the update of an existing UE context by F1AP request.
  async_task<f1ap_ue_context_update_response> handle_ue_config_request(const f1ap_ue_context_update_request& req);

  /// \brief Handle the removal of an existing UE context by F1AP request.
  async_task<void> handle_ue_delete_request(const f1ap_ue_delete_request& req);

  /// \brief Handle the DRB deactivation of an existing UE context by F1AP request.
  async_task<void> handle_ue_drb_deactivation_request(du_ue_index_t ue_index);

  void handle_reestablishment_request(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index);

  void handle_ue_config_applied(du_ue_index_t ue_index);

  /// \brief Handle the configuration of an existing UE context by RIC request.
  async_task<du_mac_sched_control_config_response> handle_ue_config_request(const du_mac_sched_control_config& req);

  /// \brief Force the interruption of all UE activity.
  async_task<void> stop();

  /// \brief Find a UE context by UE index.
  const du_ue* find_ue(du_ue_index_t ue_index) const override;
  du_ue*       find_ue(du_ue_index_t ue_index) override;

  /// \brief Number of DU UEs currently active.
  size_t nof_ues() const { return ue_db.size(); }

  const auto& get_du_ues() const { return ue_db; }

  /// \brief Schedule an asynchronous task to be executed in the UE control loop.
  void schedule_async_task(du_ue_index_t ue_index, async_task<void> task) override
  {
    ue_ctrl_loop[ue_index].schedule(std::move(task));
  }

  gtpu_teid_pool& get_f1u_teid_pool() override { return f1u_teid_pool; }

private:
  expected<du_ue*, std::string> add_ue(const du_ue_context& ue_ctx, ue_ran_resource_configurator ue_ran_res) override;
  void                          update_crnti(du_ue_index_t ue_index, rnti_t crnti) override;
  du_ue*                        find_rnti(rnti_t rnti) override;
  du_ue*                        find_f1ap_ue_id(gnb_du_ue_f1ap_id_t f1ap_ue_id) override;
  void                          remove_ue(du_ue_index_t ue_index) override;

  du_manager_params&              cfg;
  du_ran_resource_manager&        cell_res_alloc;
  const du_cell_manager&          cell_mng;
  du_procedure_metrics_collector& metrics;
  ocudulog::basic_logger&         logger;

  // Pool of available TEIDs for F1-U.
  gtpu_teid_pool& f1u_teid_pool;

  // Mapping of ue_index and rnti to UEs.
  slotted_id_table<du_ue_index_t, du_ue_controller_impl, MAX_NOF_DU_UES> ue_db;
  std::unordered_map<rnti_t, du_ue_index_t>                              rnti_to_ue_index;

  // task event loops indexed by ue_index
  slotted_array<fifo_async_task_scheduler, MAX_NOF_DU_UES> ue_ctrl_loop;

  // Whether new UEs should be created.
  bool stop_accepting_ues = false;
};

} // namespace odu
} // namespace ocudu
