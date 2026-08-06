// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_difi_udp_socket.h"
#include "ocudu/adt/complex.h"
#include "ocudu/gateways/baseband/baseband_gateway_receiver.h"
#include "ocudu/gateways/baseband/baseband_gateway_timestamp.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/radio/radio_event_notifier.h"
#include <atomic>
#include <optional>

namespace ocudu {

/// Implements a gateway receiver that unpacks IQ samples from DIFI data packets received over UDP.
class radio_difi_rx_stream : public baseband_gateway_receiver
{
public:
  /// Describes the necessary parameters to create a DIFI Rx stream.
  struct stream_description {
    /// Address to bind the receive socket on. Use "0.0.0.0" for any interface.
    std::string ip;
    /// Port to listen on.
    uint16_t port;
    /// Stream identifier. Packets with a different identifier are discarded.
    uint32_t stream_id;
    /// IQ sample bit depth, either 8 or 16.
    unsigned bit_depth;
    /// Sample rate in Hz, used to convert between timestamps and sample ticks.
    double sample_rate_Hz;
    /// Stream identifier string.
    std::string stream_id_str;
    /// Logging level.
    ocudulog::basic_levels log_level;
    /// \brief Latches the transmitter epoch onto this stream timeline instead of trusting it.
    ///
    /// Enable only for a peer running an unrelated epoch, such as equipment timestamping against UTC.
    bool latch_peer_epoch = false;
  };

  radio_difi_rx_stream(const stream_description& config_, radio_event_notifier& notifier_);

  // See interface for documentation.
  metadata receive(baseband_gateway_buffer_writer& data) override;

  bool is_successful() const { return successful; }

  uint64_t get_sample_count() const { return sample_count.load(std::memory_order_relaxed); }

  void start(baseband_gateway_timestamp init_time);

  void stop();

private:
  /// Stream configuration.
  stream_description config;
  /// Radio event notifier.
  radio_event_notifier& notifier;
  /// Logger.
  ocudulog::basic_logger& logger;
  /// Indicates whether the class was initialized successfully.
  bool successful = false;
  /// Counts the number of received samples.
  std::atomic<uint64_t> sample_count = {0};
  /// Receive socket.
  radio_difi_udp_socket socket;
  /// Reusable datagram buffer, sized for the largest possible UDP payload.
  std::vector<uint8_t> rx_buf;
  /// Offset added to a packet timestamp to place it on this stream timeline. Empty until the first packet.
  std::optional<int64_t> ts_offset;
  /// \brief Samples of a packet that ran past the end of the previous buffer.
  ///
  /// A transmitter packet size need not divide the buffer size; holding the tail avoids losing it.
  std::vector<ci16_t> carry_samples;
  /// Timestamp of the first sample in \ref carry_samples.
  uint64_t carry_ts = 0;
};

} // namespace ocudu
