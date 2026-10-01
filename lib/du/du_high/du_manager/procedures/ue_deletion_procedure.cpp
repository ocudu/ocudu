// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_deletion_procedure.h"
#include "ocudu/adt/format.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"

using namespace ocudu;
using namespace odu;

ue_deletion_procedure::ue_deletion_procedure(du_ue_index_t             ue_index_,
                                             du_ue_manager_repository& ue_mng_,
                                             const du_manager_params&  du_params_,
                                             std::chrono::milliseconds ran_res_release_timeout_) :
  ue_index(ue_index_),
  ue_mng(ue_mng_),
  du_params(du_params_),
  ran_res_release_timeout(ran_res_release_timeout_),
  proc_logger(ocudulog::fetch_basic_logger("DU-MNG"), name(), ue_index)
{
}

void ue_deletion_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  ue = ue_mng.find_ue(ue_index);
  if (ue == nullptr) {
    proc_logger.log_proc_failure("ueId does not exist.");
    CORO_EARLY_RETURN();
  }

  // > Disconnect DRBs from F1-U and SRBs from F1-C to stop handling traffic in flight and delivery notifications.
  CORO_AWAIT(stop_ue_bearer_traffic());

  // > Remove UE from F1AP.
  du_params.f1ap.ue_mng.handle_ue_deletion_request(ue_index);

  // > Remove UE from MAC.
  // Note: The UE removal from the scheduler should be done after the timeout period. Otherwise, the scheduler will
  // stop scheduling the periodic UCI/SRS grants of the UE, while the UE may keep sending UCI/SRS (remember that as per
  // TS 38.331 5.3.8.3, the UE stays active for 60msec after receiving an RRC Release). This could lead to collisions
  // between UCI/SRS and PUSCH.
  {
    CORO_AWAIT_VALUE(const mac_ue_delete_response mac_resp, launch_mac_ue_delete());
    if (not mac_resp.result) {
      proc_logger.log_proc_failure("Failed to remove UE from MAC.");
    }
  }

  // > Remove UE object from DU UE manager.
  ue_mng.remove_ue(ue_index);

  proc_logger.log_proc_completed();

  CORO_RETURN();
}

async_task<mac_ue_delete_response> ue_deletion_procedure::launch_mac_ue_delete()
{
  mac_ue_delete_request mac_msg{};
  mac_msg.ue_index          = ue->ue_index;
  mac_msg.rnti              = ue->rnti;
  mac_msg.cell_index        = ue->pcell_index;
  mac_msg.min_removal_delay = ran_res_release_timeout;
  return du_params.mac.mgr.get_ue_configurator().handle_ue_delete_request(mac_msg);
}

async_task<void> ue_deletion_procedure::stop_ue_bearer_traffic()
{
  // Note: If the DRB was not deleted on demand by the CU-CP via F1AP UE Context Modification Procedure, there is a
  // chance that the CU-UP will keep pushing new F1-U PDUs to the DU. To avoid dangling references during UE removal,
  // we start by first disconnecting the DRBs from the F1-U interface.

  return ue->handle_activity_stop_request(true);
}
