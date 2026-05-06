// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "conditional_handover_reconfiguration_routine.h"
#include "ocudu/cu_cp/cu_cp_cho_types.h"
#include "ocudu/cu_cp/cu_cp_intra_cu_ho_types.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/when_all.h"

namespace ocudu {
namespace ocucp {
class mobility_manager;

class cu_cp_ue;
class ue_manager;
class cu_cp_impl_interface;
class du_processor_repository;

/// \brief Coordinates full intra-CU CHO flow: preparation and execution/cancellation decision.
class conditional_handover_coordinator_routine
{
public:
  conditional_handover_coordinator_routine(const cu_cp_intra_cu_cho_request& request_,
                                           du_processor_repository&          du_db_,
                                           cu_cp_impl_interface&             cu_cp_handler_,
                                           ue_manager&                       ue_mng_,
                                           mobility_manager&                 mobility_mng_,
                                           ocudulog::basic_logger&           logger_);

  void operator()(coro_context<async_task<cu_cp_intra_cu_cho_response>>& ctx);

  static const char* name() { return "CHO Coordinator Routine"; }

private:
  const cu_cp_intra_cu_cho_request request;
  du_processor_repository&         du_db;
  cu_cp_impl_interface&            cu_cp_handler;
  ue_manager&                      ue_mng;
  mobility_manager&                mobility_mng; // needed for intra_cu_handover_routine
  ocudulog::basic_logger&          logger;

  cu_cp_intra_cu_cho_response response{};

  cu_cp_ue*                         source_ue = nullptr;
  std::vector<cu_cp_cho_candidate>  all_candidates;
  cu_cp_cho_reconfiguration_request cho_reconfig_request;
  bool                              cho_reconfig_result = false;

  /// \brief Builds one CHO preparation task per intra-CU candidate.
  /// Each task returns a cu_cp_cho_candidate; target_ue_index is invalid on failure.
  /// Only candidates without xnc_index (intra-CU) are included.
  std::vector<async_task<cu_cp_cho_candidate>> build_intra_cu_prep_tasks();

  /// \brief Builds the merged task vector of all candidate preparation tasks.
  std::vector<async_task<cu_cp_cho_candidate>> build_all_prep_tasks();
};

} // namespace ocucp
} // namespace ocudu
