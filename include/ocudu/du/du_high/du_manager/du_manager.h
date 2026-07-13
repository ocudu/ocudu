// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_manager/du_configurator.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

struct ul_ccch_indication_message;
struct ul_crnti_ce_indication_message;
struct mac_metric_report;
struct scheduler_cell_metrics;

namespace odu {

/// Interface used to handle external events (e.g. UL CCCH).
class du_manager_mac_event_handler
{
public:
  virtual ~du_manager_mac_event_handler() = default;

  /// \brief Handle UL CCCH message arrival.
  virtual void handle_ul_ccch_indication(const ul_ccch_indication_message& msg) = 0;

  /// \brief Handle Msg3 C-RNTI CE detection.
  virtual void handle_crnti_ce_indication(const ul_crnti_ce_indication_message& msg) = 0;
};

/// Interface used to handle metric reports from the DU-high remaining layers.
class du_manager_mac_metric_aggregator
{
public:
  virtual ~du_manager_mac_metric_aggregator() = default;

  /// \brief Handle the metrics report from the MAC.
  virtual void aggregate_mac_metrics_report(const mac_metric_report& report) = 0;
};

/// This class handles updates in cell and UE configurations coming from the F1AP.
class du_manager_f1ap_event_handler
{
public:
  virtual ~du_manager_f1ap_event_handler() = default;

  /// \brief Schedule asynchronous task in a DU-wide scope.
  virtual void schedule_async_task(async_task<void>&& task) = 0;

  /// \brief Schedule asynchronous task that is specific to a UE.
  virtual void schedule_async_task(du_ue_index_t ue_index, async_task<void>&& task) = 0;

  /// Handle the F1-C connection loss event.
  virtual void handle_f1c_connection_loss() = 0;

  /// \brief Request a currently unused and unallocated UE index from the DU.
  virtual du_ue_index_t find_unused_du_ue_index() = 0;

  /// \brief Handle reset of UE contexts as per request from the F1AP.
  virtual async_task<void> handle_f1_reset_request(const std::vector<du_ue_index_t>& ues_to_reset) = 0;

  /// \brief Handle request to update context by the CU via F1AP.
  virtual async_task<gnbcu_config_update_response>
  handle_cu_context_update_request(const gnbcu_config_update_request& request) = 0;

  /// \brief Create a new UE context in the DU with an assigned UE index.
  virtual async_task<f1ap_ue_context_creation_response>
  handle_ue_context_creation(const f1ap_ue_context_creation_request& request) = 0;

  /// \brief Update the UE configuration in the DU, namely its SRBs and DRBs.
  virtual async_task<f1ap_ue_context_update_response>
  handle_ue_context_update(const f1ap_ue_context_update_request& request) = 0;

  /// \brief Remove UE context from the DU.
  virtual async_task<void> handle_ue_delete_request(const f1ap_ue_delete_request& request) = 0;

  /// \brief Deactivate DRB activity for a given UE.
  virtual async_task<void> handle_ue_drb_deactivation_request(du_ue_index_t ue_index) = 0;

  /// \brief Handle the transfer of resources of old UE to new Reestablishing UE and deletion of the old UE context.
  virtual void handle_ue_reestablishment(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) = 0;

  /// Handle the confirmation that the UE has applied the last received RRC config.
  virtual void handle_ue_config_applied(du_ue_index_t ue_index) = 0;

  /// Retrieve handler of UE positioning.
  virtual f1ap_du_positioning_handler& get_positioning_handler() = 0;
};

/// This class handles Write-Replace Warning (PWS) requests coming from the F1AP.
class du_manager_pws_handler
{
public:
  virtual ~du_manager_pws_handler() = default;

  /// \brief Handle a Write-Replace Warning Request forwarded by the F1AP.
  /// \return Subset of \c write_replace_warning_information::cells that accepted the broadcast.
  virtual async_task<std::vector<du_cell_index_t>>
  handle_write_replace_warning(const write_replace_warning_information& req) = 0;
};

/// Interface to initiate and stop the DU manager activity.
class du_manager_controller
{
public:
  virtual ~du_manager_controller() = default;

  /// \brief Initiate the DU manager.
  /// \remark This call is blocking and only returns once the DU finishes its setup.
  virtual void start() = 0;

  /// \brief Stop the DU manager.
  /// \remark This call is blocking and only returns once all tasks in the DU manager have completed.
  virtual void stop() = 0;
};

/// Interface that provides a handle to the DU manager.
class du_manager
{
public:
  virtual ~du_manager() = default;

  /// Get controller to DU manager to start and stop its activity.
  virtual du_manager_controller& get_controller() = 0;

  /// Get handler of MAC events.
  virtual du_manager_mac_event_handler& get_mac_event_handler() = 0;

  /// Get entity responsibly for aggregating metrics from all DU layers.
  virtual du_manager_mac_metric_aggregator& get_metrics_aggregator() = 0;

  /// Get F1AP configuration handling interface of the DU manager.
  virtual du_manager_f1ap_event_handler& get_f1ap_event_handler() = 0;

  /// Get handler of F1AP Write-Replace Warning (PWS) requests.
  virtual du_manager_pws_handler& get_pws_handler() = 0;

  /// Get configuration interface with the procedures that are triggered externally to the DU.
  virtual du_configurator& get_operation_configurator() = 0;
};

} // namespace odu
} // namespace ocudu
