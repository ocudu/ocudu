// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/rlc/rlc_rx_tm_entity.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "ocudu/rlc/rlc_srb_config_factory.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>

using namespace ocudu;

/// Mocking class of the surrounding layers invoked by the RLC TM Rx entity.
class rlc_rx_tm_test_frame : public rlc_rx_upper_layer_data_notifier, public rlc_metrics_notifier
{
public:
  std::queue<byte_buffer_chain> sdu_queue;
  uint32_t                      sdu_counter = 0;

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_chain sdu) override
  {
    sdu_queue.push(std::move(sdu));
    sdu_counter++;
  }
  // rlc_metrics_notifier
  void report_metrics(const rlc_metrics& metrics) override {}
};

/// Fixture class for RLC TM Rx tests
class rlc_rx_am_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // init RLC logger
    ocudulog::fetch_basic_logger("RLC", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(-1);

    logger.info("Creating RLC Rx TM entity");

    // Create test frame
    tester = std::make_unique<rlc_rx_tm_test_frame>();

    metrics_coll = std::make_unique<rlc_bearer_metrics_collector>(
        gnb_du_id_t{}, du_ue_index_t{}, rb_id_t{}, timer_duration{1000}, tester.get(), ue_executor);

    // Create RLC AM TX entity
    rlc = std::make_unique<rlc_rx_tm_entity>(gnb_du_id_t::min,
                                             du_ue_index_t::MIN_DU_UE_INDEX,
                                             srb_id_t::srb0,
                                             make_default_srb0_rlc_config().tm.rx,
                                             *tester,
                                             *metrics_coll,
                                             pcap,
                                             ue_executor,
                                             timers);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  ocudulog::basic_logger&                       logger = ocudulog::fetch_basic_logger("TEST", false);
  timer_manager                                 timers;
  std::unique_ptr<rlc_rx_tm_test_frame>         tester;
  null_rlc_pcap                                 pcap;
  std::unique_ptr<rlc_rx_tm_entity>             rlc;
  std::unique_ptr<rlc_bearer_metrics_collector> metrics_coll;
  manual_task_worker                            ue_executor{128};
};

TEST_F(rlc_rx_am_test, create_new_entity)
{
  EXPECT_EQ(tester->sdu_counter, 0);
}

TEST_F(rlc_rx_am_test, test_rx)
{
  const uint32_t sdu_size = 4;
  uint32_t       count    = 0;

  // write first PDU into lower end
  byte_buffer pdu_buf =
      test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, count, sdu_size, count);
  byte_buffer_slice pdu = {pdu_buf.deep_copy().value()};
  rlc->handle_pdu(std::move(pdu));

  count++;

  // write second PDU into lower end
  byte_buffer pdu_buf2 =
      test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, count, sdu_size, count);
  pdu = {pdu_buf2.deep_copy().value()};
  rlc->handle_pdu(std::move(pdu));

  // read first SDU from tester
  EXPECT_EQ(tester->sdu_counter, 2);
  byte_buffer_chain& sdu = tester->sdu_queue.front();
  EXPECT_EQ(sdu, pdu_buf);
  tester->sdu_queue.pop();

  // read second SDU from tester
  byte_buffer_chain& sdu2 = tester->sdu_queue.front();
  EXPECT_EQ(sdu2, pdu_buf2);
  tester->sdu_queue.pop();
}

TEST_F(rlc_rx_am_test, rx_metrics_after_rx)
{
  const uint32_t sdu_size = 4;

  for (uint32_t i = 0; i < 2; i++) {
    byte_buffer pdu_buf = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ true, i, sdu_size, i);
    byte_buffer_slice pdu = {pdu_buf.deep_copy().value()};
    rlc->handle_pdu(std::move(pdu));
  }

  rlc_rx_metrics m = rlc->get_metrics();
  EXPECT_EQ(m.num_pdus, 2);
  EXPECT_EQ(m.num_pdu_bytes, 2 * sdu_size);
  EXPECT_EQ(m.num_sdus, 2);
  EXPECT_EQ(m.num_sdu_bytes, 2 * sdu_size);
  EXPECT_EQ(m.num_lost_pdus, 0);
}

TEST_F(rlc_rx_am_test, rx_empty_pdu)
{
  // An empty PDU must not be forwarded to the upper layers.
  byte_buffer_slice pdu = {byte_buffer{}};
  rlc->handle_pdu(std::move(pdu));

  EXPECT_EQ(tester->sdu_counter, 0);
  EXPECT_EQ(tester->sdu_queue.size(), 0);

  rlc_rx_metrics m = rlc->get_metrics();
  EXPECT_EQ(m.num_pdus, 1);
  EXPECT_EQ(m.num_pdu_bytes, 0);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
