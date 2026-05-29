// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ngap_repository.h"
#include "../ue_manager/ue_manager_impl.h"
#include "ocudu/cu_cp/cu_cp_ng_setup_notifier.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/support/async/async_task_scheduler.h"
#include <future>

namespace ocudu::ocucp {

class cu_cp_routine_manager;
struct cu_cp_configuration;

class amf_connection_manager
{
public:
  amf_connection_manager(ngap_repository&                  ngaps_,
                         cu_cp_amf_reconnection_handler&   cu_cp_notifier_,
                         timer_manager&                    timers_,
                         task_executor&                    cu_cp_exec_,
                         async_task_scheduler&             common_task_sched_,
                         cu_cp_ng_setup_complete_notifier* ng_setup_notifier_ = nullptr);

  /// \brief Initiates the connection to the AMF.
  /// A promise is passed as a parameter to enable blocking synchronization between the completion of the scheduled
  /// async task and the caller side.
  void connect_to_amf(std::promise<bool>* completion_signal = nullptr);

  /// \brief Initiate procedure to disconnect from the N2 interface.
  async_task<void> disconnect_amf();

  /// \brief Handles the loss of connection to the AMF.
  /// \param[in] amf_index The index of the AMF that has been disconnected.
  void handle_amf_connection_loss(cu_cp_amf_index_t amf_index);

  /// \brief Initiates the reconnection to the AMF.
  /// \param[in] amf_index The index of the AMF to reconnect to.
  /// \param[in] ue_mng The UE manager to re-enable UE connections in case the reconnection was successful.
  /// \param[in] amf_reconnection_retry_time The time to wait before retrying the reconnection.
  void reconnect_to_amf(cu_cp_amf_index_t         amf_index,
                        ue_manager*               ue_mng,
                        std::chrono::milliseconds amf_reconnection_retry_time);

  void stop();

  /// Checks whether the CU-CP is connected to the AMF.
  bool is_amf_connected(plmn_identity plmn) const;
  bool is_amf_connected(cu_cp_amf_index_t amf_index) const;

  size_t nof_amfs() const { return amfs_connected.size(); }

private:
  void              handle_connection_setup_result(cu_cp_amf_index_t amf_index, bool success);
  cu_cp_amf_index_t plmn_to_amf_index(plmn_identity plmn) const;

  ngap_repository&                  ngaps;
  cu_cp_amf_reconnection_handler&   cu_cp_notifier;
  timer_manager&                    timers;
  task_executor&                    cu_cp_exec;
  async_task_scheduler&             common_task_sched;
  ocudulog::basic_logger&           logger;
  cu_cp_ng_setup_complete_notifier* ng_setup_notifier;

  std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>> amfs_connected;

  std::atomic<bool> stopped{false};
};

} // namespace ocudu::ocucp
