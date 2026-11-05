// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "amf_connection_manager.h"
#include "cu_cp_ue_admission_controller.h"
#include "cu_up_connection_manager.h"
#include "du_connection_manager.h"
#include "xnc_connection_manager.h"
#include <set>

namespace ocudu::ocucp {

/// CU-CP controller configuration.
struct cu_cp_controller_config {
  unsigned max_nof_dus;
  unsigned max_nof_cu_ups;
};

/// CU-CP controller dependencies.
struct cu_cp_controller_dependencies {
  cu_cp_amf_reconnection_handler&      cu_cp_notifier;
  async_task_scheduler&                common_task_sched;
  ngap_repository&                     ngaps;
  cu_up_processor_repository&          cu_ups;
  du_processor_repository&             dus;
  xnap_repository&                     xncs;
  std::vector<xnc_connection_gateway*> xnc_gws;
  task_executor&                       ctrl_exec;
  timer_manager&                       timers;
  ocudulog::basic_logger&              logger;
  cu_cp_ng_setup_complete_notifier*    ng_setup_notifier = nullptr;
};

/// \brief Entity responsible for managing the CU-CP connections to remote nodes and determining whether the CU-CP
/// is in a state to accept new connections.
///
/// In particular, this class is responsible for:
/// - triggering an AMF connection and tracking the status of the connection;
/// - determining whether a new DU setup request should be accepted based on the status of other remote node
/// connections;
/// - determining whether new UEs should be accepted depending on the status of the CU-CP remote connections.
class cu_cp_controller : public cu_cp_ue_admission_controller
{
public:
  cu_cp_controller(const cu_cp_controller_config& configuration, cu_cp_controller_dependencies dependencies);

  /// Stops the CU-CP controller.
  void stop();

  /// Gets the AMF connection handler.
  amf_connection_manager& amf_connection_handler() { return amf_mng; }

  /// Gets the XNC connection handler.
  xnc_connection_manager& xnc_connection_handler() { return xnc_mng; }

  /// \brief Returns the subset of the given PLMN identifiers for which an AMF is connected.
  std::set<plmn_identity> get_connected_plmns(const std::set<plmn_identity>& plmn_ids);

  /// Determines whether the CU-CP should accept new UE connections.
  bool request_ue_setup() const override;

  /// \brief Decides which connected AMFs reject a new UE with the given establishment cause because of an overload.
  /// \return The AMFs that reject the UE, or std::nullopt if every connected AMF rejects the UE.
  std::optional<std::vector<cu_cp_amf_index_t>> admit_ue_setup(establishment_cause_t establishment_cause);

  /// \brief Determines whether the CU-CP should accept the resume of an RRC_INACTIVE UE.
  ///
  /// The CU-CP rejects the UE if the overload of the AMF of the UE rejects the resume cause or the Allowed NSSAI.
  bool request_ue_resume(cu_cp_amf_index_t amf_index, resume_cause_t resume_cause, span<const s_nssai_t> allowed_nssai);

  /// \brief Selects the AMF for a UE that accesses the CU-CP.
  /// \return The index of the AMF, or std::nullopt if no connected AMF supports the selected PLMN of the UE.
  std::optional<cu_cp_amf_index_t> select_amf(const amf_selection_request& request);

  /// \brief Finds the connected AMF that serves a GUAMI, or a connected AMF of its AMF Set.
  /// \return The index of the AMF, or std::nullopt if no AMF of the AMF Set is connected.
  std::optional<cu_cp_amf_index_t> select_amf_for_guami(const guami_t& guami);

  /// \brief Selects a connected AMF of an AMF Set that supports a PLMN, other than the AMF that asks for it.
  /// \return The index of the AMF, or std::nullopt if no other AMF of the AMF Set supports the PLMN.
  std::optional<cu_cp_amf_index_t>
  select_amf_for_amf_set(const plmn_identity& plmn, uint16_t amf_set_id, cu_cp_amf_index_t requesting_amf_index);

  /// Gets the F1C handler.
  cu_cp_f1c_handler& get_f1c_handler() { return du_mng; }

  /// Gets the E1 handler.
  cu_cp_e1_handler& get_e1_handler() { return cu_up_mng; }

  /// Gets the XNC handler.
  cu_cp_xnc_handler& get_xnc_handler() { return xnc_mng; }

private:
  task_executor&          ctrl_exec;
  ocudulog::basic_logger& logger;

  amf_connection_manager   amf_mng;
  du_connection_manager    du_mng;
  cu_up_connection_manager cu_up_mng;
  xnc_connection_manager   xnc_mng;

  std::mutex mutex;
  bool       running = true;

  /// NAS Node Selection Function of the CU-CP.
  amf_selector amf_sel;
};

} // namespace ocudu::ocucp
