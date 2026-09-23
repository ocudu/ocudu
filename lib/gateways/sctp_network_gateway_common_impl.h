// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/sctp_network_gateway.h"
#include "ocudu/gateways/sctp_socket.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/io/io_broker.h"

struct addrinfo;

namespace ocudu {

/// Helper generator class that traverses a list of SCTP sockaddr param candidates (ipv4, ipv6, hostnames).
class sockaddr_searcher
{
public:
  sockaddr_searcher(const std::string& address, int port, ocudulog::basic_logger& logger);
  sockaddr_searcher(const sockaddr_searcher&) = delete;
  sockaddr_searcher(sockaddr_searcher&&)      = delete;
  ~sockaddr_searcher();

  /// Get next candidate or nullptr of search has ended.
  struct addrinfo* next();

private:
  struct addrinfo* results     = nullptr;
  struct addrinfo* next_result = nullptr;
};

/// This class holds common functionality to the SCTP network server and client implementations.
class sctp_network_gateway_common_impl
{
public:
  sctp_network_gateway_common_impl(const sctp_network_gateway_config& cfg);
  ~sctp_network_gateway_common_impl();

protected:
  static constexpr uint32_t network_gateway_sctp_max_len = 9100;

  // Close socket and unsubscribe it from the io_broker.
  bool close_socket();

  // Creates an SCTP socket with the provided protocol.
  [[nodiscard]] expected<sctp_socket> create_socket(int ai_family, int ai_socktype) const;

  bool create_and_bind_common(int sock_type);

  [[nodiscard]] bool validate_and_log_sctp_notification(span<const uint8_t> payload) const;

  const sctp_network_gateway_config node_cfg;
  ocudulog::basic_logger&           logger;

  sctp_socket socket;

  io_broker::subscriber io_sub;
};

} // namespace ocudu

template <>
struct fmt::formatter<sctp_sn_type> : fmt::formatter<std::string_view> {
  auto format(sctp_sn_type v, fmt::format_context& ctx) const
  {
    std::string_view name = "UNKNOWN";
    switch (v) {
      case SCTP_DATA_IO_EVENT:
        name = "SCTP_DATA_IO_EVENT";
        break;
      case SCTP_ASSOC_CHANGE:
        name = "SCTP_ASSOC_CHANGE";
        break;
      case SCTP_PEER_ADDR_CHANGE:
        name = "SCTP_PEER_ADDR_CHANGE";
        break;
      case SCTP_SEND_FAILED:
        name = "SCTP_SEND_FAILED";
        break;
      case SCTP_REMOTE_ERROR:
        name = "SCTP_REMOTE_ERROR";
        break;
      case SCTP_SHUTDOWN_EVENT:
        name = "SCTP_SHUTDOWN_EVENT";
        break;
      case SCTP_PARTIAL_DELIVERY_EVENT:
        name = "SCTP_PARTIAL_DELIVERY_EVENT";
        break;
      case SCTP_ADAPTATION_INDICATION:
        name = "SCTP_ADAPTATION_INDICATION";
        break;
      case SCTP_AUTHENTICATION_EVENT:
        name = "SCTP_AUTHENTICATION_EVENT";
        break;
      case SCTP_SENDER_DRY_EVENT:
        name = "SCTP_SENDER_DRY_EVENT";
        break;
      case SCTP_STREAM_RESET_EVENT:
        name = "SCTP_STREAM_RESET_EVENT";
        break;
      case SCTP_ASSOC_RESET_EVENT:
        name = "SCTP_ASSOC_RESET_EVENT";
        break;
      case SCTP_STREAM_CHANGE_EVENT:
        name = "SCTP_STREAM_CHANGE_EVENT";
        break;
      case SCTP_SEND_FAILED_EVENT:
        name = "SCTP_SEND_FAILED_EVENT";
        break;
    }
    return fmt::formatter<std::string_view>::format(name, ctx);
  }
};
