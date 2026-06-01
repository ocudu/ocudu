// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "amf_connection_removal_routine.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

async_task<void>
ocudu::ocucp::start_amf_connection_removal(ngap_repository&                                          ngap_db,
                                           std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected)
{
  return launch_async<amf_connection_removal_routine>(ngap_db, amfs_connected);
}

amf_connection_removal_routine::amf_connection_removal_routine(
    ngap_repository&                                          ngap_db_,
    std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected_) :
  amfs_connected(amfs_connected_), ngaps(ngap_db_.get_ngaps()), logger(ocudulog::fetch_basic_logger("CU-CP"))
{
}

void amf_connection_removal_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  for (ngap_it = ngaps.begin(); ngap_it != ngaps.end(); ngap_it++) {
    amf_index = ngap_it->first;
    ngap      = ngap_it->second;

    // Launch procedure to remove AMF connection.
    CORO_AWAIT(ngap->handle_amf_disconnection_request());

    // Update AMF connection handler state.
    amfs_connected[amf_index] = false;
  }

  CORO_RETURN();
}
