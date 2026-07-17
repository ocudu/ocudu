// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_controller.h"
#include "../du_processor/du_processor_repository.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/plmn_identity.h"

using namespace ocudu;
using namespace ocucp;

cu_cp_controller::cu_cp_controller(const cu_cp_controller_config& configuration,
                                   cu_cp_controller_dependencies  dependencies) :
  ctrl_exec(dependencies.ctrl_exec),
  logger(dependencies.logger),
  amf_mng(amf_connection_manager_dependencies{.ngaps             = dependencies.ngaps,
                                              .cu_cp_notifier    = dependencies.cu_cp_notifier,
                                              .timers            = dependencies.timers,
                                              .cu_cp_exec        = dependencies.ctrl_exec,
                                              .common_task_sched = dependencies.common_task_sched,
                                              .logger            = logger,
                                              .ng_setup_notifier = dependencies.ng_setup_notifier}),
  du_mng(du_connection_manager_config{.max_nof_dus = configuration.max_nof_dus},
         du_connection_manager_dependencies{.dus               = dependencies.dus,
                                            .cu_cp_exec        = ctrl_exec,
                                            .common_task_sched = dependencies.common_task_sched,
                                            .logger            = logger}),
  cu_up_mng(cu_up_connection_manager_config{.max_nof_cu_ups = configuration.max_nof_cu_ups},
            cu_up_connection_manager_dependencies{.cu_ups            = dependencies.cu_ups,
                                                  .cu_cp_exec        = dependencies.ctrl_exec,
                                                  .common_task_sched = dependencies.common_task_sched,
                                                  .logger            = logger}),
  xnc_mng(xnc_connection_manager_dependencies{.xnaps             = dependencies.xncs,
                                              .xnc_gws           = std::move(dependencies.xnc_gws),
                                              .timers            = dependencies.timers,
                                              .cu_cp_exec        = dependencies.ctrl_exec,
                                              .common_task_sched = dependencies.common_task_sched,
                                              .logger            = logger})
{
}

void cu_cp_controller::stop()
{
  // Note: Called from separate outer thread.
  {
    std::scoped_lock lock(mutex);
    if (!running) {
      return;
    }
  }

  // Stop and delete Xn-C connections.
  xnc_mng.stop();

  // Stop and delete DU connections.
  du_mng.stop();

  // Stop and delete CU-UP connections.
  cu_up_mng.stop();

  // Stop and delete AMF connections.
  amf_mng.stop();
}

bool cu_cp_controller::handle_du_setup_request(const std::set<plmn_identity>& plmn_ids)
{
  bool success = false;
  for (const auto& plmn : plmn_ids) {
    if (amf_mng.is_amf_connected(plmn)) {
      success = true;
    } else {
      logger.debug("No AMF for PLMN={} is connected", plmn);
    }
  }

  // If AMF is not connected, it either means that the CU-CP is not operational state, there is a CU-CP failure or no
  // AMF for the PLMN of the DU cells was found.
  return success;
}

bool cu_cp_controller::request_ue_setup() const
{
  if (amf_mng.nof_amfs() == 0) {
    return false;
  }

  if (cu_up_mng.nof_cu_ups() == 0) {
    return false;
  }

  return true;
}

bool cu_cp_controller::is_supported_plmn(const plmn_identity& plmn) const
{
  return amf_mng.is_amf_connected(plmn);
}
