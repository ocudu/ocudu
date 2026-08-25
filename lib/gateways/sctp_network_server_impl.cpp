// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "sctp_network_server_impl.h"
#include "sctp_dtls.h"
#include "sctp_dtls_ssl.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/synchronization/sync_event.h"
#include <netinet/sctp.h>

using namespace ocudu;

/// Stream number to use for sending.
static constexpr unsigned stream_no = 0;

class sctp_network_server_impl::sctp_send_notifier : public sctp_association_sdu_notifier
{
public:
  sctp_send_notifier(sctp_network_server_impl&                                parent,
                     const sctp_network_server_impl::sctp_associaton_context& assoc,
                     ocudulog::basic_logger&                                  logger_) :
    ppid(parent.node_cfg.ppid),
    fd(assoc.fd),
    if_name(parent.node_cfg.if_name),
    assoc_id(assoc.assoc_id),
    client_addr(assoc.addr),
    assoc_shutdown_flag(assoc.association_shutdown_received),
    logger(logger_)
  {
  }

  ~sctp_send_notifier() override { close(); }

  bool on_new_sdu(byte_buffer sdu) override
  {
    if (assoc_shutdown_flag->load(std::memory_order_relaxed)) {
      // It has already been released.
      return false;
    }
    if (sdu.length() > network_gateway_sctp_max_len) {
      logger.error("{}: PDU of {} bytes exceeds maximum length of {} bytes",
                   if_name,
                   sdu.length(),
                   network_gateway_sctp_max_len);
      return false;
    }
    logger.debug("{} assoc={}: Sending PDU of {} bytes", if_name, assoc_id, sdu.length());

    span<const uint8_t> pdu_span = to_span(sdu, send_buffer);

    transport_layer_address::native_type dest_addr  = client_addr.native();
    int                                  bytes_sent = ::sctp_sendmsg(fd,
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
      logger.error("{} assoc={}: Closing SCTP association. Cause: Couldn't send {} B of data. errno={}",
                   if_name,
                   assoc_id,
                   pdu_span.size_bytes(),
                   ::strerror(errno));
      close();
      return false;
    }
    return true;
  }

private:
  void close()
  {
    if (assoc_shutdown_flag->load(std::memory_order_relaxed)) {
      // Already closed.
      return;
    }

    // Send EOF to SCTP client.
    transport_layer_address::native_type dest_addr = client_addr.native();
    struct sctp_sndinfo                  sndinfo{};
    sndinfo.snd_sid   = stream_no;
    sndinfo.snd_ppid  = htonl(ppid);
    sndinfo.snd_flags = SCTP_EOF;

    char control[CMSG_SPACE(sizeof(sndinfo))];

    struct iovec iov{};
    iov.iov_base = nullptr;
    iov.iov_len  = 0;

    struct msghdr msg{};
    msg.msg_name    = dest_addr.addr;
    msg.msg_namelen = dest_addr.addrlen;
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    msg.msg_control    = control;
    msg.msg_controllen = sizeof(control);

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level     = IPPROTO_SCTP;
    cmsg->cmsg_type      = SCTP_SNDINFO;
    cmsg->cmsg_len       = CMSG_LEN(sizeof(sndinfo));

    memcpy(CMSG_DATA(cmsg), &sndinfo, sizeof(sndinfo));

    msg.msg_controllen = cmsg->cmsg_len;

    ssize_t bytes_sent = sendmsg(fd, &msg, MSG_NOSIGNAL);

    if (bytes_sent == -1) {
      // Failed to send EOF.
      // Note: It may happen when the sender notifier is removed just before the SCTP shutdown event is handled in
      // the server recv thread.
      logger.info(
          "{} assoc={}: Couldn't send EOF during shut down (errno=\"{}\")", if_name, assoc_id, ::strerror(errno));
    } else {
      logger.debug("{} assoc={}: Sent EOF to SCTP client and closed SCTP association", if_name, assoc_id);
    }

    // Signal sender closed the channel.
    assoc_shutdown_flag->store(true, std::memory_order_relaxed);
  }

  // Note: We copy all the required params by value to avoid race conditions with the server thread.
  const uint32_t                ppid;
  const int                     fd;
  std::string                   if_name;
  const int                     assoc_id;
  const transport_layer_address client_addr;
  // This flag is shared by the server main class and this notifier and is used to signal the association shut down.
  // Note: shared_ptr copy used to avoid the case when the notifier outlives the association.
  std::shared_ptr<std::atomic<bool>> assoc_shutdown_flag;
  ocudulog::basic_logger&            logger;

  // Buffer used to store data to send to client.
  std::array<uint8_t, network_gateway_sctp_max_len> send_buffer;
};

sctp_network_server_impl::sctp_associaton_context::sctp_associaton_context(int                       assoc_id_,
                                                                           int                       fd_,
                                                                           sctp_network_server_impl& parent_) :
  assoc_id(assoc_id_), fd(fd_), parent(parent_)
{
}

void sctp_network_server_impl::sctp_associaton_context::receive()
{
  struct sctp_sndrcvinfo                            sri       = {};
  int                                               msg_flags = 0;
  std::array<uint8_t, network_gateway_sctp_max_len> temp_recv_buffer;

  // fromlen is an in/out variable in sctp_recvmsg.
  sockaddr_storage msg_src_addr;
  socklen_t        msg_src_addrlen = sizeof(msg_src_addr);

  int rx_bytes = ::sctp_recvmsg(fd,
                                temp_recv_buffer.data(),
                                temp_recv_buffer.size(),
                                (struct sockaddr*)&msg_src_addr,
                                &msg_src_addrlen,
                                &sri,
                                &msg_flags);

  if (rx_bytes == -1) {
    if (errno != EAGAIN) {
      parent.logger.error("Error reading from SCTP socket: {}", ::strerror(errno));
      while (not parent.app_exec.defer([this, keepalive = parent.keepalive_token]() {
        if (*keepalive) {
          parent.handle_sctp_comm_lost(assoc_id);
        }
      })) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    } else {
      if (!parent.node_cfg.non_blocking_mode) {
        parent.logger.debug("Socket timeout reached");
      }
    }
    return;
  }

  /// We pass the actual data and association handling back to the parent, to avoid code duplication.
  auto payload = std::vector<uint8_t>(temp_recv_buffer.begin(), temp_recv_buffer.begin() + rx_bytes);
  parent.receive_impl(std::move(payload), sri, msg_flags, msg_src_addr, msg_src_addrlen);
}

sctp_network_server_impl::sctp_network_server_impl(const ocudu::sctp_network_gateway_config& sctp_cfg_,
                                                   io_broker&                                broker_,
                                                   task_executor&                            io_rx_executor_,
                                                   task_executor&                            app_exec_,
                                                   sctp_network_association_factory&         assoc_factory_) :
  sctp_network_gateway_common_impl(sctp_cfg_),
  broker(broker_),
  io_rx_executor(io_rx_executor_),
  app_exec(app_exec_),
  assoc_factory(assoc_factory_),
  keepalive_token(std::make_shared<bool>(true))
{
}

sctp_network_server_impl::~sctp_network_server_impl()
{
  if (*keepalive_token) {
    logger.error("{}: stop() must be called before destroying the SCTP server", node_cfg.if_name);
    report_error("{}: stop() must be called before destroying the SCTP server", node_cfg.if_name);
  }
}

void sctp_network_server_impl::stop()
{
  sync_event ev;
  defer_socket_shutdown(nullptr, ev.get_token());
  ev.wait();
}

bool sctp_network_server_impl::create_and_bind()
{
  if (not this->create_and_bind_common()) {
    return false;
  }
  if (OCUDU_DTLS_SCTP_SUPPORT and dtls_cfg.has_value()) {
    dtls_ctxt = create_dtls_context(*dtls_cfg);
    if (not dtls_ctxt->init(socket.fd().value())) {
      report_error("Could not initialize DTLS context in SCTP gateway. if={}", node_cfg.if_name);
    }
  }
  return true;
}

void sctp_network_server_impl::receive()
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
                                (struct sockaddr*)&msg_src_addr,
                                &msg_src_addrlen,
                                &sri,
                                &msg_flags);

  // Handle error.
  if (rx_bytes == -1) {
    if (errno != EAGAIN) {
      logger.error("Error reading from SCTP socket: {}", ::strerror(errno));
      defer_socket_shutdown(nullptr);
    } else {
      if (!node_cfg.non_blocking_mode) {
        logger.debug("Socket timeout reached");
      }
    }
    return;
  }

  // Defer all processing after sctp_recvmsg to app_exec.
  auto payload = std::vector<uint8_t>(temp_recv_buffer.begin(), temp_recv_buffer.begin() + rx_bytes);
  receive_impl(std::move(payload), sri, msg_flags, msg_src_addr, msg_src_addrlen);
}

void sctp_network_server_impl::receive_impl(std::vector<uint8_t>   payload,
                                            struct sctp_sndrcvinfo sri,
                                            int                    msg_flags,
                                            sockaddr_storage       msg_src_addr,
                                            socklen_t              msg_src_addrlen)
{
  // The task is rebuilt on every retry, so the payload must survive a failed dispatch: hold it behind a shared_ptr
  // instead of moving it into the lambda. Costs one small allocation and never copies the payload bytes.
  auto payload_holder = std::make_shared<const std::vector<uint8_t>>(std::move(payload));
  while (not app_exec.defer(
      [this, keepalive = keepalive_token, payload = payload_holder, msg_flags, sri, msg_src_addr, msg_src_addrlen]() {
        if (!*keepalive) {
          return;
        }
        if (msg_flags & MSG_NOTIFICATION) {
          handle_notification(*payload, sri, reinterpret_cast<const sockaddr&>(msg_src_addr), msg_src_addrlen);
        } else {
          handle_data(sri.sinfo_assoc_id, *payload);
        }
      })) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void sctp_network_server_impl::handle_socket_shutdown(const char* cause)
{
  // Clean up all associations.
  while (not associations.empty()) {
    // TO-DO: send EOF to close association gracefully
    handle_association_shutdown(associations.begin()->first, cause);
    remove_association(associations.begin()->first);
  }

  // Stop handling new SCTP events.
  io_sub.reset();
}

void sctp_network_server_impl::defer_socket_shutdown(const char* cause, const std::optional<scoped_sync_token>& token)
{
  // Capture the token by copy rather than by move: defer() consumes the task on failure, so a moved token would be
  // gone on the next retry. scoped_sync_token is reference counted, so each attempt just bumps the count.
  while (not app_exec.defer([this, keepalive = keepalive_token, token]() {
    if (*keepalive) {
      *keepalive = false;
      handle_socket_shutdown(nullptr);
    }
  })) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void sctp_network_server_impl::handle_data(int assoc_id, span<const uint8_t> payload)
{
  auto assoc_it = associations.find(assoc_id);
  if (assoc_it == associations.end()) {
    logger.error("{} assoc={}: Received data on unknown SCTP association", node_cfg.if_name, assoc_id);
    return;
  }

  logger.debug("{} assoc={}: Received {} bytes", node_cfg.if_name, assoc_id, payload.size());

  // Note: For SCTP, we avoid byte buffer allocation failures by resorting to fallback allocation.
  assoc_it->second.sctp_data_recv_notifier->on_new_sdu(byte_buffer{byte_buffer::fallback_allocation_tag{}, payload});
}

async_task<bool> sctp_network_server_impl::connect(std::vector<transport_layer_address> dest_addrs)
{
  return launch_async([this,
                       dest_addrs            = std::move(dest_addrs),
                       pending_it            = pending_connects.end(),
                       sctp_connectx_success = false,
                       result                = false](coro_context<async_task<bool>>& ctx) mutable {
    CORO_BEGIN(ctx);

    if (dest_addrs.empty()) {
      logger.error("{}: Cannot initiate SCTP connection with empty destination address list", node_cfg.if_name);
      CORO_EARLY_RETURN(false);
    }

    // fmt::format of fmt::join view is required before passing to the logger, otherwise TSAN may report
    // use-after-free.
    logger.info(
        "{}: Initiating SCTP connection to [{}]", node_cfg.if_name, fmt::format("{}", fmt::join(dest_addrs, ", ")));

    // Reject if any of the requested addresses already has a pending connect in flight.
    for (const auto& dest_addr : dest_addrs) {
      if (std::any_of(pending_connects.begin(), pending_connects.end(), [&dest_addr](const pending_connect& pending) {
            return pending.contains(dest_addr);
          })) {
        logger.warning("{}: Connection overlapping with {} already in progress, rejecting duplicate connect",
                       node_cfg.if_name,
                       dest_addr);
        CORO_EARLY_RETURN(false);
      }
    }

    // Insert pending connect entry with default constructed manual_event, so the SCTP_COMM_UP / SCTP_CANT_STR_ASSOC
    // handlers can find it as soon as sctp_connectx() returns.
    pending_it             = pending_connects.emplace(pending_connects.end());
    pending_it->dest_addrs = dest_addrs;

    {
      // Pack addresses into a contiguous buffer using each address's actual sockaddr size (sockaddr_in vs
      // sockaddr_in6), as required by sctp_connectx().
      std::vector<uint8_t> packed_addrs;
      size_t               total_size = 0;
      for (const auto& addr : dest_addrs) {
        total_size += addr.native().addrlen;
      }
      packed_addrs.resize(total_size);
      size_t offset = 0;
      for (const auto& addr : dest_addrs) {
        transport_layer_address::native_type native_addr = addr.native();
        std::memcpy(packed_addrs.data() + offset, native_addr.addr, native_addr.addrlen);
        offset += native_addr.addrlen;
      }

      int ret = ::sctp_connectx(get_socket_fd(),
                                reinterpret_cast<sockaddr*>(packed_addrs.data()),
                                static_cast<int>(dest_addrs.size()),
                                nullptr);
      if (ret == -1 && errno != EINPROGRESS) {
        // fmt::format of fmt::join view is required, otherwise TSAN may report use-after-free.
        logger.error("{}: sctp_connectx to [{}] failed. errno={}",
                     node_cfg.if_name,
                     fmt::format("{}", fmt::join(dest_addrs, ", ")),
                     ::strerror(errno));
      } else {
        sctp_connectx_success = true;
      }
    }

    if (not sctp_connectx_success) {
      // Clean up the event from pending_connects on sctp_connectx() immediate failure.
      pending_connects.erase(pending_it);
      CORO_EARLY_RETURN(false);
    }

    // Wait for SCTP_COMM_UP or SCTP_CANT_STR_ASSOC.
    CORO_AWAIT_VALUE(result, pending_it->event);

    // Clean up the event from pending_connects after awaited SCTP notification received.
    pending_connects.erase(pending_it);

    CORO_RETURN(result);
  });
}

void sctp_network_server_impl::handle_notification(span<const uint8_t>           payload,
                                                   const struct sctp_sndrcvinfo& sri,
                                                   const sockaddr&               src_addr,
                                                   socklen_t                     src_addr_len)
{
  if (not validate_and_log_sctp_notification(payload)) {
    // Handle error.
    handle_association_shutdown(sri.sinfo_assoc_id, "The received message is invalid");
    return;
  }

  const auto* notif = reinterpret_cast<const union sctp_notification*>(payload.data());
  switch (notif->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE: {
      const struct sctp_assoc_change* n = &notif->sn_assoc_change;
      switch (n->sac_state) {
        case SCTP_COMM_UP:
          handle_sctp_comm_up(*n, src_addr, src_addr_len);
          break;
        case SCTP_COMM_LOST:
          handle_sctp_comm_lost(n->sac_assoc_id);
          break;
        case SCTP_CANT_STR_ASSOC:
          handle_cannot_start_association(n->sac_assoc_id, src_addr, src_addr_len);
          break;
        case SCTP_SHUTDOWN_COMP:
          handle_sctp_shutdown_comp(n->sac_assoc_id);
          break;
        default:
          break;
      }
      break;
    }
    case SCTP_SHUTDOWN_EVENT: {
      const struct sctp_shutdown_event* n = &notif->sn_shutdown_event;
      handle_association_shutdown(n->sse_assoc_id, "Client requested the shutdown");
      break;
    }
    default:
      break;
  }
}

void sctp_network_server_impl::handle_sctp_comm_up(const struct sctp_assoc_change& assoc_change,
                                                   const sockaddr&                 src_addr,
                                                   socklen_t                       src_addr_len)
{
  int  assoc_id = assoc_change.sac_assoc_id;
  auto it       = associations.find(assoc_id);
  if (it != associations.end()) {
    logger.warning("{} assoc={}: SCTP COMM UP received but association already existed", node_cfg.if_name, assoc_id);
    return;
  }

  /// Peel-off a socket. This is required to support DTLS.
  int assoc_fd_raw = sctp_peeloff(socket.fd().value(), assoc_id);
  if (assoc_fd_raw == -1) {
    logger.error(
        "{} assoc={}: Could not peel off new association. err={}", node_cfg.if_name, assoc_id, ::strerror(errno));
    /// Remove association as if it was lost. Do it directly, as we are running in the app excutor already.
    handle_association_shutdown(assoc_id, "Peel-off error");
    remove_association(assoc_id);
    return;
  }
  auto assoc_fd = unique_fd(assoc_fd_raw);

  /// Make sure peeled-off socket follows the blocking mode of the parent.
  if (node_cfg.non_blocking_mode) {
    ::set_non_blocking(assoc_fd, logger);
  }

  // Add an entry for the association in the lookup
  auto result = associations.emplace(
      std::piecewise_construct, std::forward_as_tuple(assoc_id), std::forward_as_tuple(assoc_id, assoc_fd_raw, *this));
  if (not result.second) {
    logger.error("{} assoc={}: Unable to create new SCTP association", node_cfg.if_name, assoc_id);
    return;
  }

  // Fill the association context.
  sctp_associaton_context& assoc_ctxt      = result.first->second;
  assoc_ctxt.addr                          = transport_layer_address::create_from_sockaddr(src_addr, src_addr_len);
  assoc_ctxt.association_shutdown_received = std::make_shared<std::atomic<bool>>(false);
  assoc_ctxt.sctp_data_recv_notifier =
      assoc_factory.create(std::make_unique<sctp_send_notifier>(*this, assoc_ctxt, logger),
                           sctp_association_info{assoc_ctxt.assoc_id, assoc_ctxt.addr});
  if (assoc_ctxt.sctp_data_recv_notifier == nullptr) {
    associations.erase(assoc_id);
    logger.error("{} assoc={} client={}: Unable to create a new SCTP association handler",
                 node_cfg.if_name,
                 assoc_id,
                 assoc_ctxt.addr);
    return;
  }

  /// TODO create DTLS SSL association.
  if (dtls_cfg.has_value()) {
    assoc_ctxt.ssl = create_dtls_ssl(dtls_ssl_config{dtls_cfg->mode}, {*dtls_ctxt});
  }

  logger.info("{} assoc={}: New client SCTP association (client_addr={})", node_cfg.if_name, assoc_id, assoc_ctxt.addr);

  // If this was a pending outgoing connection, defer to enqueue the success signal so that any tasks enqueued by the
  // assoc_factory.create() callback can run before the awaiting coroutine resumes.
  // Signaling inline here would resume the coroutine within this task, before the enqueued tasks that connect the
  // notifiers have a chance to finish.
  // unique_fd is move-only and defer() consumes the task on failure, so moving the fd into the lambda would close it
  // on the first failed dispatch and leave later retries subscribing an already closed socket. Hold it behind a
  // shared_ptr so ownership survives until an attempt is actually enqueued.
  auto fd_holder = std::make_shared<unique_fd>(std::move(assoc_fd));
  while (not app_exec.defer([this, addr = assoc_ctxt.addr, fd_holder, &assoc_ctxt]() mutable {
    auto pending_it = std::find_if(pending_connects.begin(),
                                   pending_connects.end(),
                                   [&addr](const pending_connect& pending) { return pending.contains(addr); });

    /// If DTLS is not configured, mark connection as complete. Otherwise, wait for the DTLS handshake before signaling
    /// the connection is set up to upper layers.
    if (pending_it != pending_connects.end() && !dtls_cfg.has_value()) {
      pending_it->event.set(true);
    }
    /// Register peeled-off socket in IO broker.
    if (not subscribe_association_to_broker(std::move(*fd_holder), assoc_ctxt)) {
      logger.error("Connection loss due to IO broker subscription failure");
      handle_association_shutdown(assoc_ctxt.assoc_id, "IO broker error");
      remove_association(assoc_ctxt.assoc_id);
      return;
    }
  })) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void sctp_network_server_impl::handle_cannot_start_association(int             assoc_id,
                                                               const sockaddr& src_addr,
                                                               socklen_t       src_addr_len)
{
  transport_layer_address addr = transport_layer_address::create_from_sockaddr(src_addr, src_addr_len);

  logger.info("{} assoc={}: SCTP association could not start (peer_addr={})", node_cfg.if_name, assoc_id, addr);

  // Signal pending connect failure.
  auto pending_it = std::find_if(pending_connects.begin(),
                                 pending_connects.end(),
                                 [&addr](const pending_connect& pending) { return pending.contains(addr); });
  if (pending_it != pending_connects.end()) {
    pending_it->event.set(false);
  } else {
    logger.warning("{}: SCTP_CANT_STR_ASSOC for unknown peer {}", node_cfg.if_name, addr);
  }
}

void sctp_network_server_impl::handle_association_shutdown(int assoc_id, const char* cause)
{
  auto assoc_it = associations.find(assoc_id);
  if (assoc_it == associations.end()) {
    logger.error("{} assoc={}: Failed to shutdown SCTP association. Cause: SCTP association Id not found",
                 node_cfg.if_name,
                 assoc_id);
    return;
  }

  // The business domain or the peer side wishes to close the association.
  // Signal that the business domain should stop sending new SCTP data (including the EOF, which would fail anyway).
  bool prev = assoc_it->second.association_shutdown_received->exchange(true);
  if (not prev and cause != nullptr) {
    // The association sender didn't yet close the connection and the association was closed by the peer side.
    logger.info("{} assoc={}: SCTP association was shut down (client_addr={}). Cause: {}",
                node_cfg.if_name,
                assoc_it->first,
                assoc_it->second.addr,
                cause);
  }
}

void sctp_network_server_impl::handle_sctp_shutdown_comp(int assoc_id)
{
  remove_association(assoc_id);
}

void sctp_network_server_impl::handle_sctp_comm_lost(int assoc_id)
{
  handle_association_shutdown(assoc_id, "SCTP_COMM_LOST");
  remove_association(assoc_id);
}

void sctp_network_server_impl::remove_association(int assoc_id)
{
  auto assoc_it = associations.find(assoc_id);
  if (assoc_it == associations.end()) {
    logger.error("{} assoc={}: Failed to remove SCTP association. Cause: SCTP association Id not found",
                 node_cfg.if_name,
                 assoc_id);
    return;
  }

  // Remove association.
  // Note: Deleting the recv notifier should trigger the deletion of the sender interface.
  associations.erase(assoc_it);
}

bool sctp_network_server_impl::listen()
{
  if (node_cfg.bind_addresses.empty()) {
    if (node_cfg.bind_addresses[0].empty()) {
      logger.error("{}: Cannot listen to new SCTP associations if an address to bind to is not provided",
                   node_cfg.if_name);
      return false;
    }
  }

  if (not socket.listen()) {
    return false;
  }

  if (not subscribe_to_broker()) {
    return false;
  }

  return true;
}

std::optional<uint16_t> sctp_network_server_impl::get_listen_port()
{
  return socket.get_bound_port();
}

bool sctp_network_server_impl::subscribe_to_broker()
{
  socket.release();
  io_sub = broker.register_fd(
      unique_fd(socket.fd().value()),
      io_rx_executor,
      [this]() { receive(); },
      [this](io_broker::error_code code) {
        logger.info("Connection loss due to IO error code={}.", (int)code);
        defer_socket_shutdown(nullptr);
      });
  return io_sub.registered();
}

bool sctp_network_server_impl::subscribe_association_to_broker(unique_fd assoc_fd, sctp_associaton_context& assoc_ctxt)
{
  assoc_ctxt.io_sub = broker.register_fd(
      std::move(assoc_fd),
      io_rx_executor,
      [&assoc_ctxt]() { assoc_ctxt.receive(); },
      [this](io_broker::error_code code) {
        logger.info("Connection loss due to IO error code={}.", (int)code);
        defer_socket_shutdown(nullptr);
      });
  return assoc_ctxt.io_sub.registered();
}

std::unique_ptr<sctp_network_server> sctp_network_server_impl::create(const sctp_network_gateway_config& sctp_cfg,
                                                                      io_broker&                         broker_,
                                                                      task_executor&                    io_rx_executor_,
                                                                      task_executor&                    app_exec_,
                                                                      sctp_network_association_factory& assoc_factory_)
{
  // Validate arguments
  if (sctp_cfg.if_name.empty()) {
    ocudulog::fetch_basic_logger("SCTP-GW").error("Cannot create SCTP server. Cause: No name was provided");
    return nullptr;
  }
  if (sctp_cfg.bind_addresses.empty()) {
    ocudulog::fetch_basic_logger("SCTP-GW").error("{}: Cannot create SCTP server without bind addresses",
                                                  sctp_cfg.if_name);
    return nullptr;
  }

  if (sctp_cfg.bind_addresses[0].empty()) {
    ocudulog::fetch_basic_logger("SCTP-GW").error("{}: Cannot create SCTP server without bind address",
                                                  sctp_cfg.if_name);
    return nullptr;
  }

  // Create a SCTP server instance.
  std::unique_ptr<sctp_network_server_impl> server{
      new sctp_network_server_impl(sctp_cfg, broker_, io_rx_executor_, app_exec_, assoc_factory_)};

  // Create a socket and bind it to the provided address.
  if (not server->create_and_bind()) {
    server->stop();
    return nullptr;
  }

  return server;
}
