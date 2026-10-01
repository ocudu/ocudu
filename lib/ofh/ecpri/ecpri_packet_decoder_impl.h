// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ecpri/ecpri_packet_decoder.h"

namespace ocudu {
namespace ecpri {

/// eCPRI packet decoder implementation.
class packet_decoder_impl : public packet_decoder
{
public:
  packet_decoder_impl(ocudulog::basic_logger& logger_, unsigned sector_) : logger(logger_), sector(sector_) {}

  // See interface for documentation.
  span<const uint8_t> decode(span<const uint8_t> packet, packet_parameters& params) override;

private:
  /// Decodes an eCPRI common header from the packet.
  ///
  /// \param packet eCPRI packet to decode.
  /// \param params eCPRI decoded parameters.
  /// \return A span to the eCPRI parameters and payload on success, otherwise an empty span.
  span<const uint8_t> decode_header(span<const uint8_t> packet, packet_parameters& params);

  /// Decodes eCPRI parameters and extracts eCPRI payload from the given packet.
  /// \param packet eCPRI parameters and payload to decode.
  /// \param params eCPRI decoded parameters.
  /// \return A span to the eCPRI payload on success, otherwise an empty span.
  virtual span<const uint8_t> decode_payload(span<const uint8_t> packet, packet_parameters& params) = 0;

protected:
  ocudulog::basic_logger& logger;
  const unsigned          sector;
};

/// \brief eCPRI packet decoder implementation utilizing payload size encoded in a eCPRI header.
class packet_decoder_use_header_payload_size : public packet_decoder_impl
{
public:
  packet_decoder_use_header_payload_size(ocudulog::basic_logger& logger_, unsigned sector_) :
    packet_decoder_impl(logger_, sector_)
  {
  }

private:
  // See interface for documentation.
  span<const uint8_t> decode_payload(span<const uint8_t> packet, packet_parameters& params) override;
};

/// \brief eCPRI packet decoder implementation ignoring payload size encoded in a eCPRI header and using remaining
/// packet bytes as a payload.
class packet_decoder_ignore_header_payload_size : public packet_decoder_impl
{
public:
  packet_decoder_ignore_header_payload_size(ocudulog::basic_logger& logger_, unsigned sector_) :
    packet_decoder_impl(logger_, sector_)
  {
  }

private:
  // See interface for documentation.
  span<const uint8_t> decode_payload(span<const uint8_t> packet, packet_parameters& params) override;
};

} // namespace ecpri
} // namespace ocudu
