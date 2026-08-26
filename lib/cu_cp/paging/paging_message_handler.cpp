// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "paging_message_handler.h"
#include "../du_processor/du_processor_repository.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/tac.h"

using namespace ocudu;
using namespace ocucp;

paging_message_handler::paging_message_handler(const paging_message_handler_dependencies& dependencies) :
  dus(dependencies.dus), logger(dependencies.logger)
{
}

void paging_message_handler::handle_paging_message(const cu_cp_paging_message& msg) const
{
  // Forward paging message to all DU processors
  bool paging_sent = false;
  for (const auto& du_idx : dus.get_du_processor_indexes()) {
    paging_sent |= handle_du_paging_message(du_idx, msg);
  }

  if (not paging_sent) {
    logger.warning("No DU processor was able to handle the paging message");
  }
}

static bool is_tac_in_list(span<const cu_cp_tai_list_for_paging_item> tai_list, tac_t tac)
{
  return std::any_of(tai_list.begin(), tai_list.end(), [&tac](const auto& tai) { return tai.tai.tac == tac; });
}

/// \brief True if any TAC the cell broadcasts appears in the paging TAI list.
///
/// A cell belongs to every tracking area it broadcasts; matching only the first drops paging for the others.
static bool is_cell_in_tai_list(span<const cu_cp_tai_list_for_paging_item> tai_list, const du_cell_configuration& cell)
{
  if (cell.tac_list.empty()) {
    return is_tac_in_list(tai_list, cell.tac);
  }

  return std::any_of(
      cell.tac_list.begin(), cell.tac_list.end(), [&tai_list](tac_t tac) { return is_tac_in_list(tai_list, tac); });
}

/// Remove recommended cells that do not match any TAC in the TAI list or that do not belong to this DU.
static void remove_non_applicable_recommended_cells(cu_cp_paging_message& msg, const du_configuration_context& du_cfg)
{
  auto& recommended_cells = msg.assist_data_for_paging.value()
                                .assist_data_for_recommended_cells.value()
                                .recommended_cells_for_paging.recommended_cell_list;

  auto is_bad_recommended_cell = [&](const cu_cp_recommended_cell_item& recommended_cell) {
    auto cell_it = std::find_if(du_cfg.served_cells.begin(),
                                du_cfg.served_cells.end(),
                                [&recommended_cell](const auto& c) { return recommended_cell.ngran_cgi == c.cgi; });
    if (cell_it == du_cfg.served_cells.end()) {
      // Recommended cell not found for this DU.
      return true;
    }
    return not is_cell_in_tai_list(msg.tai_list_for_paging, *cell_it);
  };

  recommended_cells.erase(std::remove_if(recommended_cells.begin(), recommended_cells.end(), is_bad_recommended_cell),
                          recommended_cells.end());
}

bool paging_message_handler::handle_du_paging_message(cu_cp_du_index_t            du_index,
                                                      const cu_cp_paging_message& msg_before) const
{
  du_processor&                   du     = dus.get_du_processor(du_index);
  const du_configuration_context* du_cfg = du.get_context();
  if (du_cfg == nullptr) {
    // DU has not completed F1 Setup.
    return false;
  }

  // Recommended cells will be added to the original paging message.
  cu_cp_paging_message msg_filtered{msg_before};
  if (not msg_filtered.assist_data_for_paging.has_value()) {
    msg_filtered.assist_data_for_paging.emplace();
  }
  if (not msg_filtered.assist_data_for_paging.value().assist_data_for_recommended_cells.has_value()) {
    msg_filtered.assist_data_for_paging.value().assist_data_for_recommended_cells.emplace();
  }
  auto& recommended_cells = msg_filtered.assist_data_for_paging.value()
                                .assist_data_for_recommended_cells.value()
                                .recommended_cells_for_paging.recommended_cell_list;

  // Clear recommended cells not matching any TAC in the tai_list_for_paging or that do not belong to this DU.
  remove_non_applicable_recommended_cells(msg_filtered, *du_cfg);

  for (const du_cell_configuration& cell : du_cfg->served_cells) {
    // Check if cell already exists in the list of recommended.
    if (std::any_of(recommended_cells.begin(), recommended_cells.end(), [&cell](const auto& c) {
          return c.ngran_cgi == cell.cgi;
        })) {
      continue;
    }

    // If tai_list_for_paging is empty, this is a RAN paging.
    // TODO: Support RANAC based paging.
    if (!msg_filtered.tai_list_for_paging.empty()) {
      if (not is_cell_in_tai_list(msg_filtered.tai_list_for_paging, cell)) {
        continue;
      }
    }

    // Setup recommended cell item to add in case it doesn't exist
    cu_cp_recommended_cell_item cell_item;
    cell_item.ngran_cgi = cell.cgi;
    recommended_cells.push_back(cell_item);
  }

  if (recommended_cells.empty()) {
    logger.info("du={}: No cells with matching TAC available at this DU", du_index);
    return false;
  }

  // Forward message to F1AP of the respective DU.
  du.get_f1ap_handler().get_f1ap_paging_manager().handle_paging(msg_filtered);

  return true;
}
