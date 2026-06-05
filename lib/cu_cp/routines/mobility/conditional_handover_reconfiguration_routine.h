// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../cu_cp_impl_interface.h"
#include "../../mobility_manager/mobility_manager_impl.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/async_task.h"
#include <chrono>
#include <optional>

namespace ocudu::ocucp {

/// \brief Request for CHO reconfiguration routine.
struct cu_cp_cho_reconfiguration_request {
  cu_cp_ue_index_t          source_ue_index = cu_cp_ue_index_t::invalid;
  std::chrono::milliseconds timeout         = std::chrono::milliseconds{10000};
  /// Runtime override for the T1 conditional event threshold. Replaces the configured value when set.
  std::optional<std::chrono::system_clock::time_point> t1_thres_override;
};

/// \brief Handles the reconfiguration phase of Conditional Handover (CHO).
///
/// This routine sends the RRCReconfiguration with conditionalReconfiguration-r16
/// to the source UE and waits for the initial acknowledgment (RRCReconfigurationComplete
/// on source). After acknowledgment, the UE will evaluate conditions and eventually
/// attach to one of the target cells.
///
/// Steps:
/// 1. Request measGapConfig from source DU via UE Context Modification (need_for_gap=true).
/// 2. Build RRCReconfiguration with conditionalReconfiguration-r16 for all candidates,
///    including the measGapConfig returned by the source DU for inter-frequency CHO.
/// 3. Notify CU-CP to start conditional_handover_target_routine on each target UE.
/// 4. Send via F1AP UE Context Modification without stopping source-UE data transmission.
/// 5. Wait for RRCReconfigurationComplete on SOURCE UE (acknowledgment).
/// 6. Set CHO state to executing.
class conditional_handover_reconfiguration_routine
{
public:
  conditional_handover_reconfiguration_routine(const cu_cp_cho_reconfiguration_request& request_,
                                               cu_cp_ue&                                source_ue_,
                                               f1ap_ue_context_manager&                 source_du_f1ap_ue_ctxt_mng_,
                                               cu_cp_ue_context_manipulation_handler&   cu_cp_handler_,
                                               cu_cp_ue_context_release_handler&        ue_context_release_handler_,
                                               ue_manager&                              ue_mng_,
                                               mobility_manager&                        mobility_mng_,
                                               ocudulog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "CHO Reconfiguration Routine"; }

private:
  /// \brief Build the RRC message with conditional reconfiguration for all candidates.
  /// \param[in] source_du_meas_gap_cfg measGapConfig returned by the source DU; included in the
  ///            outer CHO measConfig when candidates are on different ARFCNs.
  /// \return Packed RRC message on success, empty buffer on failure.
  byte_buffer build_conditional_reconfiguration_message(const byte_buffer& source_du_meas_gap_cfg);

  /// \brief Generate an F1AP UE Context Modification request asking the source DU to configure
  ///        measurement gaps and return the resulting measGapConfig.
  void generate_gap_request();

  /// \brief Generate the F1AP UE Context Modification request carrying the CHO RRCReconfiguration.
  void generate_ue_context_modification_request();

  /// \brief Release prepared CHO candidates and clear CHO context after failures.
  void cleanup_targets();

  const cu_cp_cho_reconfiguration_request request;

  cu_cp_ue&                              source_ue;
  f1ap_ue_context_manager&               source_du_f1ap_ue_ctxt_mng;
  cu_cp_ue_context_manipulation_handler& cu_cp_handler;
  cu_cp_ue_context_release_handler&      ue_context_release_handler;
  ue_manager&                            ue_mng;
  mobility_manager&                      mobility_mng;
  ocudulog::basic_logger&                logger;

  // RRC reconfiguration context
  uint8_t                     transaction_id = 0;
  byte_buffer                 rrc_container;
  std::optional<rrc_meas_cfg> cho_meas_result; ///< CHO measConfig sent to the UE; stored for post-ack context update.

  // (sub-)routine requests
  f1ap_ue_context_modification_request ue_context_mod_request;

  // (sub-)routine results
  f1ap_ue_context_modification_response gap_response;
  f1ap_ue_context_modification_response ue_context_mod_response;
  bool                                  reconfig_result = false;
};

} // namespace ocudu::ocucp
