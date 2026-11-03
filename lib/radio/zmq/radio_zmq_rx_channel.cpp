// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_zmq_rx_channel.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/support/synchronization/sync_event.h"
#include <atomic>
#include <cstring>
#include <set>

using namespace ocudu;

/// Lists the supported socket types.
static const std::set<int> VALID_SOCKET_TYPES = {ZMQ_REQ};
/// Wait time after a buffer try push failed.
static constexpr std::chrono::microseconds circ_buffer_try_push_sleep{1};
/// Wait time after a buffer try pop failed.
static constexpr std::chrono::microseconds circ_buffer_try_pop_sleep{1};

radio_zmq_rx_channel::radio_zmq_rx_channel(void*                      zmq_context,
                                           const channel_description& config,
                                           radio_event_notifier&      notification_handler_,
                                           task_executor&             async_executor_) :
  stream_id(config.stream_id),
  channel_id(config.channel_id),
  socket_type(config.socket_type),
  logger(ocudulog::fetch_basic_logger(config.channel_id_str, false)),
  circular_buffer(config.buffer_size),
  buffer(config.buffer_size * sizeof(cf_t)),
  context(zmq_context),
  notification_handler(notification_handler_),
  async_executor(async_executor_),
  description(config)
{
  // Set log level.
  logger.set_level(config.log_level);

  // Validate the socket type.
  if (VALID_SOCKET_TYPES.count(config.socket_type) == 0) {
    logger.error("Invalid receiver type {} ({}).", config.socket_type, config.address);
    return;
  }

  // Create the socket and connect it to the peer.
  if (!open_socket()) {
    return;
  }

  // Indicate the initialization was successful.
  state_fsm.init_successful();
}

radio_zmq_rx_channel::~radio_zmq_rx_channel()
{
  close_socket();
}

bool radio_zmq_rx_channel::open_socket()
{
  // Create socket.
  sock = ::zmq_socket(context, description.socket_type);
  if (sock == nullptr) {
    logger.error("Failed to open receiver socket ({}). {}.", description.address, ::zmq_strerror(::zmq_errno()));
    return false;
  }

  // Monitor the socket to learn when its peer disconnects. The monitor must be set before connecting, otherwise the
  // first connection is not reported.
  // Monitor socket addresses cannot be reused. Use a different address every time a socket is reopened.
  static std::atomic<unsigned> monitor_count{0};
  monitor_address = fmt::format("inproc://radio_zmq_rx_monitor_{}", monitor_count.fetch_add(1));
  if (::zmq_socket_monitor(sock, monitor_address.c_str(), ZMQ_EVENT_DISCONNECTED) == -1) {
    logger.error("Failed to monitor receiver socket ({}). {}.", description.address, ::zmq_strerror(::zmq_errno()));
    return false;
  }
  monitor = ::zmq_socket(context, ZMQ_PAIR);
  if (monitor == nullptr || ::zmq_connect(monitor, monitor_address.c_str()) == -1) {
    logger.error("Failed to open the monitor of the receiver socket ({}). {}.",
                 description.address,
                 ::zmq_strerror(::zmq_errno()));
    return false;
  }

  // Connect socket.
  logger.info("Connecting to address {}.", description.address);
  if (::zmq_connect(sock, description.address.c_str()) == -1) {
    logger.error("Failed to bind receiver socket ({}). {}.", description.address, ::zmq_strerror(::zmq_errno()));
    return false;
  }

  // If a timeout is set...
  if (description.trx_timeout_ms) {
    int timeout = description.trx_timeout_ms;

    // Set receive timeout.
    if (::zmq_setsockopt(sock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
      logger.error("Failed to set receive timeout on tx socket. {}.", ::zmq_strerror(::zmq_errno()));
      return false;
    }

    // Set send timeout.
    if (::zmq_setsockopt(sock, ZMQ_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
      logger.error("Failed to set send timeout on tx socket. {}.", ::zmq_strerror(::zmq_errno()));
      return false;
    }

    // Set linger timeout.
    timeout = description.linger_timeout_ms;
    if (::zmq_setsockopt(sock, ZMQ_LINGER, &timeout, sizeof(timeout)) == -1) {
      logger.error("Failed to set linger timeout on tx socket. {}.", ::zmq_strerror(::zmq_errno()));
      return false;
    }
  }

  return true;
}

void radio_zmq_rx_channel::close_socket()
{
  if (monitor != nullptr) {
    ::zmq_close(monitor);
    monitor = nullptr;
  }
  if (sock != nullptr) {
    // Drop the request that the lost peer did not take.
    int linger = 0;
    ::zmq_setsockopt(sock, ZMQ_LINGER, &linger, sizeof(linger));
    ::zmq_close(sock);
    sock = nullptr;
  }
}

void radio_zmq_rx_channel::process_monitor()
{
  // An event is a frame with the event and its value, and a frame with the address of the peer.
  uint8_t event[6];
  while (::zmq_recv(monitor, event, sizeof(event), ZMQ_DONTWAIT) == sizeof(event)) {
    uint16_t id = 0;
    std::memcpy(&id, event, sizeof(id));

    // Notify the monitor events to the FSM.
    switch (id) {
      case ZMQ_EVENT_DISCONNECTED:
        state_fsm.on_disconnected();
        break;
      default:
        break;
    }

    // Skip the frame with the address of the peer.
    int    more = 0;
    size_t size = sizeof(more);
    ::zmq_getsockopt(monitor, ZMQ_RCVMORE, &more, &size);
    while (more != 0) {
      ::zmq_recv(monitor, nullptr, 0, 0);
      ::zmq_getsockopt(monitor, ZMQ_RCVMORE, &more, &size);
    }
  }
}

void radio_zmq_rx_channel::send_request()
{
  // Receive Transmit request if socket type is REPLY and no request is available.
  if (socket_type == ZMQ_REQ) {
    // Receive request.
    uint8_t dummy = 0;
    int     n     = ::zmq_send(sock, &dummy, sizeof(dummy), 0);

    // Request received.
    if (n > 0) {
      logger.debug("Socket sent request.");
      state_fsm.request_sent();
      return;
    }

    // Error.
    if (n < 0) {
      // Error happened.
      int err = ::zmq_errno();
      if (err == EFSM || err == EAGAIN) {
        // Ignore timeout and FSM error.
        logger.debug("Exception to send request. {}.", ::zmq_strerror(::zmq_errno()));
      } else {
        // This error cannot be ignored.
        logger.error("Socket failed to send request. {}.", ::zmq_strerror(::zmq_errno()));
        state_fsm.on_error();
        return;
      }
    }
  }

  // Implement other socket types here.
  // ...
}

void radio_zmq_rx_channel::receive_response()
{
  // Otherwise, send samples over socket.
  int sample_size = sizeof(cf_t);
  int nbytes      = buffer.size() * sample_size;
  int n           = ::zmq_recv(sock, (void*)buffer.data(), nbytes, ZMQ_DONTWAIT);

  // Make sure the received message has not been truncated.
  if (n > nbytes) {
    logger.error("Truncated {} bytes in ZMQ message.", n - nbytes);
    state_fsm.on_error();
    return;
  }

  // Check if an error occurred.
  if (n < 0) {
    // Error happened.
    int err = ::zmq_errno();
    if (err == EFSM || err == EAGAIN) {
      // Ignore timeout and FSM error.
      return;
    }

    // This error cannot be ignored.
    logger.error("Socket failed to receive DATA. {}.", ::zmq_strerror(::zmq_errno()));
    state_fsm.on_error();
    return;
  }

  // Make sure the received number of bytes is consistent with the sample number of bytes.
  if (n % sample_size != 0) {
    logger.error("Socket failed to receive DATA. Invalid number of bytes {}%{}={}.", n, sample_size, n % sample_size);
    state_fsm.on_error();
    return;
  }

  // Convert number of bytes to samples.
  unsigned nsamples = n / sample_size;
  logger.debug("Socket received {} samples.", nsamples);

  // Make sure the buffer size has not been exceeded.
  report_fatal_error_if_not(nsamples <= buffer.size(),
                            "Buffer overflow. Buffer size ({}) is not enough for the received number of samples ({})",
                            buffer.size(),
                            nsamples);

  unsigned count = 0;
  while (state_fsm.is_running() && (count != nsamples)) {
    // Try to write samples into the buffer.
    unsigned pushed = circular_buffer.try_push(buffer.begin() + count, buffer.begin() + nsamples);

    // Check if the push was successful.
    if (pushed == 0) {
      // Notify buffer overflow.
      radio_event_notifier::event_description event = {.stream_id  = stream_id,
                                                       .channel_id = channel_id,
                                                       .source     = radio_event_source::RECEIVE,
                                                       .type       = radio_event_type::OVERFLOW,
                                                       .timestamp  = std::nullopt};
      notification_handler.on_radio_rt_event(event);

      // Wait some time before trying again.
      std::this_thread::sleep_for(circ_buffer_try_push_sleep);
    }

    // Increment sample count.
    count += pushed;
  }

  // If successful transition to wait for data.
  state_fsm.data_received();
}

void radio_zmq_rx_channel::run_async()
{
  auto token = stop_control.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  // A request socket waits for its reply for ever, so the only way to tell that the peer that was asked is gone is its
  // disconnection, which the FSM keeps only if a response is pending. A slow peer is therefore waited for, as it is not
  // disconnected.
  process_monitor();

  // Transmit request if it has no pending response, otherwise receive response.
  if (!state_fsm.has_pending_response()) {
    send_request();
  } else {
    receive_response();
  }

  // A reply that the peer sent before leaving has been received by now. If the request is still pending, its reply
  // never comes, and the socket cannot send another request, so it is replaced by a new one that connects to the next
  // peer.
  if (state_fsm.has_disconnected()) {
    logger.info("Peer disconnected, requesting data again.");
    close_socket();
    if (!open_socket()) {
      state_fsm.on_error();
    } else {
      state_fsm.on_reopen_socket();
    }
  }

  // Check if the state timer expired.
  if (state_fsm.has_wait_expired()) {
    logger.info("Waiting for {}.", state_fsm.has_pending_response() ? "data" : "request");
  }

  // Feedback task if not stopped.
  if (state_fsm.is_running()) {
    if (not async_executor.defer([this, tk = std::move(token)]() { run_async(); })) {
      logger.error("Unable to enqueue radio zmq async task");
    }
  } else {
    logger.debug("Stopped asynchronous task.");
    state_fsm.async_task_stopped();
  }
}

void radio_zmq_rx_channel::receive(span<cf_t> data)
{
  auto token = stop_control.get_token();
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    ocuduvec::zero(data);
    return;
  }

  logger.debug("Requested to receive {} samples.", data.size());

  // Create and start a timer to inform about deadlocks.
  radio_zmq_timer timer(true);

  // Try to read samples from circular buffer.
  unsigned count = 0;
  while (state_fsm.is_running() && (count != data.size())) {
    // Try to pop samples.
    unsigned popped = circular_buffer.try_pop(data.begin() + count, data.end());

    // Wait some time before trying again.
    if (popped == 0) {
      std::this_thread::sleep_for(circ_buffer_try_pop_sleep);
    }

    // Check if an excess of time passed while trying to read samples.
    if (timer.is_expired()) {
      logger.info("Waiting for reading samples. Completed {} of {} samples.", count, data.size());
      timer.start();
    }

    // Increment count.
    count += popped;
  }

  if (!state_fsm.is_running()) {
    ocuduvec::zero(data);
  }
}

void radio_zmq_rx_channel::start()
{
  stop_control.reset();

  sync_event start_event;
  if (not async_executor.defer(
          [this, stop_token = stop_control.get_token(), start_token = start_event.get_token()]() mutable {
            // Eagerly signal that we started.
            start_token.reset();
            run_async();
          })) {
    logger.error("Unable to initiate radio zmq execution");
    return;
  }
  start_event.wait();
}

void radio_zmq_rx_channel::stop()
{
  logger.debug("Stopping...");
  state_fsm.stop();
  stop_control.stop();
  logger.debug("Stopped successfully.");
}
