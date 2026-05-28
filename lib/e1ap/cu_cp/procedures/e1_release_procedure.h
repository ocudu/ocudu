// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp/ue_context/e1ap_cu_cp_ue_context.h"
#include "ocudu/asn1/e1ap/e1ap.h"
#include "ocudu/asn1/e1ap/e1ap_pdu_contents.h"
#include "ocudu/e1ap/common/e1ap_common.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_cp_ue_context_release.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class e1ap_cu_cp_notifier;
class e1ap_ue_context_list;

/// \brief Handle the E1 Release Procedure as per TS 38.463 section 8.2.7.
class e1_release_procedure
{
public:
  e1_release_procedure(const asn1::e1ap::e1_release_request_s& request_,
                       cu_cp_cu_up_index_t                     cu_up_index_,
                       e1ap_message_notifier&                  pdu_notifier_,
                       e1ap_cu_cp_notifier&                    cu_cp_notifier_,
                       e1ap_ue_context_list&                   ue_list_,
                       timer_factory                           timers_,
                       ocudulog::basic_logger&                 logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "E1 Release Procedure"; }
  void        send_e1_release_response();

  const asn1::e1ap::e1_release_request_s request;
  cu_cp_cu_up_index_t                    cu_up_index;
  e1ap_message_notifier&                 pdu_notifier;
  e1ap_cu_cp_notifier&                   cu_cp_notifier;
  e1ap_ue_context_list&                  ue_list;
  ocudulog::basic_logger&                logger;

  unique_timer                                                          ue_release_wait_timer;
  cu_cp_ue_context_release_request                                      release_request;
  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context>::iterator ue;
};

} // namespace ocudu::ocucp
