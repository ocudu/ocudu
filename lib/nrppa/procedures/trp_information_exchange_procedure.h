// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_context/nrppa_du_context.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/cause/nrppa_cause.h"
#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

/// \brief TRP Information Exchange, TS 38.455 section 8.2.8.
/// The purpose of the TRP Information Exchange procedure is to allow the LMF to request the NG-RAN node to provide
/// detailed information for TRPs hosted by the NG-RAN node. This procedure applies only if the NG-RAN node is a gNB.
class trp_information_exchange_procedure
{
public:
  trp_information_exchange_procedure(cu_cp_amf_index_t                     amf_index_,
                                     const trp_information_request_t&      request_,
                                     uint16_t                              transaction_id_,
                                     nrppa_cu_cp_notifier&                 cu_cp_notifier_,
                                     std::map<trp_id_t, cu_cp_du_index_t>& trp_id_to_du_idx_,
                                     nrppa_du_context_list&                du_ctxt_list_,
                                     ocudulog::basic_logger&               logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "TRP Information Exchange Procedure"; }

private:
  /// \brief Creates ASN.1 TRP info failure.
  /// \param[in] cause The cause of the failure.
  /// return The TRP info failure PDU.
  asn1::nrppa::nr_ppa_pdu_c create_trp_info_failure(nrppa_cause_t cause) const;

  /// \brief Creates ASN.1 TRP info response.
  /// return The TRP info response PDU.
  asn1::nrppa::nr_ppa_pdu_c create_trp_info_response();

  /// \brief Fill the procedure result, log it and forward it to the CU-CP.
  void handle_procedure_outcome();

  /// \brief Send the TRP info outcome to the CU-CP.
  void send_ul_nrppa_pdu(const asn1::nrppa::nr_ppa_pdu_c& pdu);

  cu_cp_amf_index_t                     amf_index;
  const trp_information_request_t       trp_info_request;
  uint16_t                              transaction_id;
  nrppa_cu_cp_notifier&                 cu_cp_notifier;
  std::map<trp_id_t, cu_cp_du_index_t>& trp_id_to_du_idx;
  nrppa_du_context_list&                du_ctxt_list;
  ocudulog::basic_logger&               logger;

  trp_information_cu_cp_response_t cu_cp_response;
  asn1::nrppa::nr_ppa_pdu_c        trp_info_outcome;
  byte_buffer                      ul_nrppa_pdu;
};

} // namespace ocudu::ocucp
