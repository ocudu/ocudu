// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../ue_manager/ue_manager_impl.h"
#include "../../xnap_repository.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief Cancels CHO if the UE never executes it after the execution timer fires.
///
/// Sends an RRCReconfiguration removing the conditional reconfig from the UE, cancels each candidate's RRC
/// reconfiguration transaction and clears the source CHO context.
class conditional_handover_cancellation_routine
{
public:
  conditional_handover_cancellation_routine(cu_cp_ue_index_t        source_ue_index,
                                            ue_manager&             ue_mng,
                                            xnap_repository*        xnap_db,
                                            ocudulog::basic_logger& logger);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CHO Cancellation Routine"; }

private:
  const cu_cp_ue_index_t  source_ue_index;
  ue_manager&             ue_mng;
  xnap_repository*        xnap_db;
  ocudulog::basic_logger& logger;

  cu_cp_ue*                             source_ue      = nullptr;
  bool                                  removal_result = false;
  rrc_reconfiguration_procedure_request removal_request;
};

} // namespace ocudu::ocucp
