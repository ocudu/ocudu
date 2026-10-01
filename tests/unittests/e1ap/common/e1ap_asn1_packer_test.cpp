// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/e1ap/common/e1ap_asn1_packer.h"
#include "test_helpers.h"
#include "tests/test_doubles/e1ap/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/gateways/test_helpers.h"
#include "ocudu/adt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class for E1AP ASN1 packer
class e1ap_asn1_packer_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("E1AP-ASN1-PCK").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    gw     = std::make_unique<dummy_network_gateway_data_handler>();
    e1ap   = std::make_unique<dummy_e1ap_message_handler>();
    packer = std::make_unique<ocudu::e1ap_asn1_packer>(*gw, *e1ap);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  std::unique_ptr<dummy_network_gateway_data_handler> gw;
  std::unique_ptr<dummy_e1ap_message_handler>         e1ap;
  std::unique_ptr<ocudu::e1ap_asn1_packer>            packer;
  ocudulog::basic_logger&                             test_logger = ocudulog::fetch_basic_logger("TEST");
};

/// Test successful packing and unpacking
TEST_F(e1ap_asn1_packer_test, when_packing_successful_then_unpacking_successful)
{
  // Action 1: Create valid e1ap message
  e1ap_message e1_setup_request = generate_valid_cu_up_e1_setup_request();

  // Action 2: Pack message and forward to gateway
  packer->handle_message(e1_setup_request);

  // Action 3: Unpack message and forward to e1
  packer->handle_packed_pdu(std::move(gw->last_pdu));

  // Assert that the originally created message is equal to the unpacked message
  ASSERT_EQ(e1ap->last_msg.pdu.type(), e1_setup_request.pdu.type());
}

/// Test unsuccessful packing
TEST_F(e1ap_asn1_packer_test, when_packing_unsuccessful_then_message_not_forwarded)
{
  // Action 1: Generate, pack and forward valid message to bring gw into known state
  e1ap_message e1_setup_request = generate_valid_cu_up_e1_setup_request();
  packer->handle_message(e1_setup_request);
  // store size of valid pdu
  int valid_pdu_size = gw->last_pdu.length();

  // Action 2: Create invalid e1ap message
  e1ap_message e1ap_msg = generate_cu_up_e1_setup_request_base();

  // Action 3: Pack message and forward to gateway
  packer->handle_message(e1ap_msg);

  // check that msg was not forwarded to gw
  ASSERT_EQ(gw->last_pdu.length(), valid_pdu_size);
}

// TODO: test unsuccessful unpacking
