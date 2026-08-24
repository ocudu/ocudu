// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ue_context/ngap_ue_context.h"
#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ran/inter_cu_handover_messages.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// Procedure used to request Path Switch from AMF.
class ngap_path_switch_procedure
{
public:
  ngap_path_switch_procedure(const cu_cp_path_switch_request& request_,
                             ngap_ue_context&                 ue_ctxt_,
                             ngap_ue_context_list&            ue_ctxt_list_,
                             ngap_message_notifier&           amf_notifier_);

  void operator()(coro_context<async_task<cu_cp_path_switch_response>>& ctx);

private:
  static const char*         name() { return "Path Switch Procedure"; }
  bool                       send_path_switch_request();
  cu_cp_path_switch_response handle_successful_outcome(const asn1::ngap::path_switch_request_ack_s& asn1_response);

  /// \brief Convert the Path Switch Request Ack from ASN.1 to common type.
  /// \param[out] res The common type struct to store the result.
  /// \param[in] asn1_response The ASN.1 type Path Switch Request Ack.
  /// \return true if conversion is successful, false otherwise.
  bool asn1_to_path_switch_request_ack(cu_cp_path_switch_request_ack&               res,
                                       const asn1::ngap::path_switch_request_ack_s& asn1_response) const;

  /// \brief Convert the Path Switch Request Failure from ASN.1 to common type.
  /// \param[in] asn1_fail The ASN.1 type Path Switch Request Failure.
  /// \return The converted common type Path Switch Request Failure.
  cu_cp_path_switch_request_failure
  asn1_to_path_switch_request_failure(const asn1::ngap::path_switch_request_fail_s& asn1_fail) const;

  const cu_cp_path_switch_request request;
  ngap_ue_transaction_manager&    ev_mng; // only accessed before the first suspension, to subscribe to the outcome
  ngap_ue_context_list&           ue_ctxt_list;
  ngap_message_notifier&          amf_notifier;
  // Copies of the UE identifiers and logger. The NGAP UE context can be removed while this procedure is suspended, so
  // the procedure must not hold references into it.
  const ngap_ue_ids ue_ids;
  ngap_ue_logger    logger;

  cu_cp_path_switch_response procedure_response;

  protocol_transaction_outcome_observer<asn1::ngap::path_switch_request_ack_s, asn1::ngap::path_switch_request_fail_s>
      transaction_sink;
};

} // namespace ocudu::ocucp
