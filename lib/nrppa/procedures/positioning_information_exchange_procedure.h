// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_context/nrppa_du_context.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/cu_cp/positioning_messages.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/cause/nrppa_cause.h"

namespace ocudu::ocucp {

/// \brief Positioning Information Exchange, TS 38.455 section 8.2.6.
/// The Positioning Information Exchange procedure is initiated by the LMF to request to the NG-RAN node positioning
/// information for the UE. This procedure applies only if the NG-RAN node is a gNB.
class positioning_information_exchange_procedure
{
public:
  positioning_information_exchange_procedure(const positioning_information_request_t& request_,
                                             uint16_t                                 transaction_id_,
                                             cu_cp_du_index_t                         du_index_,
                                             nrppa_du_context_list&                   du_ctxt_list_,
                                             nrppa_cu_cp_notifier&                    cu_cp_notifier_,
                                             ocudulog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Positioning Information Exchange Procedure"; }

private:
  /// \brief Creates ASN.1 positioning information failure.
  /// \param[in] cause The cause of the failure.
  /// return The positioning information failure PDU.
  asn1::nrppa::nr_ppa_pdu_c create_positioning_information_failure(nrppa_cause_t cause) const;

  /// \brief Creates ASN.1 positioning information response.
  /// return The positioning information response PDU.
  asn1::nrppa::nr_ppa_pdu_c create_positioning_information_response();

  /// \brief Fill the procedure result, log it and forward it to the CU-CP.
  void handle_procedure_outcome();

  /// \brief Send the positioning information exchange outcome to the CU-CP.
  void send_ul_nrppa_pdu(const asn1::nrppa::nr_ppa_pdu_c& pos_info_outcome);

  const positioning_information_request_t pos_info_request;
  uint16_t                                transaction_id;
  cu_cp_du_index_t                        du_index;
  nrppa_du_context_list&                  du_ctxt_list;
  nrppa_cu_cp_notifier&                   cu_cp_notifier;
  ocudulog::basic_logger&                 logger;

  expected<positioning_information_response_t, positioning_information_failure_t> procedure_outcome;

  asn1::nrppa::nr_ppa_pdu_c asn1_pos_info_outcome;
  byte_buffer               ul_nrppa_pdu;
};

} // namespace ocudu::ocucp
