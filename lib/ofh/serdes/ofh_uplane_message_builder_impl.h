// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/serdes/ofh_uplane_message_builder.h"

namespace ocudu {
namespace ofh {

class iq_compressor;
class network_order_binary_serializer;

/// Open Fronthaul User-Plane message builder implementation.
class uplane_message_builder_impl : public uplane_message_builder
{
public:
  uplane_message_builder_impl(ocudulog::basic_logger& logger_, iq_compressor& compressor_) :
    logger(logger_), compressor(compressor_)
  {
  }

  // See interface for documentation.
  unsigned
  build_message(span<uint8_t> buffer, span<const cbf16_t> iq_data, const uplane_message_params& params) override;

private:
  /// Serializes IQ data from a resource grid.
  void serialize_iq_data(network_order_binary_serializer& serializer,
                         span<const cbf16_t>              iq_data,
                         unsigned                         nof_prbs,
                         const ru_compression_params&     params);

  /// Serializes compression header. Implementation depends on whether static or non-static IQ format is configured.
  virtual void serialize_compression_header(network_order_binary_serializer& serializer,
                                            const ru_compression_params&     params) = 0;

protected:
  const bool              ud_comp_length_support = false;
  ocudulog::basic_logger& logger;
  iq_compressor&          compressor;
};

} // namespace ofh
} // namespace ocudu
