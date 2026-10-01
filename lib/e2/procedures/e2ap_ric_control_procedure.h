// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../common/e2ap_asn1_utils.h"
#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/asn1/e2sm/e2sm_rc_ies.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_event_manager.h"
#include "ocudu/e2/e2_messages.h"
#include "ocudu/e2/e2sm/e2sm_manager.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

class e2ap_ric_control_procedure
{
public:
  e2ap_ric_control_procedure(const e2_ric_control_request& request_,
                             e2_message_notifier&          notif_,
                             e2sm_manager&                 e2sm_mng_,
                             ocudulog::basic_logger&       logger);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "E2AP RIC Control Procedure"; }

  void send_e2_ric_control_acknowledge(const e2_ric_control_request&  ctrl_request,
                                       const e2_ric_control_response& ctrl_response);
  void send_e2_ric_control_failure(const e2_ric_control_request&, const e2_ric_control_response& ctrl_response);

private:
  ocudulog::basic_logger&      logger;
  e2_message_notifier&         ric_notif;
  e2sm_manager&                e2sm_mng;
  const e2_ric_control_request e2_request;
  e2sm_ric_control_response    e2sm_response;
  e2_ric_control_response      e2_response;
  e2sm_interface*              e2sm_iface;
  e2sm_ric_control_request     ric_ctrl_req;
  e2sm_control_service*        control_service;
};
} // namespace ocudu
