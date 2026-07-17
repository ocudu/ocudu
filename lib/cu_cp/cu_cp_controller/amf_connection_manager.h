// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_manager/ue_manager_impl.h"
#include "ocudu/ran/plmn_identity.h"
#include <future>

namespace ocudu {

class async_task_scheduler;

namespace ocucp {

class cu_cp_ng_setup_complete_notifier;
class ngap_repository;

/// AMF connection manager dependencies.
struct amf_connection_manager_dependencies {
  ngap_repository&                  ngaps;
  cu_cp_amf_reconnection_handler&   cu_cp_notifier;
  timer_manager&                    timers;
  task_executor&                    cu_cp_exec;
  async_task_scheduler&             common_task_sched;
  ocudulog::basic_logger&           logger;
  cu_cp_ng_setup_complete_notifier* ng_setup_notifier = nullptr;
};

/// AMF connection manager.
class amf_connection_manager
{
public:
  explicit amf_connection_manager(const amf_connection_manager_dependencies& dependencies);

  /// \brief Initiates the connection to the AMF.
  ///
  /// A promise is passed as a parameter to enable blocking synchronization between the completion of the scheduled
  /// async task and the caller side.
  /// \param[in] completion_signal Promise signalled with the result of the connection setup.
  /// \param[in] retry_time The time to wait between attempts of the AMFs that are reconnected in the background.
  void connect_to_amf(std::promise<bool>* completion_signal, std::chrono::milliseconds retry_time);

  /// Initiate procedure to disconnect from the N2 interface.
  async_task<void> disconnect_amf();

  /// \brief Handles the loss of connection to the AMF.
  /// \param[in] amf_index The index of the AMF that has been disconnected.
  void handle_amf_connection_loss(cu_cp_amf_index_t amf_index);

  /// \brief Initiates the reconnection to the AMF.
  /// \param[in] amf_index The index of the AMF to reconnect to.
  /// \param[in] ue_mng The UE manager to re-enable UE connections in case the reconnection was successful. It may be
  /// null when no UE connection was ever blocked for this AMF, e.g. when the initial connection setup failed.
  /// \param[in] amf_reconnection_retry_time The time to wait before retrying the reconnection.
  void reconnect_to_amf(cu_cp_amf_index_t         amf_index,
                        ue_manager*               ue_mng,
                        std::chrono::milliseconds amf_reconnection_retry_time);

  /// Stops the AMF connection manager.
  void stop();

  /// Checks whether the CU-CP is connected to the AMF with the given PLMN identity.
  bool is_amf_connected(plmn_identity plmn) const;

  /// Checks whether the CU-CP is connected to the AMF with the given CU_CP AMF index.
  bool is_amf_connected(cu_cp_amf_index_t amf_index) const;

  /// Returns the number of AMFs.
  size_t nof_amfs() const { return amfs_connected.size(); }

private:
  /// Handles the connection setup result.
  void handle_connection_setup_result(cu_cp_amf_index_t amf_index, bool success);

  /// Converts the given PLMN identity into a CU_CP AMF index.
  cu_cp_amf_index_t plmn_to_amf_index(plmn_identity plmn) const;

  /// \brief Schedules a background reconnection for every AMF whose N2 TNL association could not be established.
  /// \return True if no AMF was left in a state from which it cannot recover on its own, false otherwise.
  bool retry_unconnected_amfs(std::chrono::milliseconds retry_time);

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

} // namespace ocucp
} // namespace ocudu
