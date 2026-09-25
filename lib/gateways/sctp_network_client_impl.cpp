// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "sctp_network_client_impl.h"
#include "sctp_dtls_ssl.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/io/sockets.h"
#include <algorithm>
#include <netinet/sctp.h>
#include <signal.h>

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
    ssl_enabled(parent_.ssl_enabled),
    ssl(parent_.ssl),
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
    span<const uint8_t> pdu_span   = to_span(sdu, send_buffer);
    int                 bytes_sent = -1;
    if (not ssl_enabled) {
      auto dest_addr = server_addr.native();
      bytes_sent     = ::sctp_sendmsg(fd,
                                  pdu_span.data(),
                                  pdu_span.size(),
                                  const_cast<struct sockaddr*>(dest_addr.addr),
                                  dest_addr.addrlen,
                                  htonl(ppid),
                                  0,
                                  stream_no,
                                  0,
                                  0);
    } else {
      ocudu_assert(ssl, "Trying to send a PDU with SSL enabled, but no SSL association is present");
      bytes_sent = ssl->write(pdu_span);
    }
    if (bytes_sent < 0) {
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

    // Shutdown DTLS connection if enabled.
    if (ssl_enabled) {
      ssl->shutdown();
      logger.debug("{}: called shutdown for DTLS association", client_name);
    } else {
      logger.debug("{}: did not call shutdown for DTLS association", client_name);
    }

    int ret = ::shutdown(fd, SHUT_RDWR);

    if (ret == -1) {
      // Failed to send EOF.
      // Note: It may happen when the sender notifier is removed just before the SCTP shutdown event is handled in
      // the server recv thread.
      logger.info("{}: Couldn't send EOF during shut down (errno=\"{}\")", client_name, ::strerror(errno));
    } else {
      logger.debug("{}: called shutdown to SCTP client to close SCTP association", client_name);
    }

    // Signal sender closed the channel.
    closed_flag->store(true, std::memory_order_relaxed);
  }

  const std::string             client_name;
  const uint32_t                ppid;
  int                           fd;
  ocudulog::basic_logger&       logger;
  const transport_layer_address server_addr;
  bool                          ssl_enabled;
  std::unique_ptr<dtls_ssl>&    ssl;

  std::array<uint8_t, network_gateway_sctp_max_len> send_buffer;

  std::shared_ptr<std::atomic<bool>> closed_flag;
};

sctp_network_client_impl::sctp_network_client_impl(const sctp_network_connector_config& sctp_cfg,
                                                   io_broker&                           broker_,
                                                   task_executor&                       io_rx_executor_) :
  sctp_network_gateway_common_impl(sctp_cfg),
  client_cfg(sctp_cfg),
  broker(broker_),
  io_rx_executor(io_rx_executor_),
  ssl_enabled(sctp_cfg.dtls_cfg.has_value())
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
    int ret = ::shutdown(socket.fd().value(), SHUT_RDWR);

    if (ret == -1) {
      // Failed to send EOF.
      // Note: It may happen when the sender notifier is removed just before the SCTP shutdown event is handled in
      // the server recv thread.
      logger.info("{}: Couldn't send EOF during shut down (errno=\"{}\")", node_cfg.if_name, ::strerror(errno));
    } else {
      logger.debug("{}: called shutdown to SCTP client to close SCTP association", node_cfg.if_name);
    }
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
      if (not create_and_bind_common(SOCK_STREAM)) {
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
    expected<sctp_socket> outcome       = create_socket(socket_family, SOCK_STREAM);
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

  /// Socket has been created, initialize DTLS context.
  if (OCUDU_DTLS_SCTP_SUPPORT and node_cfg.dtls_cfg.has_value()) {
    dtls_ctxt = create_dtls_context(*node_cfg.dtls_cfg);
    if (not dtls_ctxt->init(socket.fd().value())) {
      report_error("Could not initialize DTLS context in SCTP gateway. if={}", node_cfg.if_name);
    }
    logger.debug("Created DTLS context. if={} cert={}", node_cfg.if_name, node_cfg.dtls_cfg->cert_filename);
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
    handle_connect_failure(fmt::format("error=\"{}\" timeout={}ms", cause, now_ms.count()));
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
    handle_connect_failure("Failed to get peer addresses.");
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

    // Report how long the connection took to establish, if it was retried.
    const std::string retry_info = nof_consecutive_connect_failures == 0
                                       ? std::string{}
                                       : fmt::format(" after {} failed attempt{}",
                                                     nof_consecutive_connect_failures,
                                                     nof_consecutive_connect_failures == 1 ? "" : "s");

    // fmt::format of fmt::join view is required before passing to the logger, otherwise TSAN may report use-after-free.
    logger.info("{}: SCTP connection to {} established{}. Configured: [{}]:{}, established: [{}]",
                node_cfg.if_name,
                client_cfg.dest_name,
                retry_info,
                fmt::format("{}", fmt::join(client_cfg.connect_addresses, ", ")),
                client_cfg.connect_port,
                fmt::format("{}", fmt::join(established_addrs, ", ")));
  }

  dtls_connect();

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
  nof_consecutive_connect_failures = 0;

  return std::make_unique<sctp_send_notifier>(*this, peer_addrs[0]);
}

void sctp_network_client_impl::handle_connect_failure(const std::string& cause)
{
  // fmt::format of the fmt::join view is required before passing it to the logger, otherwise TSAN may report a
  // use-after-free.
  const std::string msg = fmt::format("{}: Failed to connect to {} on [{}]:{}. {}",
                                      node_cfg.if_name,
                                      client_cfg.dest_name,
                                      fmt::format("{}", fmt::join(client_cfg.connect_addresses, ", ")),
                                      client_cfg.connect_port,
                                      cause);

  ++nof_consecutive_connect_failures;

  if (nof_consecutive_connect_failures == 1) {
    // First failure of this outage. Announce it in STDOUT as well, so that it is not missed. A failure that is not
    // retried is final, so it is reported as an error.
    fmt::print("{}\n", msg);
    if (client_cfg.connection_is_retried) {
      logger.warning("{}", msg);
    } else {
      logger.error("{}", msg);
    }
    return;
  }

  // The connection is being retried. Only report once every period, so that the log is not flooded but a lasting
  // outage stays visible.
  if (nof_consecutive_connect_failures % connect_failure_log_period == 0) {
    logger.warning("{} Attempt {}.", msg, nof_consecutive_connect_failures);
    return;
  }

  logger.debug("{}", msg);
}

void sctp_network_client_impl::receive()
{
  if (not node_cfg.dtls_cfg.has_value() || shutdown_received->load()) {
    receive_plain();
  } else {
    receive_dtls();
  }
}

void sctp_network_client_impl::receive_plain()
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

  if (rx_bytes == 0) {
    logger.debug("{}: Received EOF. Terminating association", node_cfg.if_name);
    handle_connection_terminated("Received SCTP EOF");
    ocudulog::flush();
    return;
  }

  span<const uint8_t> payload(temp_recv_buffer.data(), rx_bytes);
  if (msg_flags & MSG_NOTIFICATION) {
    handle_notification(payload);
  } else {
    handle_data(payload);
  }
}

void sctp_network_client_impl::receive_dtls()
{
  if (ssl == nullptr) {
    return;
  }
  if (not ssl->is_init_finished()) {
    /// Client is used in blocking mode. As such initialization must be finished by now.
    return;
  }

  expected<byte_buffer, dtls_ssl_read_error> plain = ssl->receive();
  if (not plain.has_value()) {
    logger.warning("DTLS receive returned error. err={}", plain.error());
    return;
  }

  recv_handler->on_new_sdu(std::move(plain.value()));
}

void sctp_network_client_impl::handle_dtls_notification(const union sctp_notification* notif, int assoc)
{
  logger.debug("{}: received SCTP notification from DTLS association. type={}",
               node_cfg.if_name,
               static_cast<sctp_sn_type>(notif->sn_header.sn_type));

  if (notif->sn_header.sn_length > sizeof(sctp_notification)) {
    logger.error("{}: received SCTP notification with larger length then allowed. type={} len={}",
                 node_cfg.if_name,
                 static_cast<sctp_sn_type>(notif->sn_header.sn_type),
                 notif->sn_header.sn_length);
    return;
  }

  // Copy notification from DTLS buffers to our own.
  std::vector<uint8_t> payload;
  payload.resize(notif->sn_header.sn_length);
  memcpy(payload.data(), notif, payload.size());

  handle_notification(payload);
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

void sctp_network_client_impl::dtls_connect()
{
  if (ssl_enabled) {
    ssl = create_dtls_ssl(dtls_ssl_config{dtls_mode::client, 0}, {*dtls_ctxt, *this});
    if (not ssl->init(socket.fd().value())) {
      logger.error("{} assoc={}: Could not initialize DTLS context for new association", node_cfg.if_name, 0);
      /// Remove association as if it was lost. Do it directly, as we are running in the app executor already.
      handle_connection_shutdown("DTLS initialization error");
      return;
    }
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

void sctp_network_client_impl::handle_notification(span<const uint8_t> payload)
{
  const auto* notif = reinterpret_cast<const union sctp_notification*>(payload.data());
  if (not validate_and_log_sctp_notification(payload)) {
    // Handle error.
    handle_connection_terminated("Received invalid message");
    return;
  }

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
