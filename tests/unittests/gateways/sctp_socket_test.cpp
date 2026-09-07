// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/gateways/sctp_socket.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <arpa/inet.h>
#include <array>
#include <cstring>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <optional>
#include <sys/socket.h>

using namespace ocudu;

class sctp_socket_test : public ::testing::Test
{
protected:
  sctp_socket_test() { ocudulog::init(); }

  ~sctp_socket_test() override { ocudulog::flush(); }

  static sctp_socket_params create_default_params()
  {
    sctp_socket_params params;
    params.if_name     = "test_sctp";
    params.ai_family   = AF_INET;
    params.ai_socktype = SOCK_SEQPACKET;
    return params;
  }

  /// Create an IPv4 loopback address with the given port and offset (127.0.0.x)
  static sockaddr_storage create_ipv4_loopback_addr(uint16_t port, uint8_t offset = 1)
  {
    sockaddr_storage addr_storage = {};
    auto*            addr_in      = reinterpret_cast<sockaddr_in*>(&addr_storage);
    addr_in->sin_family           = AF_INET;
    addr_in->sin_port             = htons(port);
    // 127.0.0.x where x is the offset (defaults to 1 for standard loopback)
    addr_in->sin_addr.s_addr = htonl((127 << 24) | offset);
    return addr_storage;
  }

  /// Create an IPv6 loopback address (::1) with the given port.
  static sockaddr_storage create_ipv6_loopback_addr(uint16_t port)
  {
    sockaddr_storage addr_storage = {};
    auto*            addr_in6     = reinterpret_cast<sockaddr_in6*>(&addr_storage);
    addr_in6->sin6_family         = AF_INET6;
    addr_in6->sin6_port           = htons(port);
    addr_in6->sin6_addr           = in6addr_loopback;
    return addr_storage;
  }

  /// Buffer for an SCTP address list. One entry over the maximum this test binds covers the header, see below.
  using sctp_addr_buf = std::array<sockaddr_storage, 8>;

  /// \brief Drop-in for \c sctp_getladdrs() / \c sctp_getpaddrs() that reads into a caller-owned \p buf.
  ///
  /// Next to the address list, the kernel writes an \c sctp_getaddrs header in front of it, while the \c optlen it
  /// reports back covers the list alone. Valgrind sizes the region it marks as initialized from that \c optlen and
  /// therefore leaves the last 8 bytes of the list marked as uninitialized. For a \c sockaddr_in6 those are the tail
  /// of \c sin6_addr, so comparing the address reports a false positive. libsctp allocates its buffer itself, which
  /// leaves nothing to initialize, hence the direct \c getsockopt() call on a \p buf the caller zero-initializes.
  ///
  /// \return Number of addresses in the list, or -1 if it could not be retrieved.
  static int get_sctp_addrs(int fd, int optname, sctp_addr_buf& buf, sockaddr** addrs, sctp_assoc_t assoc_id = 0)
  {
    auto* getaddrs     = reinterpret_cast<sctp_getaddrs*>(buf.data());
    getaddrs->assoc_id = assoc_id;
    *addrs             = reinterpret_cast<sockaddr*>(getaddrs->addrs);

    socklen_t len = sizeof(buf);
    return ::getsockopt(fd, IPPROTO_SCTP, optname, buf.data(), &len) == 0 ? static_cast<int>(getaddrs->addr_num) : -1;
  }

  /// Verify bound addresses using the SCTP local address list.
  static void verify_bound_address_ipv4(int fd, std::optional<uint16_t> expected_port, in_addr_t expected_addr)
  {
    sctp_addr_buf buf   = {};
    sockaddr*     addrs = nullptr;
    int           count = get_sctp_addrs(fd, SCTP_GET_LOCAL_ADDRS, buf, &addrs);
    ASSERT_GT(count, 0);

    // Verify the first address matches expectations.
    auto* sin = reinterpret_cast<sockaddr_in*>(addrs);
    EXPECT_EQ(sin->sin_family, AF_INET);
    if (expected_port.has_value()) {
      EXPECT_EQ(ntohs(sin->sin_port), expected_port.value());
    } else {
      // Check that dynamic port was assigned.
      EXPECT_GT(ntohs(sin->sin_port), 0);
    }
    EXPECT_EQ(ntohl(sin->sin_addr.s_addr), expected_addr);
  }

  /// Verify bound addresses for IPv6 using the SCTP local address list.
  static void verify_bound_address_ipv6(int fd, std::optional<uint16_t> expected_port, const in6_addr& expected_addr)
  {
    sctp_addr_buf buf   = {};
    sockaddr*     addrs = nullptr;
    int           count = get_sctp_addrs(fd, SCTP_GET_LOCAL_ADDRS, buf, &addrs);
    ASSERT_GT(count, 0);

    // Verify the first address matches expectations.
    auto* sin6 = reinterpret_cast<sockaddr_in6*>(addrs);
    EXPECT_EQ(sin6->sin6_family, AF_INET6);
    if (expected_port.has_value()) {
      EXPECT_EQ(ntohs(sin6->sin6_port), expected_port.value());
    } else {
      // Check that dynamic port was assigned.
      EXPECT_GT(ntohs(sin6->sin6_port), 0);
    }
    EXPECT_EQ(std::memcmp(&sin6->sin6_addr, &expected_addr, sizeof(expected_addr)), 0);
  }

  /// Verify multiple bound addresses using the SCTP local address list.
  static void verify_multiple_bound_addresses(int                     fd,
                                              std::optional<uint16_t> expected_port,
                                              int                     expected_ipv4_count,
                                              int                     expected_ipv6_count)
  {
    sctp_addr_buf buf   = {};
    sockaddr*     addrs = nullptr;
    int           count = get_sctp_addrs(fd, SCTP_GET_LOCAL_ADDRS, buf, &addrs);
    ASSERT_GT(count, 0) << "Expected at least one bound address";

    // Count addresses by family using offset-based parsing
    int    ipv4_count = 0;
    int    ipv6_count = 0;
    size_t offset     = 0;

    // Verify port for all addresses if specified
    for (int i = 0; i < count; ++i) {
      sockaddr* current = reinterpret_cast<sockaddr*>(reinterpret_cast<char*>(addrs) + offset);
      if (current->sa_family == AF_INET) {
        ipv4_count++;
        auto* sin = reinterpret_cast<sockaddr_in*>(current);
        if (expected_port.has_value()) {
          EXPECT_EQ(ntohs(sin->sin_port), expected_port.value());
        }
      } else if (current->sa_family == AF_INET6) {
        ipv6_count++;
        auto* sin6 = reinterpret_cast<sockaddr_in6*>(current);
        if (expected_port.has_value()) {
          EXPECT_EQ(ntohs(sin6->sin6_port), expected_port.value());
        }
      }
      size_t addr_size = (current->sa_family == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
      offset += addr_size;
    }

    // Check exact address counts
    EXPECT_EQ(ipv4_count, expected_ipv4_count);
    EXPECT_EQ(ipv6_count, expected_ipv6_count);
  }

  /// Verify multiple peer addresses using the SCTP peer address list.
  static void verify_multiple_peer_addresses(int                     fd,
                                             sctp_assoc_t            assoc_id,
                                             std::optional<uint16_t> expected_port,
                                             int                     expected_ipv4_count,
                                             int                     expected_ipv6_count)
  {
    sctp_addr_buf buf   = {};
    sockaddr*     addrs = nullptr;
    int           count = get_sctp_addrs(fd, SCTP_GET_PEER_ADDRS, buf, &addrs, assoc_id);
    ASSERT_GT(count, 0) << "Expected at least one peer address";

    // Count addresses by family using offset-based parsing
    int    ipv4_count = 0;
    int    ipv6_count = 0;
    size_t offset     = 0;

    // Verify port for all addresses if specified
    for (int i = 0; i < count; ++i) {
      sockaddr* current = reinterpret_cast<sockaddr*>(reinterpret_cast<char*>(addrs) + offset);
      if (current->sa_family == AF_INET) {
        ipv4_count++;
        auto* sin = reinterpret_cast<sockaddr_in*>(current);
        if (expected_port.has_value()) {
          EXPECT_EQ(ntohs(sin->sin_port), expected_port.value());
        }
      } else if (current->sa_family == AF_INET6) {
        ipv6_count++;
        auto* sin6 = reinterpret_cast<sockaddr_in6*>(current);
        if (expected_port.has_value()) {
          EXPECT_EQ(ntohs(sin6->sin6_port), expected_port.value());
        }
      }
      size_t addr_size = (current->sa_family == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
      offset += addr_size;
    }

    // Check exact address counts
    EXPECT_EQ(ipv4_count, expected_ipv4_count);
    EXPECT_EQ(ipv6_count, expected_ipv6_count);
  }
};

/// Test that creating a socket with empty interface name fails.
TEST_F(sctp_socket_test, create_fails_with_empty_if_name)
{
  sctp_socket_params params = create_default_params();
  params.if_name            = "";

  auto result = sctp_socket::create(params);
  ASSERT_FALSE(result.has_value());
}

/// Test that a socket can be successfully created with valid parameters.
TEST_F(sctp_socket_test, create_succeeds_with_valid_params)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().is_open());
  EXPECT_TRUE(result.value().fd().is_open());
}

/// Test that a socket can be created with IPv6 family.
TEST_F(sctp_socket_test, create_succeeds_with_ipv6)
{
  sctp_socket_params params = create_default_params();
  params.ai_family          = AF_INET6;

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().is_open());
}

/// Test that close() works properly.
TEST_F(sctp_socket_test, close_socket)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();
  EXPECT_TRUE(sock.is_open());

  EXPECT_TRUE(sock.close());
  EXPECT_FALSE(sock.is_open());
}

/// Test that close() on an already closed socket still returns true.
TEST_F(sctp_socket_test, close_already_closed_socket)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();
  EXPECT_TRUE(sock.is_open());

  EXPECT_TRUE(sock.close());
  EXPECT_FALSE(sock.is_open());

  EXPECT_TRUE(sock.close());
}

/// Test socket creation with all options set.
TEST_F(sctp_socket_test, create_with_all_options)
{
  sctp_socket_params params = create_default_params();
  params.reuse_addr         = true;
  params.non_blocking_mode  = true;
  params.rx_timeout         = std::chrono::seconds(1);
  params.rto_initial        = std::chrono::milliseconds(2);
  params.rto_min            = std::chrono::milliseconds(3);
  params.rto_max            = std::chrono::milliseconds(4);
  params.init_max_attempts  = 5;
  params.max_init_timeo     = std::chrono::milliseconds(6);
  params.hb_interval        = std::chrono::milliseconds(7);
  params.assoc_max_rxt      = 8;
  params.nodelay            = true;

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result.value().is_open());
}

/// Test bindx to loopback address with a dynamic port (when port is not set then OS picks the port number).
TEST_F(sctp_socket_test, bindx_to_loopback_with_dynamic_port)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  sockaddr_storage addr_storage = create_ipv4_loopback_addr(0);

  EXPECT_TRUE(sock.bindx({addr_storage}, ""));

  ASSERT_TRUE(sock.get_bound_port().has_value());
  EXPECT_GT(sock.get_bound_port().value(), 0);

  verify_bound_address_ipv4(sock.fd().value(), std::nullopt, INADDR_LOOPBACK);
}

/// Test bindx to loopback address with a dynamic port (when port is not set then OS picks the port number).
TEST_F(sctp_socket_test, bindx_to_loopback_with_dynamic_port_ipv6)
{
  sctp_socket_params params = create_default_params();
  params.ai_family          = AF_INET6;

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  sockaddr_storage addr_storage = create_ipv6_loopback_addr(0);

  EXPECT_TRUE(sock.bindx({addr_storage}, ""));

  ASSERT_TRUE(sock.get_bound_port().has_value());
  EXPECT_GT(sock.get_bound_port().value(), 0);

  verify_bound_address_ipv6(sock.fd().value(), std::nullopt, in6addr_loopback);
}

/// Test listen on a bound socket.
TEST_F(sctp_socket_test, listen_on_bound_socket)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  sockaddr_storage addr_storage = create_ipv4_loopback_addr(0);

  ASSERT_TRUE(sock.bindx({addr_storage}, ""));
  EXPECT_TRUE(sock.listen());
}

/// Test listen on an unbound socket fails.
TEST_F(sctp_socket_test, listen_on_unbound_socket_fails)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  EXPECT_FALSE(sock.listen());
}

/// Test get_bound_port returns 0 for an unbound socket.
TEST_F(sctp_socket_test, get_bound_port_returns_zero_for_unbound_socket)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  ASSERT_TRUE(sock.get_bound_port().has_value());
  EXPECT_EQ(sock.get_bound_port().value(), 0);
}

/// Test listen fails on a closed socket.
TEST_F(sctp_socket_test, listen_fails_on_closed_socket)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();
  sock.close();

  EXPECT_FALSE(sock.listen());
}

/// Test get_bound_port returns nullopt for a closed socket.
TEST_F(sctp_socket_test, get_bound_port_returns_nullopt_for_closed_socket)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  sockaddr_storage addr_storage = create_ipv4_loopback_addr(0);

  ASSERT_TRUE(sock.bindx({addr_storage}, ""));

  EXPECT_TRUE(sock.get_bound_port().has_value());
  EXPECT_GT(sock.get_bound_port().value(), 0);

  sock.close();

  EXPECT_FALSE(sock.get_bound_port().has_value());
}

/// Test bindx with an empty address list.
TEST_F(sctp_socket_test, bindx_with_empty_list)
{
  sctp_socket_params params = create_default_params();
  params.reuse_addr         = true;

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  std::vector<sockaddr_storage> addrs;
  EXPECT_FALSE(sock.bindx(addrs, ""));
}

/// Test bindx with a single address.
TEST_F(sctp_socket_test, bindx_with_single_address)
{
  sctp_socket_params params = create_default_params();
  params.reuse_addr         = true;

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  std::vector<sockaddr_storage> addrs;
  addrs.push_back(create_ipv4_loopback_addr(0));

  EXPECT_TRUE(sock.bindx(addrs, ""));

  // Verify bound address using the SCTP local address list.
  verify_bound_address_ipv4(sock.fd().value(), std::nullopt, INADDR_LOOPBACK);
}

/// Test bindx fails on a closed socket.
TEST_F(sctp_socket_test, bindx_fails_on_closed_socket)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();
  sock.close();

  std::vector<sockaddr_storage> addrs;
  addrs.push_back(create_ipv4_loopback_addr(0));

  EXPECT_FALSE(sock.bindx(addrs, ""));
}

/// Test connectx fails on a closed socket.
TEST_F(sctp_socket_test, connectx_fails_on_closed_socket)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();
  sock.close();

  sockaddr_storage addr_storage = create_ipv4_loopback_addr(12345);
  sctp_assoc_t     assoc_id     = 0;

  EXPECT_FALSE(sock.connectx({addr_storage}, assoc_id));
}

/// Test connectx fails with an empty address list.
TEST_F(sctp_socket_test, connectx_fails_with_empty_list)
{
  sctp_socket_params params = create_default_params();

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  std::vector<sockaddr_storage> addrs;
  sctp_assoc_t                  assoc_id = 0;
  EXPECT_FALSE(sock.connectx(addrs, assoc_id));
}

/// Test bindx with multiple IPv4 loopback addresses.
TEST_F(sctp_socket_test, bindx_with_multiple_ipv4_addresses)
{
  sctp_socket_params params = create_default_params();
  params.reuse_addr         = true;

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  std::vector<sockaddr_storage> addrs;
  addrs.push_back(create_ipv4_loopback_addr(0, 1)); // 127.0.0.1
  addrs.push_back(create_ipv4_loopback_addr(0, 2)); // 127.0.0.2
  addrs.push_back(create_ipv4_loopback_addr(0, 3)); // 127.0.0.3

  EXPECT_TRUE(sock.bindx(addrs, ""));

  // Verify socket is bound and has a port assigned
  ASSERT_TRUE(sock.get_bound_port().has_value());
  EXPECT_GT(sock.get_bound_port().value(), 0);

  // Verify all 3 IPv4 addresses are bound
  verify_multiple_bound_addresses(sock.fd().value(), std::nullopt, 3, 0);
}

// Note: bindx with multiple IPv6 addresses is not tested here because it would require
// system configuration (e.g., ULA addresses fd00::1, fd00::2, etc. must be added to the system).
// IPv6 is tested with single ::1 address and in mixed IPv4+IPv6 scenarios below.

/// Test bindx with mixed IPv4 and IPv6 addresses.
TEST_F(sctp_socket_test, bindx_with_mixed_ipv4_and_ipv6_addresses)
{
  sctp_socket_params params = create_default_params();
  params.ai_family          = AF_INET6;
  params.reuse_addr         = true;

  auto result = sctp_socket::create(params);
  ASSERT_TRUE(result.has_value());

  sctp_socket& sock = result.value();

  std::vector<sockaddr_storage> addrs;
  addrs.push_back(create_ipv4_loopback_addr(0, 1)); // 127.0.0.1
  addrs.push_back(create_ipv6_loopback_addr(0));    // ::1
  addrs.push_back(create_ipv4_loopback_addr(0, 2)); // 127.0.0.2

  EXPECT_TRUE(sock.bindx(addrs, ""));

  // Verify socket is bound and has a port assigned
  ASSERT_TRUE(sock.get_bound_port().has_value());
  EXPECT_GT(sock.get_bound_port().value(), 0);

  // Verify all 3 addresses are bound (2 IPv4 + 1 IPv6)
  // Note: IPv6 socket converts IPv4 addresses to IPv4-mapped IPv6 addresses
  verify_multiple_bound_addresses(sock.fd().value(), std::nullopt, 0, 3);

  sock.close();
}

/// Test connectx with multiple IPv4 loopback addresses.
TEST_F(sctp_socket_test, connectx_with_multiple_ipv4_addresses)
{
  // Create and bind server socket
  sctp_socket_params server_params = create_default_params();
  server_params.reuse_addr         = true;

  auto server_result = sctp_socket::create(server_params);
  ASSERT_TRUE(server_result.has_value());

  sctp_socket& server_sock = server_result.value();

  std::vector<sockaddr_storage> server_addrs;
  server_addrs.push_back(create_ipv4_loopback_addr(0, 1)); // 127.0.0.1
  server_addrs.push_back(create_ipv4_loopback_addr(0, 2)); // 127.0.0.2

  ASSERT_TRUE(server_sock.bindx(server_addrs, ""));
  ASSERT_TRUE(server_sock.listen());

  uint16_t server_port = server_sock.get_bound_port().value();

  // Create client socket
  sctp_socket_params client_params = create_default_params();
  auto               client_result = sctp_socket::create(client_params);
  ASSERT_TRUE(client_result.has_value());

  sctp_socket& client_sock = client_result.value();

  std::vector<sockaddr_storage> client_addrs;
  client_addrs.push_back(create_ipv4_loopback_addr(server_port, 1)); // 127.0.0.1
  client_addrs.push_back(create_ipv4_loopback_addr(server_port, 2)); // 127.0.0.2

  sctp_assoc_t assoc_id = 0;
  EXPECT_TRUE(client_sock.connectx(client_addrs, assoc_id));
  EXPECT_GT(assoc_id, 0);

  // Verify server's 2 IPv4 peer addresses are visible to client
  verify_multiple_peer_addresses(client_sock.fd().value(), assoc_id, server_port, 2, 0);

  // Verify client is bound to different addresses than server
  ASSERT_TRUE(client_sock.get_bound_port().has_value());
  uint16_t client_port = client_sock.get_bound_port().value();
  EXPECT_NE(client_port, server_port);

  client_sock.close();
  server_sock.close();
}

// Note: connectx with multiple IPv6 addresses is not tested here because it would require
// system configuration (e.g., ULA addresses fd00::1, fd00::2, etc. must be added to the system).
// IPv6 is tested with single ::1 address and in mixed IPv4+IPv6 scenarios below.

/// Test connectx with mixed IPv4 and IPv6 addresses.
TEST_F(sctp_socket_test, connectx_with_mixed_ipv4_and_ipv6_addresses)
{
  // Create and bind server socket
  sctp_socket_params server_params = create_default_params();
  server_params.ai_family          = AF_INET6;
  server_params.reuse_addr         = true;

  auto server_result = sctp_socket::create(server_params);
  ASSERT_TRUE(server_result.has_value());

  sctp_socket& server_sock = server_result.value();

  std::vector<sockaddr_storage> server_addrs;
  server_addrs.push_back(create_ipv4_loopback_addr(0, 1)); // 127.0.0.1
  server_addrs.push_back(create_ipv6_loopback_addr(0));    // ::1

  ASSERT_TRUE(server_sock.bindx(server_addrs, ""));
  ASSERT_TRUE(server_sock.listen());

  uint16_t server_port = server_sock.get_bound_port().value();

  // Create client socket
  sctp_socket_params client_params = create_default_params();
  client_params.ai_family          = AF_INET6;
  auto client_result               = sctp_socket::create(client_params);
  ASSERT_TRUE(client_result.has_value());

  sctp_socket& client_sock = client_result.value();

  // Explicitly bind client to specific addresses before connecting
  std::vector<sockaddr_storage> client_bind_addrs;
  client_bind_addrs.push_back(create_ipv4_loopback_addr(0, 1)); // 127.0.0.1
  client_bind_addrs.push_back(create_ipv6_loopback_addr(0));    // ::1
  ASSERT_TRUE(client_sock.bindx(client_bind_addrs, ""));

  std::vector<sockaddr_storage> client_addrs;
  client_addrs.push_back(create_ipv4_loopback_addr(server_port, 1)); // 127.0.0.1
  client_addrs.push_back(create_ipv6_loopback_addr(server_port));    // ::1

  sctp_assoc_t assoc_id = 0;
  EXPECT_TRUE(client_sock.connectx(client_addrs, assoc_id));
  EXPECT_GT(assoc_id, 0);

  // Verify server's 2 peer addresses (1 IPv4 + 1 IPv6) are visible to client
  // Note: When connecting to an IPv6 socket, IPv4 addresses become IPv4-mapped IPv6
  verify_multiple_peer_addresses(client_sock.fd().value(), assoc_id, server_port, 0, 2);

  // Verify client is bound to different addresses than server
  ASSERT_TRUE(client_sock.get_bound_port().has_value());
  uint16_t client_port = client_sock.get_bound_port().value();
  EXPECT_NE(client_port, server_port);

  // Verify client's 2 bound addresses (mixed IPv4 and IPv6)
  // Note: IPv6 socket converts IPv4 addresses to IPv4-mapped IPv6 addresses
  verify_multiple_bound_addresses(client_sock.fd().value(), std::nullopt, 0, 2);

  client_sock.close();
  server_sock.close();
}

/// Test connectx with different number of addresses on server and client.
TEST_F(sctp_socket_test, connectx_with_different_address_counts)
{
  // Create and bind server socket with 2 addresses
  sctp_socket_params server_params = create_default_params();
  server_params.reuse_addr         = true;

  auto server_result = sctp_socket::create(server_params);
  ASSERT_TRUE(server_result.has_value());

  sctp_socket& server_sock = server_result.value();

  std::vector<sockaddr_storage> server_addrs;
  server_addrs.push_back(create_ipv4_loopback_addr(0, 1)); // 127.0.0.1
  server_addrs.push_back(create_ipv4_loopback_addr(0, 2)); // 127.0.0.2

  ASSERT_TRUE(server_sock.bindx(server_addrs, ""));
  ASSERT_TRUE(server_sock.listen());

  uint16_t server_port = server_sock.get_bound_port().value();

  // Create client socket with 3 addresses
  sctp_socket_params client_params = create_default_params();
  auto               client_result = sctp_socket::create(client_params);
  ASSERT_TRUE(client_result.has_value());

  sctp_socket& client_sock = client_result.value();

  // Explicitly bind client to specific addresses before connecting
  std::vector<sockaddr_storage> client_bind_addrs;
  client_bind_addrs.push_back(create_ipv4_loopback_addr(0, 1)); // 127.0.0.1
  client_bind_addrs.push_back(create_ipv4_loopback_addr(0, 2)); // 127.0.0.2
  client_bind_addrs.push_back(create_ipv4_loopback_addr(0, 3)); // 127.0.0.3
  ASSERT_TRUE(client_sock.bindx(client_bind_addrs, ""));

  std::vector<sockaddr_storage> client_addrs;
  client_addrs.push_back(create_ipv4_loopback_addr(server_port, 1)); // 127.0.0.1
  client_addrs.push_back(create_ipv4_loopback_addr(server_port, 2)); // 127.0.0.2
  client_addrs.push_back(create_ipv4_loopback_addr(server_port, 3)); // 127.0.0.3

  sctp_assoc_t assoc_id = 0;
  // connectx should succeed even with different address counts
  EXPECT_TRUE(client_sock.connectx(client_addrs, assoc_id));
  EXPECT_GT(assoc_id, 0);

  // Server has 2 IPv4 addresses, verify peer addresses
  verify_multiple_peer_addresses(client_sock.fd().value(), assoc_id, server_port, 2, 0);

  // Verify client is bound to different addresses than server
  ASSERT_TRUE(client_sock.get_bound_port().has_value());
  uint16_t client_port = client_sock.get_bound_port().value();
  EXPECT_NE(client_port, server_port);

  // Client bound to 3 IPv4 addresses
  verify_multiple_bound_addresses(client_sock.fd().value(), client_port, 3, 0);

  client_sock.close();
  server_sock.close();
}
