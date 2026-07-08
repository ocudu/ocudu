// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/ran/rrm.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

class task_executor;

namespace odu {

/// UE-level parameters of a cell MAC scheduler that need to be configured during the DU cell operation.
struct du_mac_sched_control_config {
  /// DU-UE-F1AP-ID of the UE being reconfigured.
  gnb_du_ue_f1ap_id_t ue_id;
  /// Sets the number of HARQ processes to be used.
  std::optional<unsigned> num_harq_processes;
  /// Sets the number of HARQ retransmissions to be used.
  std::optional<unsigned> num_harq_retransmissions;
  /// Set the radio resource management policy list.
  std::vector<rrm_policy_ratio_group> rrm_policy_ratio_list;
};

/// Result of a UE-level cell MAC scheduler parameter configuration procedure.
struct du_mac_sched_control_config_response {
  bool harq_processes_result;
  bool min_prb_alloc_result;
  bool max_prb_alloc_result;
};

/// Parameters of a cell that need to be configured during the DU cell operation.
struct du_cell_param_config_request {
  du_cell_param_config_request() = default;
  du_cell_param_config_request(const std::optional<nr_cell_global_id_t>& nr_cgi_,
                               std::optional<int>                        ssb_pwr_mod_,
                               std::vector<rrm_policy_ratio_group>       rrm_policy_ratio_list_ = {}) :
    nr_cgi(nr_cgi_), ssb_pwr_mod(ssb_pwr_mod_), rrm_policy_ratio_list(std::move(rrm_policy_ratio_list_))
  {
  }

  /// \brief NR Cell Global ID of the cell being configured. If not specified, the configuration is applied to all
  /// configured cells.
  std::optional<nr_cell_global_id_t> nr_cgi;
  /// SSB power modification to be applied to the cell. If absent, no modification is applied.
  std::optional<int> ssb_pwr_mod;
  /// Radio resource management policy list to be applied to the cell, If empty, no modification is applied.
  std::vector<rrm_policy_ratio_group> rrm_policy_ratio_list;
  /// SIB update. When present, triggers SIB1 systemInfoValueTag increment.
  std::optional<sib_info> new_sys_info;
};

/// Parameters of the DU that need to be configured during operation.
struct du_param_config_request {
  /// Cells for which parameters need to be altered.
  std::vector<du_cell_param_config_request> cells;
};

/// Result of a DU dynamic parameter configuration procedure.
struct du_param_config_response {
  bool success;
};

/// NTN assistance information update. Only SIB19 fields that do not trigger SIB1 valuetag change notifications.
struct ntn_assistance_info_update {
  /// Moving reference location for NTN Earth-moving cell.
  std::optional<geodetic_coordinates_t> moving_reference_location;
  /// Ephemeris info.
  ntn_ephemeris_info_t ephemeris_info;
  /// Timing advance info (ta-Common, ta-CommonDrift, ta-CommonDriftVariant).
  std::optional<ta_info_t> ta_info;
  /// Epoch time for NTN assistance info.
  std::optional<epoch_time_t> epoch_time;
  /// Validity duration for UL sync assistance info in seconds.
  std::optional<unsigned> ntn_ul_sync_validity_dur;
};

/// NTN parameters to be updated in a single cell.
struct du_cell_ntn_param_update_request {
  using time_point = std::chrono::system_clock::time_point;
  /// NR Cell Global ID of the cell being configured.
  nr_cell_global_id_t nr_cgi;
  /// NTN assistance information update.
  std::optional<ntn_assistance_info_update> ntn_assistance_info;
  /// SI message index.
  unsigned si_msg_idx;
  /// SIB index (e.g., sib2 => value 2).
  unsigned sib_idx;
  /// Slot at which the first SI is transmitted.
  slot_point slot;
  /// SI period in nof slots, required if more than one are SI PDU passed.
  std::optional<unsigned> si_slot_period;
  /// Packed content of SIB messages.
  span<byte_buffer> si_messages;
  /// SIB19 update. When present, triggers SIB1 systemInfoValueTag increment.
  std::optional<sib19_info> sib19;
};

/// NTN Parameters to be updated for multiple cells in DU.
struct du_ntn_param_update_request {
  /// Cells for which NTN parameters need to be updated.
  std::vector<du_cell_ntn_param_update_request> cells;
};

struct du_ntn_param_update_response {
  bool success;
};

/// Interface to configure parameters of a DU in operational mode.
/// \note Use this interface to configure parameters that do not require stopping a cell and reconfiguring it.
class du_configurator
{
public:
  virtual ~du_configurator() = default;

  /// \brief Apply new config updates to a given UE present in the DU.
  virtual async_task<du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(const du_mac_sched_control_config& cfg_req) = 0;

  /// \brief Apply new config updates requested from outside the DU, in a blocking fashion.
  ///
  /// The caller will block until the procedure is complete.
  /// \param[in] req Configuration Request.
  virtual du_param_config_response handle_sync_operator_config(const du_param_config_request& req) = 0;

  /// Apply new config updates requested from outside the DU, in a non-blocking fashion.
  /// \param[in] req Configuration Request.
  /// \param[in] continuation_exec Task executor to which the configuration procedure will move to after its completion.
  virtual async_task<du_param_config_response> handle_operator_config(const du_param_config_request& req,
                                                                      task_executor& continuation_exec) = 0;

  /// Update NTN parameters from outside the DU.
  virtual void handle_ntn_param_update(const du_ntn_param_update_request& req) = 0;
};

} // namespace odu
} // namespace ocudu
