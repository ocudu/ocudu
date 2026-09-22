// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "sctp_dtls.h"
#include "sctp_network_gateway_common_impl.h"
#include "sctp_network_gateway_dtls_interface.h"
#include "ocudu/gateways/sctp_network_client.h"
#include "ocudu/support/io/transport_layer_address.h"
#include <condition_variable>
#include <mutex>

struct sctp_sndrcvinfo;

namespace ocudu {

/// \brief SCTP client implementation
///
/// This implementation assumes single-threaded access to its public interface.
class sctp_network_client_impl : public sctp_network_client,
                                 public sctp_network_gateway_dtls_interface,
                                 public sctp_network_gateway_common_impl
{
  explicit sctp_network_client_impl(const sctp_network_connector_config& sctp_cfg,
                                    io_broker&                           broker,
                                    task_executor&                       io_rx_executor_);

public:
  ~sctp_network_client_impl() override;

  /// Create an SCTP client.
  static std::unique_ptr<sctp_network_client>
  create(const sctp_network_connector_config& sctp_cfg, io_broker& broker, task_executor& io_rx_executor);

  /// Connect to an SCTP server with the provided address.
  std::unique_ptr<sctp_association_sdu_notifier>
  connect(std::unique_ptr<sctp_association_sdu_notifier> recv_handler) override;

  int get_socket_fd() const override { return socket.fd().value(); }

private:
  class sctp_send_notifier;

  void receive();

  void handle_data(span<const uint8_t> payload);
  void handle_notification(span<const uint8_t>           payload,
                           const struct sctp_sndrcvinfo& sri,
                           const sockaddr&               src_addr,
                           socklen_t                     src_addr_len);

  void handle_dtls_notification(const union sctp_notification* notif, int assoc) override {}

  void handle_connection_up();
  void handle_connection_shutdown(const char* cause);
  void handle_connection_terminated(const std::string& cause);

  const sctp_network_connector_config client_cfg;

  io_broker&     broker;
  task_executor& io_rx_executor;

  // Number of consecutive failed connection attempts, reset once a connection is established. A periodic retry would
  // otherwise flood the console and the log: the first failure of an outage is announced in STDOUT and logged, the
  // ones that follow are logged at debug level, and a warning is logged again every \c connect_failure_log_period
  // attempts so that a lasting outage stays visible. A failure that is being retried is logged as a warning rather
  // than an error, as the connection is expected to recover on its own, whereas one that is not retried is final and
  // is logged as an error. See \c sctp_network_connector_config::connection_is_retried.
  unsigned nof_consecutive_connect_failures = 0;

  // Number of consecutive connection failures between two warning level log entries.
  static constexpr unsigned connect_failure_log_period = 60;

  // Announces and logs a failed connection attempt, throttling the repetitions.
  void handle_connect_failure(const std::string& cause);

  // Handler of IO events. It is only accessed by the backend (io_broker), once the connection is set up.
  std::unique_ptr<sctp_association_sdu_notifier> recv_handler;

  // The value of std::atomic<bool> is shared between client and sender notifier.
  // The value of the shared_ptr is shared between client frontend (public interface) and backend (io_broker), and
  // needs to be mutexed on creation/reset.
  std::shared_ptr<std::atomic<bool>> shutdown_received;

  // shared between client frontend (public interface) and backend (io_broker) and needs to be mutexed on read/write.
  transport_layer_address server_addr;

  std::mutex              connection_mutex;
  std::condition_variable connection_cvar;

  /// DTLS Context
  bool                          ssl_enabled = false;
  std::unique_ptr<dtls_context> dtls_ctxt;
};

} // namespace ocudu
