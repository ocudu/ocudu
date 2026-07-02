// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/asn1/e1ap/e1ap.h"
#include "ocudu/support/async/protocol_transaction_manager.h"
#include "ocudu/support/timers.h"

namespace ocudu::ocucp {

using e1ap_outcome     = expected<asn1::e1ap::successful_outcome_s, asn1::e1ap::unsuccessful_outcome_s>;
using e1ap_transaction = protocol_transaction<e1ap_outcome>;

class e1ap_transaction_manager
{
public:
  /// Transaction Response Container, which gets indexed by transaction_id.
  static constexpr size_t                    MAX_NOF_TRANSACTIONS = 256;
  protocol_transaction_manager<e1ap_outcome> transactions;

  explicit e1ap_transaction_manager(timer_factory timers) : transactions(MAX_NOF_TRANSACTIONS, timers) {}
};

} // namespace ocudu::ocucp
