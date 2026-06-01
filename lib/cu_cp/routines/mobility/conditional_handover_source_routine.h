// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../ue_manager/ue_manager_impl.h"
#include "../../xnap_repository.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief Cancels non-winning CHO candidates upon ACCESS SUCCESS.
///
/// Runs on the source UE's task scheduler concurrently with the target routine. Responsible for source-side cleanup:
/// identifying the winner, cancelling non-winning candidates' RRC reconfiguration transactions, and clearing the source
/// CHO context.
class conditional_handover_source_routine
{
public:
  conditional_handover_source_routine(const cu_cp_access_success_indication& msg_,
                                      ue_manager&                            ue_mng_,
                                      xnap_repository*                       xnap_db_,
                                      ocudulog::basic_logger&                logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CHO Source Routine"; }

private:
  const cu_cp_access_success_indication msg;

  ue_manager&             ue_mng;
  xnap_repository*        xnap_db;
  ocudulog::basic_logger& logger;

  cu_cp_ue*            source_ue = nullptr;
  cu_cp_cho_candidate* winner    = nullptr;
};

} // namespace ocudu::ocucp
