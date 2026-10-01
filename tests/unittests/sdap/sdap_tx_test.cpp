// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/sdap/sdap_entity_tx_impl.h"
#include "ocudu/sdap/sdap.h"
#include <gtest/gtest.h>
#include <queue>

using namespace ocudu;
using namespace ocuup;

/// Mocking class of the surrounding layers invoked by the SDAP TX entity.
class sdap_tx_test_frame : public sdap_tx_pdu_notifier
{
public:
  std::queue<byte_buffer> pdu_queue;

  // sdap_tx_pdu_notifier interface
  void on_new_pdu(byte_buffer pdu) override { pdu_queue.push(std::move(pdu)); }
};

/// Fixture class for SDAP TX tests
class sdap_tx_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // init SDAP logger
    ocudulog::fetch_basic_logger("SDAP", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("SDAP", false).set_hex_dump_max_size(-1);

    logger.info("Creating SDAP TX entity.");

    // Create test frame
    tester = std::make_unique<sdap_tx_test_frame>();

    // Create SDAP TX entity
    sdap = std::make_unique<sdap_entity_tx_impl>(7, pdu_session_id_t::min, qos_flow_id_t::min, drb_id_t::drb1, *tester);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  ocudulog::basic_logger&              logger = ocudulog::fetch_basic_logger("TEST", false);
  std::unique_ptr<sdap_tx_test_frame>  tester;
  std::unique_ptr<sdap_entity_tx_impl> sdap;
};

/// \brief Test correct creation of SDAP TX entity
TEST_F(sdap_tx_test, create_new_entity)
{
  ASSERT_NE(sdap, nullptr);
}

/// \brief Test simple PDU transmission
TEST_F(sdap_tx_test, test_tx)
{
  const std::array<uint8_t, 4> sdu_buf = {0x00, 0x01, 0x02, 0x03};
  byte_buffer                  sdu     = byte_buffer::create(sdu_buf).value();

  sdap->handle_sdu(sdu.deep_copy().value());

  ASSERT_FALSE(tester->pdu_queue.empty());
  EXPECT_EQ(tester->pdu_queue.front(), sdu);
  tester->pdu_queue.pop();
  ASSERT_TRUE(tester->pdu_queue.empty());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
