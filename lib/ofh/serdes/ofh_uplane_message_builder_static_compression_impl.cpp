// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_uplane_message_builder_static_compression_impl.h"

using namespace ocudu;
using namespace ofh;

void ofh_uplane_message_builder_static_compression_impl::serialize_compression_header(
    network_order_binary_serializer& serializer,
    const ru_compression_params&     params)
{
  // When the static IQ format is configured the udCompHdr and reserved fields are absent.
}

units::bytes
ofh_uplane_message_builder_static_compression_impl::get_header_size(const ru_compression_params& params) const
{
  units::bytes hdr_size(8);
  if (params.type == compression_type::bfp_selective || params.type == compression_type::mod_selective) {
    hdr_size += units::bytes(2);
  }

  return hdr_size;
}
