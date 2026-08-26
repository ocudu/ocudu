// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/generic_functions/precoding/channel_precoder.h"
#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/phy/support/resource_grid_writer.h"

namespace ocudu {

/// Implements a resource grid mapper.
class resource_grid_mapper_impl : public resource_grid_mapper
{
public:
  resource_grid_mapper_impl(std::unique_ptr<channel_precoder> precoder_);

  ~resource_grid_mapper_impl() = default;

  // See interface for documentation.
  void map(resource_grid_writer&                      writer,
           const re_buffer_reader<>&                  input,
           const re_pattern&                          pattern,
           const precoding_beamforming_configuration& precoding) override;

  // See interface for documentation.
  void map(resource_grid_writer&                      grid,
           symbol_buffer&                             buffer,
           const allocation_configuration&            allocation,
           const re_pattern_list&                     reserved,
           const precoding_beamforming_configuration& precoding,
           unsigned                                   re_skip) const override;

  // See interface for documentation.
  void map(resource_grid_writer&           grid,
           symbol_buffer&                  buffer,
           const allocation_configuration& allocation,
           const re_pattern_list&          reserved,
           span<const unsigned>            ports,
           const precoding_configuration&  precoding,
           unsigned                        re_skip) const override;

private:
  /// Maximum number of subcarriers that can be accommodated in an OFDM symbol.
  static constexpr unsigned max_nof_subcarriers = MAX_NOF_SUBCARRIERS;
  /// Maximum number of ports to map in a mapping call.
  static constexpr unsigned max_nof_ports = precoding_constants::MAX_NOF_PORTS;

  /// \brief Maps a Resource Element block in an OFDM symbol.
  ///
  /// \param[out] writer       Destination resource grid.
  /// \param[in]  block_mask   Allocation mask.
  /// \param[in]  ports        List of port identifiers onto which the complex symbols are mapped in the resource grid.
  /// \param[in]  prg_weights  Precoding weights for the block.
  /// \param[in]  block        Complex symbols to map.
  /// \param[in]  i_symbol     Destination OFDM symbol index within the slot.
  /// \param[in]  i_subc       Initial subcarrier index at which start mapping.
  void map_re_block(resource_grid_writer&                      writer,
                    const bounded_bitset<max_nof_subcarriers>& block_mask,
                    span<const unsigned>                       ports,
                    const precoding_weight_matrix&             prg_weights,
                    span<const ci8_t>                          block,
                    unsigned                                   i_symbol,
                    unsigned                                   i_subc) const;

  /// Channel precoder.
  std::unique_ptr<channel_precoder> precoder;
};

} // namespace ocudu
