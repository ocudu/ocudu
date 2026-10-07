// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_difi_udp_socket.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <chrono>
#include <cstring>
#include <filesystem>
#include <gtest/gtest.h>
#include <thread>

using namespace ocudu;

/// Logger for sockets built directly by these tests.
static ocudulog::basic_logger& test_socket_logger()
{
  return ocudulog::fetch_basic_logger("difi:test", false);
}

/// How long a test waits for a datagram it expects to arrive.
static constexpr std::chrono::milliseconds RECV_TIMEOUT{100};

/// Waits up to \ref RECV_TIMEOUT for a datagram on \p sock and receives it into \p buf.
static expected<span<uint8_t>, difi_recv_error> recv_within(radio_difi_udp_socket& sock, span<uint8_t> buf)
{
  if (!sock.wait_readable(RECV_TIMEOUT)) {
    return make_unexpected(difi_recv_error::timeout);
  }
  return sock.try_recv(buf);
}

namespace {

/// Loopback port used by these tests. Chosen to avoid clashing with DIFI defaults.
constexpr uint16_t TEST_PORT = 19981;

/// Number of descriptors this process currently holds open, used to detect socket leaks.
int count_open_fds()
{
  int count = 0;
  for (const auto& entry : std::filesystem::directory_iterator("/proc/self/fd")) {
    (void)entry;
    ++count;
  }
  return count;
}

TEST(RadioDifiUdpSocketTest, OpenTxAndRxSucceeds)
{
  radio_difi_udp_socket rx(test_socket_logger());
  radio_difi_udp_socket tx(test_socket_logger());

  ASSERT_TRUE(rx.open_rx("127.0.0.1", TEST_PORT));
  ASSERT_TRUE(tx.open_tx("127.0.0.1", TEST_PORT));

  EXPECT_TRUE(rx.is_open());
  EXPECT_TRUE(tx.is_open());
}

TEST(RadioDifiUdpSocketTest, OversizedReceiveBufferIsCappedNotRejected)
{
  radio_difi_udp_socket rx(test_socket_logger());

  // Far above any default net.core.rmem_max. The kernel caps the request, so opening must still succeed.
  ASSERT_TRUE(rx.open_rx("127.0.0.1", TEST_PORT + 12, units::bytes(1024 * 1024 * 1024)));
  EXPECT_TRUE(rx.is_open());
}

TEST(RadioDifiUdpSocketTest, CloseIsIdempotent)
{
  radio_difi_udp_socket sock(test_socket_logger());
  ASSERT_TRUE(sock.open_rx("127.0.0.1", TEST_PORT + 1));
  sock.close();
  EXPECT_FALSE(sock.is_open());
  sock.close(); // second close must not crash
  EXPECT_FALSE(sock.is_open());
}

TEST(RadioDifiUdpSocketTest, SendAndReceiveLoopback)
{
  radio_difi_udp_socket rx(test_socket_logger());
  radio_difi_udp_socket tx(test_socket_logger());

  ASSERT_TRUE(rx.open_rx("127.0.0.1", TEST_PORT + 2));
  ASSERT_TRUE(tx.open_tx("127.0.0.1", TEST_PORT + 2));

  const uint8_t tx_buf[] = {0xde, 0xad, 0xbe, 0xef, 0x01, 0x02, 0x03, 0x04};
  ASSERT_TRUE(tx.send(tx_buf));

  uint8_t    rx_buf[64] = {};
  const auto received   = recv_within(rx, rx_buf);

  ASSERT_TRUE(received.has_value());
  ASSERT_EQ(received.value().size(), sizeof(tx_buf));
  EXPECT_EQ(std::memcmp(received.value().data(), tx_buf, sizeof(tx_buf)), 0);
}

TEST(RadioDifiUdpSocketTest, TryRecvReturnsImmediatelyWhenNothingIsQueued)
{
  radio_difi_udp_socket rx(test_socket_logger());
  ASSERT_TRUE(rx.open_rx("127.0.0.1", TEST_PORT + 10));

  // An empty queue must not wait out the 100 ms socket timeout.
  uint8_t    buf[64]  = {};
  const auto t0       = std::chrono::steady_clock::now();
  const auto received = rx.try_recv(buf);
  const auto elapsed  = std::chrono::steady_clock::now() - t0;

  ASSERT_FALSE(received.has_value());
  EXPECT_EQ(received.error(), difi_recv_error::timeout);
  EXPECT_LT(elapsed, std::chrono::milliseconds(50));
}

TEST(RadioDifiUdpSocketTest, TryRecvTakesAQueuedDatagram)
{
  radio_difi_udp_socket rx(test_socket_logger());
  radio_difi_udp_socket tx(test_socket_logger());

  ASSERT_TRUE(rx.open_rx("127.0.0.1", TEST_PORT + 11));
  ASSERT_TRUE(tx.open_tx("127.0.0.1", TEST_PORT + 11));

  const uint8_t tx_buf[] = {0xca, 0xfe};
  ASSERT_TRUE(tx.send(tx_buf));

  uint8_t    rx_buf[64] = {};
  const auto received   = recv_within(rx, rx_buf);

  ASSERT_TRUE(received.has_value());
  ASSERT_EQ(received.value().size(), sizeof(tx_buf));
  EXPECT_EQ(std::memcmp(received.value().data(), tx_buf, sizeof(tx_buf)), 0);
}

TEST(RadioDifiUdpSocketTest, ZeroLengthDatagramIsNotATimeout)
{
  radio_difi_udp_socket rx(test_socket_logger());
  radio_difi_udp_socket tx(test_socket_logger());

  ASSERT_TRUE(rx.open_rx("127.0.0.1", TEST_PORT + 5));
  ASSERT_TRUE(tx.open_tx("127.0.0.1", TEST_PORT + 5));

  // A zero-length UDP datagram is legal, and must be reported as a received empty packet rather
  // than being confused with the timeout case.
  ASSERT_TRUE(tx.send({}));

  uint8_t    buf[64]  = {};
  const auto received = recv_within(rx, buf);

  ASSERT_TRUE(received.has_value());
  EXPECT_TRUE(received.value().empty());
}

TEST(RadioDifiUdpSocketTest, SendAndRecvOnClosedSocketFail)
{
  radio_difi_udp_socket sock(test_socket_logger());

  // Never opened.
  uint8_t       buf[8] = {};
  const uint8_t payload[4]{};
  EXPECT_FALSE(sock.send(payload));
  auto received = sock.try_recv(buf);
  ASSERT_FALSE(received.has_value());
  EXPECT_EQ(received.error(), difi_recv_error::failure);

  // Opened and then closed again.
  ASSERT_TRUE(sock.open_tx("127.0.0.1", TEST_PORT + 6));
  sock.close();
  EXPECT_FALSE(sock.send(payload));
  received = sock.try_recv(buf);
  ASSERT_FALSE(received.has_value());
  EXPECT_EQ(received.error(), difi_recv_error::failure);
}

TEST(RadioDifiUdpSocketTest, ReopeningDoesNotLeakDescriptors)
{
  radio_difi_udp_socket sock(test_socket_logger());

  ASSERT_TRUE(sock.open_rx("127.0.0.1", TEST_PORT + 7));
  const int first_fd = count_open_fds();

  // Each re-open must close the previous socket rather than overwrite the descriptor.
  for (unsigned i = 0; i != 16; ++i) {
    ASSERT_TRUE(sock.open_rx("127.0.0.1", TEST_PORT + 7));
  }

  EXPECT_EQ(count_open_fds(), first_fd);
}

TEST(RadioDifiUdpSocketTest, MultiplePacketsArrivedInOrder)
{
  radio_difi_udp_socket rx(test_socket_logger());
  radio_difi_udp_socket tx(test_socket_logger());

  ASSERT_TRUE(rx.open_rx("127.0.0.1", TEST_PORT + 4));
  ASSERT_TRUE(tx.open_tx("127.0.0.1", TEST_PORT + 4));

  for (uint8_t i = 0; i < 4; ++i) {
    ASSERT_TRUE(tx.send(span<const uint8_t>(&i, 1)));
  }

  for (uint8_t i = 0; i < 4; ++i) {
    uint8_t    val      = 0xff;
    const auto received = recv_within(rx, span<uint8_t>(&val, 1));
    ASSERT_TRUE(received.has_value());
    ASSERT_EQ(received.value().size(), 1U);
    EXPECT_EQ(val, i);
  }
}

} // namespace
