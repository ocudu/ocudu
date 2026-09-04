// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_cplane_message_builder_dynamic_compression_impl.h"
#include "../support/network_order_binary_serializer.h"

using namespace ocudu;
using namespace ofh;

void cplane_message_builder_dynamic_compression_impl::serialize_compression_header(
    network_order_binary_serializer& serializer,
    const ru_compression_params&     compr,
    data_direction                   direction) const
{
  // Downlink always encodes a 0.
  if (direction == data_direction::downlink) {
    static constexpr uint8_t reserved = 0;
    serializer.write(reserved);
    return;
  }

  uint8_t value = 0;
  // Note that an IQ bitwidth of 16 bits translates to a value of 0.
  value |= uint8_t(compr.data_width == 16 ? 0 : compr.data_width) << 4;
  value |= uint8_t(to_underlying(compr.type));

  serializer.write(value);
}
