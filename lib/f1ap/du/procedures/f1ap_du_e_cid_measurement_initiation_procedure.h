// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/f1ap_ue_context.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/f1ap/du/f1ap_du_positioning_handler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::odu {

class f1ap_du_ue;
class f1ap_du_ue_manager;

/// \brief Handles the E-CID Measurement Initiation procedure, as per TS 38.473 section 8.13.12.
///
/// The CU-CP asks the DU for the E-CID measurements of one UE. The DU measures the requested quantities and sends
/// the result in the E-CID MEASUREMENT INITIATION RESPONSE. The DU keeps no state after the response.
class f1ap_du_e_cid_measurement_initiation_procedure
{
public:
  f1ap_du_e_cid_measurement_initiation_procedure(const asn1::f1ap::e_c_id_meas_initiation_request_s& msg_,
                                                 f1ap_du_positioning_handler&                        du_mng_,
                                                 f1ap_du_ue_manager&                                 ues_);

  void operator()(coro_context<async_task<void>>& ctx);

  const char* name() const { return "E-CID Measurement Initiation Procedure"; }

private:
  /// \brief Reads the requested measurement quantities into \c quantities.
  ///
  /// Returns false if the DU cannot produce every requested quantity, as per TS 38.473 section 8.13.12.3.
  bool read_request();

  async_task<du_e_cid_meas_response> request_e_cid_measurement();

  void send_response() const;
  void send_failure() const;

  const asn1::f1ap::e_c_id_meas_initiation_request_s msg;
  f1ap_du_positioning_handler&                       du_mng;
  f1ap_du_ue_manager&                                ues;
  /// Identifier of the UE. The UE context can be removed while the measurement runs, so the procedure looks the UE
  /// up again instead of holding a reference to it.
  const gnb_du_ue_f1ap_id_t du_ue_id;
  /// Copy of the UE context, for the procedure logs.
  const f1ap_ue_context   ue_ctxt;
  ocudulog::basic_logger& logger;

  std::vector<e_cid_meas_quantity> quantities;
  du_e_cid_meas_response           du_result;
};

} // namespace ocudu::odu
