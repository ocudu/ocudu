// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/serdes/ofh_cplane_message_builder.h"

namespace ocudu {
namespace ofh {

class network_order_binary_serializer;

/// Open Fronthaul Control-Plane message builder implementation.
class cplane_message_builder_impl : public cplane_message_builder
{
public:
  // See interface for documentation.
  unsigned build_dl_ul_radio_channel_message(span<uint8_t>                          buffer,
                                             const cplane_section_type1_parameters& msg_params) override;

  // See interface for documentation.
  unsigned build_idle_guard_period_message(span<uint8_t>                          buffer,
                                           const cplane_section_type0_parameters& msg_params) override;

  // See interface for documentation.
  unsigned build_prach_mixed_numerology_message(span<uint8_t>                          buffer,
                                                const cplane_section_type3_parameters& msg_params) override;

private:
  /// Serializes the compression header using the given serializer, compression parameters and direction.
  virtual void serialize_compression_header(network_order_binary_serializer& serializer,
                                            const ru_compression_params&     compr,
                                            data_direction                   direction) const = 0;
};

} // namespace ofh
} // namespace ocudu
