// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ethernet/ethernet_frame_builder.h"

namespace ocudu {
namespace ether {

/// Implementation for the Ethernet frame builder with no VLAN tag insertion.
class frame_builder_impl : public frame_builder
{
public:
  /// Constructor.
  ///
  /// \param[in] eth_params Ethernet message parameters.
  explicit frame_builder_impl(const vlan_frame_params& eth_params);

  // See interface for documentation.
  units::bytes get_header_size() const override;

  // See interface for documentation.
  void build_frame(span<uint8_t> buffer) override;

private:
  /// Ethernet message parameters.
  const vlan_frame_params eth_params;
};

} // namespace ether
} // namespace ocudu
