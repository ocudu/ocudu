// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "gtpu_test_shared.h"
#include "lib/gtpu/gtpu_pdu.h"
#include "ocudu/gtpu/gtpu_tunnel_common_rx.h"
#include "ocudu/gtpu/gtpu_tunnel_pdcp_factory.h"
#include "ocudu/gtpu/gtpu_tunnel_pdcp_tx.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <sys/socket.h>

using namespace ocudu;

namespace {

class gtpu_tunnel_rx_lower_dummy : public gtpu_tunnel_pdcp_rx_lower_layer_notifier
{
public:
  void on_new_sdu(byte_buffer tpdu, uint32_t pdcp_pdu_number) final
  {
    last_tpdu            = std::move(tpdu);
    last_pdcp_pdu_number = pdcp_pdu_number;
  }

  byte_buffer last_tpdu;
  uint32_t    last_pdcp_pdu_number;
};
class gtpu_tunnel_tx_upper_dummy : public gtpu_tunnel_common_tx_upper_layer_notifier
{
public:
  void on_new_pdu(byte_buffer gpdu, const ::sockaddr_storage& dest_addr) final
  {
    last_tx   = std::move(gpdu);
    last_addr = dest_addr;
  }

  byte_buffer      last_tx;
  sockaddr_storage last_addr = {};
};

class gtpu_tunnel_rx_upper_dummy : public gtpu_tunnel_common_rx_upper_layer_interface
{
public:
  void handle_pdu(byte_buffer gpdu, const sockaddr_storage& src_addr) final
  {
    last_rx   = std::move(gpdu);
    last_addr = src_addr;
  }

  byte_buffer      last_rx;
  sockaddr_storage last_addr = {};
};

/// Fixture class for GTP-U tunnel PDCP tests.
class gtpu_tunnel_pdcp_test : public ::testing::Test
{
public:
  gtpu_tunnel_pdcp_test() :
    logger(ocudulog::fetch_basic_logger("TEST", false)), gtpu_logger(ocudulog::fetch_basic_logger("GTPU", false))
  {
  }

protected:
  void SetUp() override
  {
    // init test's logger.
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // init GTP-U logger.
    gtpu_logger.set_level(ocudulog::basic_levels::debug);
    gtpu_logger.set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test.
    ocudulog::flush();
  }

  // Test logger.
  ocudulog::basic_logger& logger;

  // GTP-U logger.
  ocudulog::basic_logger& gtpu_logger;
  gtpu_tunnel_logger      gtpu_rx_logger{"GTPU", {gtpu_logical_interface::xnu, {}, gtpu_teid_t{1}, "DL"}};

  // GTP-U tunnel entity.
  std::unique_ptr<gtpu_tunnel_pdcp> gtpu;

  // Surrounding tester.
  gtpu_tunnel_rx_lower_dummy gtpu_rx = {};
  gtpu_tunnel_tx_upper_dummy gtpu_tx = {};
};

/// \brief Test correct creation of GTP-U entity.
TEST_F(gtpu_tunnel_pdcp_test, entity_creation_pdcp_sn_12bit)
{
  null_dlt_pcap dummy_pcap;

  // init GTP-U entity.
  gtpu_tunnel_pdcp_creation_message msg = {};
  msg.cfg.rx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.rx.local_teid                 = gtpu_teid_t{0x1};
  msg.cfg.rx.pdcp_sn_len                = pdcp_sn_size::size12bits;
  msg.cfg.tx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.tx.peer_teid                  = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                  = "127.0.0.1";
  msg.cfg.tx.pdcp_sn_len                = pdcp_sn_size::size12bits;
  msg.gtpu_pcap                         = &dummy_pcap;
  msg.rx_lower                          = &gtpu_rx;
  msg.tx_upper                          = &gtpu_tx;
  gtpu                                  = create_gtpu_tunnel_pdcp(msg);

  ASSERT_NE(gtpu, nullptr);
}

/// \brief Test correct creation of GTP-U entity.
TEST_F(gtpu_tunnel_pdcp_test, entity_creation_pdcp_sn_18bit)
{
  null_dlt_pcap dummy_pcap;

  // init GTP-U entity.
  gtpu_tunnel_pdcp_creation_message msg = {};
  msg.cfg.rx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.rx.local_teid                 = gtpu_teid_t{0x1};
  msg.cfg.rx.pdcp_sn_len                = pdcp_sn_size::size18bits;
  msg.cfg.tx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.tx.peer_teid                  = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                  = "127.0.0.1";
  msg.cfg.tx.pdcp_sn_len                = pdcp_sn_size::size18bits;
  msg.gtpu_pcap                         = &dummy_pcap;
  msg.rx_lower                          = &gtpu_rx;
  msg.tx_upper                          = &gtpu_tx;
  gtpu                                  = create_gtpu_tunnel_pdcp(msg);

  ASSERT_NE(gtpu, nullptr);
}

/// \brief Test correct reception of GTP-U packet with PDCP PDU number.
TEST_F(gtpu_tunnel_pdcp_test, rx_pdcp_sn_12bit)
{
  null_dlt_pcap dummy_pcap;

  // init GTP-U entity.
  gtpu_tunnel_pdcp_creation_message msg = {};
  msg.cfg.rx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.rx.local_teid                 = gtpu_teid_t{0x2};
  msg.cfg.rx.pdcp_sn_len                = pdcp_sn_size::size12bits;
  msg.cfg.tx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.tx.peer_teid                  = gtpu_teid_t{0xbc1e3be9};
  msg.cfg.tx.peer_addr                  = "127.0.0.1";
  msg.cfg.tx.pdcp_sn_len                = pdcp_sn_size::size12bits;
  msg.gtpu_pcap                         = &dummy_pcap;
  msg.rx_lower                          = &gtpu_rx;
  msg.tx_upper                          = &gtpu_tx;
  gtpu                                  = create_gtpu_tunnel_pdcp(msg);

  sockaddr_storage   orig_addr = {};
  byte_buffer        orig_vec  = byte_buffer::create(gpdu_tpdu_1_teid_1_pdcp_sn_1).value();
  byte_buffer        strip_vec = byte_buffer::create(gpdu_tpdu_1_teid_1_pdcp_sn_1).value();
  gtpu_dissected_pdu dissected_pdu;
  bool               read_ok = gtpu_dissect_pdu(dissected_pdu, strip_vec.deep_copy().value(), gtpu_rx_logger);
  ASSERT_EQ(read_ok, true);

  gtpu_tunnel_common_rx_upper_layer_interface* rx = gtpu->get_rx_upper_layer_interface();
  rx->handle_pdu(std::move(orig_vec), orig_addr);
  ASSERT_EQ(gtpu_extract_msg(std::move(dissected_pdu)), gtpu_rx.last_tpdu);
  ASSERT_EQ(1, gtpu_rx.last_pdcp_pdu_number);
}

/// \brief Test correct reception of GTP-U packet with PDCP PDU number.
TEST_F(gtpu_tunnel_pdcp_test, rx_pdcp_sn_18bit)
{
  null_dlt_pcap dummy_pcap;

  // init GTP-U entity.
  gtpu_tunnel_pdcp_creation_message msg = {};
  msg.cfg.rx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.rx.local_teid                 = gtpu_teid_t{0x2};
  msg.cfg.rx.pdcp_sn_len                = pdcp_sn_size::size18bits;
  msg.cfg.tx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.tx.peer_teid                  = gtpu_teid_t{0xbc1e3be9};
  msg.cfg.tx.peer_addr                  = "127.0.0.1";
  msg.cfg.tx.pdcp_sn_len                = pdcp_sn_size::size18bits;
  msg.gtpu_pcap                         = &dummy_pcap;
  msg.rx_lower                          = &gtpu_rx;
  msg.tx_upper                          = &gtpu_tx;
  gtpu                                  = create_gtpu_tunnel_pdcp(msg);

  sockaddr_storage   orig_addr = {};
  byte_buffer        orig_vec  = byte_buffer::create(gpdu_tpdu_1_teid_1_long_pdcp_sn_1).value();
  byte_buffer        strip_vec = byte_buffer::create(gpdu_tpdu_1_teid_1_long_pdcp_sn_1).value();
  gtpu_dissected_pdu dissected_pdu;
  bool               read_ok = gtpu_dissect_pdu(dissected_pdu, strip_vec.deep_copy().value(), gtpu_rx_logger);
  ASSERT_EQ(read_ok, true);

  gtpu_tunnel_common_rx_upper_layer_interface* rx = gtpu->get_rx_upper_layer_interface();
  rx->handle_pdu(std::move(orig_vec), orig_addr);
  ASSERT_EQ(gtpu_extract_msg(std::move(dissected_pdu)), gtpu_rx.last_tpdu);
  ASSERT_EQ(1, gtpu_rx.last_pdcp_pdu_number);
}

/// \brief Test correct transmission of GTP-U packet with PDCP PDU number.
TEST_F(gtpu_tunnel_pdcp_test, tx_pdcp_sn_12bit)
{
  null_dlt_pcap dummy_pcap;

  // init GTP-U entity.
  gtpu_tunnel_pdcp_creation_message msg = {};
  msg.cfg.rx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.rx.local_teid                 = gtpu_teid_t{0x1};
  msg.cfg.rx.pdcp_sn_len                = pdcp_sn_size::size12bits;
  msg.cfg.tx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.tx.peer_teid                  = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                  = "127.0.0.1";
  msg.cfg.tx.pdcp_sn_len                = pdcp_sn_size::size12bits;
  msg.gtpu_pcap                         = &dummy_pcap;
  msg.rx_lower                          = &gtpu_rx;
  msg.tx_upper                          = &gtpu_tx;
  gtpu                                  = create_gtpu_tunnel_pdcp(msg);

  byte_buffer tpdu = byte_buffer::create(tpdu_1).value();
  byte_buffer gpdu = byte_buffer::create(gpdu_tpdu_1_teid_1_pdcp_sn_1).value();

  gtpu_tunnel_pdcp_tx_lower_layer_interface* tx = gtpu->get_tx_lower_layer_interface();
  tx->handle_sdu(std::move(tpdu), 1);
  ASSERT_EQ(gpdu, gtpu_tx.last_tx);
}

/// \brief Test correct transmission of GTP-U packet with PDCP PDU number.
TEST_F(gtpu_tunnel_pdcp_test, tx_pdcp_sn_18bit)
{
  null_dlt_pcap dummy_pcap;

  // init GTP-U entity.
  gtpu_tunnel_pdcp_creation_message msg = {};
  msg.cfg.rx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.rx.local_teid                 = gtpu_teid_t{0x1};
  msg.cfg.rx.pdcp_sn_len                = pdcp_sn_size::size18bits;
  msg.cfg.tx.lif                        = gtpu_logical_interface::xnu;
  msg.cfg.tx.peer_teid                  = gtpu_teid_t{0x2};
  msg.cfg.tx.peer_addr                  = "127.0.0.1";
  msg.cfg.tx.pdcp_sn_len                = pdcp_sn_size::size18bits;
  msg.gtpu_pcap                         = &dummy_pcap;
  msg.rx_lower                          = &gtpu_rx;
  msg.tx_upper                          = &gtpu_tx;
  gtpu                                  = create_gtpu_tunnel_pdcp(msg);

  byte_buffer tpdu = byte_buffer::create(tpdu_1).value();
  byte_buffer gpdu = byte_buffer::create(gpdu_tpdu_1_teid_1_long_pdcp_sn_1).value();

  gtpu_tunnel_pdcp_tx_lower_layer_interface* tx = gtpu->get_tx_lower_layer_interface();
  tx->handle_sdu(std::move(tpdu), 1);
  ASSERT_EQ(gpdu, gtpu_tx.last_tx);
}

} // namespace

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
