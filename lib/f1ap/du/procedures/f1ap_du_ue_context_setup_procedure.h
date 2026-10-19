// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../f1ap_du_context.h"
#include "../ue_context/f1ap_du_ue.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"

namespace ocudu::odu {

class f1ap_du_ue_manager;

/// This procedure handles UE CONTEXT SETUP REQUEST as per TS38.473, Section 8.3.1.
class f1ap_du_ue_context_setup_procedure
{
public:
  /// Creates a setup procedure bound to the current CU-CP association.
  f1ap_du_ue_context_setup_procedure(const asn1::f1ap::ue_context_setup_request_s& msg,
                                     f1ap_du_ue_manager&                           ue_mng_,
                                     f1ap_du_configurator&                         du_mng_,
                                     du_ue_index_t                                 ue_index_,
                                     const f1ap_du_context&                        ctxt_,
                                     f1ap_message_notifier&                        tx_pdu_notifier_);

  /// Validates the SpCell and establishes the UE context while the CU-CP association remains valid.
  void operator()(coro_context<async_task<void>>& ctx);

private:
  /// Returns whether the CU-CP association that received the request is still active.
  bool is_cu_cp_association_valid() const;

  /// Initiates UE Configuration in the DU.
  async_task<f1ap_ue_context_update_response> request_du_ue_config();

  /// Sends UE Context Setup Response to CU.
  void send_ue_context_setup_response();

  /// Sends UE Context Setup Failure to CU.
  ///
  /// \param[in] cause Cause value reported to the CU, defaults to no radio resources available.
  void send_ue_context_setup_failure(asn1::f1ap::cause_radio_network_opts::options cause =
                                         asn1::f1ap::cause_radio_network_opts::no_radio_res_available);

  /// Sends UE Context Setup Failure to CU and removes the UE, if created by this procedure.
  async_task<void> handle_procedure_failure();

  /// Handles the RRC container.
  async_task<bool> handle_rrc_container();

  /// Gets the cell index that matches the given NR-CGI from the F1AP DU context.
  expected<du_cell_index_t> get_cell_index_from_nr_cgi(nr_cell_global_id_t nr_cgi) const;

  /// Returns the name of this procedure.
  static const char* name() { return "UE Context Setup"; }

  const asn1::f1ap::ue_context_setup_request_s msg;
  f1ap_du_ue_manager&                          ue_mng;
  f1ap_du_configurator&                        du_mng;
  f1ap_message_notifier&                       tx_pdu_notifier;
  const du_ue_index_t                          ue_index;
  ocudulog::basic_logger&                      logger;

  f1ap_du_ue* ue = nullptr;

  const f1ap_du_context& du_ctxt;
  /// CU-CP association generation captured when the request is scheduled.
  const unsigned            initial_cu_cp_assoc_gen;
  expected<du_cell_index_t> sp_cell_index = make_unexpected(default_error_t());

  std::optional<f1ap_ue_context_creation_response> du_ue_create_response;
  f1ap_ue_context_update_response                  du_ue_cfg_response;
  std::vector<f1ap_drb_failed_to_setupmod>         failed_drbs;
  bool                                             rrc_container_delivered = false;
};
} // namespace ocudu::odu
