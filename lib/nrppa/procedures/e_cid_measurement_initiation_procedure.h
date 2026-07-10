// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../nrppa_impl.h"
#include "../ue_context/nrppa_ue_context.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/cause/nrppa_cause.h"

namespace ocudu::ocucp {

/// \brief E-CID Measurement Initiation, TS 38.455 section 8.2.1.
/// The purpose of E-CID Measurement Initiation procedure is to allow the LMF to request the NG-RAN node to report E-CID
/// measurements used by LMF to compute the location of the UE.
class e_cid_measurement_initiation_procedure
{
public:
  e_cid_measurement_initiation_procedure(cu_cp_ue_index_t                           ue_index_,
                                         const nrppa_e_cid_meas_initiation_request& request_,
                                         uint16_t                                   transaction_id_,
                                         nrppa_ue_context_list&                     ue_ctxt_list_,
                                         nrppa_cu_cp_notifier&                      cu_cp_notifier_,
                                         const std::map<plmn_identity, unsigned>&   plmn_to_tac_,
                                         nrppa_impl&                                parent_,
                                         ocudulog::basic_logger&                    logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "E-CID Measurement Initiation Procedure"; }

private:
  /// \brief Setup periodic measurement for a UE.
  void setup_periodic_measurement();

  /// \brief Get measurement result for a UE.
  void get_measurement_result();

  /// \brief Creates ASN.1 E-CID measurement initiation failure.
  /// \param[in] cause The cause of the failure.
  /// return The E-CID measurement initiation failure PDU.
  asn1::nrppa::nr_ppa_pdu_c create_e_cid_measurement_initiation_failure(nrppa_cause_t cause);

  /// \brief Creates ASN.1 E-CID measurement initiation response.
  /// \param[in] on_demand True if the measurement is on-demand.
  /// return The E-CID measurement initiation response PDU.
  asn1::nrppa::nr_ppa_pdu_c create_e_cid_measurement_initiation_response(bool on_demand);

  /// \brief Fill the procedure result, log it and forward it to the CU-CP.
  void handle_procedure_outcome(bool on_demand);

  cu_cp_ue_index_t                          ue_index;
  const nrppa_e_cid_meas_initiation_request e_cid_meas_init_request;
  uint16_t                                  transaction_id;
  nrppa_ue_context_list&                    ue_ctxt_list;
  nrppa_cu_cp_notifier&                     cu_cp_notifier;
  const std::map<plmn_identity, unsigned>&  plmn_to_tac;
  nrppa_impl&                               parent;
  ocudulog::basic_logger&                   logger;

  bool is_on_demand_measurement = false;

  nrppa_ue_context* ue_ctxt = nullptr;

  expected<nrppa_e_cid_meas_result, nrppa_cause_t> e_cid_meas_results;

  asn1::nrppa::nr_ppa_pdu_c e_cid_meas_outcome;
};

} // namespace ocudu::ocucp
