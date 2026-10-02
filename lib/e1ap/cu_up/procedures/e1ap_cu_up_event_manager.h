// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/e1ap/e1ap.h"
#include "ocudu/support/async/protocol_transaction_manager.h"

namespace ocudu::ocuup {

/// Response type of E1AP CU-UP transaction.
using e1ap_transaction_response = expected<asn1::e1ap::successful_outcome_s, asn1::e1ap::unsuccessful_outcome_s>;

/// E1AP protocol transaction type.
using e1ap_transaction = protocol_transaction<e1ap_transaction_response>;

class e1ap_event_manager
{
  /// Transaction Response Container, which gets indexed by transaction_id.
  static constexpr size_t MAX_NOF_TRANSACTIONS = 256;

public:
  protocol_transaction_manager<e1ap_transaction_response> transactions;

  explicit e1ap_event_manager(timer_factory timer_service) : transactions(MAX_NOF_TRANSACTIONS, timer_service) {}
};

} // namespace ocudu::ocuup
