// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/support/async/protocol_transaction_manager.h"

namespace ocudu::ocucp {

class ngap_ue_transaction_manager
{
public:
  ngap_ue_transaction_manager(timer_factory timers) :
    handover_preparation_outcome(timers),
    handover_cancel_outcome(timers),
    dl_ran_status_transfer_outcome(timers),
    path_switch_outcome(timers)
  {
  }

  void cancel_all()
  {
    handover_preparation_outcome.stop();
    handover_cancel_outcome.stop();
  }

  /// Handover Preparation Response/Failure Event Source.
  protocol_transaction_event_source<asn1::ngap::ho_cmd_s, asn1::ngap::ho_prep_fail_s> handover_preparation_outcome;

  /// Handover Cancel Ack Event Source.
  protocol_transaction_event_source<asn1::ngap::ho_cancel_ack_s> handover_cancel_outcome;

  /// DL RAN Status Transfer source.
  protocol_transaction_event_source<asn1::ngap::dl_ran_status_transfer_s> dl_ran_status_transfer_outcome;

  /// Path Switch Request Ack/Failure Event Source.
  protocol_transaction_event_source<asn1::ngap::path_switch_request_ack_s, asn1::ngap::path_switch_request_fail_s>
      path_switch_outcome;
};

} // namespace ocudu::ocucp
