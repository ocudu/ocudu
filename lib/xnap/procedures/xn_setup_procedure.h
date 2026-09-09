// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../xnap_context.h"
#include "../xnap_tx_pdu_notifier_with_log.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/async/protocol_transaction_manager.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_configuration.h"
#include "ocudu/xnap/xnap_message.h"

namespace ocudu::ocucp {

class xn_setup_procedure
{
public:
  xn_setup_procedure(
      const xnap_configuration&                                                                    xnap_cfg_,
      span<const cu_cp_served_cell_info>                                                           served_cells_,
      std::optional<xnap_context>&                                                                 peer_ctxt_,
      xnap_tx_pdu_notifier_with_logging&                                                           tx_notifier_,
      protocol_transaction_event_source<asn1::xnap::xn_setup_resp_s, asn1::xnap::xn_setup_fail_s>& xn_setup_outcome_,
      timer_factory                                                                                timers_,
      ocudulog::basic_logger&                                                                      logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "XN Setup Procedure"; }

private:
  /// Checks whether the XNAP should attempt again to connect to Xn-C peer.
  bool retry_required();

  static bool is_failure_misconfiguration(const asn1::xnap::cause_c& cause);

  const xnap_configuration&                                                                    xnap_cfg;
  span<const cu_cp_served_cell_info>                                                           served_cells;
  std::optional<xnap_context>&                                                                 peer_ctxt;
  xnap_tx_pdu_notifier_with_logging&                                                           tx_notifier;
  protocol_transaction_event_source<asn1::xnap::xn_setup_resp_s, asn1::xnap::xn_setup_fail_s>& xn_setup_outcome;
  ocudulog::basic_logger&                                                                      logger;

  unique_timer xn_setup_wait_timer;

  std::chrono::milliseconds time_to_wait{0};

  xnap_message                                            xn_setup_req;
  asn1::xnap::xn_setup_resp_s                             received_xn_setup_resp;
  error_type<std::pair<asn1::xnap::cause_c, std::string>> validation_error;

  protocol_transaction_outcome_observer<asn1::xnap::xn_setup_resp_s, asn1::xnap::xn_setup_fail_s> transaction_sink;
};

} // namespace ocudu::ocucp
