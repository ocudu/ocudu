// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_sn_status_transfer_procedure.h"
#include "sn_status_transfer_asn1_helpers.h"
#include <chrono>

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::xnap;

xnap_sn_status_transfer_procedure::xnap_sn_status_transfer_procedure(
    std::chrono::milliseconds                                            procedure_timeout_,
    protocol_transaction_event_source<asn1::xnap::sn_status_transfer_s>& sn_status_transfer_outcome_,
    xnap_ue_logger&                                                      logger_) :
  procedure_timeout(procedure_timeout_), sn_status_transfer_outcome(sn_status_transfer_outcome_), logger(logger_)
{
}

void xnap_sn_status_transfer_procedure::operator()(coro_context<async_task<expected<cu_cp_status_transfer>>>& ctx)
{
  CORO_BEGIN(ctx);
  logger.log_info("\"{}\" started...", name());
  transaction_sink.subscribe_to(sn_status_transfer_outcome, procedure_timeout);

  CORO_AWAIT(transaction_sink);

  if (transaction_sink.timeout_expired()) {
    logger.log_warning("\"{}\" timed out after {}ms", name(), procedure_timeout.count());
    CORO_EARLY_RETURN(make_unexpected(default_error_t{}));
  }

  if (not transaction_sink.successful()) {
    logger.log_warning("\"{}\" failed", name());
    CORO_EARLY_RETURN(make_unexpected(default_error_t{}));
  }

  sn_status_transfer.ue_index = ue_index;
  if (not asn1_to_sn_status_transfer(sn_status_transfer, transaction_sink.response())) {
    logger.log_warning("\"{}\" failed", name());
    CORO_EARLY_RETURN(make_unexpected(default_error_t{}));
  }

  logger.log_info("\"{}\" finished successfully", name());
  CORO_RETURN(sn_status_transfer);
}
