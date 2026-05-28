// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../du_processor/du_processor_repository.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

/// \brief Handles the TRP Information Exchange procedure at the CU-CP.
class trp_information_exchange_routine
{
public:
  trp_information_exchange_routine(const trp_information_request_t&                request_,
                                   du_processor_repository&                        du_db_,
                                   std::map<cu_cp_du_index_t, nrppa_f1ap_adapter>& nrppa_f1ap_ev_notifiers);

  void operator()(coro_context<async_task<trp_information_cu_cp_response_t>>& ctx);

  static const char* name() { return "TRP Information Exchange Routine"; }

private:
  void handle_sub_procedure_outcome();

  const trp_information_request_t                 request;
  du_processor_repository&                        du_db;
  std::map<cu_cp_du_index_t, nrppa_f1ap_adapter>& nrppa_f1ap_ev_notifiers;
  ocudulog::basic_logger&                         logger;

  std::vector<cu_cp_du_index_t> du_indexes;

  std::vector<cu_cp_du_index_t>::iterator du_it;
  cu_cp_du_index_t                        du_index = cu_cp_du_index_t::invalid;
  f1ap_cu*                                f1ap     = nullptr;

  // Subroutine results.
  expected<trp_information_response_t, trp_information_failure_t> sub_proc_outcome;

  trp_information_cu_cp_response_t result_msg = {};
};

} // namespace ocudu::ocucp
