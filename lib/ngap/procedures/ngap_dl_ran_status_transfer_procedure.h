// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ue_context/ngap_ue_logger.h"
#include "ue_context/ngap_ue_transaction_manager.h"
#include "ocudu/ran/inter_cu_handover_messages.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// Procedure used to await for the DL status
class ngap_dl_ran_status_transfer_procedure
{
public:
  ngap_dl_ran_status_transfer_procedure(ngap_ue_transaction_manager& ev_mng, ngap_ue_logger& logger_);

  void operator()(coro_context<async_task<expected<cu_cp_status_transfer>>>& ctx);

private:
  static const char* name() { return "DL RAN Status Transfer Procedure"; }
  bool               fill_ngap_dl_ran_status_transfer();

  ngap_ue_transaction_manager& ev_mng;
  ngap_ue_logger               logger;

  cu_cp_ue_index_t      ue_index;
  cu_cp_status_transfer dl_ran_status_transfer;

  protocol_transaction_outcome_observer<asn1::ngap::dl_ran_status_transfer_s> transaction_sink;
  protocol_transaction_outcome_observer<asn1::ngap::ho_cancel_ack_s> dl_status_transfer_cancel_transaction_sink;
};

} // namespace ocudu::ocucp
