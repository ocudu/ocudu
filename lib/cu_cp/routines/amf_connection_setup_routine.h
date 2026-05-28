// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ngap_repository.h"
#include "ocudu/cu_cp/cu_cp_ng_setup_notifier.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

async_task<bool> start_amf_connection_setup(ngap_repository&                                          ngap_db,
                                            std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected,
                                            cu_cp_ng_setup_complete_notifier* ng_setup_notifier = nullptr);

/// \brief Handles the setup of the connection between the CU-CP and AMF, handling in particular the NG Setup procedure.
class amf_connection_setup_routine
{
public:
  amf_connection_setup_routine(ngap_repository&                                          ngap_db_,
                               std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected_,
                               cu_cp_ng_setup_complete_notifier*                         ng_setup_notifier_ = nullptr);

  void operator()(coro_context<async_task<bool>>& ctx);

private:
  void handle_connection_setup_result();

  ngap_repository&                                          ngap_db;
  std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected;
  cu_cp_ng_setup_complete_notifier*                         ng_setup_notifier;
  std::map<cu_cp_amf_index_t, ngap_interface*>              ngaps;
  ocudulog::basic_logger&                                   logger;

  std::map<cu_cp_amf_index_t, ngap_interface*>::iterator ngap_it;
  cu_cp_amf_index_t                                      amf_index = cu_cp_amf_index_t::invalid;
  ngap_interface*                                        ngap      = nullptr;

  ngap_ng_setup_result result_msg = {};
  bool                 success    = false;
};

} // namespace ocudu::ocucp
