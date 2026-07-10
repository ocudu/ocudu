// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_context/nrppa_du_context.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/cause/nrppa_cause.h"
#include "ocudu/ran/positioning/positioning_messages.h"

namespace ocudu::ocucp {

/// \brief Positioning Activation, TS 38.455 section 8.2.9
/// The Positioning Activation procedure is initiated by the LMF to request the NG-RAN node to activate semi-persistent
/// or trigger aperiodic UL SRS transmission by the UE. This procedure applies only if the NG-RAN node is a gNB.
class positioning_activation_procedure
{
public:
  positioning_activation_procedure(const positioning_activation_request_t& request_,
                                   uint16_t                                transaction_id_,
                                   cu_cp_du_index_t                        du_index_,
                                   nrppa_du_context_list&                  du_ctxt_list_,
                                   nrppa_cu_cp_notifier&                   cu_cp_notifier_,
                                   ocudulog::basic_logger&                 logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Positioning Activation Procedure"; }

private:
  /// \brief Creates ASN.1 positioning activation failure.
  /// \param[in] cause The cause of the failure.
  /// return The positioning activation failure PDU.
  asn1::nrppa::nr_ppa_pdu_c create_positioning_activation_failure(nrppa_cause_t cause) const;

  /// \brief Creates ASN.1 positioning activation response.
  /// return The positioning activation response PDU.
  asn1::nrppa::nr_ppa_pdu_c create_positioning_activation_response();

  /// \brief Fill the procedure result, log it and forward it to the CU-CP.
  void handle_procedure_outcome();

  const positioning_activation_request_t pos_act_request;
  uint16_t                               transaction_id;
  cu_cp_du_index_t                       du_index;
  nrppa_du_context_list&                 du_ctxt_list;
  nrppa_cu_cp_notifier&                  cu_cp_notifier;
  ocudulog::basic_logger&                logger;

  expected<positioning_activation_response_t, positioning_activation_failure_t> procedure_outcome;

  asn1::nrppa::nr_ppa_pdu_c asn1_pos_act_outcome;
};

} // namespace ocudu::ocucp
