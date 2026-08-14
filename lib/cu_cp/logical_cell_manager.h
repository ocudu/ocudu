// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/cu_cp_types.h"
#include <map>

namespace ocudu::ocucp {

/// CU-CP-side managed object for a cell: operator intent plus its realization by a connected DU.
///
/// The intent half (\c admin_locked, \c barred) is owned by the CU-CP — declared in configuration and
/// mutated by operator commands — and outlives any DU connection. The realization half tracks whether (and
/// where) a connected DU currently serves the cell; it is filled at F1 setup and cleared when the DU is
/// removed. Operational state (served vs deactivated) intentionally stays in the DU configuration records:
/// the logical cell stores what the operator wants, not what the network currently does.
struct logical_cell {
  /// NR Cell Identity of the cell.
  nr_cell_identity nci;
  /// Administrative state: when locked, the CU-CP does not activate the cell.
  bool admin_locked = false;
  /// Intended MIB cellBarred state, applied via the TS 38.473 Cells to be Barred List whenever active.
  bool barred = false;

  /// Whether a connected DU currently serves this cell.
  bool realized = false;
  /// Index of the realizing DU. Only valid when realized.
  cu_cp_du_index_t du_index = cu_cp_du_index_t::invalid;
};

/// Registry of the CU-CP's logical cells, keyed by NR Cell Identity.
///
/// Declared cells are seeded from configuration at construction and exist before any DU connects, and the
/// declared set doubles as the activation whitelist: when it is non-empty, a reported cell outside it is
/// added dynamically in locked state (not activated until explicitly unlocked). When no cells are declared,
/// dynamic cells get default (unlocked, unbarred) intent, preserving the pre-logical-cell behaviour for
/// undeclared deployments. Intent survives DU removal.
class logical_cell_manager
{
public:
  explicit logical_cell_manager(span<const cu_cp_logical_cell_config> declared_cells);

  /// Find the logical cell with the given NCI. Returns nullptr if unknown.
  logical_cell*       find_cell(nr_cell_identity nci);
  const logical_cell* find_cell(nr_cell_identity nci) const;

  /// \brief Realize a cell reported by a DU, creating a dynamic logical cell if it was not declared.
  ///
  /// The dynamic cell is created locked when any cells were declared in configuration (declared set =
  /// activation whitelist), unlocked otherwise.
  /// \return The (created or updated) logical cell record.
  logical_cell& realize_cell(nr_cell_identity nci, cu_cp_du_index_t du_index);

  /// De-realize all cells realized by the given DU, keeping their operator intent.
  void derealize_du_cells(cu_cp_du_index_t du_index);

private:
  std::map<nr_cell_identity, logical_cell> cells;

  /// Whether any cells were declared in configuration. When true, the declared set acts as the activation
  /// whitelist and undeclared reported cells are realized locked.
  const bool any_cells_declared;

  ocudulog::basic_logger& logger;
};

} // namespace ocudu::ocucp
