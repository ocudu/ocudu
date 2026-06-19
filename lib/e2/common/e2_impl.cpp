// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2_impl.h"
#include "procedures/e2ap_connection_update_procedure.h"
#include "procedures/e2ap_removal_procedure.h"
#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/e2/e2.h"
#include <memory>

using namespace ocudu;
using namespace asn1::e2ap;
using namespace asn1;

e2_impl::e2_impl(ocudulog::basic_logger&  logger_,
                 e2ap_e2agent_notifier&   agent_notifier_,
                 timer_factory            timers_,
                 e2_connection_client&    e2_client_,
                 e2_subscription_manager& subscription_mngr_,
                 e2sm_manager&            e2sm_mngr_,
                 task_executor&           task_exec_) :
  logger(logger_),
  timers(timers_),
  ctrl_exec(task_exec_),
  cancel_event(timers_),
  subscription_proc(subscription_mngr_),
  e2sm_mngr(e2sm_mngr_),
  events(std::make_unique<e2_event_manager>(timers)),
  async_tasks(10),
  connection_handler(e2_client_, *this, agent_notifier_, task_exec_)
{
}

bool e2_impl::handle_e2_tnl_connection_request()
{
  // Reset the old channel first so its destructor does not clobber the new connection's
  // internal notifier inside e2_connection_handler.
  tx_pdu_notifier.reset();
  tx_pdu_notifier = connection_handler.connect_to_ric();
  return tx_pdu_notifier != nullptr;
}

async_task<void> e2_impl::handle_e2_node_initiated_removal_request()
{
  if (tx_pdu_notifier == nullptr) {
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }
  return launch_async<e2ap_removal_procedure>(*tx_pdu_notifier, *events, logger);
}

async_task<void> e2_impl::handle_e2_disconnection_request()
{
  return connection_handler.handle_tnl_association_removal();
}

async_task<e2_setup_response_message> e2_impl::handle_e2_setup_request(const e2_setup_request_message& request)
{
  if (tx_pdu_notifier == nullptr) {
    logger.warning("E2 TNL not established; aborting E2 Setup");
    return launch_async([](coro_context<async_task<e2_setup_response_message>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(e2_setup_response_message{});
    });
  }
  return launch_async<e2ap_setup_procedure>(request, *tx_pdu_notifier, *events, logger, cancel_event);
}

void e2_impl::handle_ric_control_request(const asn1::e2ap::ric_ctrl_request_s msg)
{
  logger.info("Received RIC Control Request");
  e2_ric_control_request request;
  request.request = msg;
  async_tasks.schedule<e2ap_ric_control_procedure>(request, *tx_pdu_notifier, e2sm_mngr, logger);
}

void e2_impl::handle_ric_subscription_request(const asn1::e2ap::ric_sub_request_s& msg)
{
  logger.info("Received RIC Subscription Request");
  async_tasks.schedule(launch_async<e2ap_subscription_setup_procedure>(
      msg, *events, *tx_pdu_notifier, subscription_proc, timers, logger));
}

void e2_impl::handle_ric_subscription_delete_request(const asn1::e2ap::ric_sub_delete_request_s& msg)
{
  logger.info("Received RIC Subscription Delete Request");
  async_tasks.schedule(launch_async<e2ap_subscription_delete_procedure>(
      msg, *events, *tx_pdu_notifier, subscription_proc, timers, logger));
}

void e2_impl::handle_e2_connection_update(const asn1::e2ap::e2conn_upd_s& msg)
{
  logger.info("Received E2 Connection Update");
  async_tasks.schedule(launch_async<e2ap_connection_update_procedure>(msg, *tx_pdu_notifier, timers, logger));
}

void e2_impl::handle_message(const e2_message& msg)
{
  if (not ctrl_exec.execute([this, msg]() {
        expected<uint8_t> transaction_id = get_transaction_id(msg.pdu);
        if (transaction_id.has_value()) {
          logger.info("E2AP msg, \"{}.{}\", transaction id={}",
                      msg.pdu.type().to_string(),
                      get_message_type_str(msg.pdu),
                      transaction_id.value());
        } else {
          logger.info("E2AP SDU, \"{}.{}\"", msg.pdu.type().to_string(), get_message_type_str(msg.pdu));
        }

        switch (msg.pdu.type().value) {
          case asn1::e2ap::e2ap_pdu_c::types_opts::init_msg:
            handle_initiating_message(msg.pdu.init_msg());
            break;
          case asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome:
            handle_successful_outcome(msg.pdu.successful_outcome());
            break;
          case asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome:
            handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
            break;
          default:
            logger.error("Invalid E2 PDU type");
            break;
        }
      })) {
    logger.error("Failed to dispatch E2 PDU to E2AP executor. Dropping PDU of type {}", msg.pdu.type().to_string());
  }
}

void e2_impl::handle_initiating_message(const asn1::e2ap::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::options::ric_sub_request:
      handle_ric_subscription_request(msg.value.ric_sub_request());
      break;
    case asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::options::ric_sub_delete_request:
      handle_ric_subscription_delete_request(msg.value.ric_sub_delete_request());
      break;
    case asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::options::ric_ctrl_request:
      handle_ric_control_request(msg.value.ric_ctrl_request());
      break;
    case asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::options::e2conn_upd:
      handle_e2_connection_update(msg.value.e2conn_upd());
      break;
    default:
      logger.error("Invalid E2AP initiating message type");
      break;
  }
}

void e2_impl::handle_successful_outcome(const asn1::e2ap::successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::e2ap::e2ap_elem_procs_o::successful_outcome_c::types_opts::options::e2setup_resp:
      events->e2_setup_outcome.set(outcome.value.e2setup_resp());
      break;
    case asn1::e2ap::e2ap_elem_procs_o::successful_outcome_c::types_opts::options::e2_removal_resp:
      events->e2_removal_outcome.set(outcome.value.e2_removal_resp());
      break;
    default:
      logger.error("Invalid E2AP successful outcome message type");
      break;
  }
}

void e2_impl::handle_unsuccessful_outcome(const asn1::e2ap::unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::e2setup_fail:
      events->e2_setup_outcome.set(outcome.value.e2setup_fail());
      break;
    case asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::e2_removal_fail:
      events->e2_removal_outcome.set(outcome.value.e2_removal_fail());
      break;
    default:
      logger.error("Invalid E2AP unsuccessful outcome message type");
      break;
  }
}
