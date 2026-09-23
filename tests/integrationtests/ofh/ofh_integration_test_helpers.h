// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "transmitter/ofh_uplane_fragment_size_calculator.h"
#include "ocudu/ofh/compression/compression_factory.h"
#include "ocudu/ofh/ecpri/ecpri_factories.h"
#include "ocudu/ofh/ethernet/ethernet_factories.h"
#include "ocudu/ofh/serdes/ofh_serdes_factories.h"

namespace ocudu {
namespace ofh {
namespace test {

/// \brief Calculates the number of downlink User-Plane messages the OFH transmitter generates per symbol and eAxC.
///
/// \param[in] mtu                   Ethernet frame size.
/// \param[in] nof_prbs              Number of PRBs transmitted per symbol.
/// \param[in] compr_params          Downlink IQ data compression parameters.
/// \param[in] is_static_compr_hdr   Downlink static compression header flag.
/// \param[in] is_vlan_enabled       Set to true if the Ethernet frames carry a VLAN tag.
/// \param[in] logger                Logger passed to the message builders.
inline unsigned calculate_nof_dl_uplane_messages_per_symbol(units::bytes                 mtu,
                                                            unsigned                     nof_prbs,
                                                            const ru_compression_params& compr_params,
                                                            bool                         is_static_compr_hdr,
                                                            bool                         is_vlan_enabled,
                                                            ocudulog::basic_logger&      logger)
{
  ether::vlan_frame_params ether_params{};
  if (is_vlan_enabled) {
    ether_params.vlan_config = ether::vlan_parameters{.tci_vid = 1};
  }
  std::unique_ptr<ether::frame_builder> eth_builder =
      is_vlan_enabled ? ether::create_vlan_frame_builder(ether_params) : ether::create_frame_builder(ether_params);
  std::unique_ptr<ecpri::packet_builder> ecpri_builder = ecpri::create_ecpri_packet_builder();

  std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors;
  for (auto& compressor : compressors) {
    compressor = create_iq_compressor(compression_type::none, logger);
  }
  std::unique_ptr<iq_compressor> compressor_sel = create_iq_compressor_selector(std::move(compressors));

  std::unique_ptr<uplane_message_builder> uplane_builder =
      is_static_compr_hdr ? create_static_compr_method_ofh_user_plane_packet_builder(logger, *compressor_sel)
                          : create_dynamic_compr_method_ofh_user_plane_packet_builder(logger, *compressor_sel);

  units::bytes headers_size = eth_builder->get_header_size() +
                              ecpri_builder->get_header_size(ecpri::message_type::iq_data) +
                              uplane_builder->get_header_size(compr_params);

  return ofh_uplane_fragment_size_calculator::calculate_nof_segments(mtu, nof_prbs, compr_params, headers_size);
}

} // namespace test
} // namespace ofh
} // namespace ocudu
