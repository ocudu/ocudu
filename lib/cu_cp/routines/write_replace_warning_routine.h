// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/cu_cp/f1ap_warning.h"
#include "ocudu/ngap/ngap_warning.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/async_task.h"
#include <vector>

namespace ocudu::ocucp {

class du_processor_repository;

/// \brief Handles the WRITE-REPLACE WARNING procedure at the CU-CP level.
///
/// Translates the NGAP request into F1AP requests and sends them to all connected DUs in parallel.
/// Defined in TS 38.413 section 8.9.1.
class cu_cp_write_replace_warning_routine
{
public:
  cu_cp_write_replace_warning_routine(const ngap_write_replace_warning_request& request_,
                                      du_processor_repository&                  du_db_,
                                      ocudulog::basic_logger&                   logger_);

  void operator()(coro_context<async_task<ngap_write_replace_warning_response>>& ctx);

  static const char* name() { return "Write-Replace Warning Routine"; }

private:
  bool                          build_f1ap_requests();
  std::vector<async_task<bool>> build_du_tasks();

  const ngap_write_replace_warning_request request;
  du_processor_repository&                 du_db;
  ocudulog::basic_logger&                  logger;

  // NR CGI filter extracted from the NGAP warning area list (absent = send to all cells of all DUs).
  std::optional<ngap_nr_cgi_list_for_warning> cgi_filter;

  // One entry per SIB type to broadcast (SIB6 alone, SIB6+SIB7, or SIB8). Acts as a template: build_du_tasks()
  // hands each DU its own copy, with cells_to_be_broadcast filled in, to avoid DUs racing on shared state.
  std::vector<f1ap_write_replace_warning_request> f1ap_requests;

  // DU indexes targeted by build_du_tasks(), in the same order as the tasks passed to when_all(), so results
  // can be matched back to their DU for logging.
  std::vector<cu_cp_du_index_t> targeted_du_indexes;

  std::vector<bool>                   du_results;
  ngap_write_replace_warning_response response;
};

} // namespace ocudu::ocucp
