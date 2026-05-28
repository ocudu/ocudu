// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "amf_connection_setup_routine.h"
#include "ocudu/ngap/ngap_setup.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/coroutine.h"

using namespace ocudu;
using namespace ocucp;

async_task<bool>
ocudu::ocucp::start_amf_connection_setup(ngap_repository&                                          ngap_db,
                                         std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected,
                                         cu_cp_ng_setup_complete_notifier*                         ng_setup_notifier)
{
  return launch_async<amf_connection_setup_routine>(ngap_db, amfs_connected, ng_setup_notifier);
}

amf_connection_setup_routine::amf_connection_setup_routine(
    ngap_repository&                                          ngap_db_,
    std::unordered_map<cu_cp_amf_index_t, std::atomic<bool>>& amfs_connected_,
    cu_cp_ng_setup_complete_notifier*                         ng_setup_notifier_) :
  ngap_db(ngap_db_),
  amfs_connected(amfs_connected_),
  ng_setup_notifier(ng_setup_notifier_),
  ngaps(ngap_db_.get_ngaps()),
  logger(ocudulog::fetch_basic_logger("CU-CP"))
{
}

void amf_connection_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  // TODO: Use when_all.
  for (ngap_it = ngaps.begin(); ngap_it != ngaps.end(); ngap_it++) {
    amf_index = ngap_it->first;
    ngap      = ngap_it->second;

    if (not ngap->handle_amf_tnl_connection_request()) {
      CORO_EARLY_RETURN(false);
    }

    // Initiate NG Setup.
    CORO_AWAIT_VALUE(result_msg, ngap->handle_ng_setup_request(/*max_setup_retries*/ 1));

    success = std::holds_alternative<ngap_ng_setup_response>(result_msg);

    // Handle result of NG setup.
    handle_connection_setup_result();

    if (success) {
      // Notify successful NG Setup and deliver packed NG setup PDU bytes via notifier.
      if (ng_setup_notifier != nullptr) {
        auto& ng_resp = std::get<ngap_ng_setup_response>(result_msg);
        ng_setup_notifier->on_ng_setup_complete(
            std::move(ng_resp.packed_ng_setup_request), std::move(ng_resp.packed_ng_setup_response), ng_resp.amf_name);
      }

      // Update PLMN lookups in NGAP repository after successful NGSetup.
      ngap_db.update_plmn_lookup(amf_index);

      std::string plmn_list;
      for (const auto& plmn : ngap->get_ngap_context().get_supported_plmns()) {
        plmn_list += plmn.to_string() + " ";
      }

      logger.info("Connected to AMF. Supported PLMNs: {}", plmn_list);
    } else {
      logger.error("Failed to connect to AMF");
      CORO_EARLY_RETURN(false);
    }
  }

  CORO_RETURN(success);
}

void amf_connection_setup_routine::handle_connection_setup_result()
{
  // Update AMF connection handler state.
  amfs_connected.emplace(amf_index, success);
}
