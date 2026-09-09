// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "sctp_server_test_helpers.h"
#include "ocudu/gateways/sctp_network_server_factory.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include <arpa/inet.h>
#include <gtest/gtest.h>

using namespace ocudu;

class sctp_network_server_peer_test : public ::testing::TestWithParam<bool>
{
protected:
  sctp_network_server_peer_test()
  {
    ocudulog::fetch_basic_logger("SCTP-GW").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    /// Test addresses.
    std::string node_addr1 = "127.0.0.1";
    std::string node_addr2 = "127.0.0.2";
    std::string node_addr3 = "127.0.0.3";

    test_dtls                       = GetParam();
    server_cfg1.sctp.if_name        = "SERVER1";
    server_cfg1.sctp.ppid           = XNAP_PPID;
    server_cfg1.sctp.bind_addresses = {node_addr1};
    server_cfg1.sctp.bind_port      = 0;
    if (test_dtls) {
      /// Map used for connections that do not follow the default mode.
      /// In this test, node 1 is allways the server, so this map is empty.
      std::map<transport_layer_address, dtls_mode> mode_map = {};
      server_cfg1.sctp.dtls_cfg                             = {dtls_mode::server,
                                                               "1",
                                                               std::string(TEST_CERT_DIR) + "/link12.crt",
                                                               std::string(TEST_CERT_DIR) + "/link12.key",
                                                               mode_map};
    }

    server_cfg2.sctp.if_name        = "SERVER2";
    server_cfg2.sctp.ppid           = XNAP_PPID;
    server_cfg2.sctp.bind_addresses = {node_addr2};
    server_cfg2.sctp.bind_port      = 0;
    if (test_dtls) {
      /// Map used for connections that do not follow the default mode.
      /// In this test, node 2 is allways the client, so this map is empty.
      std::map<transport_layer_address, dtls_mode> mode_map = {};
      server_cfg2.sctp.dtls_cfg                             = {dtls_mode::client,
                                                               "2",
                                                               std::string(TEST_CERT_DIR) + "/link21.crt",
                                                               std::string(TEST_CERT_DIR) + "/link21.key",
                                                               mode_map};
    }

    server_cfg3.sctp.if_name        = "SERVER3";
    server_cfg3.sctp.ppid           = XNAP_PPID;
    server_cfg3.sctp.bind_addresses = {node_addr3};
    server_cfg3.sctp.bind_port      = 0;
    if (test_dtls) {
      /// Map used for connections that do not follow the default mode.
      /// In this test, node 1 is always the server and 2 the client, so this map sets them accordingly.
      std::map<transport_layer_address, dtls_mode> mode_map = {
          {transport_layer_address::create_from_string(node_addr1), dtls_mode::client},
          {transport_layer_address::create_from_string(node_addr2), dtls_mode::server}};
      server_cfg3.sctp.dtls_cfg = {dtls_mode::server,
                                   "3",
                                   std::string(TEST_CERT_DIR) + "/link31.crt",
                                   std::string(TEST_CERT_DIR) + "/link31.key",
                                   mode_map};
    }
  }

  ~sctp_network_server_peer_test() override
  {
    if (server1) {
      server1->stop();
    }
    if (server2) {
      server2->stop();
    }
    if (server3) {
      server3->stop();
    }
    ocudulog::flush();
  }

  sctp_network_server_config server_cfg1{{}, broker1, io_rx_executor, app_executor, assoc_factory1};
  sctp_network_server_config server_cfg2{{}, broker2, io_rx_executor, app_executor, assoc_factory2};
  sctp_network_server_config server_cfg3{{}, broker3, io_rx_executor, app_executor, assoc_factory3};

  dummy_io_broker broker1;
  dummy_io_broker broker2;
  dummy_io_broker broker3;

  inline_task_executor                 io_rx_executor;
  inline_task_executor                 app_executor;
  std::unique_ptr<sctp_network_server> server1;
  std::unique_ptr<sctp_network_server> server2;
  std::unique_ptr<sctp_network_server> server3;

  std::string server1_addr_str            = "127.0.0.1";
  std::string server2_addr_str            = "127.0.0.2";
  std::string server3_addr_str            = "127.0.0.3";
  std::string server1_multihomed_addr_str = "127.0.0.4";
  std::string server2_multihomed_addr_str = "127.0.0.5";

  association_factory assoc_factory1;
  association_factory assoc_factory2;
  association_factory assoc_factory3;

  bool test_dtls;
};

TEST_P(sctp_network_server_peer_test, when_config_is_valid_then_server_is_created_successfully)
{
  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  server3 = create_sctp_network_server(server_cfg3);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);
  ASSERT_NE(server3, nullptr);
}

TEST_P(sctp_network_server_peer_test, when_association_requested_association_initiates_successfully)
{
  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  server3 = create_sctp_network_server(server_cfg3);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);
  ASSERT_NE(server3, nullptr);

  server1->listen();
  int server1_listen_fd = broker1.get_last_registered_fd();
  server2->listen();
  int server2_listen_fd = broker2.get_last_registered_fd();
  server3->listen();
  int server3_listen_fd = broker3.get_last_registered_fd();

  transport_layer_address addr1 = transport_layer_address::create_from_string(server1_addr_str);
  std::optional<uint16_t> port1 = server1->get_listen_port();
  ASSERT_TRUE(port1);
  addr1.set_port(*port1);

  transport_layer_address addr2 = transport_layer_address::create_from_string(server2_addr_str);
  std::optional<uint16_t> port2 = server2->get_listen_port();
  ASSERT_TRUE(port2);
  addr2.set_port(*port2);

  transport_layer_address addr3 = transport_layer_address::create_from_string(server3_addr_str);
  std::optional<uint16_t> port3 = server3->get_listen_port();
  ASSERT_TRUE(port3);
  addr3.set_port(*port3);

  // Create associations between S1 <-> S2
  async_task<bool>         connect1 = server1->connect({addr2});
  lazy_task_launcher<bool> l1(connect1);

  ASSERT_EQ(0, assoc_factory1.association_count());
  ASSERT_EQ(0, assoc_factory2.association_count());
  ASSERT_EQ(0, assoc_factory3.association_count());
  broker1.handle_receive(server1_listen_fd); // COMM_UP
  int server_1_2_assoc_fd = broker1.get_last_registered_fd();
  broker2.handle_receive(server2_listen_fd); // COMM_UP
  int server_2_1_assoc_fd = broker2.get_last_registered_fd();
  ASSERT_EQ(1, assoc_factory1.association_count());
  ASSERT_EQ(1, assoc_factory2.association_count());
  ASSERT_EQ(0, assoc_factory3.association_count());

  // Setup DTLS handshake.
  if (test_dtls) {
    broker1.handle_receive(server_1_2_assoc_fd);
    broker2.handle_receive(server_2_1_assoc_fd);
    broker1.handle_receive(server_1_2_assoc_fd);
    broker2.handle_receive(server_2_1_assoc_fd);
  }

  // Create associations between S1 <-> S3
  async_task<bool>         connect2 = server1->connect({addr3});
  lazy_task_launcher<bool> l2(connect2);

  broker1.handle_receive(server1_listen_fd); // COMM_UP
  int server_1_3_assoc_fd = broker1.get_last_registered_fd();
  broker3.handle_receive(server3_listen_fd); // COMM_UP
  int server_3_1_assoc_fd = broker3.get_last_registered_fd();
  ASSERT_EQ(2, assoc_factory1.association_count());
  ASSERT_EQ(1, assoc_factory2.association_count());
  ASSERT_EQ(1, assoc_factory3.association_count());

  // Setup DTLS handshake.
  if (test_dtls) {
    broker1.handle_receive(server_1_3_assoc_fd);
    broker3.handle_receive(server_3_1_assoc_fd);
    broker1.handle_receive(server_1_3_assoc_fd);
    broker3.handle_receive(server_3_1_assoc_fd);
  }

  // Create associations between S2 <-> S3
  async_task<bool>         connect3 = server2->connect({addr3});
  lazy_task_launcher<bool> l3(connect3);

  broker2.handle_receive(server2_listen_fd); // COMM_UP
  int server_2_3_assoc_fd = broker2.get_last_registered_fd();
  broker3.handle_receive(server3_listen_fd); // COMM_UP
  int server_3_2_assoc_fd = broker3.get_last_registered_fd();
  ASSERT_EQ(2, assoc_factory1.association_count());
  ASSERT_EQ(2, assoc_factory2.association_count());
  ASSERT_EQ(2, assoc_factory3.association_count());

  // Setup DTLS handshake.
  if (test_dtls) {
    broker2.handle_receive(server_2_3_assoc_fd);
    broker3.handle_receive(server_3_2_assoc_fd);
    broker2.handle_receive(server_2_3_assoc_fd);
    broker3.handle_receive(server_3_2_assoc_fd);
  }

  // Send data from S1 to S2 over the first association.
  {
    std::array<uint8_t, 4> data12 = {0x01, 0x02, 0x01, 0x02};
    byte_buffer            tx_sdu12;
    ASSERT_TRUE(tx_sdu12.append(data12));
    ASSERT_TRUE(assoc_factory1.association_senders[0]->on_new_sdu(tx_sdu12.copy()));
    broker2.handle_receive(server_2_1_assoc_fd); // RX DATA
    ASSERT_EQ(assoc_factory2.last_sdu, tx_sdu12);
  }
  // Send data from S2 to S1.
  {
    std::array<uint8_t, 4> data21 = {0x02, 0x01, 0x02, 0x01};
    byte_buffer            tx_sdu21;
    ASSERT_TRUE(tx_sdu21.append(data21));
    ASSERT_TRUE(assoc_factory2.association_senders[0]->on_new_sdu(tx_sdu21.copy()));
    broker1.handle_receive(server_1_2_assoc_fd); // RX DATA
    ASSERT_EQ(assoc_factory1.last_sdu, tx_sdu21);
  }
  // Send data from S1 to S3 over the first association.
  {
    std::array<uint8_t, 4> data13 = {0x01, 0x03, 0x01, 0x03};
    byte_buffer            tx_sdu13;
    ASSERT_TRUE(tx_sdu13.append(data13));
    ASSERT_TRUE(assoc_factory1.association_senders[1]->on_new_sdu(tx_sdu13.copy()));
    broker3.handle_receive(server_3_1_assoc_fd); // RX DATA
    ASSERT_EQ(assoc_factory3.last_sdu, tx_sdu13);
  }

  // Send data from S3 to S1.
  {
    std::array<uint8_t, 4> data31 = {0x03, 0x01, 0x03, 0x01};
    byte_buffer            tx_sdu31;
    ASSERT_TRUE(tx_sdu31.append(data31));
    ASSERT_TRUE(assoc_factory3.association_senders[0]->on_new_sdu(tx_sdu31.copy()));
    broker1.handle_receive(server_1_3_assoc_fd); // RX DATA
    ASSERT_EQ(assoc_factory1.last_sdu, tx_sdu31);
  }

  // Send data from S2 to S3 over the first association.
  {
    std::array<uint8_t, 4> data23 = {0x02, 0x03, 0x02, 0x03};
    byte_buffer            tx_sdu23;
    ASSERT_TRUE(tx_sdu23.append(data23));
    ASSERT_TRUE(assoc_factory2.association_senders[1]->on_new_sdu(tx_sdu23.copy()));
    broker3.handle_receive(server_3_2_assoc_fd); // RX DATA
    ASSERT_EQ(assoc_factory3.last_sdu, tx_sdu23);
  }

  // Send data from S3 to S2.
  {
    std::array<uint8_t, 4> data32 = {0x03, 0x02, 0x03, 0x02};
    byte_buffer            tx_sdu32;
    ASSERT_TRUE(tx_sdu32.append(data32));
    ASSERT_TRUE(assoc_factory3.association_senders[1]->on_new_sdu(tx_sdu32.copy()));
    broker2.handle_receive(server_2_3_assoc_fd); // RX DATA
    ASSERT_EQ(assoc_factory2.last_sdu, tx_sdu32);
  }
}

TEST_P(sctp_network_server_peer_test, when_connect_called_with_empty_address_list_then_returns_false)
{
  server1 = create_sctp_network_server(server_cfg1);
  ASSERT_NE(server1, nullptr);
  server1->listen();

  async_task<bool>         connect_task = server1->connect({});
  lazy_task_launcher<bool> launcher(connect_task);
  ASSERT_TRUE(connect_task.ready());
  ASSERT_FALSE(connect_task.get());
  ASSERT_EQ(0, assoc_factory1.association_count());
}

TEST_P(sctp_network_server_peer_test, when_connect_uses_multiple_destination_addresses_then_association_succeeds)
{
  server_cfg1.sctp.bind_addresses = {server1_addr_str, server1_multihomed_addr_str};
  server_cfg2.sctp.bind_addresses = {server2_addr_str, server2_multihomed_addr_str};

  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);

  server1->listen();
  int server1_listen_fd = broker1.get_last_registered_fd();
  server2->listen();
  int server2_listen_fd = broker2.get_last_registered_fd();

  std::optional<uint16_t> port2 = server2->get_listen_port();
  ASSERT_TRUE(port2);

  transport_layer_address primary_addr = transport_layer_address::create_from_string(server2_addr_str);
  primary_addr.set_port(*port2);
  transport_layer_address secondary_addr = transport_layer_address::create_from_string(server2_multihomed_addr_str);
  secondary_addr.set_port(*port2);

  // Connect with both server2 addresses for SCTP multihoming.
  async_task<bool>         connect_task = server1->connect({primary_addr, secondary_addr});
  lazy_task_launcher<bool> launcher(connect_task);

  broker1.handle_receive(server1_listen_fd); // COMM_UP completes the connect task on server1
  int server_1_2_assoc_fd = broker1.get_last_registered_fd();
  broker2.handle_receive(server2_listen_fd); // COMM_UP arrives on server2 and creates the association handler
  int server_2_1_assoc_fd = broker2.get_last_registered_fd();

  /// Handle DTLS handshake.
  if (test_dtls) {
    broker1.handle_receive(server_1_2_assoc_fd);
    broker2.handle_receive(server_2_1_assoc_fd);
    broker1.handle_receive(server_1_2_assoc_fd);
    broker2.handle_receive(server_2_1_assoc_fd);
  }
  ASSERT_TRUE(connect_task.ready());
  ASSERT_TRUE(connect_task.get());
  ASSERT_EQ(1, assoc_factory1.association_count());
  ASSERT_EQ(1, assoc_factory2.association_count());

  // Sanity check: data exchange works over the multihomed association.
  std::array<uint8_t, 4> data = {0x10, 0x11, 0x12, 0x13};
  byte_buffer            tx_sdu;
  ASSERT_TRUE(tx_sdu.append(data));
  ASSERT_TRUE(assoc_factory1.association_senders[0]->on_new_sdu(tx_sdu.copy()));
  int server2_association_fd = broker2.get_last_registered_fd();
  broker2.handle_receive(server2_association_fd);
  ASSERT_EQ(assoc_factory2.last_sdu, tx_sdu);
}

TEST_P(sctp_network_server_peer_test, when_pending_connects_overlap_then_second_connect_is_rejected)
{
  server_cfg1.sctp.bind_addresses = {server1_addr_str, server1_multihomed_addr_str};
  server_cfg2.sctp.bind_addresses = {server2_addr_str, server2_multihomed_addr_str};

  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);

  server1->listen();
  int server1_listen_fd = broker1.get_last_registered_fd();
  server2->listen();
  int server2_listen_fd = broker2.get_last_registered_fd();

  std::optional<uint16_t> port2 = server2->get_listen_port();
  ASSERT_TRUE(port2);

  transport_layer_address primary_addr = transport_layer_address::create_from_string(server2_addr_str);
  primary_addr.set_port(*port2);
  transport_layer_address secondary_addr = transport_layer_address::create_from_string(server2_multihomed_addr_str);
  secondary_addr.set_port(*port2);

  // First connect attempt — multihomed; pending until broker processes COMM_UP.
  async_task<bool>         connect1 = server1->connect({primary_addr, secondary_addr});
  lazy_task_launcher<bool> launcher1(connect1);
  ASSERT_FALSE(connect1.ready());

  // Second connect attempt overlaps on primary_addr — should be rejected immediately.
  async_task<bool>         connect2 = server1->connect({primary_addr});
  lazy_task_launcher<bool> launcher2(connect2);

  ASSERT_TRUE(connect2.ready());
  ASSERT_FALSE(connect2.get());

  // Completing the first connect leaves only one association on server1.
  broker1.handle_receive(server1_listen_fd);
  int server_1_2_assoc_fd = broker1.get_last_registered_fd();
  broker2.handle_receive(server2_listen_fd);
  int server_2_1_assoc_fd = broker2.get_last_registered_fd();

  /// Handle DTLS handshake.
  if (test_dtls) {
    broker1.handle_receive(server_1_2_assoc_fd);
    broker2.handle_receive(server_2_1_assoc_fd);
    broker1.handle_receive(server_1_2_assoc_fd);
    broker2.handle_receive(server_2_1_assoc_fd);
  }

  ASSERT_TRUE(connect1.ready());
  ASSERT_TRUE(connect1.get());
  ASSERT_EQ(1, assoc_factory1.association_count());
}

TEST_P(sctp_network_server_peer_test, when_server_is_destroyed_then_associations_are_cleaned_up)
{
  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);

  server1->listen();
  int server1_listen_fd = broker1.get_last_registered_fd();
  server2->listen();
  int server2_listen_fd = broker2.get_last_registered_fd();

  transport_layer_address addr2 = transport_layer_address::create_from_string(server2_addr_str);
  std::optional<uint16_t> port2 = server2->get_listen_port();
  ASSERT_TRUE(port2);
  addr2.set_port(*port2);

  // Create association S1 <-> S2.
  async_task<bool>         connect_task = server1->connect({addr2});
  lazy_task_launcher<bool> launcher(connect_task);
  broker1.handle_receive(server1_listen_fd); // CONN_UP notification completes the connect task
  broker2.handle_receive(server2_listen_fd); // CONN_UP
  ASSERT_EQ(1, assoc_factory1.association_count());
  ASSERT_EQ(1, assoc_factory2.association_count());
  ASSERT_FALSE(assoc_factory1.association_destroyed);
  ASSERT_FALSE(assoc_factory2.association_destroyed);

  // Destroy server1. The destructor should clean up all its associations.
  server1->stop();

  ASSERT_TRUE(assoc_factory1.association_destroyed);
  ASSERT_EQ(0, assoc_factory1.association_count());

  // TODO: Once handle_socket_shutdown sends EOF to peers, verify peer-side cleanup here.

  server2->stop();
}

#ifdef OCUDU_HAVE_OPENSSL_DTLS
INSTANTIATE_TEST_SUITE_P(sctp_network_server_peer_test_with_and_without_dtls,
                         sctp_network_server_peer_test,
                         ::testing::Values(false, true));
#else
INSTANTIATE_TEST_SUITE_P(sctp_network_server_peer_test_with_and_without_dtls,
                         sctp_network_server_peer_test,
                         ::testing::Values(false));
#endif
