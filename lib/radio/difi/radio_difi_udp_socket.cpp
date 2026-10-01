// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_difi_udp_socket.h"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace ocudu;

radio_difi_udp_socket::~radio_difi_udp_socket()
{
  close();
}

void radio_difi_udp_socket::close()
{
  if (fd >= 0) {
    ::close(fd);
    fd = -1;
  }
}

bool radio_difi_udp_socket::open_tx(const std::string& ip, uint16_t port)
{
  // Re-opening would otherwise overwrite fd and leak the previous descriptor.
  close();

  fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (fd < 0) {
    logger.error("failed to create socket: {}", std::strerror(errno));
    return false;
  }

  std::memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port   = htons(port);
  if (::inet_pton(AF_INET, ip.c_str(), &dest_addr.sin_addr) != 1) {
    logger.error("invalid destination IP '{}'", ip);
    close();
    return false;
  }

  return true;
}

bool radio_difi_udp_socket::open_rx(const std::string& ip, uint16_t port)
{
  // Re-opening would otherwise overwrite fd and leak the previous descriptor.
  close();

  fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (fd < 0) {
    logger.error("failed to create socket: {}", std::strerror(errno));
    return false;
  }

  // Large receive buffer to absorb bursts without dropping packets.
  const int buf_size = static_cast<int>(RX_SOCKET_BUFFER_BYTES.value());
  if (::setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) < 0) {
    logger.error("failed to set SO_RCVBUF: {}", std::strerror(errno));
    close();
    return false;
  }

  // 100 ms receive timeout so recv() unblocks periodically for stop checks.
  struct timeval tv = {};
  tv.tv_sec         = 0;
  tv.tv_usec        = RX_TIMEOUT_MS * 1000;
  if (::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    logger.error("failed to set SO_RCVTIMEO: {}", std::strerror(errno));
    close();
    return false;
  }

  struct sockaddr_in bind_addr = {};
  bind_addr.sin_family         = AF_INET;
  bind_addr.sin_port           = htons(port);
  if (::inet_pton(AF_INET, ip.c_str(), &bind_addr.sin_addr) != 1) {
    logger.error("invalid bind IP '{}'", ip);
    close();
    return false;
  }

  if (::bind(fd, reinterpret_cast<const struct sockaddr*>(&bind_addr), sizeof(bind_addr)) < 0) {
    logger.error("bind to {}:{} failed: {}", ip, port, std::strerror(errno));
    close();
    return false;
  }

  return true;
}

bool radio_difi_udp_socket::send(span<const uint8_t> buf)
{
  if (fd < 0) {
    return false;
  }

  ssize_t sent =
      ::sendto(fd, buf.data(), buf.size(), 0, reinterpret_cast<const struct sockaddr*>(&dest_addr), sizeof(dest_addr));
  if (sent < 0 || static_cast<size_t>(sent) != buf.size()) {
    logger.error("sendto failed: {}", std::strerror(errno));
    return false;
  }
  return true;
}

expected<span<uint8_t>, difi_recv_error> radio_difi_udp_socket::recv(span<uint8_t> buf)
{
  if (fd < 0) {
    return make_unexpected(difi_recv_error::failure);
  }

  ssize_t n = ::recvfrom(fd, buf.data(), buf.size(), 0, nullptr, nullptr);
  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // No data within RX_TIMEOUT_MS; the caller checks its stop flag and retries.
      return make_unexpected(difi_recv_error::timeout);
    }
    logger.error("recvfrom error: {}", std::strerror(errno));
    return make_unexpected(difi_recv_error::failure);
  }

  return buf.first(static_cast<size_t>(n));
}

bool radio_difi_udp_socket::wait_readable(std::chrono::microseconds timeout)
{
  if (fd < 0) {
    return false;
  }

  struct pollfd pfd = {};
  pfd.fd            = fd;
  pfd.events        = POLLIN;

  // ppoll() rather than poll() so sub-millisecond budgets are honoured: at a slot of 1 ms, the
  // millisecond granularity of poll() would be the whole budget.
  const auto      secs = std::chrono::duration_cast<std::chrono::seconds>(timeout);
  struct timespec ts   = {};
  ts.tv_sec            = secs.count();
  ts.tv_nsec           = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout - secs).count();

  const int ret = ::ppoll(&pfd, 1, &ts, nullptr);
  if (ret < 0) {
    if (errno == EINTR) {
      // Interrupted by a signal — treat as "nothing ready yet"; the caller re-checks its deadline.
      return false;
    }
    logger.error("ppoll error: {}", std::strerror(errno));
    return false;
  }

  return (ret > 0) && ((pfd.revents & POLLIN) != 0);
}
