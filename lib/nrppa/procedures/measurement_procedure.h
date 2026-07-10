// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_context/nrppa_du_context.h"
#include "../meas_context/nrppa_meas_context.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/cause/nrppa_cause.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/positioning/positioning_ids.h"
#include "ocudu/ran/positioning/positioning_messages.h"

namespace ocudu::ocucp {

/// \brief Measurement, TS 38.455 section 8.5.1.
/// The Measurement procedure allows the LMF to request one or more TRPs in the NG-RAN node to perform and report
/// positioning measurements. This procedure applies only if the NG-RAN node is a gNB.
class measurement_procedure
{
public:
  measurement_procedure(cu_cp_amf_index_t                           amf_index_,
                        const measurement_request_t&                request_,
                        uint16_t                                    transaction_id_,
                        const std::map<trp_id_t, cu_cp_du_index_t>& trp_id_to_du_idx_,
                        nrppa_meas_context_list&                    meas_ctxt_list_,
                        nrppa_du_context_list&                      du_ctxt_list_,
                        nrppa_cu_cp_notifier&                       cu_cp_notifier_,
                        ocudulog::basic_logger&                     logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Measurement Information Transfer Procedure"; }

private:
  /// \brief Creates ASN.1 measurement failure.
  /// \param[in] cause The cause of the failure.
  /// return The measurement failure PDU.
  asn1::nrppa::nr_ppa_pdu_c create_measurement_failure(nrppa_cause_t cause) const;

  /// \brief Creates ASN.1 measurement response.
  /// return The measurement response PDU.
  asn1::nrppa::nr_ppa_pdu_c create_measurement_response();

  /// \brief Create the measurement context for this measurement request.
  bool create_measurement_context();

  /// \brief Prepare the DU measurement information requests.
  bool prepare_du_measurement_information_requests();

  /// \brief Handle the outcome of the DU measurement information.
  void handle_du_measurement_information_outcome(cu_cp_du_index_t du_index);

  /// \brief Fill the procedure result, log it and forward it to the CU-CP.
  void handle_procedure_outcome();

  cu_cp_amf_index_t                           amf_index;
  const measurement_request_t                 meas_request;
  uint16_t                                    transaction_id;
  const std::map<trp_id_t, cu_cp_du_index_t>& trp_id_to_du_idx;
  nrppa_meas_context_list&                    meas_ctxt_list;
  nrppa_du_context_list&                      du_ctxt_list;
  nrppa_cu_cp_notifier&                       cu_cp_notifier;
  ocudulog::basic_logger&                     logger;

  ran_meas_id_t ran_meas_id = ran_meas_id_t::min;

  std::map<ocudu::cu_cp_du_index_t, ocudu::ocucp::measurement_request_t>::iterator du_request_it;
  std::map<cu_cp_du_index_t, measurement_request_t>                                du_meas_requests;
  cu_cp_du_index_t                                                                 sub_procedure_du_index;
  measurement_request_t                                                            sub_procedure_meas_request;
  expected<measurement_response_t, measurement_failure_t>                          du_meas_outcome;

  std::optional<measurement_response_t> procedure_outcome = std::nullopt;

  asn1::nrppa::nr_ppa_pdu_c meas_outcome;
};

} // namespace ocudu::ocucp
