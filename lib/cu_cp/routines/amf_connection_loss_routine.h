// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_controller/cu_cp_controller.h"
#include "../ue_manager/ue_manager_impl.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief Handles the reconnection between the CU-CP and AMF.
class amf_connection_loss_routine
{
public:
  amf_connection_loss_routine(cu_cp_amf_index_t                 amf_index_,
                              const cu_cp_configuration&        cu_cp_cfg_,
                              std::vector<plmn_identity>&       plmns_,
                              du_processor_repository&          du_db_,
                              cu_cp_ue_context_release_handler& ue_release_handler_,
                              ue_manager&                       ue_mng_,
                              cu_cp_controller&                 controller_,
                              ocudulog::basic_logger&           logger_);

  static std::string name() { return "AMF Connection Loss Routine"; }

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const cu_cp_amf_index_t           amf_index;
  const cu_cp_configuration&        cu_cp_cfg;
  std::vector<plmn_identity>&       plmns;
  du_processor_repository&          du_db;
  cu_cp_ue_context_release_handler& ue_release_handler;
  ue_manager&                       ue_mng;
  cu_cp_controller&                 controller;
  ocudulog::basic_logger&           logger;
};

} // namespace ocudu::ocucp
