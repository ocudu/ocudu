// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../xnap_context.h"
#include "../xnap_tx_pdu_notifier_with_log.h"
#include "ngran_node_cfg_update_asn1_helpers.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/protocol_transaction_manager.h"
#include "ocudu/xnap/xnap_configuration.h"

namespace ocudu::ocucp {

/// \brief Reports the cells this node serves to an Xn-C peer, as defined in TS 38.423 section 8.4.1.
class ngran_node_cfg_update_procedure
{
public:
  using cfg_update_event_source =
      protocol_transaction_event_source<asn1::xnap::ngran_node_cfg_upd_ack_s, asn1::xnap::ngran_node_cfg_upd_fail_s>;

  ngran_node_cfg_update_procedure(const xnap_configuration&            xnap_cfg_,
                                  std::vector<cu_cp_served_cell_info>  served_cells_,
                                  std::vector<cu_cp_served_cell_info>& advertised_cells_,
                                  const std::optional<xnap_context>&   peer_ctxt_,
                                  xnap_tx_pdu_notifier_with_logging&   tx_notifier_,
                                  cfg_update_event_source&             cfg_update_outcome_,
                                  ocudulog::basic_logger&              logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "NG-RAN Node Configuration Update Procedure"; }

private:
  const xnap_configuration&            xnap_cfg;
  std::vector<cu_cp_served_cell_info>  served_cells;
  std::vector<cu_cp_served_cell_info>& advertised_cells;
  const std::optional<xnap_context>&   peer_ctxt;
  xnap_tx_pdu_notifier_with_logging&   tx_notifier;
  cfg_update_event_source&             cfg_update_outcome;
  ocudulog::basic_logger&              logger;

  xnap_served_cells_update update;

  protocol_transaction_outcome_observer<asn1::xnap::ngran_node_cfg_upd_ack_s, asn1::xnap::ngran_node_cfg_upd_fail_s>
      transaction_sink;
};

} // namespace ocudu::ocucp
