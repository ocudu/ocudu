// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_difi_context_packet.h"
#include "radio_difi_data_packet.h"
#include "radio_difi_udp_socket.h"
#include "ocudu/gateways/baseband/baseband_gateway_timestamp.h"
#include "ocudu/gateways/baseband/baseband_gateway_transmitter.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/radio/radio_event_notifier.h"
#include <cstdint>
#include <string>
#include <vector>

namespace ocudu {

/// Implements a baseband gateway transmitter that sends IQ samples as DIFI data packets over UDP.
class radio_difi_tx_stream : public baseband_gateway_transmitter
{
public:
  /// Parameters required to create a DIFI transmit stream.
  struct stream_description {
    /// Destination IP address for outgoing DIFI packets.
    std::string ip;
    /// Destination UDP port for outgoing DIFI data packets.
    uint16_t port = 0;
    /// DIFI 32-bit stream identifier embedded in every packet header.
    uint32_t stream_id = 0;
    /// IQ sample bit depth: 8 or 16.
    unsigned bit_depth = 16;
    /// Sample rate in Hz, used for timestamp conversion (sample ticks <-> seconds).
    double sample_rate_Hz = 0.0;
    /// RF centre frequency in Hz, encoded in the DIFI context packet rf_ref_freq field.
    double center_freq_Hz = 0.0;
    /// RF gain in dB, encoded in the lower 16 bits of the context packet gains field.
    double rf_gain_dB = 0.0;
    /// IF gain in dB, encoded in the upper 16 bits of the context packet gains field.
    double if_gain_dB = 0.0;
    /// Stream identifier string.
    std::string stream_id_str;
    /// Logging level.
    ocudulog::basic_levels log_level = ocudulog::basic_levels::info;
  };

  /// \brief Maximum IQ samples per data packet; larger transmit buffers are fragmented.
  ///
  /// 7708 bytes at 16-bit: inside one datagram and a typical 9000-byte receive buffer, and divides every slot.
  static constexpr unsigned MAX_PKT_SAMPLES = 1920;

  radio_difi_tx_stream(const stream_description& config_, radio_event_notifier& notifier_);

  /// Returns true if the stream was constructed successfully.
  bool is_successful() const { return successful; }

  // See baseband_gateway_transmitter for documentation.
  void transmit(const baseband_gateway_buffer_reader&        data,
                const baseband_gateway_transmitter_metadata& metadata) override;

  /// Starts the stream at the given initial timestamp (in sample ticks).
  void start(baseband_gateway_timestamp init_time);

  /// Stops the stream, flushing any pending data.
  void stop();

  /// Update the RF centre frequency and immediately re-send the context packet.
  void set_freq(double center_freq_Hz);

  /// Update the RF and IF gains and immediately re-send the context packet.
  void set_gain(double rf_gain_dB, double if_gain_dB);

private:
  /// Build and send a context packet with the supplied timestamp.
  void send_context_packet(uint32_t full_secs, uint64_t frac_ps);

  /// How often (in data packets) to re-send the context packet.
  static constexpr unsigned CTX_INTERVAL = 100;

  stream_description    config;
  radio_event_notifier& notifier;
  /// Logger.
  ocudulog::basic_logger& logger;
  bool                    successful = false;
  radio_difi_udp_socket   socket;
  /// Context packet sequence counter (mod-16, independent of data packet counter).
  uint8_t ctx_pkt_n = 0;
  /// Data packet sequence counter (mod-16).
  uint8_t data_pkt_n = 0;
  /// Number of data packets sent since the last context packet. Initialised to 1 so the first transmit() does not
  /// immediately re-send the context packet that start() already sent.
  unsigned pkts_since_ctx = 1;
  /// Packet buffer, sized at construction for the largest fragment so that transmit() never allocates.
  std::vector<uint8_t> tx_buf;
  /// Timestamp of the most recently sent context packet — reused by set_freq/set_gain.
  uint32_t last_ctx_full_secs = 0;
  uint64_t last_ctx_frac_ps   = 0;
  /// Expected next transmit timestamp (sample ticks); used to detect underflow gaps.
  uint64_t last_tx_end_ts = 0;
  bool     started        = false;
};

} // namespace ocudu
