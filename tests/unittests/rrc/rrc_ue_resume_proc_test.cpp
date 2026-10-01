// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_test_helpers.h"
#include "rrc_ue_test_messages.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class for RRC Resume tests preparation
class rrc_ue_resume : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override { init(); }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }
};

/// A UE that completed RRC Setup but never activated AS security is no longer in RRC_IDLE, so an RRCResumeRequest
/// takes the local resume path. The procedure has no keys to verify the ResumeMAC-I with, and must release the UE
/// rather than refresh keys derived from an unselected algorithm.
TEST_F(rrc_ue_resume, when_resume_request_received_without_security_context_then_ue_released)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-10");

  receive_setup_request();
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);
  receive_setup_complete();

  receive_resume_request();

  check_ue_release_requested();
}
