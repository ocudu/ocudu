// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ofh_message_receiver.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ofh/ethernet/ethernet_controller.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {
namespace ofh {

/// Open Fronthaul message receiver interface implementation that dispatches tasks to the actual receiver.
class ofh_message_receiver_task_dispatcher : public message_receiver, public operation_controller
{
public:
  ofh_message_receiver_task_dispatcher(ocudulog::basic_logger&          logger_,
                                       message_receiver&                msg_receiver_,
                                       task_executor&                   executor_,
                                       unsigned                         sector_,
                                       std::unique_ptr<ether::receiver> eth_receiver_) :
    logger(logger_),
    msg_receiver(msg_receiver_),
    executor(executor_),
    sector(sector_),
    eth_receiver(std::move(eth_receiver_))
  {
    ocudu_assert(eth_receiver, "Invalid Ethernet receiver");
  }

  // See interface for documentation.
  void start() override
  {
    stop_manager.reset();
    eth_receiver->get_operation_controller().start(*this);
  }

  // See interface for documentation.
  void stop() override
  {
    // Ethernet blocks until stop is completed.
    eth_receiver->get_operation_controller().stop();
    stop_manager.stop();
  }

  // See interface for documentation.
  operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void on_new_frame(ether::unique_rx_buffer buffer) override
  {
    auto token = stop_manager.get_token();
    if (OCUDU_UNLIKELY(token.is_stop_requested())) {
      return;
    }

    if (!executor.defer([this, buff = std::move(buffer), tk = std::move(token)]() mutable {
          msg_receiver.on_new_frame(std::move(buff));
        })) {
      logger.warning("Failed to dispatch receiver task for sector#{}", sector);
    }
  }

  // See interface for the documentation.
  message_receiver_metrics_collector* get_metrics_collector() override { return msg_receiver.get_metrics_collector(); }

private:
  ocudulog::basic_logger&          logger;
  message_receiver&                msg_receiver;
  task_executor&                   executor;
  const unsigned                   sector;
  std::unique_ptr<ether::receiver> eth_receiver;
  rt_stop_event_source             stop_manager;
};

} // namespace ofh
} // namespace ocudu
