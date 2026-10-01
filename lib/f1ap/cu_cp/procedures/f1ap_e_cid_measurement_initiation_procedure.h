// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/f1ap_cu_ue_context.h"
#include "ocudu/f1ap/cu_cp/f1ap_configuration.h"
#include "ocudu/ran/positioning/e_cid_measurement.h"

namespace ocudu::ocucp {

/// \brief E-CID Measurement Initiation, TS 38.473 section 8.13.12.
/// The E-CID Measurement Initiation procedure is initiated by the gNB-CU to request the gNB-DU to perform E-CID
/// measurements for a UE. The procedure uses UE-associated signalling.
class f1ap_e_cid_measurement_initiation_procedure
{
public:
  f1ap_e_cid_measurement_initiation_procedure(const f1ap_configuration&          f1ap_cfg_,
                                              const e_cid_measurement_request_t& request_,
                                              f1ap_ue_context&                   ue_ctxt_,
                                              f1ap_message_notifier&             f1ap_notif_,
                                              ocudulog::basic_logger&            logger_);

  void operator()(coro_context<async_task<expected<e_cid_measurement_response_t, e_cid_measurement_failure_t>>>& ctx);

  static const char* name() { return "E-CID Measurement Initiation Procedure"; }

private:
  /// \brief Send F1 E-CID Measurement Initiation Request to DU.
  /// \returns True if the request was sent, false if it cannot be encoded.
  bool send_e_cid_measurement_initiation_request();

  /// Creates procedure result to send back to procedure caller.
  expected<e_cid_measurement_response_t, e_cid_measurement_failure_t> create_e_cid_measurement_result();

  const f1ap_configuration&         f1ap_cfg;
  const e_cid_measurement_request_t request;
  f1ap_ue_context&                  ue_ctxt;
  f1ap_message_notifier&            f1ap_notifier;
  ocudulog::basic_logger&           logger;

  protocol_transaction_outcome_observer<asn1::f1ap::e_c_id_meas_initiation_resp_s,
                                        asn1::f1ap::e_c_id_meas_initiation_fail_s>
      transaction_sink;
};

} // namespace ocudu::ocucp
