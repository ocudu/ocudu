// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_setup_routine.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace ocuup;

cu_up_setup_routine::cu_up_setup_routine(cu_up_setup_routine_config              cfg,
                                         const cu_up_setup_routine_dependencies& dependencies) :
  cu_up_id(cfg.cu_up_id),
  cu_up_name(std::move(cfg.cu_up_name)),
  plmns(std::move(cfg.plmns)),
  logger(dependencies.logger),
  e1ap_conn_mng(dependencies.e1ap_conn_mng),
  e1_setup_notifier(dependencies.e1_setup_notifier)
{
}

void cu_up_setup_routine::operator()(coro_context<async_task<cu_up_setup_result>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("cu-up={}: \"{}\" initialized.", cu_up_id, name());

  // Connect to CU-CP.
  if (not e1ap_conn_mng.connect_to_cu_cp()) {
    CORO_EARLY_RETURN(cu_up_setup_result{cu_up_setup_result::outcome::tnl_connection_failure});
  }

  // Initiate E1 Setup.
  CORO_AWAIT_VALUE(response_msg, start_cu_up_e1_setup_request());

  if (not response_msg.success) {
    // Tear the E1 TNL association down, so that the E1 Setup can be retried over a new one.
    CORO_AWAIT(e1ap_conn_mng.disconnect_from_cu_cp());
    CORO_EARLY_RETURN(cu_up_setup_result{cu_up_setup_result::outcome::e1_setup_failure,
                                         response_msg.cause.has_value() ? fmt::format("{}", *response_msg.cause)
                                                                        : std::string{"unspecified"}});
  }

  logger.debug("cu-up={}: \"{}\" finalized.", cu_up_id, name());

  // Notify successful setup and deliver packed E1 setup PDU bytes via notifier.
  if (e1_setup_notifier) {
    e1_setup_notifier->on_e1_setup_complete(
        std::move(response_msg.packed_e1_setup_request), std::move(response_msg.packed_e1_setup_response), cu_up_id);
  }

  CORO_RETURN(cu_up_setup_result{});
}

async_task<cu_up_e1_setup_response> cu_up_setup_routine::start_cu_up_e1_setup_request()
{
  std::vector<supported_plmns_item_t> supported_plmns;
  supported_plmns.reserve(plmns.size());
  for (const auto& plmn_id : plmns) {
    supported_plmns_item_t plmn_item;
    plmn_item.plmn_id = plmn_id;
    supported_plmns.emplace_back(plmn_item);
  }

  // Prepare request to send to E1.
  cu_up_e1_setup_request request_msg{.gnb_cu_up_id   = to_underlying(cu_up_id),
                                     .gnb_cu_up_name = cu_up_name,
                                     // We only support 5G.
                                     .cn_support         = cu_up_cn_support_t::c_5gc,
                                     .supported_plmns    = std::move(supported_plmns),
                                     .gnb_cu_up_capacity = std::nullopt};

  // Initiate E1 Setup Request.
  return e1ap_conn_mng.handle_cu_up_e1_setup_request(request_msg);
}
