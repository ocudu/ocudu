// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ue_context/ngap_ue_context.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ngap/ngap_handover.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class ngap_handover_resource_allocation_procedure
{
public:
  ngap_handover_resource_allocation_procedure(const ngap_handover_request& request_,
                                              const amf_ue_id_t            amf_ue_id_,
                                              ngap_ue_context_list&        ue_ctxt_list_,
                                              ngap_cu_cp_notifier&         cu_cp_notifier_,
                                              ngap_message_notifier&       amf_notifier_,
                                              ocudulog::basic_logger&      logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Handover Resource Allocation Procedure"; }

private:
  bool create_ngap_ue(cu_cp_ue_index_t ue_index);

  // Result senders.
  bool send_handover_request_ack(cu_cp_ue_index_t                  ue_index,
                                 ran_ue_id_t                       ran_ue_id,
                                 const cu_cp_handover_request_ack& ho_request_ack);
  void send_handover_failure(const cu_cp_handover_request_failure& ho_request_failure);

  const ngap_handover_request request;
  const amf_ue_id_t           amf_ue_id;
  ngap_ue_context_list&       ue_ctxt_list;
  ngap_cu_cp_notifier&        cu_cp_notifier;
  ngap_message_notifier&      amf_notifier;
  ocudulog::basic_logger&     logger;

  // (sub-)routine results
  cu_cp_handover_resource_allocation_response response;
};

} // namespace ocudu::ocucp
