// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "sctp_server_test_helpers.h"
#include "ocudu/gateways/sctp_network_server_factory.h"
#include "ocudu/gateways/sctp_socket.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include <arpa/inet.h>
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

class base_sctp_network_test
{
protected:
  base_sctp_network_test()
  {
    ocudulog::fetch_basic_logger("SCTP-GW").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
  }
  ~base_sctp_network_test() { ocudulog::flush(); }

  dummy_io_broker     broker;
  association_factory assoc_factory;
};

class dummy_sctp_client : public dummy_sctp_node
{
public:
  dummy_sctp_client() : dummy_sctp_node("CLIENT") {}

  bool connect(int ppid_, const std::string& server_addr_, int server_port_)
  {
    ppid        = ppid_;
    server_addr = server_addr_;
    server_port = server_port_;

    auto result = sctp_socket::create(sctp_socket_params{"CLIENT", AF_INET, SOCK_SEQPACKET});
    if (not result.has_value()) {
      return false;
    }
    socket = std::move(result.value());

    sockaddr_storage addr_storage = {};
    auto*            addr         = reinterpret_cast<sockaddr_in*>(&addr_storage);
    addr->sin_family              = AF_INET;
    addr->sin_port                = htons(server_port);
    addr->sin_addr.s_addr         = ::inet_addr(server_addr.c_str());
    sctp_assoc_t assoc_id         = 0;
    if (not socket.connectx({addr_storage}, assoc_id)) {
      socket.close();
      return false;
    }
    logger.info("Client connected to {}:{}", server_addr, server_port);
    return true;
  }

  bool send_data(const std::vector<uint8_t>& bytes)
  {
    return dummy_sctp_node::send_data(bytes, ppid, server_addr, server_port);
  }

  std::string server_addr;
  int         server_port;
  int         ppid;
};

} // namespace

class sctp_network_server_test : public base_sctp_network_test, public ::testing::Test
{
protected:
  sctp_network_server_test()
  {
    server_cfg.sctp.if_name        = "SERVER";
    server_cfg.sctp.ppid           = NGAP_PPID;
    server_cfg.sctp.bind_addresses = {"127.0.0.1"};
    server_cfg.sctp.bind_port      = 0;
  }

  ~sctp_network_server_test() override
  {
    if (server) {
      server->stop();
    }
  }

  bool connect_client(bool broker_trigger_required = true)
  {
    if (not client.connect(
            server_cfg.sctp.ppid, server_cfg.sctp.bind_addresses[0], server->get_listen_port().value())) {
      return false;
    }
    if (broker_trigger_required) {
      trigger_broker();
    }
    return true;
  }

  bool close_client(bool broker_trigger_required = true, std::optional<int> assoc_fd = std::nullopt)
  {
    bool ret = client.close();
    if (broker_trigger_required) {
      // SCTP SHUTDOWN EVENT
      trigger_broker(assoc_fd);
      // SCTP SHUTDOWN COMP
      trigger_broker(assoc_fd);
    }
    return ret;
  }

  bool send_data(const std::vector<uint8_t>& bytes, bool broker_triggered = true)
  {
    if (not client.send_data(bytes)) {
      return false;
    }
    if (broker_triggered) {
      trigger_broker();
    }
    return true;
  }

  void trigger_broker(std::optional<int> fd = std::nullopt)
  {
    if (not fd.has_value()) {
      fd = server->get_socket_fd();
    }
    broker.handle_receive(*fd);
  }

  sctp_network_server_config server_cfg{{}, broker, io_rx_executor, app_executor, assoc_factory};

  inline_task_executor                 io_rx_executor;
  inline_task_executor                 app_executor;
  std::unique_ptr<sctp_network_server> server;
  dummy_sctp_client                    client;
};

TEST_F(sctp_network_server_test, when_config_is_valid_then_server_is_created_successfully)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_NE(server, nullptr);
}

TEST_F(sctp_network_server_test, when_bind_interface_is_invalid_then_server_is_not_created)
{
  server_cfg.sctp.bind_interface = "invalid";
  server                         = create_sctp_network_server(server_cfg);
  ASSERT_EQ(server, nullptr);
}

TEST_F(sctp_network_server_test, when_binding_on_bogus_address_then_bind_fails)
{
  server_cfg.sctp.bind_addresses = {"1.1.1.1"};
  server                         = create_sctp_network_server(server_cfg);
  ASSERT_EQ(server, nullptr);
}

TEST_F(sctp_network_server_test, when_bind_address_is_empty_then_server_is_not_created)
{
  server_cfg.sctp.bind_addresses = {""};
  server                         = create_sctp_network_server(server_cfg);
  ASSERT_EQ(server, nullptr);
}

TEST_F(sctp_network_server_test, when_broker_rejects_subscriber_then_listen_fails)
{
  broker.accept_next_fd = false;

  server = create_sctp_network_server(server_cfg);

  ASSERT_FALSE(server->listen());
  ASSERT_EQ(broker.nof_registered_sockets(), 0);
  ASSERT_EQ(broker.get_nof_deregistrations(), 0) << "If the subscription fails, no deregister should be called";
}

TEST_F(sctp_network_server_test, when_broker_accepts_subscriber_then_listen_succeeds)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  ASSERT_EQ(broker.get_last_registered_fd(), server->get_socket_fd());
}

TEST_F(sctp_network_server_test, when_server_is_shutdown_then_fd_is_deregistered_from_broker)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  int fd = server->get_socket_fd();
  // ASSERT_EQ(broker.last_unregistered_fd, -1);
  server->stop();
  server.reset();
  ASSERT_EQ(broker.get_last_registered_fd(), fd);
}

TEST_F(sctp_network_server_test, when_client_connects_then_server_request_new_sctp_association_handler_creation)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  // Connect client to server.
  ASSERT_FALSE(assoc_factory.association_created);
  ASSERT_TRUE(connect_client());

  // Ensure SCTP server requested the creation of a new SCTP association handler.
  ASSERT_TRUE(assoc_factory.association_created);
  ASSERT_FALSE(assoc_factory.association_destroyed);
}

TEST_F(sctp_network_server_test, when_client_connects_then_client_receives_sctp_comm_up_notification)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  // Connect client to server.
  ASSERT_TRUE(connect_client(false));

  // Ensure SCTP ASSOC COMM UP is received by the client, even before the broker handles the SCTP association on the
  // server side.
  auto result = client.receive();
  ASSERT_TRUE(result.has_value());
  ASSERT_TRUE(result->has_notification());
  ASSERT_EQ(result->sctp_notification(), SCTP_ASSOC_CHANGE);
  ASSERT_EQ(result->sctp_assoc_change().sac_state, SCTP_COMM_UP);
  client.logger.info("Client received SCTP COMM UP notification for assoc_id={}",
                     result->sctp_assoc_change().sac_assoc_id);

  // server handles now the SCTP association event.
  trigger_broker();
}

TEST_F(sctp_network_server_test, when_client_disconnects_then_server_deletes_association_handler)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());
  int assoc_fd = broker.get_last_registered_fd();
  ASSERT_FALSE(assoc_factory.association_destroyed);
  ASSERT_TRUE(close_client(true, assoc_fd));
  ASSERT_TRUE(assoc_factory.association_destroyed);
}

TEST_F(sctp_network_server_test, when_client_sends_sctp_message_then_message_is_forwarded_to_association_handler)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());
  int assoc_fd = broker.get_last_registered_fd();

  ASSERT_EQ(assoc_factory.last_sdu.length(), 0);
  std::vector<uint8_t> bytes = {0x01, 0x02, 0x03, 0x04};
  ASSERT_TRUE(send_data(bytes, false));

  trigger_broker(assoc_fd); // Should handle packet receive

  // Ensure SCTP server forwarded the message to the association handler.
  ASSERT_EQ(assoc_factory.last_sdu, bytes);
}

TEST_F(sctp_network_server_test,
       when_client_sends_sctp_message_and_closes_before_server_handles_events_then_events_are_still_handled)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());
  int assoc_fd = broker.get_last_registered_fd();

  std::vector<uint8_t> bytes = {0x01, 0x02, 0x03, 0x04};

  ASSERT_TRUE(send_data(bytes, false));
  ASSERT_TRUE(close_client(false));

  ASSERT_EQ(assoc_factory.last_sdu.length(), 0);
  trigger_broker(assoc_fd); // Should handle packet receive
  ASSERT_EQ(assoc_factory.last_sdu, bytes);
  ASSERT_FALSE(assoc_factory.association_destroyed) << "Association Handler was destroyed too early";
  trigger_broker(assoc_fd); // SCTP_SHUTDOWN_EVENT
  trigger_broker(assoc_fd); // SCTP_SHUTDOWN_COMP
  ASSERT_TRUE(assoc_factory.association_destroyed) << "Association Handler was not destroyed";
}

TEST_F(sctp_network_server_test, when_association_handler_closes_connection_then_sctp_eof_is_sent_to_client)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());
  ASSERT_TRUE(connect_client());
  // Client receives SCTP COMM UP
  auto result = client.receive();

  // Server-side association handler closes, signalling that the server wants to close association.
  assoc_factory.association_senders.clear();

  // Ensure SCTP EOF is sent to client.
  result = client.receive();
  ASSERT_TRUE(result.has_value());
  ASSERT_TRUE(result->has_notification());
  ASSERT_EQ(result->sctp_notification(), SCTP_SHUTDOWN_EVENT);

  // IO broker in the server handles SCTP_SHUTDOWN_COMP
  trigger_broker();
}

TEST_F(sctp_network_server_test, when_multiple_clients_connect_then_multiple_association_handlers_are_created)
{
  server = create_sctp_network_server(server_cfg);
  ASSERT_TRUE(server->listen());

  uint16_t port = server->get_listen_port().value();
  // First client connect.
  ASSERT_TRUE(connect_client());
  int assoc_fd1 = broker.get_last_registered_fd();

  // Client 2 connects.
  assoc_factory.association_created = false;
  dummy_sctp_client client2;
  client2.connect(server_cfg.sctp.ppid, server_cfg.sctp.bind_addresses[0], port);
  trigger_broker();
  int assoc_fd2 = broker.get_last_registered_fd();

  // Handle client 2 association creation.
  ASSERT_TRUE(assoc_factory.association_created);
  ASSERT_FALSE(assoc_factory.association_destroyed);

  // Close Client 1.
  client.close();

  // Close Client 2.
  client2.close();

  // SCTP shutdown client 1
  trigger_broker(assoc_fd1); // < Client 1: SCTP SHUTDOWN EVENT
  trigger_broker(assoc_fd1); // < Client 1: SCTP SHUTDOWN COMP
  ASSERT_TRUE(assoc_factory.association_destroyed) << "Client 1 shutdown was not processed";
  assoc_factory.association_destroyed = false;
  // SCTP shutdown client 2
  trigger_broker(assoc_fd2); // < Client2: SCTP SHUTDOWN EVENT
  trigger_broker(assoc_fd2); // < Client2: SCTP SHUTDOWN COMP
  ASSERT_TRUE(assoc_factory.association_destroyed) << "Client 2 shutdown was not processed";
}

// IPv6 tests

TEST_F(sctp_network_server_test, when_binding_on_bogus_v6_address_then_server_is_not_created)
{
  server_cfg.sctp.bind_addresses = {"1:1::"};
  server                         = create_sctp_network_server(server_cfg);
  ASSERT_EQ(server, nullptr);
}

TEST_F(sctp_network_server_test, when_binding_on_v6_localhost_then_server_is_created)
{
  server_cfg.sctp.bind_addresses = {"::1"};
  server                         = create_sctp_network_server(server_cfg);
  ASSERT_NE(server, nullptr);
  ASSERT_TRUE(server->listen());
  std::optional<uint16_t> port = server->get_listen_port();
  ASSERT_TRUE(port.has_value());
  ASSERT_NE(port.value(), 0);
}

// SCTP socket option tests

TEST_F(sctp_network_server_test, when_rto_is_set_then_rto_changes)
{
  std::chrono::milliseconds rto_init{120};
  std::chrono::milliseconds rto_min{120};
  std::chrono::milliseconds rto_max{800};

  server_cfg.sctp.rto_initial = rto_init;
  server_cfg.sctp.rto_min     = rto_min;
  server_cfg.sctp.rto_max     = rto_max;

  server = create_sctp_network_server(server_cfg);
  ASSERT_NE(server, nullptr);

  int fd = server->get_socket_fd();

  // Check used RTO values.
  sctp_rtoinfo rto_opts  = {};
  socklen_t    rto_sz    = sizeof(sctp_rtoinfo);
  rto_opts.srto_assoc_id = 0;
  ASSERT_EQ(getsockopt(fd, SOL_SCTP, SCTP_RTOINFO, &rto_opts, &rto_sz), 0) << ::strerror(errno);
  ASSERT_EQ(rto_opts.srto_initial, rto_init.count());
  ASSERT_EQ(rto_opts.srto_min, rto_min.count());
  ASSERT_EQ(rto_opts.srto_max, rto_max.count());
}

TEST_F(sctp_network_server_test, when_init_msg_is_set_then_init_msg_changes)
{
  uint32_t                  init_max_attempts = 1;
  std::chrono::milliseconds max_init_timeo{120};

  server_cfg.sctp.init_max_attempts = init_max_attempts;
  server_cfg.sctp.max_init_timeo    = max_init_timeo;

  server = create_sctp_network_server(server_cfg);
  ASSERT_NE(server, nullptr);

  int fd = server->get_socket_fd();

  // Check used SCTP_INITMSG values.
  sctp_initmsg init_opts = {};
  socklen_t    init_sz   = sizeof(sctp_initmsg);
  ASSERT_EQ(getsockopt(fd, SOL_SCTP, SCTP_INITMSG, &init_opts, &init_sz), 0);

  ASSERT_EQ(init_opts.sinit_max_attempts, init_max_attempts);
  ASSERT_EQ(init_opts.sinit_max_init_timeo, max_init_timeo.count());
}

TEST_F(sctp_network_server_test, when_assoc_is_set_then_assoc_changes)
{
  uint32_t assoc_max_rxt = 5;

  server_cfg.sctp.assoc_max_rxt = assoc_max_rxt;

  server = create_sctp_network_server(server_cfg);
  ASSERT_NE(server, nullptr);

  int fd = server->get_socket_fd();

  // Check used SCTP_ASSOCINFO values.
  sctp_assocparams assoc_opts = {};
  socklen_t        assoc_sz   = sizeof(sctp_assocparams);
  ASSERT_EQ(getsockopt(fd, SOL_SCTP, SCTP_ASSOCINFO, &assoc_opts, &assoc_sz), 0);

  ASSERT_EQ(assoc_opts.sasoc_asocmaxrxt, assoc_max_rxt);
}

TEST_F(sctp_network_server_test, when_paddr_is_set_then_paddr_changes)
{
  std::chrono::milliseconds hb_interval{5000};

  server_cfg.sctp.hb_interval = hb_interval;

  server = create_sctp_network_server(server_cfg);
  ASSERT_NE(server, nullptr);

  int fd = server->get_socket_fd();

  // Check used SCTP_PEER_ADDR_PARAMS values.
  sctp_paddrparams paddr_opts = {};
  socklen_t        paddr_sz   = sizeof(sctp_paddrparams);
  ASSERT_EQ(getsockopt(fd, SOL_SCTP, SCTP_PEER_ADDR_PARAMS, &paddr_opts, &paddr_sz), 0);

  ASSERT_EQ(paddr_opts.spp_hbinterval, hb_interval.count());
}
