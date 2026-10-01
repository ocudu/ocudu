// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "../ethernet_rx_metrics_collector_impl.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "ocudu/ofh/ethernet/ethernet_controller.h"
#include "ocudu/ofh/ethernet/ethernet_receiver.h"
#include "ocudu/support/ocudu_assert.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {

class task_executor;

namespace ether {

class frame_notifier;

/// DPDK Ethernet receiver implementation.
class dpdk_receiver_impl : public receiver, private receiver_operation_controller
{
public:
  dpdk_receiver_impl(task_executor&                     executor_,
                     std::shared_ptr<dpdk_port_context> port_ctx_,
                     ocudulog::basic_logger&            logger_,
                     bool                               are_metrics_enabled);

  // See interface for documentation.
  receiver_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  receiver_metrics_collector* get_metrics_collector() override;

private:
  // See interface for documentation.
  void start(frame_notifier& notifier) override;

  // See interface for documentation.
  void stop() override;

  /// Main receiving loop.
  void receive_loop();

  /// Receives new Ethernet frames from the socket.
  void receive();

  ocudulog::basic_logger&            logger;
  task_executor&                     executor;
  frame_notifier*                    notifier;
  std::shared_ptr<dpdk_port_context> port_ctx;
  receiver_metrics_collector_impl    metrics_collector;
  rt_stop_event_source               stop_manager;
};

} // namespace ether
} // namespace ocudu
