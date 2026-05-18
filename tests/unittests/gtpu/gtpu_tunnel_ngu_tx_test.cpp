// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/gtpu/gtpu_pdu.h"
#include "lib/gtpu/gtpu_tunnel_ngu_rx_impl.h"
#include "lib/gtpu/gtpu_tunnel_ngu_tx_impl.h"
#include "tests/unittests/gtpu/gtpu_test_shared.h"
#include "ocudu/support/bit_encoding.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <sys/socket.h>

using namespace ocudu;

class gtpu_tunnel_tx_upper_dummy : public gtpu_tunnel_common_tx_upper_layer_notifier
{
  void on_new_pdu(byte_buffer buf, const ::sockaddr_storage& dest_addr) final
  {
    tx_ul_pdus.push_back(buf);
    last_dest_addr = dest_addr;
  }

public:
  void clear()
  {
    tx_ul_pdus.clear();
    last_dest_addr = {};
  }

  std::vector<byte_buffer> tx_ul_pdus;
  ::sockaddr_storage       last_dest_addr = {};
};

// Extract the TEID field (bytes 4-7, big-endian) from a captured GTP-U PDU.
static uint32_t extract_gtpu_teid(const byte_buffer& pdu)
{
  auto it = pdu.begin();
  std::advance(it, 4);
  uint32_t teid = 0;
  for (int i = 0; i < 4; ++i, ++it) {
    teid = (teid << 8U) | static_cast<uint8_t>(*it);
  }
  return teid;
}

// Extract the IPv4 destination address from a sockaddr_storage.
static std::string dest_addr_to_str(const ::sockaddr_storage& addr)
{
  char buf[INET6_ADDRSTRLEN] = {};
  if (addr.ss_family == AF_INET) {
    ::inet_ntop(AF_INET, &reinterpret_cast<const ::sockaddr_in&>(addr).sin_addr, buf, sizeof(buf));
  } else if (addr.ss_family == AF_INET6) {
    ::inet_ntop(AF_INET6, &reinterpret_cast<const ::sockaddr_in6&>(addr).sin6_addr, buf, sizeof(buf));
  }
  return buf;
}

/// Fixture class for GTP-U tunnel NG-U Rx tests
class gtpu_tunnel_ngu_tx_test : public ::testing::Test
{
public:
  gtpu_tunnel_ngu_tx_test() :
    logger(ocudulog::fetch_basic_logger("TEST", false)), gtpu_logger(ocudulog::fetch_basic_logger("GTPU", false))
  {
  }

protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // init GTP-U logger
    gtpu_logger.set_level(ocudulog::basic_levels::debug);
    gtpu_logger.set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    tx_upper.clear();
    ocudulog::flush();
  }

  /// \brief Helper to advance the timers
  /// \param nof_tick Number of ticks to advance timers
  void tick_all(uint32_t nof_ticks)
  {
    for (uint32_t i = 0; i < nof_ticks; i++) {
      timers_manager.tick();
      worker.run_pending_tasks();
    }
  }

  // Test logger
  ocudulog::basic_logger& logger;

  // GTP-U logger
  ocudulog::basic_logger& gtpu_logger;
  gtpu_tunnel_logger      gtpu_rx_logger{"GTPU", {{}, gtpu_teid_t{1}, "DL"}};

  // Timers
  manual_task_worker worker{64};
  timer_manager      timers_manager;
  timer_factory      timers{timers_manager, worker};

  // GTP-U tunnel Rx entity
  std::unique_ptr<gtpu_tunnel_ngu_tx_impl> tx;

  // Surrounding tester
  gtpu_tunnel_tx_upper_dummy tx_upper = {};

  null_dlt_pcap dummy_pcap;
};

/// \brief Test correct creation of Rx entity
TEST_F(gtpu_tunnel_ngu_tx_test, entity_creation)
{
  // create Tx entity
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config tx_cfg = {};
  tx_cfg.peer_addr                                         = "127.0.0.1";
  tx_cfg.peer_teid                                         = gtpu_teid_t{0x1};

  tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);

  ASSERT_NE(tx, nullptr);
}

/// \brief Test reception of PDUs with no SN
TEST_F(gtpu_tunnel_ngu_tx_test, tx_sdus)
{
  // create Rx entity
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config tx_cfg = {};
  tx_cfg.peer_addr                                         = "127.0.0.1";
  tx_cfg.peer_teid                                         = gtpu_teid_t{0x2};

  tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);
  ASSERT_NE(tx, nullptr);

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu = byte_buffer::create(gtpu_ping_sdu).value();
    byte_buffer pdu = byte_buffer::create(gtpu_ping_vec_teid_2_qfi_1_ul).value();

    tx->handle_sdu(std::move(sdu), uint_to_qos_flow_id(1));
    ASSERT_EQ(pdu, tx_upper.tx_ul_pdus[i]);
  }
}

/// \brief Test in-order reception of PDUs
TEST_F(gtpu_tunnel_ngu_tx_test, tx_stop)
{
  // create Rx entity
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config tx_cfg = {};
  tx_cfg.peer_addr                                         = "127.0.0.1";
  tx_cfg.peer_teid                                         = gtpu_teid_t{0x2};

  tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);
  ASSERT_NE(tx, nullptr);

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu = byte_buffer::create(gtpu_ping_sdu).value();
    byte_buffer pdu = byte_buffer::create(gtpu_ping_vec_teid_2_qfi_1_ul).value();

    tx->handle_sdu(std::move(sdu), uint_to_qos_flow_id(1));
    ASSERT_EQ(pdu, tx_upper.tx_ul_pdus[i]);
  }
  tx->stop();
  tx_upper.tx_ul_pdus.clear();

  // No more PDUs should be accepted.
  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu = byte_buffer::create(gtpu_ping_sdu).value();
    tx->handle_sdu(std::move(sdu), uint_to_qos_flow_id(1));
    ASSERT_TRUE(tx_upper.tx_ul_pdus.empty());
  }
}

/// \brief After update_tx_endpoint() subsequent SDUs carry the new TEID in the GTP-U header.
TEST_F(gtpu_tunnel_ngu_tx_test, update_tx_endpoint_changes_teid_in_pdu)
{
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config tx_cfg = {};
  tx_cfg.peer_addr                                         = "127.0.0.1";
  tx_cfg.peer_teid                                         = gtpu_teid_t{0x2};

  tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);
  ASSERT_NE(tx, nullptr);

  // SDU before the update should carry the original TEID (0x2).
  byte_buffer sdu_before = byte_buffer::create(gtpu_ping_sdu).value();
  tx->handle_sdu(std::move(sdu_before), uint_to_qos_flow_id(1));
  ASSERT_EQ(tx_upper.tx_ul_pdus.size(), 1U);
  EXPECT_EQ(extract_gtpu_teid(tx_upper.tx_ul_pdus[0]), 0x2U);
  tx_upper.clear();

  // Update the endpoint to a new TEID.
  tx->update_tx_endpoint("127.0.0.2", 2152, 0x5);

  // SDU after the update must carry the new TEID (0x5).
  byte_buffer sdu_after = byte_buffer::create(gtpu_ping_sdu).value();
  tx->handle_sdu(std::move(sdu_after), uint_to_qos_flow_id(1));
  ASSERT_EQ(tx_upper.tx_ul_pdus.size(), 1U);
  EXPECT_EQ(extract_gtpu_teid(tx_upper.tx_ul_pdus[0]), 0x5U);

  // Destination address should reflect the new peer.
  EXPECT_EQ(dest_addr_to_str(tx_upper.last_dest_addr), "127.0.0.2");
}

/// \brief Calling update_tx_endpoint() a second time overrides the first update.
TEST_F(gtpu_tunnel_ngu_tx_test, update_tx_endpoint_second_call_overrides_first)
{
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config tx_cfg = {};
  tx_cfg.peer_addr                                         = "127.0.0.1";
  tx_cfg.peer_teid                                         = gtpu_teid_t{0x1};

  tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);

  tx->update_tx_endpoint("10.0.0.1", 2152, 0xaa);
  tx->update_tx_endpoint("10.0.0.2", 2152, 0xbb);

  byte_buffer sdu = byte_buffer::create(gtpu_ping_sdu).value();
  tx->handle_sdu(std::move(sdu), uint_to_qos_flow_id(1));
  ASSERT_EQ(tx_upper.tx_ul_pdus.size(), 1U);

  // Only the second update should be in effect.
  EXPECT_EQ(extract_gtpu_teid(tx_upper.tx_ul_pdus[0]), 0xbbU);
  EXPECT_EQ(dest_addr_to_str(tx_upper.last_dest_addr), "10.0.0.2");
}

/// \brief Without calling update_tx_endpoint() the original TEID and address remain unchanged
/// (negative: no accidental mutation of TX state at construction or stop).
TEST_F(gtpu_tunnel_ngu_tx_test, no_update_keeps_original_endpoint)
{
  gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config tx_cfg = {};
  tx_cfg.peer_addr                                         = "127.0.0.1";
  tx_cfg.peer_teid                                         = gtpu_teid_t{0x2};

  tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu = byte_buffer::create(gtpu_ping_sdu).value();
    tx->handle_sdu(std::move(sdu), uint_to_qos_flow_id(1));
    EXPECT_EQ(extract_gtpu_teid(tx_upper.tx_ul_pdus[i]), 0x2U);
    EXPECT_EQ(dest_addr_to_str(tx_upper.last_dest_addr), "127.0.0.1");
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
