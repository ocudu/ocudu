// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ofh_uplane_message_builder_impl.h"

namespace ocudu {
namespace ofh {

class iq_compressor;
class network_order_binary_serializer;

/// Open Fronthaul User-Plane message builder implementation with dynamic IQ compression.
class ofh_uplane_message_builder_dynamic_compression_impl : public uplane_message_builder_impl
{
public:
  ofh_uplane_message_builder_dynamic_compression_impl(ocudulog::basic_logger& logger_, iq_compressor& compressor_) :
    uplane_message_builder_impl(logger_, compressor_)
  {
  }

  // See interface for documentation.
  units::bytes get_header_size(const ru_compression_params& params) const override;

private:
  // See parent for documentation.
  void serialize_compression_header(network_order_binary_serializer& serializer,
                                    const ru_compression_params&     params) override;
};

} // namespace ofh
} // namespace ocudu
