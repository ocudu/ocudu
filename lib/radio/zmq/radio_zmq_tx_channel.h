// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_zmq_tx_channel_fsm.h"
#include "ocudu/adt/blocking_queue.h"
#include "ocudu/adt/complex.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/radio/radio_event_notifier.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/synchronization/stop_event.h"
#include <atomic>
#include <zmq.h>

namespace ocudu {

/// Radio transmit channel over ZeroMQ socket.
class radio_zmq_tx_channel
{
  /// Indicates the stream identifier.
  unsigned stream_id;
  /// Channel identifier.
  unsigned channel_id;
  /// Indicates the stream identifier string.
  std::string channel_id_str;
  /// Indicates the current state of the stream.
  radio_zmq_tx_channel_fsm state_fsm;
  /// ZMQ socket.
  void* sock = nullptr;
  /// ZMQ socket type.
  int socket_type;
  /// Logger.
  ocudulog::basic_logger& logger;
  /// Stores transmit buffer.
  blocking_queue<cf_t> circular_buffer;
  /// Transmission buffer.
  std::vector<cf_t> buffer;
  /// Notification handler.
  radio_event_notifier& notification_handler;
  /// Asynchronous task executor.
  task_executor& async_executor;
  /// Indicates the number of transmitted samples. Protected for concurrent read-write.
  std::atomic<uint64_t> sample_count = {0};
  /// Protects concurrent transmit alignment operations from the receiver thread.
  std::mutex              transmit_alignment_mutex;
  std::condition_variable transmit_alignment_cvar;
  /// Stop control.
  rt_stop_event_source stop_control;
  std::atomic<bool>    is_tx_enabled = {false};

public:
  /// Describes the necessary parameters to create a ZMQ Tx channel.
  struct channel_description {
    /// Indicates the socket type.
    int socket_type;
    /// Indicates the address to bind.
    std::string address;
    /// Stream identifier.
    unsigned stream_id;
    /// Channel identifier.
    unsigned channel_id;
    /// Stream identifier string.
    std::string channel_id_str;
    /// Logging level.
    ocudulog::basic_levels log_level;
    /// Indicates the socket send and receive timeout in milliseconds. It is ignored if it is zero.
    unsigned trx_timeout_ms;
    /// Indicates the socket linger timeout in milliseconds. If is ignored if trx_timeout_ms is zero.
    unsigned linger_timeout_ms;
    /// Buffer size.
    unsigned buffer_size;
  };

  radio_zmq_tx_channel(void*                      zmq_context,
                       const channel_description& config,
                       radio_event_notifier&      notification_handler,
                       task_executor&             async_executor_);

  ~radio_zmq_tx_channel();

  bool is_successful() const { return state_fsm.is_initiated(); }

  void start(uint64_t init_time);

  void stop();

  bool align(uint64_t timestamp, std::chrono::milliseconds timeout);

  void transmit(span<const cf_t> buffer);

private:
  void receive_request();

  void send_response();

  void run_async();

  void transmit_samples(span<const cf_t> data);
};

} // namespace ocudu
