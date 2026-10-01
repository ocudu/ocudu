// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/support/async/protocol_transaction_manager.h"

namespace ocudu {
namespace ocucp {

class ngap_transaction_manager
{
public:
  ngap_transaction_manager(timer_factory timers) : ng_setup_outcome(timers), ng_reset_outcome(timers) {}

  /// NG Setup Response/Failure Event Source.
  protocol_transaction_event_source<asn1::ngap::ng_setup_resp_s, asn1::ngap::ng_setup_fail_s> ng_setup_outcome;

  /// NG Reset Acknowledge Event Source.
  protocol_transaction_event_source<asn1::ngap::ng_reset_ack_s> ng_reset_outcome;
};

} // namespace ocucp
} // namespace ocudu
