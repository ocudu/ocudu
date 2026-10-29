// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_zmq_baseband_gateway.h"
#include "ocudu/radio/capabilities/radio_rx_gain_control.h"
#include "ocudu/radio/capabilities/radio_tx_gain_control.h"
#include "ocudu/radio/radio_session.h"
#include "ocudu/support/executors/task_executor.h"
#include <zmq.h>

namespace ocudu {

/// Describes a ZeroMQ radio based session.
class radio_session_zmq_impl : public radio_session,
                               public radio_capabilities_registry,
                               private radio_tx_gain_control,
                               private radio_rx_gain_control
{
  /// Radio session logger.
  ocudulog::basic_logger& logger;
  /// ZMQ context.
  void* zmq_context;
  /// Stores transmit streams.
  std::vector<std::unique_ptr<radio_zmq_baseband_gateway>> bb_gateways;
  /// Indicates the session has been created successfully.
  bool successful              = false;
  using port_to_stream_channel = std::pair<unsigned, unsigned>;
  static_vector<port_to_stream_channel, RADIO_MAX_NOF_PORTS> tx_port_map;
  static_vector<port_to_stream_channel, RADIO_MAX_NOF_PORTS> rx_port_map;

public:
  /// \brief Default constructor.
  ///
  /// \param[in] config Provides the required parameters to start a ZMQ radio based session.
  /// \param[in] async_task_executor Provides a task executor to perform asynchronous tasks.
  /// \param[in] notification_handler Provides a radio event notification handler.
  /// \note Use is_successful() to check that the instance was successfully initialized.
  radio_session_zmq_impl(const radio_configuration::radio& config,
                         task_executor&                    async_task_executor,
                         radio_event_notifier&             notification_handler);

  /// Default destructor.
  ~radio_session_zmq_impl() override;

  /// Indicates if the instance was successfully initialized.
  bool is_successful() const { return successful; }

  // See the radio_session interface for documentation.
  radio_capabilities_registry& get_capabilities_registry() override { return *this; }

  // See the radio_session interface for documentation.
  baseband_gateway& get_baseband_gateway(unsigned stream_id) override
  {
    ocudu_assert(stream_id < bb_gateways.size(),
                 "Stream identifier (i.e., {}) exceeds the number of baseband gateways (i.e., {})",
                 stream_id,
                 bb_gateways.size());
    return *bb_gateways[stream_id];
  }

  // See the radio_session interface for documentation.
  baseband_gateway_timestamp read_current_time() override;

  // See the radio_session interface for documentation.
  void start(baseband_gateway_timestamp init_time) override;

  // See the radio_session interface for documentation.
  void stop() override;

private:
  // See the radio_tx_gain_control interface for documentation.
  bool set_tx_gain(unsigned port_id, double gain_dB) override;

  // See the radio_rx_gain_control interface for documentation.
  bool set_rx_gain(unsigned port_id, double gain_dB) override;

  // See the radio_capabilities_registry interface for documentation.
  void* query(radio_capability_key key) override
  {
    if (key == get_radio_capability_key<radio_tx_gain_control>()) {
      return static_cast<radio_tx_gain_control*>(this);
    }
    if (key == get_radio_capability_key<radio_rx_gain_control>()) {
      return static_cast<radio_rx_gain_control*>(this);
    }
    return nullptr;
  }
};

} // namespace ocudu
