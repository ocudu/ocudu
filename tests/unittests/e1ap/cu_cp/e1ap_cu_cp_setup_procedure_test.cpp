// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../common/test_helpers.h"
#include "e1ap_cu_cp_test_helpers.h"
#include "lib/e1ap/common/e1ap_asn1_utils.h"
#include "tests/test_doubles/e1ap/e1ap_cu_cp_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_factory.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/////////////////////////////////////
// CU-UP initiated E1 Setup Procedure
/////////////////////////////////////

/// Test the successful CU-UP initiated e1 setup procedure
TEST_F(e1ap_cu_cp_test, when_received_cu_up_e1_setup_request_valid_then_connect_cu_up)
{
  // Action 1: Receive CuUpE1SetupRequest message
  test_logger.info("TEST: Receive CuUpE1SetupRequest message...");

  // Generate E1SetupRequest
  e1ap_message e1_setup_msg = generate_valid_cu_up_e1_setup_request();
  e1ap->handle_message(e1_setup_msg);

  // Action 2: Check if CuUpE1SetupRequest was forwarded to NGAP
  ASSERT_EQ(cu_up_processor_notifier.last_cu_up_e1_setup_request.gnb_cu_up_name.value(), "OCUDU CU-UP");

  // Action 3: Transmit CuUpE1SetupResponse message
  test_logger.info("TEST: Transmit CuUpE1SetupResponse message...");
  cu_up_e1_setup_response msg = {};
  msg.success                 = true;
  e1ap->handle_cu_up_e1_setup_response(msg);

  // Check the generated PDU is indeed the E1 Setup response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_cu_up_e1_setup_resp,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.successful_outcome().value.type());
}

/// Test the e1 setup failure
TEST_F(e1ap_cu_cp_test, when_received_cu_up_e1_setup_request_invalid_then_reject_cu_up)
{
  // Generate CuUpE1SetupRequest
  e1ap_message e1_setup_msg = generate_cu_up_e1_setup_request_base();
  e1ap->handle_message(e1_setup_msg);

  // Action 2 : Check if E1SetupRequest was forwarded to NGAP
  ASSERT_EQ(cu_up_processor_notifier.last_cu_up_e1_setup_request.gnb_cu_up_name.value(), "OCUDU CU-UP");

  // Action 3: Transmit E1SetupFailure message
  test_logger.info("TEST: Transmit CuUpE1SetupFailure message...");
  cu_up_e1_setup_response msg = {};
  msg.success                 = false;
  msg.cause                   = e1ap_cause_radio_network_t::unspecified;
  e1ap->handle_cu_up_e1_setup_response(msg);

  // Check the generated PDU is indeed the E1 Setup failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_e1_setup_fail,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.unsuccessful_outcome().value.type());
}
