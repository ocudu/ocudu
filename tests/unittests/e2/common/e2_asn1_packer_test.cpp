// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e2_test_helpers.h"
#include "e2ap_asn1_packer.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// Fixture class for E2AP ASN1 packer
class e2_asn1_packer_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("E2-ASN1-PCK").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    gw     = std::make_unique<dummy_sctp_association_sdu_notifier>();
    e2     = std::make_unique<dummy_e2_message_handler>();
    pcap   = std::make_unique<dummy_e2ap_pcap>();
    packer = std::make_unique<e2ap_asn1_packer>(*gw, *e2, *pcap);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  std::unique_ptr<dummy_sctp_association_sdu_notifier> gw;
  std::unique_ptr<dummy_e2_message_handler>            e2;
  std::unique_ptr<dummy_e2ap_pcap>                     pcap;
  std::unique_ptr<e2ap_asn1_packer>                    packer;
  ocudulog::basic_logger&                              test_logger = ocudulog::fetch_basic_logger("TEST");
};

/// Test successful packing and unpacking
TEST_F(e2_asn1_packer_test, when_packing_successful_then_unpacking_successful)
{
  // Action 1: Create valid e2 message
  e2_message e2_setup_request = generate_e2_setup_request(e2sm_kpm_asn1_packer::oid);

  // Action 2: Pack message and forward to gateway
  packer->handle_message(e2_setup_request);

  // Action 3: Unpack message and forward to e2
  packer->handle_packed_pdu(std::move(gw->last_pdu));

  // Assert that the originally created message is equal to the unpacked message
  ASSERT_EQ(e2->last_msg.pdu.type(), e2_setup_request.pdu.type());
}
