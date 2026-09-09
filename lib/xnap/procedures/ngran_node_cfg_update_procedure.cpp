// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngran_node_cfg_update_procedure.h"
#include "../xnap_asn1_utils.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::xnap;

/// \brief Determine the cell changes to report, by comparing the cells this node serves against the ones the Xn-C peer
/// was last told about.
/// \param[in] advertised_cells The cells this node advertised to the Xn-C peer.
/// \param[in] served_cells The cells this node currently serves.
/// \return The changes to report to the Xn-C peer.
static xnap_served_cells_update create_served_cells_update(span<const cu_cp_served_cell_info> advertised_cells,
                                                           span<const cu_cp_served_cell_info> served_cells)
{
  auto find_cell = [](span<const cu_cp_served_cell_info> cells, const nr_cell_global_id_t& cgi) {
    return std::find_if(
        cells.begin(), cells.end(), [&cgi](const cu_cp_served_cell_info& cell) { return cell.nr_cgi == cgi; });
  };

  xnap_served_cells_update update;
  for (const auto& served_cell : served_cells) {
    auto advertised_it = find_cell(advertised_cells, served_cell.nr_cgi);
    if (advertised_it == advertised_cells.end()) {
      update.cells_to_add.push_back(served_cell);
    } else if (*advertised_it != served_cell) {
      update.cells_to_modify.push_back(served_cell);
    }
  }

  for (const auto& advertised_cell : advertised_cells) {
    if (find_cell(served_cells, advertised_cell.nr_cgi) == served_cells.end()) {
      update.cells_to_delete.push_back(advertised_cell.nr_cgi);
    }
  }

  return update;
}

ngran_node_cfg_update_procedure::ngran_node_cfg_update_procedure(const xnap_configuration&            xnap_cfg_,
                                                                 std::vector<cu_cp_served_cell_info>  served_cells_,
                                                                 std::vector<cu_cp_served_cell_info>& advertised_cells_,
                                                                 const std::optional<xnap_context>&   peer_ctxt_,
                                                                 xnap_tx_pdu_notifier_with_logging&   tx_notifier_,
                                                                 cfg_update_event_source& cfg_update_outcome_,
                                                                 ocudulog::basic_logger&  logger_) :
  xnap_cfg(xnap_cfg_),
  served_cells(std::move(served_cells_)),
  advertised_cells(advertised_cells_),
  peer_ctxt(peer_ctxt_),
  tx_notifier(tx_notifier_),
  cfg_update_outcome(cfg_update_outcome_),
  logger(logger_)
{
}

void ngran_node_cfg_update_procedure::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not peer_ctxt.has_value()) {
    // The cells are reported in the XN Setup Request/Response of the pending XN setup.
    CORO_EARLY_RETURN(false);
  }

  update = create_served_cells_update(advertised_cells, served_cells);
  if (update.empty()) {
    CORO_EARLY_RETURN(true);
  }

  logger.info("\"{}\" started...", name());

  // Subscribe to the publisher of the NG-RAN Node Configuration Update Acknowledge/Failure.
  transaction_sink.subscribe_to(cfg_update_outcome, xnap_cfg.procedure_timeout);

  if (not tx_notifier.on_new_message(generate_asn1_ngran_node_cfg_update(update))) {
    logger.warning("\"{}\" failed. Cause: Cannot send NGRANNodeConfigurationUpdate", name());
    CORO_EARLY_RETURN(false);
  }

  CORO_AWAIT(transaction_sink);

  if (transaction_sink.cancelled()) {
    logger.info("\"{}\" aborted. Cause: The Xn-C connection is being torn down", name());
    CORO_EARLY_RETURN(false);
  }

  if (transaction_sink.timeout_expired()) {
    logger.warning("\"{}\" timed out after {}ms", name(), xnap_cfg.procedure_timeout.count());
    CORO_EARLY_RETURN(false);
  }

  if (transaction_sink.failed()) {
    logger.warning("\"{}\" failed. Cause: Xn-C peer responded with \"{}\"",
                   name(),
                   asn1_utils::get_cause_str(transaction_sink.failure()->cause));
    CORO_EARLY_RETURN(false);
  }

  if (not transaction_sink.successful()) {
    logger.warning("\"{}\" failed. Cause: No response received", name());
    CORO_EARLY_RETURN(false);
  }

  advertised_cells = std::move(served_cells);

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN(true);
}
