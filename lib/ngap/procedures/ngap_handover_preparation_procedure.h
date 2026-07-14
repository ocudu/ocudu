// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_context/ngap_ue_context.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ngap/ngap_handover.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

class ngap_handover_preparation_procedure
{
public:
  ngap_handover_preparation_procedure(const ngap_handover_preparation_request& request_,
                                      const ngap_ue_ids&                       ue_ids_,
                                      ngap_message_notifier&                   amf_notifier_,
                                      ngap_rrc_ue_notifier&                    rrc_ue_notifier_,
                                      ngap_cu_cp_notifier&                     cu_cp_notifier_,
                                      ngap_ue_transaction_manager&             ev_mng_,
                                      timer_factory                            timers,
                                      ngap_ue_logger&                          logger_);

  void operator()(coro_context<async_task<ngap_handover_preparation_response>>& ctx);

  static const char* name() { return "Handover Preparation Procedure"; }

  const ngap_handover_preparation_request request;
  const ngap_ue_ids                       ue_ids;
  ngap_message_notifier&                  amf_notifier;
  ngap_rrc_ue_notifier&                   rrc_ue_notifier;
  ngap_cu_cp_notifier&                    cu_cp_notifier;
  ngap_ue_transaction_manager&            ev_mng;
  ngap_ue_logger&                         logger;

  unique_timer tng_reloc_prep_timer;

  ngap_ue_source_handover_context ho_ue_context;

  protocol_transaction_outcome_observer<asn1::ngap::ho_cmd_s, asn1::ngap::ho_prep_fail_s> transaction_sink;
  protocol_transaction_outcome_observer<asn1::ngap::ho_cancel_ack_s>                      ho_cancel_transaction_sink;
  byte_buffer                                                                             rrc_ho_cmd_pdu;
  bool                                                                                    rrc_reconfig_success = false;

  void        get_required_handover_context();
  bool        send_handover_required();
  bool        send_handover_cancel();
  byte_buffer get_rrc_handover_command() const;

  // ASN.1 helpers
  void        fill_asn1_target_ran_node_id(asn1::ngap::target_id_c& target_id);
  void        fill_asn1_pdu_session_res_list(asn1::ngap::pdu_session_res_list_ho_rqd_l& pdu_session_res_list);
  byte_buffer fill_asn1_source_to_target_transparent_container();

  // TNG Reloc Prep timeout for Handover Preparation procedure.
  // Handover cancellation procedure will be initialized if timer fires.
  // FIXME: Set to appropriate value
  const std::chrono::milliseconds tng_reloc_prep_ms{1000};
};

} // namespace ocudu::ocucp
