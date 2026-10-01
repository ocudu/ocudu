// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_cplane_message_builder_static_compression_impl.h"
#include "../support/network_order_binary_serializer.h"

using namespace ocudu;
using namespace ofh;

void cplane_message_builder_static_compression_impl::serialize_compression_header(
    network_order_binary_serializer& serializer,
    const ru_compression_params&     compr,
    data_direction                   direction) const
{
  // Static compression always encodes a 0.
  static constexpr uint8_t reserved = 0;
  serializer.write(reserved);
}
