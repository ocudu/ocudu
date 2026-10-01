// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "ocudu/ofh/ethernet/ethernet_controller.h"
#include "ocudu/ofh/ethernet/ethernet_frame_notifier.h"
#include "ocudu/ofh/ethernet/ethernet_properties.h"
#include "ocudu/ofh/ethernet/ethernet_receiver.h"
#include "ocudu/ofh/ethernet/ethernet_receiver_metrics_collector.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter_config.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

/// Encapsulates Ethernet transmitter and receiver functionalities used by the RU emulator.
class ru_emulator_transceiver
{
public:
  ru_emulator_transceiver(std::unique_ptr<ether::receiver>    receiver_,
                          std::unique_ptr<ether::transmitter> transmitter_) :
    receiver(std::move(receiver_)), transmitter(std::move(transmitter_))
  {
    ocudu_assert(receiver, "Invalid Ethernet receiver passed to RU emulator");
    ocudu_assert(transmitter, "Invalid Ethernet transmitter passed to RU emulator");
  }

  /// Starts the Ethernet receiver operation.
  void start(ether::frame_notifier& notifier) { receiver->get_operation_controller().start(notifier); }

  /// Stops the Ethernet receiver operation.
  void stop() { receiver->get_operation_controller().stop(); }

  /// Sends the given list of frames through the underlying Ethernet link.
  void send(span<span<const uint8_t>> frames) { transmitter->send(frames); }

private:
  std::unique_ptr<ether::receiver>    receiver;
  std::unique_ptr<ether::transmitter> transmitter;
};

/// Ethernet receiver implementation based on DPDK library.
class ru_emu_dpdk_receiver : public ether::receiver, public ether::receiver_operation_controller
{
  enum class status { idle, running, stop_requested, stopped };

public:
  ru_emu_dpdk_receiver(ocudulog::basic_logger&                   logger_,
                       task_executor&                            executor_,
                       std::shared_ptr<ether::dpdk_port_context> port_ctx_ptr_) :
    logger(logger_), executor(executor_), port_ctx_ptr(port_ctx_ptr_), port_ctx(*port_ctx_ptr)
  {
    ocudu_assert(port_ctx_ptr, "Invalid port context");
  }

  // See interface for documentation.
  ether::receiver_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start(ether::frame_notifier& notifier_) override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  ether::receiver_metrics_collector* get_metrics_collector() override { return nullptr; }

protected:
  /// Main receiving loop.
  void receive_loop();

  /// Receives new Ethernet frames from the socket.
  void receive();

private:
  ocudulog::basic_logger&                   logger;
  task_executor&                            executor;
  std::shared_ptr<ether::dpdk_port_context> port_ctx_ptr;
  ether::dpdk_port_context&                 port_ctx;
  ether::frame_notifier*                    notifier;
  std::atomic<status>                       trx_status{status::idle};
};

/// Ethernet transmitter implementation based on DPDK library.
class ru_emu_dpdk_transmitter : public ether::transmitter
{
public:
  ru_emu_dpdk_transmitter(ocudulog::basic_logger& logger_, std::shared_ptr<ether::dpdk_port_context> port_ctx_ptr_) :
    logger(logger_), port_ctx_ptr(port_ctx_ptr_), port_ctx(*port_ctx_ptr)
  {
    ocudu_assert(port_ctx_ptr, "Invalid port context");
  }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

  // See interface for documentation.
  ether::transmitter_metrics_collector* get_metrics_collector() override { return nullptr; }

private:
  ocudulog::basic_logger&                   logger;
  std::shared_ptr<ether::dpdk_port_context> port_ctx_ptr;
  ether::dpdk_port_context&                 port_ctx;
};

/// Ethernet receiver implementation based on regular UNIX sockets.
class ru_emu_socket_receiver : public ether::receiver, public ether::receiver_operation_controller
{
public:
  ru_emu_socket_receiver(ocudulog::basic_logger&          logger_,
                         task_executor&                   executor_,
                         const ether::transmitter_config& cfg);

  // See interface for documentation.
  ether::receiver_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  ether::receiver_metrics_collector* get_metrics_collector() override { return nullptr; }

  // See interface for documentation.
  void start(ether::frame_notifier& notifier_) override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<ether::receiver> receiver;
};

/// Ethernet transmitter implementation based on regular UNIX sockets.
class ru_emu_socket_transmitter : public ether::transmitter
{
public:
  ru_emu_socket_transmitter(ocudulog::basic_logger& logger_, const ether::transmitter_config& cfg);

  // See interface for documentation.
  ether::transmitter_metrics_collector* get_metrics_collector() override { return nullptr; }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

private:
  std::unique_ptr<ether::transmitter> transmitter;
};

/// Creates RU emulator transceiver based on DPDK library.
std::unique_ptr<ru_emulator_transceiver>
ru_emu_create_dpdk_transceiver(ocudulog::basic_logger&                   logger,
                               task_executor&                            executor,
                               std::shared_ptr<ether::dpdk_port_context> context);

/// Creates RU emulator transceiver based on regular UNIX sockets.
std::unique_ptr<ru_emulator_transceiver> ru_emu_create_socket_transceiver(ocudulog::basic_logger&          logger,
                                                                          task_executor&                   executor,
                                                                          const ether::transmitter_config& config);

} // namespace ocudu
