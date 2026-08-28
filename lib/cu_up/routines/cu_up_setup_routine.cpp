// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_setup_routine.h"
#include "ocudu/e1ap/common/e1_setup_messages.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <utility>

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

void cu_up_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("cu-up={}: \"{}\" initialized.", cu_up_id, name());

  // Connect to CU-CP.
  if (not e1ap_conn_mng.connect_to_cu_cp()) {
    CORO_EARLY_RETURN(false);
  }

  // Initiate E1 Setup.
  CORO_AWAIT_VALUE(response_msg, start_cu_up_e1_setup_request());

  // Handle E1 setup result.
  handle_cu_up_e1_setup_response(response_msg);

  // Notify successful setup and deliver packed E1 setup PDU bytes via notifier.
  if (e1_setup_notifier) {
    e1_setup_notifier->on_e1_setup_complete(
        std::move(response_msg.packed_e1_setup_request), std::move(response_msg.packed_e1_setup_response), cu_up_id);
  }

  CORO_RETURN(true);
}

async_task<cu_up_e1_setup_response> cu_up_setup_routine::start_cu_up_e1_setup_request()
{
  // Prepare request to send to E1.
  cu_up_e1_setup_request request_msg = {};

  request_msg.gnb_cu_up_id   = to_underlying(cu_up_id);
  request_msg.gnb_cu_up_name = cu_up_name;

  // We only support 5G
  request_msg.cn_support = cu_up_cn_support_t::c_5gc;

  for (const auto& plmn_id : plmns) {
    supported_plmns_item_t plmn_item;
    plmn_item.plmn_id = plmn_id;
    request_msg.supported_plmns.push_back(plmn_item);
  }

  // Initiate E1 Setup Request.
  return e1ap_conn_mng.handle_cu_up_e1_setup_request(request_msg);
}

void cu_up_setup_routine::handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& resp)
{
  // TODO
  if (!resp.success) {
    report_fatal_error("CU-UP E1 Setup failed");
  }

  logger.debug("cu-up={}: \"{}\" finalized.", cu_up_id, name());
}
