// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/batched_dispatch_queue.h"
#include "ocudu/gateways/udp_network_gateway.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/io/unique_fd.h"
#include <netdb.h>
#include <sys/socket.h>

namespace ocudu {

constexpr uint32_t network_gateway_udp_max_len = 9100;

struct udp_tx_pdu_t {
  byte_buffer      pdu;
  sockaddr_storage dst_addr;
};

/// Receive context used by the recvmmsg syscall.
struct receive_context {
  explicit receive_context(unsigned rx_max_mmsg);

  std::vector<std::vector<uint8_t>> rx_mem;
  std::vector<::sockaddr_storage>   rx_srcaddr;
  std::vector<::mmsghdr>            rx_msghdr;
  std::vector<::iovec>              rx_iovecs;
};

/// Transmit context used by the sendmmsg syscall.
struct transmit_context {
  explicit transmit_context(unsigned tx_max_mmsg, unsigned tx_max_segments)
  {
    mmsg.resize(tx_max_mmsg);
    msgs.resize(tx_max_mmsg);
    for (unsigned i = 0; i < tx_max_mmsg; i++) {
      msgs[i].resize(tx_max_segments);
    }
  }

  std::vector<::mmsghdr>            mmsg;
  std::vector<std::vector<::iovec>> msgs;
};

class udp_network_gateway_impl final : public udp_network_gateway
{
public:
  explicit udp_network_gateway_impl(udp_network_gateway_config                   config_,
                                    network_gateway_data_notifier_with_src_addr& data_notifier_,
                                    task_executor&                               io_tx_executor_,
                                    task_executor&                               io_rx_executor_);
  ~udp_network_gateway_impl() override { io_subcriber.reset(); }

  void stop() override;

  bool subscribe_to(io_broker& broker) override;

private:
  bool set_sockopts();

  // udp_network_gateway_data_handler interface, called from CU-UP executor.
  void handle_pdu(byte_buffer pdu, const sockaddr_storage& dest_addr) override;

  // Actual PDU handling, shall run in IO executor.
  void handle_pdu_impl(span<udp_tx_pdu_t> pdus);

  // Handle error detected by io_broker that led to the io deregistration.
  void handle_io_error(io_broker::error_code code);

  // udp_network_gateway_controller interface
  bool                    create_and_bind() override;
  void                    receive() override;
  int                     get_socket_fd() override;
  std::optional<uint16_t> get_bind_port() const override;
  bool                    get_bind_address(std::string& ip_address) const override;

  // socket helpers
  bool set_non_blocking();
  bool set_receive_timeout(std::chrono::seconds rx_timeout);
  bool set_reuse_addr();
  bool set_dscp();
  bool close_socket();

  udp_network_gateway_config                   config; // configuration
  network_gateway_data_notifier_with_src_addr& data_notifier;
  ocudulog::basic_logger&                      logger;
  task_executor&                               io_tx_executor;
  task_executor&                               io_rx_executor;

  unique_fd             sock_fd;
  io_broker::subscriber io_subcriber;

  receive_context                      rx_context;
  transmit_context                     tx_ctx;
  batched_dispatch_queue<udp_tx_pdu_t> batched_queue;

  sockaddr_storage local_addr        = {}; // the local address
  socklen_t        local_addrlen     = 0;
  int              local_ai_family   = 0;
  int              local_ai_socktype = 0;
  int              local_ai_protocol = 0;

  // Helper boolean to avoid spamming the logs in case of buffer pool depletion
  bool warn_low_buffer_pool = true;
};

} // namespace ocudu
