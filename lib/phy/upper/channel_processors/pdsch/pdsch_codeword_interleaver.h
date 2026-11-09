// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/resource_grid_mapper.h"
#include <array>

namespace ocudu {

/// \brief Symbol buffer that joins the symbols of two codewords mapped onto overlapping resource grid ports.
///
/// For each resource element, it delivers the symbols of the first codeword layers followed by the symbols of the
/// second codeword layers, as per TS38.211 Section 7.3.1.3. The resulting buffer carries all the transmission layers
/// and is mapped with the precoding configuration of the whole transmission.
class pdsch_codeword_interleaver : public resource_grid_mapper::symbol_buffer
{
public:
  /// [Implementation-defined] Maximum number of symbols delivered by a single pop.
  static constexpr unsigned max_block_size = 4096;

  /// \brief Creates an interleaver over the symbol buffers of two codewords.
  /// \param[in] cw0_            Symbol buffer of the first codeword.
  /// \param[in] nof_layers_cw0_ Number of layers of the first codeword.
  /// \param[in] cw1_            Symbol buffer of the second codeword.
  /// \param[in] nof_layers_cw1_ Number of layers of the second codeword.
  pdsch_codeword_interleaver(symbol_buffer& cw0_,
                             unsigned       nof_layers_cw0_,
                             symbol_buffer& cw1_,
                             unsigned       nof_layers_cw1_);

  // See the resource_grid_mapper::symbol_buffer interface for documentation.
  unsigned get_max_block_size() const override;

  // See the resource_grid_mapper::symbol_buffer interface for documentation.
  bool empty() const override;

  // See the resource_grid_mapper::symbol_buffer interface for documentation.
  span<const ci8_t> pop_symbols(unsigned block_size) override;

private:
  /// Symbol buffer of the first codeword.
  symbol_buffer& cw0;
  /// Symbol buffer of the second codeword.
  symbol_buffer& cw1;
  /// Number of layers of the first codeword.
  unsigned nof_layers_cw0;
  /// Number of layers of the second codeword.
  unsigned nof_layers_cw1;
  /// Total number of layers of the transmission.
  unsigned nof_layers;
  /// Interleaved symbols.
  std::array<ci8_t, max_block_size> temp_symbols;
};

} // namespace ocudu
