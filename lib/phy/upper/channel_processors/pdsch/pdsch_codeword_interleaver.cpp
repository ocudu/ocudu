// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch_codeword_interleaver.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ran/precoding/precoding_constants.h"
#include <algorithm>

using namespace ocudu;

pdsch_codeword_interleaver::pdsch_codeword_interleaver(symbol_buffer& cw0_,
                                                       unsigned       nof_layers_cw0_,
                                                       symbol_buffer& cw1_,
                                                       unsigned       nof_layers_cw1_) :
  cw0(cw0_),
  cw1(cw1_),
  nof_layers_cw0(nof_layers_cw0_),
  nof_layers_cw1(nof_layers_cw1_),
  nof_layers(nof_layers_cw0_ + nof_layers_cw1_)
{
  ocudu_assert((nof_layers_cw0 > 0) && (nof_layers_cw1 > 0), "Each codeword must have at least one layer.");
  ocudu_assert(nof_layers <= precoding_constants::MAX_NOF_LAYERS,
               "The number of layers (i.e., {}) exceeds the maximum (i.e., {}).",
               nof_layers,
               precoding_constants::MAX_NOF_LAYERS);
}

unsigned pdsch_codeword_interleaver::get_max_block_size() const
{
  unsigned nof_re = std::min({cw0.get_max_block_size() / nof_layers_cw0,
                              cw1.get_max_block_size() / nof_layers_cw1,
                              max_block_size / nof_layers});
  return nof_re * nof_layers;
}

bool pdsch_codeword_interleaver::empty() const
{
  bool is_empty = cw0.empty();
  ocudu_assert(is_empty == cw1.empty(), "The codeword symbol buffers must run out of symbols at the same time.");
  return is_empty;
}

span<const ci8_t> pdsch_codeword_interleaver::pop_symbols(unsigned block_size)
{
  ocudu_assert(block_size % nof_layers == 0,
               "The block size (i.e., {}) must be a multiple of the number of layers (i.e., {}).",
               block_size,
               nof_layers);
  ocudu_assert(block_size <= max_block_size,
               "The block size (i.e., {}) exceeds the maximum (i.e., {}).",
               block_size,
               max_block_size);

  unsigned          nof_re      = block_size / nof_layers;
  span<const ci8_t> symbols_cw0 = cw0.pop_symbols(nof_re * nof_layers_cw0);
  span<const ci8_t> symbols_cw1 = cw1.pop_symbols(nof_re * nof_layers_cw1);

  span<ci8_t> output = span<ci8_t>(temp_symbols).first(block_size);
  for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
    span<ci8_t> re_symbols = output.subspan(i_re * nof_layers, nof_layers);
    ocuduvec::copy(re_symbols.first(nof_layers_cw0), symbols_cw0.subspan(i_re * nof_layers_cw0, nof_layers_cw0));
    ocuduvec::copy(re_symbols.last(nof_layers_cw1), symbols_cw1.subspan(i_re * nof_layers_cw1, nof_layers_cw1));
  }

  return output;
}
