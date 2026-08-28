// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e1ap_cu_up_test_helpers.h"
#include "../common/e1ap_cu_up_test_messages.h"
#include "lib/e1ap/common/e1ap_asn1_utils.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/async/async_test_utils.h"

using namespace ocudu;
using namespace ocuup;

namespace {

class dummy_e1ap_tx_pdu_notifier : public e1ap_message_notifier
{
public:
  dummy_e1ap_tx_pdu_notifier(e1ap_message& last_tx_pdu_) : last_tx_pdu(last_tx_pdu_) {}

  void on_new_message(const e1ap_message& msg) override { last_tx_pdu = msg; }

  e1ap_message& last_tx_pdu;
};

} // namespace

std::unique_ptr<e1ap_message_notifier> dummy_e1_connection_client::handle_cu_up_connection_request(
    std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier_)
{
  cu_up_rx_pdu_notifier = std::move(cu_up_rx_pdu_notifier_);
  return std::make_unique<dummy_e1ap_tx_pdu_notifier>(last_tx_e1ap_pdu);
}

//////////////////////////////////

e1ap_cu_up_test::e1ap_cu_up_test()
{
  test_logger.set_level(ocudulog::basic_levels::debug);
  e1ap_logger.set_level(ocudulog::basic_levels::debug);
  ocudulog::init();

  e1ap = create_e1ap(e1ap_configuration{.max_nof_ues      = 16384,
                                        .json_log_enabled = true,
                                        .metrics_period   = timer_duration(0),
                                        .e1_index         = static_cast<cu_up_e1_index_t>(0)},
                     e1ap_cu_up_impl_dependencies{.e1_client_handler = e1ap_gw,
                                                  .cu_up_notifier    = cu_up_notifier,
                                                  .timers            = timers,
                                                  .cu_up_exec        = cu_up_worker});
}

void e1ap_cu_up_test::run_e1_setup_procedure()
{
  // > Establish connection to CU-CP.
  bool ret = e1ap->connect_to_cu_cp();
  ocudu_assert(ret, "Failed to connect to CU-CP");

  // > Launch E1 setup procedure
  cu_up_e1_setup_request request_msg = generate_cu_up_e1_setup_request();
  test_logger.info("Launch CU-UP e1 setup request procedure...");
  async_task<cu_up_e1_setup_response>         t = e1ap->handle_cu_up_e1_setup_request(request_msg);
  lazy_task_launcher<cu_up_e1_setup_response> t_launcher(t);

  // > E1 setup response received.
  unsigned     transaction_id    = get_transaction_id(e1ap_gw.last_tx_e1ap_pdu.pdu).value();
  e1ap_message e1_setup_response = generate_cu_up_e1_setup_response(transaction_id);
  test_logger.info("Injecting CuUpE1SetupResponse");
  e1ap->handle_message(e1_setup_response);
}

e1ap_cu_up_test::~e1ap_cu_up_test()
{
  // flush logger after each test
  ocudulog::flush();
}

void e1ap_cu_up_test::setup_bearer(unsigned cu_cp_ue_e1ap_id)
{
  // Generate BearerContextSetupRequest
  e1ap_message bearer_context_setup = generate_bearer_context_setup_request(cu_cp_ue_e1ap_id);

  e1ap->handle_message(bearer_context_setup);
}
