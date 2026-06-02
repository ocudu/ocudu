// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_manager/ue_manager_impl.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/rrc/rrc_du.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/xnap/xnap.h"

namespace ocudu::ocucp {

/// \brief Removes a UE from the CU-CP
class ue_removal_routine
{
public:
  ue_removal_routine(cu_cp_ue_index_t                     ue_index_,
                     rrc_ue_handler*                      rrc_du_notifier_,
                     e1ap_bearer_context_removal_handler* e1ap_removal_handler_,
                     f1ap_ue_context_removal_handler*     f1ap_removal_handler_,
                     ngap_ue_context_removal_handler*     ngap_removal_handler_,
                     nrppa_ue_context_removal_handler*    nrppa_removal_handler_,
                     xnap_ue_context_removal_handler*     xnap_removal_handler_,
                     ue_manager&                          ue_mng_,
                     ocudulog::basic_logger&              logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Removal Routine"; }

private:
  const cu_cp_ue_index_t ue_index;
  // Handlers to trigger UE removal in the respective layers.
  rrc_ue_handler*                      rrc_du_notifier       = nullptr;
  e1ap_bearer_context_removal_handler* e1ap_removal_handler  = nullptr;
  f1ap_ue_context_removal_handler*     f1ap_removal_handler  = nullptr;
  ngap_ue_context_removal_handler*     ngap_removal_handler  = nullptr;
  nrppa_ue_context_removal_handler*    nrppa_removal_handler = nullptr;
  xnap_ue_context_removal_handler*     xnap_removal_handler  = nullptr;
  // To remove UE context from DU processor.
  ue_manager&             ue_mng;
  ocudulog::basic_logger& logger;
};

} // namespace ocudu::ocucp
