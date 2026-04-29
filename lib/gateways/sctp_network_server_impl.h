// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "sctp_network_gateway_common_impl.h"
#include "ocudu/gateways/sctp_network_server.h"
#include "ocudu/support/async/manual_event.h"
#include <algorithm>
#include <list>
#include <unordered_map>
#include <vector>

struct sctp_sndrcvinfo;
struct sctp_assoc_change;

namespace ocudu {

/// Implements an SCTP server, capable of handling multiple SCTP associations.
///
/// The io_broker thread only performs the raw sctp_recvmsg.
//  All data and notification handling is deferred back to app_exec.
class sctp_network_server_impl : public sctp_network_server, public sctp_network_gateway_common_impl
{
  explicit sctp_network_server_impl(const sctp_network_gateway_config& sctp_cfg,
                                    io_broker&                         broker,
                                    task_executor&                     io_rx_executor,
                                    task_executor&                     app_exec,
                                    sctp_network_association_factory&  assoc_factory);

public:
  ~sctp_network_server_impl() override;

  void stop() override;

  static std::unique_ptr<sctp_network_server> create(const sctp_network_gateway_config& sctp_cfg,
                                                     io_broker&                         broker,
                                                     task_executor&                     io_rx_executor,
                                                     task_executor&                     app_exec,
                                                     sctp_network_association_factory&  assoc_factory);

  int get_socket_fd() const override { return socket.fd().value(); }

  void receive();

  bool listen() override;

  std::optional<uint16_t> get_listen_port() override;

  async_task<bool> connect(std::vector<transport_layer_address> dest_addrs) override;

private:
  class sctp_send_notifier;

  struct sctp_associaton_context {
    const int                          assoc_id;
    transport_layer_address            addr;
    std::shared_ptr<std::atomic<bool>> association_shutdown_received;

    std::unique_ptr<sctp_association_sdu_notifier> sctp_data_recv_notifier;

    sctp_associaton_context(int assoc_id);
  };

  // We use unique_ptr to maintain address stability.
  using association_map      = std::unordered_map<int, sctp_associaton_context>;
  using association_iterator = association_map::iterator;

  // Create a bind SCTP socket.
  bool create_and_bind();

  // Subscribe to IO broker to listen for incoming SCTP messages/events.
  bool subscribe_to_broker();

  void handle_socket_shutdown(const char* cause);

  void handle_data(int assoc_id, span<const uint8_t> payload);
  void handle_notification(span<const uint8_t>           payload,
                           const struct sctp_sndrcvinfo& sri,
                           const sockaddr&               src_addr,
                           socklen_t                     src_addr_len);
  void handle_association_shutdown(int assoc_id, const char* cause);
  void handle_sctp_shutdown_comp(int assoc_id);
  void handle_sctp_comm_lost(int assoc_id);
  void
  handle_sctp_comm_up(const struct sctp_assoc_change& assoc_change, const sockaddr& src_addr, socklen_t src_addr_len);
  void handle_cannot_start_association(int assoc_id, const sockaddr& src_addr, socklen_t src_addr_len);

  /// Remove association from the map, triggering recv notifier destruction.
  void remove_association(int assoc_id);

  struct pending_connect {
    std::vector<transport_layer_address> dest_addrs;
    manual_event<bool>                   event;

    bool contains(const transport_layer_address& addr) const
    {
      return std::find(dest_addrs.begin(), dest_addrs.end(), addr) != dest_addrs.end();
    }
  };

  io_broker&                        broker;
  task_executor&                    io_rx_executor;
  task_executor&                    app_exec;
  sctp_network_association_factory& assoc_factory;

  /// Keep-alive token used to cancel deferred tasks on shutdown. Only accessed from app_exec.
  std::shared_ptr<bool> keepalive_token;

  association_map            associations;
  std::list<pending_connect> pending_connects;
};

} // namespace ocudu
