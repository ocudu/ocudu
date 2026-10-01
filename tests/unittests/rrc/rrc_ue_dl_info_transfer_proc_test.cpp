// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_test_helpers.h"
#include "rrc_ue_test_messages.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class RRC DL info transfer tests preparation (to bring UE in RRC connected state)
class rrc_ue_dl_info_transfer : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override
  {
    init();

    ocudulog::basic_logger& rrc_logger = ocudulog::fetch_basic_logger("RRC", false);
    rrc_logger.set_level(ocudulog::basic_levels::debug);
    rrc_logger.set_hex_dump_max_size(32);

    receive_setup_request();

    // check if the RRC setup message was generated
    ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

    // check if SRB1 was created
    check_srb1_exists();

    receive_setup_complete();
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }
};

/// Test the RRC setup with connected AMF
TEST_F(rrc_ue_dl_info_transfer, when_srb2_missing_dl_info_tranfer_goes_over_srb1)
{
  send_dl_info_transfer(byte_buffer::create({0x00, 0x01, 0x02, 0x03}).value());
  ASSERT_EQ(get_last_srb(), srb_id_t::srb1);
}

/// Test the RRC setup with connected AMF
TEST_F(rrc_ue_dl_info_transfer, when_srb2_present_dl_info_tranfer_goes_over_srb2)
{
  create_srb2();
  send_dl_info_transfer(byte_buffer::create({0x00, 0x01, 0x02, 0x03}).value());
  ASSERT_EQ(get_last_srb(), srb_id_t::srb2);
}
