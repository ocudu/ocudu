// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2_setup_routine.h"
#include "../common/e2ap_asn1_helpers.h"
#include "../common/e2ap_asn1_utils.h"
#include "ocudu/e2/e2_node_component_config.h"

using namespace ocudu;
using namespace asn1::e2ap;

e2_setup_routine::e2_setup_routine(const e2ap_configuration&          cfg_,
                                   e2_node_component_config_provider& node_cfg_provider_,
                                   e2sm_manager&                      e2sm_mngr_,
                                   e2_connection_manager&             e2_conn_mng_,
                                   timer_factory                      timers_,
                                   ocudulog::basic_logger&            logger_) :
  cfg(cfg_),
  node_cfg_provider(node_cfg_provider_),
  e2sm_mngr(e2sm_mngr_),
  e2_conn_mng(e2_conn_mng_),
  timers(timers_),
  logger(logger_)
{
}

void e2_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  CORO_AWAIT_VALUE(response_msg, start_e2_setup_request());
  handle_e2_setup_response(response_msg);

  CORO_RETURN(response_msg.success);
}

async_task<e2_setup_response_message> e2_setup_routine::start_e2_setup_request()
{
  return launch_async(
      [this,
       request   = e2_setup_request_message{},
       node_cfgs = std::vector<e2_node_component_config>{},
       resp      = e2_setup_response_message{}](coro_context<async_task<e2_setup_response_message>>& ctx) mutable {
        CORO_BEGIN(ctx);

        CORO_AWAIT_VALUE(node_cfgs, node_cfg_provider.get_configs());

        if (node_cfgs.empty()) {
          logger.warning("No node component configs received; aborting E2 Setup");
          CORO_EARLY_RETURN(e2_setup_response_message{});
        }

        fill_asn1_e2ap_setup_request(logger, request.request, cfg, e2sm_mngr, node_cfgs);
        request.max_setup_retries = cfg.max_setup_retries;

        for (const auto& ran_function : request.request->ran_functions_added) {
          const auto& ran_function_item = ran_function.value().ran_function_item();
          uint16_t    id                = ran_function_item.ran_function_id;
          logger.info("Added RAN function OID {} to candidate list under RAN Function ID {}",
                      ran_function_item.ran_function_o_id.to_string().c_str(),
                      id);
          candidate_ran_functions[id] = ran_function_item;
        }

        CORO_AWAIT_VALUE(resp, e2_conn_mng.handle_e2_setup_request(request));

        CORO_RETURN(resp);
      });
}

void e2_setup_routine::handle_e2_setup_response(const e2_setup_response_message& msg)
{
  if (!msg.success) {
    logger.error("E2 Setup failed: cause={}", get_cause_str(msg.failure->cause));
    if (msg.failure->time_to_wait_present) {
      logger.info("RIC signaled retry after {}", msg.failure->time_to_wait.to_string());
    }
    return;
  }

  if (!msg.response->ran_functions_accepted_present) {
    logger.warning("E2 Setup Response contains no accepted RAN functions");
    return;
  }

  for (unsigned i = 0; i < msg.response->ran_functions_accepted.size(); ++i) {
    const auto& ran_function_item = msg.response->ran_functions_accepted[i].value().ran_function_id_item();
    uint16_t    id                = ran_function_item.ran_function_id;

    if (candidate_ran_functions.count(id) == 0) {
      logger.warning("RAN function with id {} is not a candidate", id);
      continue;
    }

    const std::string ran_func_oid = candidate_ran_functions[id].ran_function_o_id.to_string();
    e2sm_mngr.add_supported_ran_function(id, ran_func_oid);
    logger.info("Added supported RAN function with id {} and OID {}", id, ran_func_oid);
  }
}
