// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ofh/ethernet/vlan_ethernet_frame_params.h"

namespace ocudu {
namespace ether {

/// \brief Describes the VLAN Ethernet frame decoder.
///
/// Decodes a VLAN Ethernet frame following the IEEE 802.3 and IEEE 802.1Q specifications.
class vlan_frame_decoder
{
public:
  /// Default destructor.
  virtual ~vlan_frame_decoder() = default;

  /// \brief Decodes the contents in the given buffer into a VLAN Ethernet frame.
  ///
  /// Decodes the contents of the buffer, filling the fields in the \c eth_params structure. Returns a new span with the
  /// VLAN Ethernet header removed.
  ///
  /// \param[in]  frame Frame to be decoded.
  /// \param[out] eth_params Ethernet message parameters or undefined contents in case of error.
  /// \return A span containing the payload of the VLAN Ethernet frame or an empty span in case of error.
  virtual span<const uint8_t> decode(span<const uint8_t> frame, vlan_frame_params& eth_params) = 0;
};

} // namespace ether
} // namespace ocudu
