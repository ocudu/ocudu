// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/f1ap_du_ue.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"

namespace ocudu {
namespace odu {

struct f1ap_du_context;

class f1ap_du_ue_context_modification_procedure
{
public:
  f1ap_du_ue_context_modification_procedure(const asn1::f1ap::ue_context_mod_request_s& msg,
                                            f1ap_du_ue&                                 ue_,
                                            const f1ap_du_context&                      ctxt_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "UE Context Modification"; }

  void create_du_request(const asn1::f1ap::ue_context_mod_request_s& msg);
  void send_ue_context_modification_response();
  void send_ue_context_modification_failure();

  async_task<bool> handle_rrc_container();
  async_task<void> handle_tx_action_indicator();

  const asn1::f1ap::ue_context_mod_request_s req;
  f1ap_du_ue&                                ue;
  const f1ap_du_context&                     du_ctxt;
  f1ap_ue_context_update_request             du_request;

  f1ap_ue_context_update_response du_response;

  ocudulog::basic_logger& logger;
};

} // namespace odu
} // namespace ocudu
