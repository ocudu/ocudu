// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "sctp_test_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/gateways/sctp_network_client_factory.h"
#include "ocudu/gateways/sctp_socket.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include "ocudu/support/executors/unique_thread.h"
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace ocudu;

namespace {

int get_fd_family(int fd)
{
  sockaddr_storage ss{};
  socklen_t        len = sizeof(ss);
  if (::getsockname(fd, (sockaddr*)&ss, &len) == 0) {
    return ss.ss_family;
  }
  return -1;
}
class sctp_recv_notifier_factory
{
public:
  bool        destroyed = false;
  byte_buffer last_sdu;

  class dummy_sctp_recv_notifier : public sctp_association_sdu_notifier
  {
  public:
    dummy_sctp_recv_notifier(sctp_recv_notifier_factory& parent_) : parent(parent_) {}
    ~dummy_sctp_recv_notifier() override { parent.destroyed = true; }

    bool on_new_sdu(byte_buffer sdu) override
    {
      parent.last_sdu = std::move(sdu);
      return true;
    }

  private:
    sctp_recv_notifier_factory& parent;
  };

  std::unique_ptr<sctp_association_sdu_notifier> create() { return std::make_unique<dummy_sctp_recv_notifier>(*this); }
};

class dummy_sctp_server : public dummy_sctp_node
{
public:
  dummy_sctp_server() : dummy_sctp_node("SERVER")
  {
    auto result = sctp_socket::create(sctp_socket_params{"SERVER", AF_INET, SOCK_SEQPACKET});
    report_fatal_error_if_not(result.has_value(), "Failed to create SCTP socket");
    socket = std::move(result.value());

    sockaddr_storage addr_storage = {};
    auto*            addr         = reinterpret_cast<sockaddr_in*>(&addr_storage);
    addr->sin_family              = AF_INET;
    addr->sin_port                = htons(0);
    addr->sin_addr.s_addr         = ::inet_addr(address.c_str());
    report_fatal_error_if_not(socket.bindx({addr_storage}, ""), "Failed to bind server socket");
    report_fatal_error_if_not(socket.listen(), "Failed to listen to new connections");
    report_fatal_error_if_not(socket.set_non_blocking(), "Failed to set as non-blocking");
    bind_port = socket.get_bound_port().value();
  }

  const std::string address   = "127.0.0.1";
  int               bind_port = -1;
};

class dummy_sctp_server_ipv6 : public dummy_sctp_node
{
public:
  dummy_sctp_server_ipv6() : dummy_sctp_node("SERVER_IPV6")
  {
    auto result = sctp_socket::create(sctp_socket_params{"SERVER_IPV6", AF_INET6, SOCK_SEQPACKET});
    report_fatal_error_if_not(result.has_value(), "Failed to create SCTP socket");
    socket = std::move(result.value());

    sockaddr_storage addr_storage = {};
    auto*            addr6        = reinterpret_cast<sockaddr_in6*>(&addr_storage);
    addr6->sin6_family            = AF_INET6;
    addr6->sin6_port              = htons(0);
    addr6->sin6_addr              = in6addr_loopback;
    report_fatal_error_if_not(socket.bindx({addr_storage}, ""), "Failed to bind server socket");
    report_fatal_error_if_not(socket.listen(), "Failed to listen to new connections");
    report_fatal_error_if_not(socket.set_non_blocking(), "Failed to set as non-blocking");
    bind_port = socket.get_bound_port().value();
  }

  const std::string address   = "::1";
  int               bind_port = -1;
};

class dummy_sctp_server_multihomed_ipv4 : public dummy_sctp_node
{
public:
  dummy_sctp_server_multihomed_ipv4() : dummy_sctp_node("SERVER_MULTIHOMED_IPV4")
  {
    auto result = sctp_socket::create(sctp_socket_params{"SERVER_MULTIHOMED_IPV4", AF_INET, SOCK_SEQPACKET});
    report_fatal_error_if_not(result.has_value(), "Failed to create SCTP socket");
    socket = std::move(result.value());

    std::vector<sockaddr_storage> addrs;

    // Add multiple IPv4 loopback addresses
    sockaddr_storage addr4_storage = {};
    auto*            addr4         = reinterpret_cast<sockaddr_in*>(&addr4_storage);
    addr4->sin_family              = AF_INET;
    addr4->sin_port                = htons(0);
    addr4->sin_addr.s_addr         = ::inet_addr("127.0.0.1");
    addrs.push_back(addr4_storage);

    report_fatal_error_if_not(socket.bindx(addrs, ""), "Failed to bind server socket");
    report_fatal_error_if_not(socket.listen(), "Failed to listen to new connections");
    report_fatal_error_if_not(socket.set_non_blocking(), "Failed to set as non-blocking");
    bind_port = socket.get_bound_port().value();
  }

  const std::vector<std::string> addresses = {"127.0.0.1", "127.0.0.2", "127.0.0.3"};
  int                            bind_port = -1;
};

class dummy_sctp_server_mixed : public dummy_sctp_node
{
public:
  dummy_sctp_server_mixed() : dummy_sctp_node("SERVER_MULTIHOMED_IPV4_IPV6")
  {
    auto result = sctp_socket::create(sctp_socket_params{"SERVER_MULTIHOMED_IPV4_IPV6", AF_INET6, SOCK_SEQPACKET});
    report_fatal_error_if_not(result.has_value(), "Failed to create SCTP socket");
    socket = std::move(result.value());

    std::vector<sockaddr_storage> addrs;

    // Add IPv4 loopback address
    sockaddr_storage addr4_storage = {};
    auto*            addr4         = reinterpret_cast<sockaddr_in*>(&addr4_storage);
    addr4->sin_family              = AF_INET;
    addr4->sin_port                = htons(0);
    addr4->sin_addr.s_addr         = ::inet_addr("127.0.0.1");
    addrs.push_back(addr4_storage);

    addr4->sin_addr.s_addr = ::inet_addr("127.0.0.2");
    addrs.push_back(addr4_storage);

    addr4->sin_addr.s_addr = ::inet_addr("127.0.0.3");
    addrs.push_back(addr4_storage);

    // Add IPv6 loopback address
    sockaddr_storage addr6_storage = {};
    auto*            addr6         = reinterpret_cast<sockaddr_in6*>(&addr6_storage);
    addr6->sin6_family             = AF_INET6;
    addr6->sin6_port               = htons(0);
    addr6->sin6_addr               = in6addr_loopback;
    addrs.push_back(addr6_storage);

    report_fatal_error_if_not(socket.bindx(addrs, ""), "Failed to bind server socket");
    report_fatal_error_if_not(socket.listen(), "Failed to listen to new connections");
    report_fatal_error_if_not(socket.set_non_blocking(), "Failed to set as non-blocking");
    bind_port = socket.get_bound_port().value();
  }

  const std::vector<std::string> addresses = {"127.0.0.1", "::1"};
  int                            bind_port = -1;
};

} // namespace

class sctp_network_client_test : public ::testing::Test
{
public:
  sctp_network_client_test()
  {
    ocudulog::fetch_basic_logger("SCTP-GW").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    client_cfg.sctp.if_name = "client";
    client_cfg.sctp.ppid    = NGAP_PPID;
  }
  ~sctp_network_client_test() override
  {
    close_client();
    ocudulog::flush();
  }

  // Forces client shutdown.
  void close_client()
  {
    // We may need io_broker running asynchronously to complete SCTP event handling and shutdown.
    if (client_sender != nullptr) {
      std::atomic<bool> finished{false};
      // Handle SCTP SHUTDOWN
      unique_thread t("io_broker", [this, &finished]() {
        while (not finished and broker.nof_registered_sockets() != 0) {
          broker.handle_receive(client_fd);
          std::this_thread::sleep_for(std::chrono::microseconds{10});
        }
      });
      // client dtor waits for SCTP SHUTDOWN COMP before returning.
      client.reset();
      finished = true;
      t.join();
    }
  }

  void trigger_broker() { broker.handle_receive(client->get_socket_fd()); }

  bool connect_to_server()
  {
    auto sender = client->connect(recv_notifier_factory.create());
    if (sender != nullptr) {
      client_sender = std::move(sender);
      client_fd     = client->get_socket_fd();
      return true;
    }
    return false;
  }

  bool send_data(const std::vector<uint8_t>& data)
  {
    return client_sender->on_new_sdu(byte_buffer::create(data).value());
  }

protected:
  int                        client_fd = -1;
  inline_task_executor       io_rx_executor;
  dummy_io_broker            broker;
  sctp_network_client_config client_cfg{sctp_network_connector_config{}, broker, io_rx_executor};

  sctp_recv_notifier_factory recv_notifier_factory;

  dummy_sctp_server server;

  std::unique_ptr<sctp_network_client> client;

  std::unique_ptr<sctp_association_sdu_notifier> client_sender;
};

TEST_F(sctp_network_client_test, when_bind_address_is_valid_then_client_is_created_successfully)
{
  client_cfg.sctp.bind_addresses = {"127.0.0.2"};
  client_cfg.sctp.bind_port      = server.bind_port;
  client                         = create_sctp_network_client(client_cfg);
  ASSERT_NE(client, nullptr);
}

TEST_F(sctp_network_client_test, when_bind_address_not_provided_then_client_is_created_successfully)
{
  client = create_sctp_network_client(client_cfg);
  ASSERT_NE(client, nullptr);
}

TEST_F(sctp_network_client_test, when_bind_interface_is_invalid_then_client_connection_fails)
{
  client_cfg.sctp.bind_interface = "invalid";
  client_cfg.sctp.bind_addresses = {"127.0.0.1"};
  client_cfg.sctp.bind_port      = server.bind_port;
  client                         = create_sctp_network_client(client_cfg);
  ASSERT_NE(client, nullptr);
  ASSERT_FALSE(connect_to_server());
}

TEST_F(sctp_network_client_test, when_server_does_not_exist_then_connection_fails)
{
  client_cfg.sctp.connect_addresses = {"127.0.0.1"};
  client                            = create_sctp_network_client(client_cfg);

  ASSERT_FALSE(connect_to_server());
  ASSERT_EQ(broker.nof_registered_sockets(), 0);
  ASSERT_EQ(client_sender, nullptr);
  ASSERT_EQ(broker.get_nof_deregistrations(), 0);
}

TEST_F(sctp_network_client_test, when_broker_rejects_subscriber_then_connect_fails)
{
  broker.accept_next_fd             = false;
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);

  ASSERT_FALSE(connect_to_server());
  ASSERT_GE(broker.nof_registered_sockets(), 0);
  ASSERT_EQ(broker.get_nof_deregistrations(), 0) << "If the subscription fails, no deregister should be called";
}

TEST_F(sctp_network_client_test, when_server_exists_then_connection_succeeds)
{
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);

  ASSERT_TRUE(connect_to_server());
  ASSERT_TRUE(broker.is_socket_registered(client->get_socket_fd()));
  ASSERT_NE(client_sender, nullptr);
  ASSERT_EQ(broker.nof_registered_sockets(), 1);

  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_TRUE(server_recv.has_value());
  ASSERT_TRUE(server_recv.value().has_notification());
  ASSERT_EQ(server_recv.value().sctp_notification(), SCTP_ASSOC_CHANGE);
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server does not receive more data.
  server_recv = server.receive();
  ASSERT_FALSE(server_recv.has_value());

  // Client receives SCTP_COMM_UP
  trigger_broker();

  // Client sends EOF
  client_sender = nullptr;

  // Client receives SCTP_COMM_COMP and closes connection.
  trigger_broker();
}

TEST_F(sctp_network_client_test, when_client_binds_address_then_connection_succeeds)
{
  client_cfg.sctp.bind_addresses    = {"127.0.0.2"};
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());
}

TEST_F(sctp_network_client_test, when_client_sends_data_then_server_receives_it)
{
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());
  std::vector<uint8_t> sent_data = {0x1, 0x2, 0x3};
  ASSERT_TRUE(send_data(sent_data));

  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server receives data.
  server_recv = server.receive();
  ASSERT_TRUE(server_recv.has_value());
  ASSERT_TRUE(server_recv.value().has_data());
  ASSERT_EQ(server_recv.value().data, sent_data);
}

TEST_F(sctp_network_client_test, when_server_sends_data_then_client_receives_it)
{
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());

  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Client receives SCTP_COMM_UP
  trigger_broker();

  std::vector<uint8_t> sent_data = {0x1, 0x2, 0x3};
  ASSERT_TRUE(server.send_data(sent_data,
                               client_cfg.sctp.ppid,
                               (const struct sockaddr&)server_recv->msg_src_addr,
                               server_recv->msg_src_addrlen));

  // Client receives data.
  ASSERT_TRUE(recv_notifier_factory.last_sdu.empty());
  trigger_broker();
  ASSERT_EQ(recv_notifier_factory.last_sdu, sent_data);
}

TEST_F(sctp_network_client_test, when_client_sender_is_destroyed_then_client_sends_eof)
{
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());

  // Client processes SCTP COMM UP
  trigger_broker();

  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Client wants to shut down connection.
  client_sender.reset();

  // Server receives SCTP COMM SHUTDOWN
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_notification(), SCTP_SHUTDOWN_EVENT);

  // Server receives SCTP SHUTDOWN COMP
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_SHUTDOWN_COMP);

  // Client receives an SCTP SHUTDOWN COMP
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);
}

TEST_F(sctp_network_client_test, when_client_is_destroyed_then_server_gets_eof)
{
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());
  // Client processes SCTP COMM UP
  trigger_broker();
  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  close_client();

  // Server receives SCTP COMM SHUTDOWN
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_notification(), SCTP_SHUTDOWN_EVENT);

  // Server receives SCTP SHUTDOWN COMP
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_SHUTDOWN_COMP);
}

TEST_F(sctp_network_client_test, when_server_is_destroyed_then_client_receives_sctp_shutdown_event)
{
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());
  // Client processes SCTP COMM UP
  trigger_broker();
  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  server.close();

  // Client receives an SCTP SHUTDOWN EVENT
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);

  // Client receives an SCTP SHUTDOWN COMP
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);
}

TEST_F(sctp_network_client_test, when_server_sends_eof_then_client_receives_sctp_shutdown_event)
{
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());
  // Client processes SCTP COMM UP
  trigger_broker();
  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server sends EOF
  ASSERT_TRUE(server.send_eof(
      client_cfg.sctp.ppid, (const struct sockaddr&)server_recv->msg_src_addr, server_recv->msg_src_addrlen));

  // Client receives an SCTP SHUTDOWN EVENT
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);

  // Client receives an SCTP SHUTDOWN COMP
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);

  // Server receives SCTP SHUTDOWN COMP
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_SHUTDOWN_COMP);
}

TEST_F(sctp_network_client_test, when_client_sends_eof_before_processing_incoming_eof_then_shutdown_is_successful)
{
  client_cfg.sctp.connect_addresses = {server.address};
  client_cfg.sctp.connect_port      = server.bind_port;
  client_cfg.sctp.dest_name         = "server";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());
  // Client processes SCTP COMM UP
  trigger_broker();
  // Server receives SCTP COMM UP
  auto server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server sends EOF
  ASSERT_TRUE(server.send_eof(
      client_cfg.sctp.ppid, (const struct sockaddr&)server_recv->msg_src_addr, server_recv->msg_src_addrlen));

  // Client sends EOF
  client_sender.reset();

  // Client receives an SCTP SHUTDOWN EVENT
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);

  // Client receives an SCTP SHUTDOWN COMP
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);

  // Server receives SCTP SHUTDOWN COMP
  server_recv = server.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_SHUTDOWN_COMP);
}

TEST_F(sctp_network_client_test, when_server_is_ipv6_and_connection_succeeds_then_data_exchange_works)
{
  dummy_sctp_server_ipv6 server_ipv6;

  client_cfg.sctp.connect_addresses = {server_ipv6.address};
  client_cfg.sctp.connect_port      = server_ipv6.bind_port;
  client_cfg.sctp.dest_name         = "server_ipv6";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());

  std::vector<uint8_t> sent_data = {0x1, 0x2, 0x3};
  ASSERT_TRUE(send_data(sent_data));

  // Server receives SCTP COMM UP
  auto server_recv = server_ipv6.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server receives data.
  server_recv = server_ipv6.receive();
  ASSERT_TRUE(server_recv.has_value());
  ASSERT_TRUE(server_recv.value().has_data());
  ASSERT_EQ(server_recv.value().data, sent_data);

  // Server sends data to client
  ASSERT_TRUE(server_ipv6.send_data(sent_data,
                                    client_cfg.sctp.ppid,
                                    (const struct sockaddr&)server_recv->msg_src_addr,
                                    server_recv->msg_src_addrlen));

  // Client receives SCTP_COMM_UP
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);

  // Client receives data
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);
  ASSERT_EQ(recv_notifier_factory.last_sdu, sent_data);

  // Client sends EOF
  client_sender.reset();

  // Process shutdown events
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);
}

TEST_F(sctp_network_client_test, when_server_has_multihomed_ipv4_addresses_then_data_exchange_works)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-4");

  dummy_sctp_server_multihomed_ipv4 server_multihomed;

  client_cfg.sctp.connect_addresses = server_multihomed.addresses;
  client_cfg.sctp.connect_port      = server_multihomed.bind_port;
  client_cfg.sctp.bind_addresses    = {"127.0.0.4", "127.0.0.5"};
  client_cfg.sctp.dest_name         = "server_multihomed";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());
  std::vector<uint8_t> sent_data = {0x7, 0x8, 0x9};
  ASSERT_TRUE(send_data(sent_data));

  // Server receives SCTP COMM UP
  auto server_recv = server_multihomed.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server receives data.
  server_recv = server_multihomed.receive();
  ASSERT_TRUE(server_recv.has_value());
  ASSERT_TRUE(server_recv.value().has_data());
  ASSERT_EQ(server_recv.value().data, sent_data);

  // Server sends data to client
  ASSERT_TRUE(server_multihomed.send_data(sent_data,
                                          client_cfg.sctp.ppid,
                                          (const struct sockaddr&)server_recv->msg_src_addr,
                                          server_recv->msg_src_addrlen));

  // Client receives SCTP_COMM_UP
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);

  // Client receives data
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);
  ASSERT_EQ(recv_notifier_factory.last_sdu, sent_data);

  // Client sends EOF
  client_sender.reset();

  // Process shutdown events
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);
}

TEST_F(sctp_network_client_test, when_server_has_multihomed_mixed_ipv4_and_ipv6_addresses_then_data_exchange_works)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-4");

  dummy_sctp_server_mixed server_mixed;

  client_cfg.sctp.connect_addresses = server_mixed.addresses;
  client_cfg.sctp.connect_port      = server_mixed.bind_port;
  client_cfg.sctp.dest_name         = "server_mixed";
  // Bind client to both IPv4 and IPv6 for multihoming
  // Bind client to loopback addresses available by default
  client_cfg.sctp.bind_addresses = {"127.0.0.4", "::1"};
  client                         = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(connect_to_server());

  std::vector<uint8_t> sent_data = {0x4, 0x5, 0x6};
  ASSERT_TRUE(send_data(sent_data));

  // Server receives SCTP COMM UP
  auto server_recv = server_mixed.receive();
  ASSERT_EQ(server_recv.value().sctp_assoc_change().sac_state, SCTP_COMM_UP);

  // Server receives data.
  server_recv = server_mixed.receive();
  ASSERT_TRUE(server_recv.has_value());
  ASSERT_TRUE(server_recv.value().has_data());
  ASSERT_EQ(server_recv.value().data, sent_data);

  // Server sends data to client
  ASSERT_TRUE(server_mixed.send_data(sent_data,
                                     client_cfg.sctp.ppid,
                                     (const struct sockaddr&)server_recv->msg_src_addr,
                                     server_recv->msg_src_addrlen));

  // Client receives SCTP_COMM_UP
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);

  // Client receives data
  trigger_broker();
  ASSERT_FALSE(recv_notifier_factory.destroyed);
  ASSERT_EQ(recv_notifier_factory.last_sdu, sent_data);

  // Client sends EOF
  client_sender.reset();

  // Process shutdown events
  trigger_broker();
  ASSERT_TRUE(recv_notifier_factory.destroyed);
}

TEST_F(sctp_network_client_test,
       ipv4_bind_and_ipv4_and_ipv6_connect_addresses_filters_ipv6_and_connects_successfully_over_ipv4_socket)
{
  // Server is IPv4 only
  dummy_sctp_server server_ipv4;
  client_cfg.sctp.bind_addresses    = {"127.0.0.2"};
  client_cfg.sctp.bind_port         = server_ipv4.bind_port;
  client_cfg.sctp.connect_addresses = {server_ipv4.address, "::1"}; // IPv4 and IPv6
  client_cfg.sctp.connect_port      = server_ipv4.bind_port;
  client_cfg.sctp.dest_name         = "server_ipv4";
  client                            = create_sctp_network_client(client_cfg);
  ASSERT_TRUE(client != nullptr);
  // Should only use IPv4 address, so connect should succeed
  ASSERT_TRUE(connect_to_server());
  // Check socket family is AF_INET
  int fd = client->get_socket_fd();
  ASSERT_GT(fd, 0);
  ASSERT_EQ(get_fd_family(fd), AF_INET);
}
