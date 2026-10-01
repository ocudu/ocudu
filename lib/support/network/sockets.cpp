// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/io/sockets.h"
#include "ocudu/support/io/unique_fd.h"
#include "ocudu/support/ocudu_assert.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace ocudu;

bool ocudu::getnameinfo(const struct sockaddr&        ai_addr,
                        const socklen_t&              ai_addrlen,
                        std::array<char, NI_MAXHOST>& ip_address,
                        int&                          port)
{
  char port_nr[NI_MAXSERV];
  if (getnameinfo(
          &ai_addr, ai_addrlen, ip_address.data(), NI_MAXHOST, port_nr, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) !=
      0) {
    return false;
  }
  port = std::stoi(port_nr);
  return true;
}

socket_name_info ocudu::get_nameinfo(const struct sockaddr& ai_addr, const socklen_t& ai_addrlen)
{
  std::array<char, NI_MAXHOST> ip_addr;
  int                          port;
  if (not getnameinfo(ai_addr, ai_addrlen, ip_addr, port)) {
    return {std::string(""), -1};
  }
  return socket_name_info{std::string(ip_addr.data()), port};
}

bool ocudu::set_reuse_addr(const unique_fd& fd, ocudulog::basic_logger& logger)
{
  ocudu_assert(fd.is_open(), "fd is not open");
  int one = 1;
  if (::setsockopt(fd.value(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) != 0) {
    logger.error("fd={}: Couldn't set reuseaddr for socket: {}", fd.value(), ::strerror(errno));
    return false;
  }
  return true;
}

bool ocudu::bind_to_interface(const unique_fd& fd, const std::string& interface, ocudulog::basic_logger& logger)
{
  if (interface.empty() || interface == "auto") {
    // no need to change anything
    return true;
  }

  ifreq ifr{};
  std::strncpy(ifr.ifr_ifrn.ifrn_name, interface.c_str(), IFNAMSIZ - 1);
  ifr.ifr_ifrn.ifrn_name[IFNAMSIZ - 1] = 0; // ensure null termination in case input exceeds maximum length

  if (::setsockopt(fd.value(), SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0) {
    logger.error("fd={}: Could not bind socket to interface \"{}\". errno=\"{}\"",
                 fd.value(),
                 ifr.ifr_ifrn.ifrn_name,
                 ::strerror(errno));
    return false;
  }
  return true;
}

bool ocudu::sockaddr_to_ip_str(const sockaddr* addr, std::string& ip_address, ocudulog::basic_logger& logger)
{
  char addr_str[INET6_ADDRSTRLEN] = {};
  if (addr->sa_family == AF_INET) {
    if (inet_ntop(AF_INET, &((sockaddr_in*)addr)->sin_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in to string. errno=\"{}\"", ::strerror(errno));
      return false;
    }
  } else if (addr->sa_family == AF_INET6) {
    if (inet_ntop(AF_INET6, &((sockaddr_in6*)addr)->sin6_addr, addr_str, INET6_ADDRSTRLEN) == nullptr) {
      logger.error("Could not convert sockaddr_in6 to string. errno=\"{}\"", ::strerror(errno));
      return false;
    }
  } else {
    logger.error("Unhandled address family.");
    return false;
  }

  ip_address = addr_str;
  return true;
}

uint16_t ocudu::sockaddr_to_port(const sockaddr* addr, ocudulog::basic_logger& logger)
{
  if (addr->sa_family == AF_INET) {
    return ntohs(((sockaddr_in*)addr)->sin_port);
  }
  if (addr->sa_family == AF_INET6) {
    return ntohs(((sockaddr_in6*)addr)->sin6_port);
  }
  logger.error("Unhandled address family.");
  return 0;
}

bool ocudu::set_receive_timeout(const unique_fd& fd, std::chrono::seconds rx_timeout, ocudulog::basic_logger& logger)
{
  ocudu_sanity_check(fd.is_open(), "Invalid FD");
  struct timeval tv;
  tv.tv_sec  = rx_timeout.count();
  tv.tv_usec = 0;

  if (::setsockopt(fd.value(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) != 0) {
    logger.error("SCTP failed to be created. Cause: Couldn't set receive timeout for socket: {}", ::strerror(errno));
    return false;
  }
  return true;
}

bool ocudu::set_non_blocking(const unique_fd& fd, ocudulog::basic_logger& logger)
{
  if (not fd.is_open()) {
    logger.error("Failed to set socket as non-blocking. Cause: Socket is closed");
    return false;
  }
  int flags = ::fcntl(fd.value(), F_GETFL, 0);
  if (flags == -1) {
    logger.error("Failed to set socket as non-blocking. Cause: Error getting socket flags: {}", ::strerror(errno));
    return false;
  }

  int s = ::fcntl(fd.value(), F_SETFL, flags | O_NONBLOCK);
  if (s == -1) {
    logger.error("Failed to set socket as non-blocking. Cause: Error {}", ::strerror(errno));
    return false;
  }

  return true;
}

std::string ocudu::sock_type_to_str(int type)
{
  switch (type) {
    case SOCK_STREAM:
      return "SOCK_STREAM";
    case SOCK_DGRAM:
      return "SOCK_DGRAM";
    case SOCK_RAW:
      return "SOCK_RAW";
    case SOCK_RDM:
      return "SOCK_RDM";
    case SOCK_SEQPACKET:
      return "SOCK_SEQPACKET";
    case SOCK_DCCP:
      return "SOCK_DCCP";
    case SOCK_PACKET:
      return "SOCK_PACKET";
  }
  return "unknown type";
}

bool ocudu::sockaddr_storage_equal(const sockaddr_storage& a, const sockaddr_storage& b)
{
  const auto* sa_a = reinterpret_cast<const sockaddr*>(&a);
  const auto* sa_b = reinterpret_cast<const sockaddr*>(&b);

  if (sa_a->sa_family != sa_b->sa_family) {
    return false;
  }

  if (sa_a->sa_family == AF_INET) {
    const auto* in_a = reinterpret_cast<const sockaddr_in*>(&a);
    const auto* in_b = reinterpret_cast<const sockaddr_in*>(&b);
    return in_a->sin_port == in_b->sin_port && in_a->sin_addr.s_addr == in_b->sin_addr.s_addr;
  }
  if (sa_a->sa_family == AF_INET6) {
    const auto* in6_a = reinterpret_cast<const sockaddr_in6*>(&a);
    const auto* in6_b = reinterpret_cast<const sockaddr_in6*>(&b);
    return in6_a->sin6_port == in6_b->sin6_port &&
           std::memcmp(&in6_a->sin6_addr, &in6_b->sin6_addr, sizeof(in6_addr)) == 0 &&
           in6_a->sin6_scope_id == in6_b->sin6_scope_id;
  }
  return false;
}

bool ocudu::sockaddr_storage_less::operator()(const sockaddr_storage& a, const sockaddr_storage& b) const
{
  const auto* sa_a = reinterpret_cast<const sockaddr*>(&a);
  const auto* sa_b = reinterpret_cast<const sockaddr*>(&b);

  // Compare family first
  if (sa_a->sa_family != sa_b->sa_family) {
    return sa_a->sa_family < sa_b->sa_family;
  }

  // Compare based on family type, then port, then address, then scope_id for IPv6 only
  if (sa_a->sa_family == AF_INET) {
    const auto* in_a = reinterpret_cast<const sockaddr_in*>(&a);
    const auto* in_b = reinterpret_cast<const sockaddr_in*>(&b);
    if (in_a->sin_port != in_b->sin_port) {
      return in_a->sin_port < in_b->sin_port;
    }
    return in_a->sin_addr.s_addr < in_b->sin_addr.s_addr;
  }
  if (sa_a->sa_family == AF_INET6) {
    const auto* in6_a = reinterpret_cast<const sockaddr_in6*>(&a);
    const auto* in6_b = reinterpret_cast<const sockaddr_in6*>(&b);
    if (in6_a->sin6_port != in6_b->sin6_port) {
      return in6_a->sin6_port < in6_b->sin6_port;
    }
    int addr_cmp = std::memcmp(&in6_a->sin6_addr, &in6_b->sin6_addr, sizeof(in6_addr));
    if (addr_cmp != 0) {
      return addr_cmp < 0;
    }
    return in6_a->sin6_scope_id < in6_b->sin6_scope_id;
  }
  return false;
}
