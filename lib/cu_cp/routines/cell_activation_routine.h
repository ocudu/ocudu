// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_processor/du_processor.h"
#include "../du_processor/du_processor_repository.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include <unordered_set>

namespace ocudu::ocucp {

/// \brief Handles the activation of the cell.
class cell_activation_routine
{
public:
  cell_activation_routine(const cu_cp_configuration&        cu_cp_cfg_,
                          const std::vector<plmn_identity>& plmns_,
                          du_processor_repository&          du_db_,
                          ocudulog::basic_logger&           logger_);
  ~cell_activation_routine() = default;

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Cell Activation Routine"; }

  void get_plmns_to_activate(const du_cell_configuration& cell_cfg);

  bool generate_gnb_cu_configuration_update();

private:
  const cu_cp_configuration&       cu_cp_cfg;
  const std::vector<plmn_identity> plmns;
  du_processor_repository&         du_db;
  ocudulog::basic_logger&          logger;

  // (Sub-)Routine requests.
  f1ap_gnb_cu_configuration_update f1ap_cu_cfg_update;

  // (Sub-)Routine results.
  f1ap_gnb_cu_configuration_update_response f1ap_cu_cfg_update_response;
  bool                                      routine_success = true;

  std::unordered_set<plmn_identity>       plmns_to_activate;
  std::vector<cu_cp_du_index_t>           du_indexes;
  std::vector<cu_cp_du_index_t>::iterator du_idx_it;
  du_processor*                           du_proc = nullptr;
};

} // namespace ocudu::ocucp
