// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "sctp_server_test_helpers.h"
#include "ocudu/gateways/sctp_network_server_factory.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include <arpa/inet.h>
#include <gtest/gtest.h>

using namespace ocudu;

class sctp_network_server_peer_test : public ::testing::Test
{
protected:
  sctp_network_server_peer_test()
  {
    ocudulog::fetch_basic_logger("SCTP-GW").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    server_cfg1.sctp.if_name        = "SERVER1";
    server_cfg1.sctp.ppid           = XNAP_PPID;
    server_cfg1.sctp.bind_addresses = {"127.0.0.1"};
    server_cfg1.sctp.bind_port      = 0;

    server_cfg2.sctp.if_name        = "SERVER2";
    server_cfg2.sctp.ppid           = XNAP_PPID;
    server_cfg2.sctp.bind_addresses = {"127.0.0.2"};
    server_cfg2.sctp.bind_port      = 0;

    server_cfg3.sctp.if_name        = "SERVER3";
    server_cfg3.sctp.ppid           = XNAP_PPID;
    server_cfg3.sctp.bind_addresses = {"127.0.0.3"};
    server_cfg3.sctp.bind_port      = 0;
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
};

TEST_F(sctp_network_server_peer_test, when_config_is_valid_then_server_is_created_successfully)
{
  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  server3 = create_sctp_network_server(server_cfg3);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);
  ASSERT_NE(server3, nullptr);
}

TEST_F(sctp_network_server_peer_test, when_association_requested_association_initiates_successfully)
{
  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  server3 = create_sctp_network_server(server_cfg3);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);
  ASSERT_NE(server3, nullptr);

  server1->listen();
  server2->listen();
  server3->listen();

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
  broker1.handle_receive(); // CONN_UP
  broker2.handle_receive(); // CONN_UP
  ASSERT_EQ(1, assoc_factory1.association_count());
  ASSERT_EQ(1, assoc_factory2.association_count());
  ASSERT_EQ(0, assoc_factory3.association_count());

  // Create associations between S1 <-> S3
  async_task<bool>         connect2 = server1->connect({addr3});
  lazy_task_launcher<bool> l2(connect2);

  broker1.handle_receive(); // CONN_UP
  broker3.handle_receive(); // CONN_UP
  ASSERT_EQ(2, assoc_factory1.association_count());
  ASSERT_EQ(1, assoc_factory2.association_count());
  ASSERT_EQ(1, assoc_factory3.association_count());

  // Create associations between S2 <-> S3
  async_task<bool>         connect3 = server2->connect({addr3});
  lazy_task_launcher<bool> l3(connect3);

  broker2.handle_receive(); // CONN_UP
  broker3.handle_receive(); // CONN_UP
  ASSERT_EQ(2, assoc_factory1.association_count());
  ASSERT_EQ(2, assoc_factory2.association_count());
  ASSERT_EQ(2, assoc_factory3.association_count());

  // Send data from S1 to S2 over the first association.
  std::array<uint8_t, 4> data1 = {0x00, 0x01, 0x02, 0x03};
  byte_buffer            tx_sdu;
  ASSERT_TRUE(tx_sdu.append(data1));
  ASSERT_TRUE(assoc_factory1.association_senders[0]->on_new_sdu(tx_sdu.copy()));
  broker2.handle_receive(); // RX DATA
  ASSERT_EQ(assoc_factory2.last_sdu, tx_sdu);

  // Send data from S2 to S1.
  std::array<uint8_t, 4> data2 = {0x04, 0x05, 0x06, 0x07};
  byte_buffer            tx_sdu2;
  ASSERT_TRUE(tx_sdu2.append(data2));
  ASSERT_TRUE(assoc_factory2.association_senders[0]->on_new_sdu(tx_sdu2.copy()));
  broker1.handle_receive(); // RX DATA
  ASSERT_EQ(assoc_factory1.last_sdu, tx_sdu2);
}

TEST_F(sctp_network_server_peer_test, when_connect_called_with_empty_address_list_then_returns_false)
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

TEST_F(sctp_network_server_peer_test, when_connect_uses_multiple_destination_addresses_then_association_succeeds)
{
  server_cfg1.sctp.bind_addresses = {server1_addr_str, server1_multihomed_addr_str};
  server_cfg2.sctp.bind_addresses = {server2_addr_str, server2_multihomed_addr_str};

  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);

  server1->listen();
  server2->listen();

  std::optional<uint16_t> port2 = server2->get_listen_port();
  ASSERT_TRUE(port2);

  transport_layer_address primary_addr = transport_layer_address::create_from_string(server2_addr_str);
  primary_addr.set_port(*port2);
  transport_layer_address secondary_addr = transport_layer_address::create_from_string(server2_multihomed_addr_str);
  secondary_addr.set_port(*port2);

  // Connect with both server2 addresses for SCTP multihoming.
  async_task<bool>         connect_task = server1->connect({primary_addr, secondary_addr});
  lazy_task_launcher<bool> launcher(connect_task);

  broker1.handle_receive(); // CONN_UP completes the connect task on server1
  broker2.handle_receive(); // CONN_UP arrives on server2 and creates the association handler
  ASSERT_TRUE(connect_task.ready());
  ASSERT_TRUE(connect_task.get());
  ASSERT_EQ(1, assoc_factory1.association_count());
  ASSERT_EQ(1, assoc_factory2.association_count());

  // Sanity check: data exchange works over the multihomed association.
  std::array<uint8_t, 4> data = {0x10, 0x11, 0x12, 0x13};
  byte_buffer            tx_sdu;
  ASSERT_TRUE(tx_sdu.append(data));
  ASSERT_TRUE(assoc_factory1.association_senders[0]->on_new_sdu(tx_sdu.copy()));
  broker2.handle_receive();
  ASSERT_EQ(assoc_factory2.last_sdu, tx_sdu);
}

TEST_F(sctp_network_server_peer_test, when_pending_connects_overlap_then_second_connect_is_rejected)
{
  server_cfg1.sctp.bind_addresses = {server1_addr_str, server1_multihomed_addr_str};
  server_cfg2.sctp.bind_addresses = {server2_addr_str, server2_multihomed_addr_str};

  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);

  server1->listen();
  server2->listen();

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
  broker1.handle_receive();
  broker2.handle_receive();
  ASSERT_TRUE(connect1.ready());
  ASSERT_TRUE(connect1.get());
  ASSERT_EQ(1, assoc_factory1.association_count());
}

TEST_F(sctp_network_server_peer_test, when_server_is_destroyed_then_associations_are_cleaned_up)
{
  server1 = create_sctp_network_server(server_cfg1);
  server2 = create_sctp_network_server(server_cfg2);
  ASSERT_NE(server1, nullptr);
  ASSERT_NE(server2, nullptr);

  server1->listen();
  server2->listen();

  transport_layer_address addr2 = transport_layer_address::create_from_string(server2_addr_str);
  std::optional<uint16_t> port2 = server2->get_listen_port();
  ASSERT_TRUE(port2);
  addr2.set_port(*port2);

  // Create association S1 <-> S2.
  async_task<bool>         connect_task = server1->connect({addr2});
  lazy_task_launcher<bool> launcher(connect_task);
  broker1.handle_receive(); // CONN_UP notification completes the connect task
  broker2.handle_receive(); // CONN_UP
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
