// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "sctp_network_client_impl.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/io/sockets.h"
#include <algorithm>
#include <netinet/sctp.h>

using namespace ocudu;

/// Stream number to use for sending.
static constexpr unsigned stream_no = 0;

class sctp_network_client_impl::sctp_send_notifier final : public sctp_association_sdu_notifier
{
public:
  sctp_send_notifier(sctp_network_client_impl& parent_, const transport_layer_address& server_addr_) :
    client_name(parent_.node_cfg.if_name),
    ppid(parent_.node_cfg.ppid),
    fd(parent_.socket.fd().value()),
    logger(parent_.logger),
    server_addr(server_addr_),
    closed_flag(parent_.shutdown_received)
  {
  }
  ~sctp_send_notifier() override { close(); }

  bool on_new_sdu(byte_buffer sdu) override
  {
    if (closed_flag->load(std::memory_order_relaxed)) {
      // Already closed.
      return false;
    }

    if (sdu.length() > network_gateway_sctp_max_len) {
      logger.error("PDU of {} bytes exceeds maximum length of {} bytes", sdu.length(), network_gateway_sctp_max_len);
      return false;
    }
    logger.debug("{}: Sending PDU of {} bytes", client_name, sdu.length());

    // Note: each sender needs its own buffer to avoid race conditions with the recv.
    span<const uint8_t> pdu_span = to_span(sdu, send_buffer);

    auto dest_addr  = server_addr.native();
    int  bytes_sent = ::sctp_sendmsg(fd,
                                    pdu_span.data(),
                                    pdu_span.size(),
                                    const_cast<struct sockaddr*>(dest_addr.addr),
                                    dest_addr.addrlen,
                                    htonl(ppid),
                                    0,
                                    stream_no,
                                    0,
                                    0);
    if (bytes_sent == -1) {
      logger.error("{}: Closing SCTP association. Cause: Couldn't send {} B of data. errno={}",
                   client_name,
                   pdu_span.size_bytes(),
                   ::strerror(errno));
      close();
      return false;
    }
    return true;
  }

private:
  // Closes the SCTP association, sending an EOF, when required.
  void close()
  {
    if (closed_flag->load(std::memory_order_relaxed)) {
      // Already closed.
      return;
    }

    // Send EOF to SCTP server.
    auto dest_addr  = server_addr.native();
    int  bytes_sent = ::sctp_sendmsg(fd,
                                    nullptr,
                                    0,
                                    const_cast<struct sockaddr*>(dest_addr.addr),
                                    dest_addr.addrlen,
                                    htonl(ppid),
                                    SCTP_EOF,
                                    stream_no,
                                    0,
                                    0);

    if (bytes_sent == -1) {
      // Failed to send EOF.
      // Note: It may happen when the sender notifier is removed just before the SCTP shutdown event is handled in
      // the server recv thread.
      logger.info("{}: Couldn't send EOF during shut down (errno=\"{}\")", client_name, ::strerror(errno));
    } else {
      logger.debug("{}: Sent EOF to SCTP client and closed SCTP association", client_name);
    }

    // Signal sender closed the channel.
    closed_flag->store(true, std::memory_order_relaxed);
  }

  const std::string             client_name;
  const uint32_t                ppid;
  int                           fd;
  ocudulog::basic_logger&       logger;
  const transport_layer_address server_addr;

  std::array<uint8_t, network_gateway_sctp_max_len> send_buffer;

  std::shared_ptr<std::atomic<bool>> closed_flag;
};

sctp_network_client_impl::sctp_network_client_impl(const sctp_network_connector_config& sctp_cfg,
                                                   io_broker&                           broker_,
                                                   task_executor&                       io_rx_executor_) :
  sctp_network_gateway_common_impl(sctp_cfg), client_cfg(sctp_cfg), broker(broker_), io_rx_executor(io_rx_executor_)
{
}

sctp_network_client_impl::~sctp_network_client_impl()
{
  logger.debug("{}: Closing...", node_cfg.if_name);

  // If there is a connection on-going.
  bool                    eof_needed = false;
  transport_layer_address server_addr_cpy;
  {
    std::lock_guard<std::mutex> lock(connection_mutex);
    // If there is no connection, return right away.
    if (server_addr.empty()) {
      return;
    }
    eof_needed      = not shutdown_received->exchange(true);
    server_addr_cpy = server_addr;
  }

  // Signal that the upper layer sender should stop sending new SCTP data (including the EOF).
  if (eof_needed) {
    ::sctp_sendmsg(socket.fd().value(),
                   nullptr,
                   0,
                   const_cast<struct sockaddr*>(server_addr_cpy.native().addr),
                   server_addr_cpy.native().addrlen,
                   htonl(node_cfg.ppid),
                   SCTP_EOF,
                   stream_no,
                   0,
                   0);
  }

  // No subscription is on-going. It is now safe to close the socket.
  std::unique_lock<std::mutex> lock(connection_mutex);
  connection_cvar.wait(lock, [this]() { return server_addr.empty(); });
}

std::unique_ptr<sctp_association_sdu_notifier>
sctp_network_client_impl::connect(std::unique_ptr<sctp_association_sdu_notifier> recv_handler_)
{
  {
    std::lock_guard<std::mutex> lock(connection_mutex);
    if (not server_addr.empty()) {
      // If this is not the first connection.
      logger.error("{}: Connection to {}:{} failed. Cause: Connection is already in progress",
                   node_cfg.if_name,
                   client_cfg.connect_addresses[0],
                   client_cfg.connect_port);
      return nullptr;
    }
  }

  // If a bind address is provided, create a socket here and bind it.
  if (not node_cfg.bind_addresses.empty()) {
    if (not node_cfg.bind_addresses[0].empty()) {
      if (not create_and_bind_common()) {
        return nullptr;
      }
    }
  }

  auto start = std::chrono::steady_clock::now();
  // Create SCTP socket only if not created earlier during bind. Otherwise, reuse socket.
  bool reuse_socket = socket.is_open();

  // Resolve all destination addresses, remove duplicates and determine required socket family.
  // If socket was already created during bind, its family is already set and cannot be changed.
  // If it was created as an IPv4 socket, but destination addresses list contains some IPv6 addresses,
  // then those will be filtered out later to avoid "Invalid argument" error with sctp_connectx().
  bool                          has_ipv6_dest_addr = false;
  std::vector<sockaddr_storage> resolved_addrs;

  for (const auto& addr : client_cfg.connect_addresses) {
    sockaddr_searcher searcher{addr, client_cfg.connect_port, logger};

    for (struct addrinfo* result = searcher.next(); result != nullptr; result = searcher.next()) {
      struct sockaddr_storage storage;
      std::memcpy(&storage, result->ai_addr, result->ai_addrlen);
      resolved_addrs.emplace_back(storage);

      if (result->ai_family == AF_INET6) {
        has_ipv6_dest_addr = true;
      }
    }
  }

  std::sort(resolved_addrs.begin(), resolved_addrs.end(), sockaddr_storage_less{});
  auto last = std::unique(resolved_addrs.begin(), resolved_addrs.end(), sockaddr_storage_equal);
  resolved_addrs.erase(last, resolved_addrs.end());

  if (not reuse_socket) {
    // Create SCTP socket only if not created earlier through bind or another connection.
    int                   socket_family = has_ipv6_dest_addr ? AF_INET6 : AF_INET;
    expected<sctp_socket> outcome       = create_socket(socket_family, SOCK_SEQPACKET);
    if (outcome.has_value()) {
      socket = std::move(outcome.value());
    }
  } else {
    // Socket was already created during bind. Filter out addresses that don't match the socket family.
    // An IPv4 socket cannot connect to IPv6 addresses. An IPv6 socket with IPV6_V6ONLY=0 can handle both.
    auto sock_family = socket.get_address_family();
    if (sock_family.has_value() && sock_family.value() == AF_INET) {
      auto orig_size = resolved_addrs.size();
      resolved_addrs.erase(std::remove_if(resolved_addrs.begin(),
                                          resolved_addrs.end(),
                                          [](const sockaddr_storage& addr) { return addr.ss_family == AF_INET6; }),
                           resolved_addrs.end());
      if (resolved_addrs.size() < orig_size) {
        logger.warning("{}: Filtered out {} IPv6 destination address(es) because socket is IPv4-only. "
                       "To enable IPv6, add an IPv6 bind address.",
                       node_cfg.if_name,
                       orig_size - resolved_addrs.size());
      }
      if (resolved_addrs.empty()) {
        logger.error("{}: No compatible destination addresses remain after filtering. "
                     "Socket has only IPv4 bind addresses but all destination addresses are IPv6.",
                     node_cfg.if_name);
        return nullptr;
      }
    }
  }

  sctp_assoc_t assoc_id           = 0;
  bool         connection_success = socket.connectx(resolved_addrs, assoc_id);

  if (not connection_success or assoc_id == 0) {
    if (not reuse_socket) {
      // Connection failed, make sure the just created socket is deleted.
      socket.close();
    }

    auto        end    = std::chrono::steady_clock::now();
    auto        now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::string cause  = ::strerror(errno);
    if (errno == 0) {
      cause = "IO broker could not register socket";
    }
    if (not connect_failure_printed) {
      connect_failure_printed = true;
      fmt::print("{}: Failed to connect to {} on [{}]:{}. error=\"{}\" timeout={}ms\n",
                 node_cfg.if_name,
                 client_cfg.dest_name,
                 fmt::join(client_cfg.connect_addresses, ", "),
                 client_cfg.connect_port,
                 cause,
                 now_ms.count());
    }
    logger.error("{}: Failed to connect to {} on [{}]:{}. error=\"{}\" timeout={}ms",
                 node_cfg.if_name,
                 client_cfg.dest_name,
                 fmt::join(client_cfg.connect_addresses, ", "),
                 client_cfg.connect_port,
                 cause,
                 now_ms.count());
    return nullptr;
  }

  // Create objects representing state of the client.
  recv_handler = std::move(recv_handler_);

  // Determine actual connected peer addresses via sctp_getpaddrs.
  std::vector<transport_layer_address> peer_addrs;
  struct sockaddr*                     paddrs = nullptr;

  int paddr_count = ::sctp_getpaddrs(socket.fd().value(), assoc_id, &paddrs);
  if (paddr_count > 0 && paddrs != nullptr) {
    const sockaddr* current = paddrs;
    for (int i = 0; i < paddr_count; ++i) {
      if (current->sa_family != AF_INET && current->sa_family != AF_INET6) {
        break;
      }
      socklen_t peer_len = (current->sa_family == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
      peer_addrs.push_back(transport_layer_address::create_from_sockaddr(*current, peer_len));
      current = reinterpret_cast<const sockaddr*>(reinterpret_cast<const uint8_t*>(current) + peer_len);
    }
    ::sctp_freepaddrs(paddrs);
  }

  if (peer_addrs.empty()) {
    if (not connect_failure_printed) {
      connect_failure_printed = true;
      fmt::print("{}: Failed to connect to {} on [{}]:{}. Failed to get peer addresses.\n",
                 node_cfg.if_name,
                 client_cfg.dest_name,
                 fmt::join(client_cfg.connect_addresses, ", "),
                 client_cfg.connect_port);
    }
    logger.error("{}: Failed to connect to {} on [{}]:{}. Failed to get peer addresses.",
                 node_cfg.if_name,
                 client_cfg.dest_name,
                 fmt::join(client_cfg.connect_addresses, ", "),
                 client_cfg.connect_port);
    return nullptr;
  }

  {
    // Save server address and create a shutdown context flag.
    std::unique_lock<std::mutex> lock(connection_mutex);
    server_addr       = peer_addrs[0];
    shutdown_received = std::make_shared<std::atomic<bool>>(false);
  }

  // Log configured and established addresses.
  {
    std::vector<std::string> established_addrs;
    established_addrs.reserve(peer_addrs.size());
    for (const auto& peer_addr : peer_addrs) {
      established_addrs.push_back(peer_addr.to_string());
    }

    // fmt::format of fmt::join view is required before passing to the logger, otherwise TSAN may report use-after-free.
    logger.info("{}: SCTP connection to {} established. Configured: [{}]:{}, established: [{}]",
                node_cfg.if_name,
                client_cfg.dest_name,
                fmt::format("{}", fmt::join(client_cfg.connect_addresses, ", ")),
                client_cfg.connect_port,
                fmt::format("{}", fmt::join(established_addrs, ", ")));
  }

  // Register the socket in the IO broker.
  socket.release();
  io_sub = broker.register_fd(
      unique_fd(socket.fd().value()),
      io_rx_executor,
      [this]() { receive(); },
      [this](io_broker::error_code code) {
        std::string cause = fmt::format("IO error code={}", (int)code);
        handle_connection_terminated(cause);
      });
  if (not io_sub.registered()) {
    // IO subscription failed.
    {
      std::unique_lock<std::mutex> lock(connection_mutex);
      server_addr = {};
      shutdown_received.reset();
    }
    if (not reuse_socket) {
      socket.close();
    }
    recv_handler.reset();
    return nullptr;
  }

  // The connection is up, so a failure of a future one is worth announcing again.
  connect_failure_printed = false;

  return std::make_unique<sctp_send_notifier>(*this, peer_addrs[0]);
}

void sctp_network_client_impl::receive()
{
  struct sctp_sndrcvinfo                            sri       = {};
  int                                               msg_flags = 0;
  std::array<uint8_t, network_gateway_sctp_max_len> temp_recv_buffer;

  // fromlen is an in/out variable in sctp_recvmsg.
  sockaddr_storage msg_src_addr;
  socklen_t        msg_src_addrlen = sizeof(msg_src_addr);

  int rx_bytes = ::sctp_recvmsg(socket.fd().value(),
                                temp_recv_buffer.data(),
                                temp_recv_buffer.size(),
                                reinterpret_cast<sockaddr*>(&msg_src_addr),
                                &msg_src_addrlen,
                                &sri,
                                &msg_flags);

  // Handle error.
  if (rx_bytes == -1) {
    if (errno != EAGAIN) {
      std::string cause = fmt::format("Error reading from SCTP socket: {}", ::strerror(errno));
      handle_connection_terminated(cause);
    } else {
      if (!node_cfg.non_blocking_mode) {
        logger.debug("{}: Socket timeout reached", node_cfg.if_name);
      }
    }
    return;
  }

  span<const uint8_t> payload(temp_recv_buffer.data(), rx_bytes);
  if (msg_flags & MSG_NOTIFICATION) {
    handle_notification(payload, sri, *reinterpret_cast<const sockaddr*>(&msg_src_addr), msg_src_addrlen);
  } else {
    handle_data(payload);
  }
}

void sctp_network_client_impl::handle_connection_shutdown(const char* cause)
{
  // Signal that the upper layer sender should stop sending new SCTP data (including the EOF, which would fail
  // anyway).
  bool prev = shutdown_received->exchange(true);

  if (not prev and cause != nullptr) {
    // The SCTP sender (the upper layers) didn't yet close the connection.
    logger.info("{}: SCTP connection was shut down. Cause: {}", node_cfg.if_name, cause);
  }
}

void sctp_network_client_impl::handle_connection_terminated(const std::string& cause)
{
  logger.info("{}: {}. Notifying connection drop to upper layers", node_cfg.if_name, cause);

  // Notify SCTP sender that there is no need to send EOF.
  shutdown_received->store(true);

  // Notify the connection drop to the upper layers.
  recv_handler.reset();

  // Unsubscribe from listening to new IO events.
  io_sub.reset();

  // Erase server_addr and notify dtor that connection is closed.
  std::unique_lock<std::mutex> lock(connection_mutex);
  server_addr       = {};
  shutdown_received = nullptr;
  connection_cvar.notify_one();
}

void sctp_network_client_impl::handle_data(span<const uint8_t> payload)
{
  logger.debug("{}: Received {} bytes", node_cfg.if_name, payload.size());

  // Note: For SCTP, we avoid byte buffer allocation failures by resorting to fallback allocation.
  recv_handler->on_new_sdu(byte_buffer{byte_buffer::fallback_allocation_tag{}, payload});
}

void sctp_network_client_impl::handle_notification(span<const uint8_t>           payload,
                                                   const struct sctp_sndrcvinfo& sri,
                                                   const sockaddr&               src_addr,
                                                   socklen_t                     src_addr_len)
{
  if (not validate_and_log_sctp_notification(payload)) {
    // Handle error.
    handle_connection_terminated("Received invalid message");
    return;
  }

  const auto* notif = reinterpret_cast<const union sctp_notification*>(payload.data());
  switch (notif->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE: {
      const struct sctp_assoc_change* n = &notif->sn_assoc_change;
      switch (n->sac_state) {
        case SCTP_COMM_UP:
          break;
        case SCTP_COMM_LOST:
          handle_connection_terminated("Communication to the server was lost");
          break;
        case SCTP_SHUTDOWN_COMP:
          handle_connection_terminated("Received SCTP shutdown completed");
          break;
        case SCTP_CANT_STR_ASSOC:
          handle_connection_terminated("Can't start association");
          break;
        default:
          break;
      }
      break;
    }
    case SCTP_SHUTDOWN_EVENT: {
      handle_connection_shutdown("Server closed SCTP association");
      break;
    }
    default:
      break;
  }
}

std::unique_ptr<sctp_network_client> sctp_network_client_impl::create(const sctp_network_connector_config& sctp_cfg,
                                                                      io_broker&                           broker_,
                                                                      task_executor& io_rx_executor)
{
  // Validate arguments.
  if (sctp_cfg.if_name.empty()) {
    ocudulog::fetch_basic_logger("SCTP-GW").error("Cannot create SCTP client. Cause: No name was provided");
    return nullptr;
  }

  // Create a SCTP client instance.
  std::unique_ptr<sctp_network_client_impl> client{new sctp_network_client_impl(sctp_cfg, broker_, io_rx_executor)};
  return client;
}
