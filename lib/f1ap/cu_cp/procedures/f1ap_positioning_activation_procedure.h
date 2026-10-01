// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/f1ap_cu_ue_context.h"
#include "ocudu/f1ap/cu_cp/f1ap_configuration.h"

namespace ocudu::ocucp {

/// \brief Positioning Activation, TS 38.473 section 8.13.10.
/// The Positioning Activation procedure is initiated by the gNB-CU to request the gNB-DU to activate semi-persistent or
/// trigger aperiodic UL SRS transmission by the UE. The procedure uses UE-associated signalling.
class f1ap_positioning_activation_procedure
{
public:
  f1ap_positioning_activation_procedure(const f1ap_configuration&               f1ap_cfg_,
                                        const positioning_activation_request_t& request_,
                                        f1ap_ue_context&                        ue_ctxt_,
                                        f1ap_message_notifier&                  f1ap_notif_,
                                        ocudulog::basic_logger&                 logger_);

  void operator()(
      coro_context<async_task<expected<positioning_activation_response_t, positioning_activation_failure_t>>>& ctx);

  static const char* name() { return "Positioning Activation Procedure"; }

private:
  /// Send F1 Positioning Activation Request to DU.
  bool send_positioning_activation_request();

  /// Creates procedure result to send back to procedure caller.
  expected<positioning_activation_response_t, positioning_activation_failure_t> create_positioning_activation_result();

  const f1ap_configuration&              f1ap_cfg;
  const positioning_activation_request_t request;
  f1ap_ue_context&                       ue_ctxt;
  f1ap_message_notifier&                 f1ap_notifier;
  ocudulog::basic_logger&                logger;

  protocol_transaction_outcome_observer<asn1::f1ap::positioning_activation_resp_s,
                                        asn1::f1ap::positioning_activation_fail_s>
      transaction_sink;
};

} // namespace ocudu::ocucp
