// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ofh_cplane_message_builder_impl.h"

namespace ocudu {
namespace ofh {

/// Open Fronthaul Control-Plane message builder for dynamic compression.
class cplane_message_builder_dynamic_compression_impl : public cplane_message_builder_impl
{
private:
  // See interface for documentation
  void serialize_compression_header(network_order_binary_serializer& serializer,
                                    const ru_compression_params&     compr,
                                    data_direction                   direction) const override;
};

} // namespace ofh
} // namespace ocudu
