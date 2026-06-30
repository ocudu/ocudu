// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/plmn_identity.h"
#include <optional>
#include <vector>

namespace ocudu::ocucp {

class du_processor_repository;
class ue_manager;

/// \brief A single cell, already resolved to its serving DU, targeted by a cell lifecycle routine.
///
/// Selecting which cells to act on (from a PLMN set or an explicit CGI) is the caller's responsibility, so that the
/// activation/deactivation routines stay agnostic to what triggered the change (AMF (dis)connection, an operator
/// command via cu_cp_cell_command_handler, ...).
struct cell_lifecycle_target {
  /// Index of the DU that serves the cell.
  cu_cp_du_index_t du_index = cu_cp_du_index_t::invalid;
  /// Global identity of the cell.
  nr_cell_global_id_t cgi;
  /// Physical cell ID. Only carried for activation; left unset for deactivation.
  std::optional<pci_t> pci;
  /// PLMNs to (re)activate on the cell. Only carried for activation.
  std::vector<plmn_identity> plmns;
};

/// \brief Resolve the deactivated cells (across all DUs) that serve any of the given PLMNs, i.e. the cells to activate.
std::vector<cell_lifecycle_target> resolve_activation_targets(du_processor_repository&          du_db,
                                                              const std::vector<plmn_identity>& plmns);

/// \brief Resolve the served cells (across all DUs) that lose any of the given PLMNs, i.e. the cells to deactivate.
std::vector<cell_lifecycle_target> resolve_deactivation_targets(du_processor_repository&          du_db,
                                                                const std::vector<plmn_identity>& plmns);

/// \brief Collect the CU-CP UEs whose PLMN is among the given PLMNs, so they can be released before deactivation.
std::vector<cu_cp_ue_index_t> collect_ues_for_plmns(ue_manager& ue_mng, const std::vector<plmn_identity>& plmns);

} // namespace ocudu::ocucp
