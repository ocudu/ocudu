// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief Requests a measurement gap from the DU for the location measurements of a UE, and sends the gap to the UE.
///
/// The CU-CP forwards the LocationMeasurementInfo of the UE to the DU in the CU to DU RRC Information (TS 38.473
/// section 9.3.1.25). The DU returns the MeasGapConfig in the DU to CU RRC Information (TS 38.473 section 9.3.1.26),
/// and the CU-CP sends it to the UE in an RRC Reconfiguration.
class location_measurement_indication_routine
{
public:
  location_measurement_indication_routine(cu_cp_ue_index_t         ue_index_,
                                          byte_buffer              location_meas_info_,
                                          f1ap_ue_context_manager& f1ap_ue_ctxt_mng_,
                                          rrc_ue_interface&        rrc_ue_,
                                          ocudulog::basic_logger&  logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Location Measurement Indication Routine"; }

private:
  const cu_cp_ue_index_t   ue_index;
  const byte_buffer        location_meas_info;
  f1ap_ue_context_manager& f1ap_ue_ctxt_mng;
  rrc_ue_interface&        rrc_ue;
  ocudulog::basic_logger&  logger;

  // (sub-)routine requests
  f1ap_ue_context_modification_request  ue_context_mod_request;
  rrc_reconfiguration_procedure_request rrc_reconfig_args;

  // (sub-)routine results
  f1ap_ue_context_modification_response ue_context_mod_response;
  bool                                  rrc_reconfig_result = false;
};

} // namespace ocudu::ocucp
