// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_lifecycle_target.h"
#include "../du_processor/du_processor.h"
#include "../du_processor/du_processor_repository.h"
#include "../ue_manager/ue_manager_impl.h"
#include <algorithm>
#include <set>

using namespace ocudu;
using namespace ocudu::ocucp;

std::vector<cell_lifecycle_target> ocudu::ocucp::resolve_activation_targets(du_processor_repository&          du_db,
                                                                            const std::vector<plmn_identity>& plmns)
{
  std::vector<cell_lifecycle_target> targets;
  const std::set<plmn_identity>      requested(plmns.begin(), plmns.end());

  for (cu_cp_du_index_t du_index : du_db.get_du_processor_indexes()) {
    du_processor* du_proc = du_db.find_du_processor(du_index);
    if (du_proc == nullptr) {
      continue;
    }
    const du_configuration_context* du_ctxt = du_proc->get_context();
    if (du_ctxt == nullptr) {
      continue;
    }

    // A deactivated cell is reactivated for whichever of its deactivated PLMNs are in the requested set.
    for (const du_cell_configuration& cell : du_ctxt->deactivated_cells) {
      std::vector<plmn_identity> plmns_to_activate;
      for (const plmn_identity& plmn : cell.deactivated_plmns) {
        if (requested.count(plmn) != 0) {
          plmns_to_activate.push_back(plmn);
        }
      }
      if (!plmns_to_activate.empty()) {
        targets.push_back({du_index, cell.cgi, cell.pci, std::move(plmns_to_activate)});
      }
    }
  }

  return targets;
}

std::vector<cell_lifecycle_target> ocudu::ocucp::resolve_deactivation_targets(du_processor_repository&          du_db,
                                                                              const std::vector<plmn_identity>& plmns)
{
  std::vector<cell_lifecycle_target> targets;
  const std::set<plmn_identity>      lost(plmns.begin(), plmns.end());

  for (cu_cp_du_index_t du_index : du_db.get_du_processor_indexes()) {
    du_processor* du_proc = du_db.find_du_processor(du_index);
    if (du_proc == nullptr) {
      continue;
    }
    const du_configuration_context* du_ctxt = du_proc->get_context();
    if (du_ctxt == nullptr) {
      continue;
    }

    // A served cell is deactivated when it loses any of the lost PLMNs.
    // TODO: Deactivate only the lost PLMNs when a cell keeps serving the others.
    for (const du_cell_configuration& cell : du_ctxt->served_cells) {
      const bool loses_any = std::any_of(cell.served_plmns.begin(), cell.served_plmns.end(), [&lost](const auto& plmn) {
        return lost.count(plmn) != 0;
      });
      if (loses_any) {
        targets.push_back({du_index, cell.cgi, std::nullopt, {}});
      }
    }
  }

  return targets;
}

std::vector<cu_cp_ue_index_t> ocudu::ocucp::collect_ues_for_plmns(ue_manager&                       ue_mng,
                                                                  const std::vector<plmn_identity>& plmns)
{
  std::vector<cu_cp_ue_index_t> ue_indexes;
  for (const plmn_identity& plmn : plmns) {
    for (cu_cp_ue* ue : ue_mng.find_ues(plmn)) {
      if (ue != nullptr) {
        ue_indexes.push_back(ue->get_ue_index());
      }
    }
  }
  return ue_indexes;
}
