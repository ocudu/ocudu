// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_manager/ue_manager_impl.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/eager_async_task.h"

namespace ocudu {
namespace ocucp {

/// \brief Routine to handle the request to the AMF to remove the UE context from the CU-CP.
///
/// If the UE does not have an NG context, this procedure fallbacks to a removal of the UE context from the DU, CU-UP
/// and CU-CP without AMF involvement.
class ue_amf_context_release_request_routine
{
public:
  ue_amf_context_release_request_routine(const cu_cp_ue_context_release_request& request_,
                                         ngap_control_message_handler*           ng_release_handler_,
                                         cu_cp_ue_context_release_handler&       cu_cp_ue_ctxt_release_handler_,
                                         ocudulog::basic_logger&                 logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const cu_cp_ue_context_release_request request;
  ngap_control_message_handler*          ng_release_handler = nullptr;
  cu_cp_ue_context_release_handler&      cu_cp_ue_ctxt_release_handler;
  ocudulog::basic_logger&                logger;

  bool amf_ue_release_requested = false;
};

} // namespace ocucp
} // namespace ocudu
