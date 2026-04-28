// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "gtpu_test_shared.h"
#include "lib/gtpu/gtpu_pdu.h"
#include "ocudu/gtpu/gtpu_demux.h"
#include "ocudu/gtpu/gtpu_demux_factory.h"
#include "ocudu/gtpu/gtpu_tunnel_common_tx.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <sys/socket.h>

using namespace ocudu;

// dummy CU-UP GTP-U TEID lingering interface
class dummy_gtpu_teid_lingering_interface : public gtpu_teid_lingering_interface
{
public:
  dummy_gtpu_teid_lingering_interface() = default;

  bool is_teid_lingering(gtpu_teid_t teid) override { return lingering; }

  void set_teid_lingering(bool is_lingering) { lingering = is_lingering; }

private:
  bool lingering = false;
};

class gtpu_tunnel_rx_upper_dummy : public gtpu_tunnel_common_rx_upper_layer_interface
{
public:
  void handle_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) final { last_rx = std::move(pdu); }

  byte_buffer last_rx;
};

class gtpu_tx_upper_dummy : public gtpu_tunnel_common_tx_upper_layer_notifier
{
public:
  void on_new_pdu(byte_buffer buf, const sockaddr_storage& addr) final { last_tx = std::move(buf); }

  byte_buffer last_tx;
};

/// Fixture class for GTPU demux tests
class gtpu_demux_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    gtpu_tunnel = std::make_unique<gtpu_tunnel_rx_upper_dummy>();

    // create DUT object
    gtpu_demux_creation_request msg = {};
    msg.teid_linger_checker         = &teid_linger_checker;
    msg.gtpu_pcap                   = &dummy_pcap;
    dut                             = create_gtpu_demux(msg);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  std::unique_ptr<gtpu_tunnel_rx_upper_dummy> gtpu_tunnel;
  manual_task_worker                          teid_worker{128};
  dummy_gtpu_teid_lingering_interface         teid_linger_checker;
  null_dlt_pcap                               dummy_pcap;

  std::unique_ptr<gtpu_demux> dut;
  ocudulog::basic_logger&     test_logger = ocudulog::fetch_basic_logger("TEST", false);
};

//// GTPU demux tesst
TEST_F(gtpu_demux_test, when_tunnel_not_registered_pdu_is_dropped)
{
  sockaddr_storage src_addr = {};
  byte_buffer      pdu      = byte_buffer::create(gtpu_ping_vec_teid_1).value();

  // Configure error indication TX so we can verify it is sent.
  gtpu_tx_upper_dummy dummy_tx;
  dut->set_error_indication_tx(dummy_tx, "127.0.0.1");

  dut->handle_pdu(std::move(pdu), src_addr);
  teid_worker.run_pending_tasks();

  ASSERT_EQ(gtpu_tunnel->last_rx.length(), 0);
  ASSERT_GT(dummy_tx.last_tx.length(), 0);

  // Verify the error indication TEID IE matches the received PDU.
  dummy_tx.last_tx.trim_head(GTPU_EXTENDED_HEADER_LEN);
  gtpu_msg_error_indication err_ind = {};
  ASSERT_TRUE(gtpu_read_msg_error_indication(err_ind, dummy_tx.last_tx, test_logger));
  ASSERT_EQ(err_ind.teid_i.teid_i, gtpu_teid_t{0x1}.value());
}

TEST_F(gtpu_demux_test, when_tunnel_registered_pdu_is_forwarded)
{
  sockaddr_storage src_addr = {};
  byte_buffer      pdu      = byte_buffer::create(gtpu_ping_vec_teid_1).value();
  auto             queue    = dut->add_tunnel(gtpu_teid_t{0x1}, teid_worker, gtpu_tunnel.get());

  // Configure error indication TX so we can verify it is not sent.
  gtpu_tx_upper_dummy dummy_tx;
  dut->set_error_indication_tx(dummy_tx, "127.0.0.1");

  dut->handle_pdu(std::move(pdu), src_addr);
  teid_worker.run_pending_tasks();

  ASSERT_EQ(gtpu_tunnel->last_rx.length(), sizeof(gtpu_ping_vec_teid_1));
  ASSERT_TRUE(dummy_tx.last_tx.empty());
}

TEST_F(gtpu_demux_test, when_tunnel_was_removed_pdu_is_dropped)
{
  sockaddr_storage src_addr = {};
  byte_buffer      pdu      = byte_buffer::create(gtpu_ping_vec_teid_1).value();
  auto             queue    = dut->add_tunnel(gtpu_teid_t{0x1}, teid_worker, gtpu_tunnel.get());
  dut->remove_tunnel(gtpu_teid_t{0x1});

  // Configure error indication TX so we can verify it is sent.
  gtpu_tx_upper_dummy dummy_tx;
  dut->set_error_indication_tx(dummy_tx, "127.0.0.1");

  // pass and handle PDU when tunnel was already removed
  dut->handle_pdu(std::move(pdu), src_addr);
  teid_worker.run_pending_tasks();

  ASSERT_EQ(gtpu_tunnel->last_rx.length(), 0);
  ASSERT_GT(dummy_tx.last_tx.length(), 0);

  // Verify the error indication TEID IE matches the removed tunnel.
  dummy_tx.last_tx.trim_head(GTPU_EXTENDED_HEADER_LEN);
  gtpu_msg_error_indication err_ind = {};
  ASSERT_TRUE(gtpu_read_msg_error_indication(err_ind, dummy_tx.last_tx, test_logger));
  ASSERT_EQ(err_ind.teid_i.teid_i, gtpu_teid_t{0x1}.value());
}

TEST_F(gtpu_demux_test, when_tunnel_is_being_removed_pdu_is_dropped)
{
  sockaddr_storage src_addr = {};
  byte_buffer      pdu      = byte_buffer::create(gtpu_ping_vec_teid_1).value();
  auto             queue    = dut->add_tunnel(gtpu_teid_t{0x1}, teid_worker, gtpu_tunnel.get());

  // pass PDU while tunnel is present
  dut->handle_pdu(std::move(pdu), src_addr);

  // remove tunnel while the PDU handler is queued for execution
  dut->remove_tunnel(gtpu_teid_t{0x1});

  // handle PDU when tunnel was removed; the PDU shall be dropped
  teid_worker.run_pending_tasks();

  ASSERT_EQ(gtpu_tunnel->last_rx.length(), 0);
}

TEST_F(gtpu_demux_test, when_different_tunnel_registered_pdu_is_dropped)
{
  sockaddr_storage src_addr = {};
  byte_buffer      pdu      = byte_buffer::create(gtpu_ping_vec_teid_2).value();
  auto             queue    = dut->add_tunnel(gtpu_teid_t{0x1}, teid_worker, gtpu_tunnel.get());

  dut->handle_pdu(std::move(pdu), src_addr);
  teid_worker.run_pending_tasks();

  ASSERT_EQ(gtpu_tunnel->last_rx.length(), 0);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
