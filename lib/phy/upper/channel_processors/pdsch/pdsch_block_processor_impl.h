// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "ocudu/phy/upper/channel_modulation/modulation_mapper.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_block_processor.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"

namespace ocudu {

/// \brief PDSCH block processor.
///
/// Contains the required dependencies to process batches of PDSCH codeblocks,
/// with low memory footprint and parameterizable parallelization.
///
/// For each codeblock in the batch, the processing consists of:
/// 1. Extracts the codeblock information bits from the transport block;
/// 2. Appends transport block CRC (if applicable);
/// 3. Appends zero padding;
/// 4. Appends codeblock CRC (if applicable);
/// 5. Appends filler bits as zeros;
/// 6. Applies LDPC encode;
/// 7. Applies LDPC rate matching;
/// 8. Applies scrambling; and
/// 9. Maps bits into modulation symbols onto the resource grid.
class pdsch_block_processor_impl : public pdsch_block_processor, private resource_grid_mapper::symbol_buffer
{
public:
  pdsch_block_processor_impl(std::unique_ptr<ldpc_encoder>            encoder_,
                             std::unique_ptr<ldpc_rate_matcher>       rate_matcher_,
                             std::unique_ptr<pseudo_random_generator> scrambler_,
                             std::unique_ptr<modulation_mapper>       modulator_) :
    encoder(std::move(encoder_)),
    rate_matcher(std::move(rate_matcher_)),
    scrambler(std::move(scrambler_)),
    modulator(std::move(modulator_))
  {
    ocudu_assert(encoder, "Invalid LDPC encoder.");
    ocudu_assert(rate_matcher, "Invalid LDPC rate matcher.");
    ocudu_assert(scrambler, "Invalid scrambler.");
    ocudu_assert(modulator, "Invalid modulator.");
  }

  // See the pdsch_block_processor interface for documentation.
  resource_grid_mapper::symbol_buffer& configure_new_transmission(span<const uint8_t>          data,
                                                                  unsigned                     i_cw,
                                                                  const configuration&         config,
                                                                  const ldpc_segmenter_buffer& segment_buffer,
                                                                  unsigned                     start_i_cb,
                                                                  unsigned                     cb_batch_len) override;

  // See the resource_grid_mapper::symbol_buffer interface for documentation.
  unsigned get_max_block_size() const override;

  // See the resource_grid_mapper::symbol_buffer interface for documentation.
  bool empty() const override;

  // See the resource_grid_mapper::symbol_buffer interface for documentation.
  span<const ci8_t> pop_symbols(unsigned block_size) override;

private:
  /// Pointer to an LDPC encoder.
  std::unique_ptr<ldpc_encoder> encoder;
  /// Pointer to an LDPC rate matcher.
  std::unique_ptr<ldpc_rate_matcher> rate_matcher;
  /// Pointer to scrambling pseudo-random generator.
  std::unique_ptr<pseudo_random_generator> scrambler;
  /// Pointer to modulation mapper.
  std::unique_ptr<modulation_mapper> modulator;
  /// Pointer to an LDPC segmenter output buffer interface.
  const ldpc_segmenter_buffer* segment_buffer;
  /// Current transmission modulation.
  modulation_scheme modulation;
  /// Current codeblock index.
  unsigned next_i_cb = 0;
  /// Last codeblock index.
  unsigned last_i_cb = 0;
  /// Current view of the codeblock modulated symbols.
  span<ci8_t> codeblock_symbols;
  /// Codeblock size. It includes information, CRC, and filler bits.
  units::bits cb_size;
  /// Local pointer to the input data.
  span<const uint8_t> transport_block;
  /// Temporary storage of codeblock symbols.
  std::array<ci8_t, ldpc::MAX_CODEBLOCK_RM_SIZE> temp_codeblock_symbols;

  /// Processes a new codeblock and writes the new data in \ref temp_codeblock_symbols.
  void new_codeblock();
};

} // namespace ocudu
