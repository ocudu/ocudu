// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../common/test_helpers.h"
#include "ocudu/e1ap/common/e1ap_common.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "ocudu/e1ap/gateways/e1_connection_client.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace ocudu {
namespace ocuup {

class dummy_e1_connection_client : public ocuup::e1_connection_client
{
public:
  e1ap_message last_tx_e1ap_pdu;

  std::unique_ptr<e1ap_message_notifier>
  handle_cu_up_connection_request(std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier) override;

private:
  std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier;
};

/// Fixture class for E1AP
class e1ap_cu_up_test : public ::testing::Test
{
protected:
  e1ap_cu_up_test();
  ~e1ap_cu_up_test() override;

  /// \brief Run E1 Setup Procedure to completion.
  void run_e1_setup_procedure();

  /// \brief Helper method to setup a bearer at the CU-UP
  void setup_bearer(unsigned cu_cp_ue_e1ap_id);

  timer_manager timers;

  ocudulog::basic_logger& e1ap_logger = ocudulog::fetch_basic_logger("E1AP");
  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");

  dummy_e1ap_cu_up_notifier       cu_up_notifier;
  manual_task_worker              cu_up_worker{128};
  std::unique_ptr<e1ap_interface> e1ap;

  /// Dummy E1AP gateway to connect to CU-CP and send E1AP PDUs.
  dummy_e1_connection_client e1ap_gw;
};

} // namespace ocuup
} // namespace ocudu
