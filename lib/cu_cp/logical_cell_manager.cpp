// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "logical_cell_manager.h"

using namespace ocudu;
using namespace ocudu::ocucp;

logical_cell_manager::logical_cell_manager(span<const cu_cp_logical_cell_config> declared_cells) :
  any_cells_declared(not declared_cells.empty()), logger(ocudulog::fetch_basic_logger("CU-CP"))
{
  for (const cu_cp_logical_cell_config& cell_cfg : declared_cells) {
    logical_cell& cell = cells[cell_cfg.nci];
    cell.nci           = cell_cfg.nci;
    cell.admin_locked  = cell_cfg.admin_locked;
    cell.barred        = cell_cfg.barred;
    logger.debug("Declared logical cell nci={:#x} admin_locked={} barred={}",
                 cell_cfg.nci.value(),
                 cell_cfg.admin_locked,
                 cell_cfg.barred);
  }
}

logical_cell* logical_cell_manager::find_cell(nr_cell_identity nci)
{
  auto it = cells.find(nci);
  return it != cells.end() ? &it->second : nullptr;
}

const logical_cell* logical_cell_manager::find_cell(nr_cell_identity nci) const
{
  auto it = cells.find(nci);
  return it != cells.end() ? &it->second : nullptr;
}

logical_cell& logical_cell_manager::realize_cell(nr_cell_identity nci, cu_cp_du_index_t du_index)
{
  auto it = cells.find(nci);
  if (it == cells.end()) {
    it             = cells.emplace(nci, logical_cell{}).first;
    it->second.nci = nci;
    if (any_cells_declared) {
      // Cells were declared in configuration, so the declared set acts as the activation whitelist: a
      // reported cell outside it comes up locked, staying deactivated until an explicit cell_unlock command
      // or a configuration declaration.
      it->second.admin_locked = true;
      logger.warning("Cell nci={:#x} reported by a DU is not declared in the CU-CP configuration: keeping it "
                     "locked (not activated)",
                     nci.value());
    } else {
      // No cells declared in configuration: create a dynamic logical cell with default intent, so
      // undeclared deployments keep the pre-logical-cell behaviour.
      logger.debug("Added dynamic logical cell nci={:#x}", nci.value());
    }
  }

  logical_cell& cell = it->second;
  if (cell.realized && cell.du_index != du_index) {
    logger.warning("Logical cell nci={:#x} reported by du={} is already realized by du={}",
                   nci.value(),
                   fmt::underlying(du_index),
                   fmt::underlying(cell.du_index));
  }
  cell.realized = true;
  cell.du_index = du_index;

  logger.info("Logical cell nci={:#x} realized by du={} (admin_locked={} barred={})",
              nci.value(),
              fmt::underlying(du_index),
              cell.admin_locked,
              cell.barred);

  return cell;
}

void logical_cell_manager::derealize_du_cells(cu_cp_du_index_t du_index)
{
  for (auto& [nci, cell] : cells) {
    if (cell.realized && cell.du_index == du_index) {
      cell.realized = false;
      cell.du_index = cu_cp_du_index_t::invalid;
      logger.info("Logical cell nci={:#x} de-realized (du={} removed). Operator intent kept (admin_locked={} "
                  "barred={})",
                  nci.value(),
                  fmt::underlying(du_index),
                  cell.admin_locked,
                  cell.barred);
    }
  }
}
