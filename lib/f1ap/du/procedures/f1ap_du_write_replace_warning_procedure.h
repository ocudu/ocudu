// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {

class f1ap_message_notifier;

namespace odu {

struct f1ap_du_context;

/// \brief Handles a Write-Replace Warning Request message as per TS 38.473, Section 8.5.1.
class f1ap_du_write_replace_warning_procedure
{
public:
  f1ap_du_write_replace_warning_procedure(const asn1::f1ap::write_replace_warning_request_s& msg_,
                                          const f1ap_du_context&                             ctxt_,
                                          f1ap_du_pws_notifier&                              pws_notifier_,
                                          f1ap_message_notifier&                             cu_notif_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  /// Parses the request IEs and resolves the targeted cells to local DU cell indexes.
  write_replace_warning_information build_request() const;

  void send_response();

  const asn1::f1ap::write_replace_warning_request_s request;
  const f1ap_du_context&                            ctxt;
  f1ap_du_pws_notifier&                             pws_notifier;
  f1ap_message_notifier&                            cu_notif;

  std::vector<du_cell_index_t> accepted_cells;
};

} // namespace odu
} // namespace ocudu
