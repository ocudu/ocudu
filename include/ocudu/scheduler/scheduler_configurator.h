// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/drx_config.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/meas_gap_config.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/rrm.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/scheduler/config/logical_channel_config.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/config/si_scheduling_config.h"
#include "ocudu/scheduler/config/slice_rrm_policy_config.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"

namespace ocudu {

class scheduler_cell_metrics_notifier;

/// Cell Configuration Request for the scheduler.
struct sched_cell_configuration_request_message {
  struct metrics_config {
    scheduler_cell_metrics_notifier* notifier = nullptr;
    /// Maximum number of UE events per report.
    unsigned max_ue_events_per_report = 64;
  };

  /// Cell identifier internal to the DU.
  du_cell_index_t cell_index;

  /// Cell group identifier internal to the DU.
  du_cell_group_index_t cell_group_index;

  /// RAN-specific cell parameters.
  ran_cell_config ran;

  /// Scheduling of SI messages.
  si_scheduling_config si_scheduling;

  /// List of RAN slices to support in the scheduler.
  std::vector<slice_rrm_policy_config> rrm_policy_members;

  /// Configuration of scheduler cell metrics.
  metrics_config metrics;
};

/// Cell Reconfiguration Request.
struct sched_cell_reconfiguration_request_message {
  std::optional<du_cell_slice_reconfig_request> slice_reconf_req;
};

/// Parameters provided to the scheduler to configure the resource allocation of a specific UE.
struct sched_ue_resource_alloc_config {
  /// Minimum and maximum PDSCH grant sizes for the given UE.
  prb_interval pdsch_grant_size_limits{0, MAX_NOF_PRBS};
  /// Minimum and maximum PUSCH grant sizes for the given UE.
  prb_interval pusch_grant_size_limits{0, MAX_NOF_PRBS};
  /// Maximum PDSCH HARQ retransmissions.
  unsigned max_pdsch_harq_retxs = 4;
  /// Maximum PUSCH HARQ retransmissions.
  unsigned max_pusch_harq_retxs = 4;
  // RRM policy for the UE.
  rrm_policy_ratio_group rrm_policy_group;
};

struct ue_cell_config {
  serving_cell_config serv_cell_cfg;
  // List of BWP configurations for this UE in this cell.
  std::vector<ue_bwp_config> bwps;

  /// Returns the initial BWP configuration for this UE in this cell.
  ue_bwp_config& init_bwp() { return bwps[0]; }
};

/// Request for a new UE configuration provided to the scheduler during UE creation or reconfiguration.
struct sched_ue_config_request {
  /// Possible contexts which triggered this UE config update.
  /// \note The scheduler will handle the UE configuration procedure differently depending on the cause. For instance,
  /// if the cause is not a RRC procedure (not_rrc_proc), the UE does not need to enter fallback mode.
  enum class causes { other_rrc_proc, rrc_reconf_after_reest, not_rrc_proc };

  /// List of configured Logical Channels. See \c mac-LogicalChannelConfig, TS38.331.
  std::optional<std::vector<logical_channel_config>> lc_config_list;
  /// UE-dedicated configuration for the PCell and SCells.
  std::optional<std::vector<ue_cell_config>> cells;
  /// Resource allocation configuration for the given UE.
  std::optional<sched_ue_resource_alloc_config> res_alloc_cfg;
  /// DRX-Config.
  std::optional<drx_config> drx_cfg;
  /// measGapConfig.
  std::optional<meas_gap_config> meas_gap_cfg;
  /// Whether this configuration was triggered by RRC.
  causes cause = causes::other_rrc_proc;
};

/// Request to create a new UE in scheduler.
struct sched_ue_creation_request_message {
  du_ue_index_t ue_index;
  rnti_t        crnti;
  /// Whether the UE starts in fallback mode, i.e. without using its dedicated configuration.
  bool starts_in_fallback;
  /// Slot at which UL-CCCH message was received, in case of RA-based UE creation. Invalid, otherwise.
  std::optional<slot_point> ul_ccch_slot_rx;
  /// Whether the UE is expecting a CFRA.
  bool cfra_enabled = false;
  /// Configuration to be applied to the new UE.
  sched_ue_config_request cfg;
};

/// UE Reconfiguration Request.
struct sched_ue_reconfiguration_message {
  du_ue_index_t           ue_index;
  rnti_t                  crnti;
  sched_ue_config_request cfg;
};

/// UE Delete Request.
struct sched_ue_delete_message {
  du_ue_index_t ue_index;
  rnti_t        crnti;
};

/// Interface to Add/Remove UEs and Cells.
class scheduler_configurator
{
public:
  virtual ~scheduler_configurator() = default;

  /// \brief Reconfigure cell.
  ///
  /// This method cannot be called for an existing cell index without first removing it.
  virtual bool handle_cell_configuration_request(const sched_cell_configuration_request_message& msg) = 0;

  /// \brief Handle cell configuration removal.
  virtual void handle_cell_removal_request(du_cell_index_t cell_index) = 0;

  /// \brief Activate a configured cell. This method has no effect if the cell is already active.
  /// \remark This method needs to be called in the same thread as the slot_indication() method.
  virtual void handle_cell_activation_request(du_cell_index_t cell_index) = 0;

  /// \brief Deactivate a configured cell. This method has no effect if the cell is already deactivated.
  /// \remark This method needs to be called after the last slot_indication() call.
  virtual void handle_cell_deactivation_request(du_cell_index_t cell_index) = 0;

  /// \brief Handle slice reconfiguration request of a cell.
  virtual void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& msg) = 0;
};

class scheduler_ue_configurator
{
public:
  virtual ~scheduler_ue_configurator()                                                               = default;
  virtual void handle_ue_creation_request(const sched_ue_creation_request_message& ue_request)       = 0;
  virtual void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) = 0;
  virtual void handle_ue_removal_request(du_ue_index_t ue_index)                                     = 0;
  virtual void handle_ue_config_applied(du_ue_index_t ue_index)                                      = 0;
  virtual void handle_ue_deactivation_request(du_ue_index_t ue_index)                                = 0;
};

/// Interface used by scheduler to notify MAC that a configuration is complete.
class sched_configuration_notifier
{
public:
  virtual ~sched_configuration_notifier() = default;

  /// Called by scheduler when UE creation/modification is completed.
  virtual void on_ue_config_complete(du_ue_index_t ue_index, bool ue_creation_result) = 0;

  /// Called by scheduler when UE removal is completed.
  virtual void on_ue_deletion_completed(du_ue_index_t ue_index) = 0;
};

} // namespace ocudu
