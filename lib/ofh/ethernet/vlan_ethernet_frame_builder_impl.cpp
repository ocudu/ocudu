// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "vlan_ethernet_frame_builder_impl.h"
#include "../support/network_order_binary_serializer.h"
#include "ethernet_constants.h"

using namespace ocudu;
using namespace ether;

/// Encodes VLAN TCI PCP and VID fields into a 2 bytes value.
static uint16_t encode_tci(uint16_t vlan_id, uint8_t pcp)
{
  // VLAN ID occupies 12 LSB bits.
  uint16_t word = vlan_id;
  // PCP occupies 3 MSB bits.
  word |= uint16_t(static_cast<uint16_t>(pcp) << 13u);

  return word;
}

vlan_frame_builder_impl::vlan_frame_builder_impl(const ocudu::ether::vlan_frame_params& eth_params_) :
  eth_params(eth_params_)
{
  ocudu_assert(eth_params.vlan_config.has_value(), "Expected VLAN TCI fields");
}

units::bytes vlan_frame_builder_impl::get_header_size() const
{
  return ETH_HEADER_SIZE + ETH_VLAN_TAG_SIZE;
}

void vlan_frame_builder_impl::build_frame(span<uint8_t> buffer)
{
  ofh::network_order_binary_serializer serializer(buffer.data());

  // Write destination MAC address (6 Bytes).
  serializer.write(eth_params.mac_dst_address);

  // Write source MAC address (6 Bytes).
  serializer.write(eth_params.mac_src_address);

  // Write VLAN TPID (2 Bytes).
  serializer.write(VLAN_TPID);

  // Write VLAN TCI (2 Bytes).
  serializer.write(encode_tci(eth_params.vlan_config->tci_vid, eth_params.vlan_config->tci_pcp));

  // Write Ethernet Type (2 Bytes).
  serializer.write(eth_params.eth_type);
}
