// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_processor_test_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_up_test_messages.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;
using namespace asn1::e1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* E1 setup                                                                         */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the cu-up processor start
TEST_F(cu_up_processor_test, when_valid_e1_setup_received_then_e1_setup_response_sent)
{
  // Generate valid CuUpE1SetupRequest
  cu_up_e1_setup_request e1_setup_request = ocudu::ocuup::generate_cu_up_e1_setup_request();

  // Pass message to CU-UP processor
  cu_up_processor_obj->handle_cu_up_e1_setup_request(e1_setup_request);

  // check that CU-UP E1 setup response was sent
  ASSERT_EQ(e1ap_notifier.last_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(e1ap_notifier.last_msg.pdu.successful_outcome().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_up_e1_setup_resp);
}
