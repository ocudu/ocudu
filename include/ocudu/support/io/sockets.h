// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"
#include <chrono>
#include <netdb.h>
#include <sys/socket.h>

namespace ocudu {

class unique_fd;

/// Get the IP address and port from sockaddr structure.
bool getnameinfo(const struct sockaddr&        ai_addr,
                 const socklen_t&              ai_addrlen,
                 std::array<char, NI_MAXHOST>& ip_address,
                 int&                          port);

/// Get the IP address and port from sockaddr structure.
struct socket_name_info {
  std::string address;
  int         port;
};
[[gnu::pure]] socket_name_info get_nameinfo(const struct sockaddr& ai_addr, const socklen_t& ai_addrlen);

bool set_reuse_addr(const unique_fd& fd, ocudulog::basic_logger& logger);

bool bind_to_interface(const unique_fd& fd, const std::string& interface, ocudulog::basic_logger& logger);

bool sockaddr_to_ip_str(const sockaddr* addr, std::string& ip_address, ocudulog::basic_logger& logger);

uint16_t sockaddr_to_port(const sockaddr* addr, ocudulog::basic_logger& logger);

bool sockaddr_storage_equal(const sockaddr_storage& a, const sockaddr_storage& b);

struct sockaddr_storage_less {
  bool operator()(const sockaddr_storage& a, const sockaddr_storage& b) const;
};

/// Set a receive timeout for a socket.
bool set_receive_timeout(const unique_fd& fd, std::chrono::seconds rx_timeout, ocudulog::basic_logger& logger);

/// Set a socket as non-blocking.
bool set_non_blocking(const unique_fd& fd, ocudulog::basic_logger& logger);

std::string sock_type_to_str(int type);

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::socket_name_info> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::socket_name_info& val, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}:{}", val.address, val.port);
  }
};

} // namespace fmt
