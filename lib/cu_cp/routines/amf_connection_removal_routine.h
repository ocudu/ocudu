// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ngap_repository.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

async_task<void> start_amf_connection_removal(ngap_repository&                                          ngap_db,
                                              std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected);

/// \brief Handles the removal of the connection between the CU-CP and AMF.
class amf_connection_removal_routine
{
public:
  amf_connection_removal_routine(ngap_repository&                                          ngap_db_,
                                 std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected;
  std::map<cu_cp_amf_index_t, ngap_interface*>              ngaps;
  ocudulog::basic_logger&                                   logger;

  std::map<cu_cp_amf_index_t, ngap_interface*>::iterator ngap_it;
  cu_cp_amf_index_t                                      amf_index = cu_cp_amf_index_t::invalid;
  ngap_interface*                                        ngap      = nullptr;
};

} // namespace ocudu::ocucp
