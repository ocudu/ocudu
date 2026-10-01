// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ofh/ethernet/vlan_ethernet_frame_params.h"
#include "ocudu/support/units.h"

namespace ocudu {
namespace ether {

/// \brief Describes the Ethernet frame builder.
///
/// Builds an Ethernet frame following the IEEE 802.3 and IEEE 802.1Q specifications.
class frame_builder
{
public:
  /// Default destructor.
  virtual ~frame_builder() = default;

  /// Returns the Ethernet header size in bytes.
  virtual units::bytes get_header_size() const = 0;

  /// \brief Builds an Ethernet frame using the given parameters into \c buffer.
  ///
  /// This function expects that the buffer already contains the payload of the frame. The builder will write the
  /// header in the first \ref frame_builder::get_header_size bytes of the buffer.
  ///
  /// \param[in] buffer Buffer where the frame will be built.
  virtual void build_frame(span<uint8_t> buffer) = 0;
};

} // namespace ether
} // namespace ocudu
