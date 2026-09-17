// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "beamforming_weights_compressor.h"
#include "packing_utils_generic.h"
#include "quantizer.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ofh/compression/compression_properties.h"
#include "ocudu/ofh/ofh_constants.h"
#include <algorithm>

using namespace ocudu;
using namespace ofh;

namespace {

/// Quantized values of a beamforming weight vector, holding the real and imaginary part of every weight.
using quantized_weights = static_vector<int16_t, 2 * MAX_NOF_BEAMFORMING_WEIGHTS>;
} // namespace

/// Quantizes the real and imaginary parts of the given weights using the given bit width.
static quantized_weights quantize_weights(span<const cf_t> weights, unsigned bit_width)
{
  quantizer q(bit_width);

  quantized_weights quantized;
  for (const cf_t& weight : weights) {
    quantized.push_back(q.to_fixed_point(weight.real()));
    quantized.push_back(q.to_fixed_point(weight.imag()));
  }

  return quantized;
}

/// \brief Determines the exponent of a BFP compression block.
///
/// Implements the exponent calculation of the algorithm given in O-RAN.WG4.CUS, Annex A.1.2. The compression block
/// spans the entire weight vector, see O-RAN.WG4.CUS, 7.7.1.2.
static uint8_t determine_block_exponent(span<const int16_t> quantized, unsigned data_width)
{
  int max_value = *std::max_element(quantized.begin(), quantized.end());
  int min_value = *std::min_element(quantized.begin(), quantized.end());

  int max_abs = std::max(max_value, std::abs(min_value) - 1);
  if (max_abs <= 0) {
    // If the input is all zeros, the max absolute value is negative.
    // If the input is a mix of 0 and -1, the max absolute value becomes 0.
    return 0;
  }

  // Number of significant bits of the maximum absolute value.
  int nof_significant_bits = std::numeric_limits<unsigned>::digits - __builtin_clz(static_cast<unsigned>(max_abs));

  return static_cast<uint8_t>(std::max(nof_significant_bits - static_cast<int>(data_width) + 1, 0));
}

void ocudu::ofh::compress_beamforming_weights(span<uint8_t>                buffer,
                                              span<const cf_t>             weights,
                                              const ru_compression_params& compr_params)
{
  ocudu_assert((compr_params.type == compression_type::none) || (compr_params.type == compression_type::BFP),
               "Unsupported beamforming weight compression method '{}'",
               to_string(compr_params.type));
  ocudu_assert((compr_params.data_width != 0) && (compr_params.data_width <= MAX_IQ_WIDTH),
               "The beamforming weight IQ bit width '{}' is out of the range [1, {}]",
               compr_params.data_width,
               MAX_IQ_WIDTH);

  ocudu_assert(!weights.empty(), "The number of beamforming weights must be non-zero");
  ocudu_assert(weights.size() <= MAX_NOF_BEAMFORMING_WEIGHTS,
               "The number of beamforming weights '{}' exceeds the maximum of '{}'",
               weights.size(),
               MAX_NOF_BEAMFORMING_WEIGHTS);

  units::bytes compressed_size = get_packed_beamforming_weights_size(weights.size(), compr_params);
  ocudu_assert(buffer.size() == compressed_size.value(),
               "The buffer size of '{}' Bytes does not match the compressed beamforming weights size of '{}' Bytes",
               buffer.size(),
               compressed_size.value());

  if (compr_params.type == compression_type::none) {
    quantized_weights quantized = quantize_weights(weights, compr_params.data_width);

    bit_buffer bit_buf = bit_buffer::from_bytes(buffer);
    pack_bytes(bit_buf, quantized, compr_params.data_width);

    return;
  }

  // Block floating point quantizes using the full bit width and then scales the block down by the shared exponent.
  quantized_weights quantized = quantize_weights(weights, Q_BIT_WIDTH);
  uint8_t           exponent  = determine_block_exponent(quantized, compr_params.data_width);
  for (int16_t& value : quantized) {
    value >>= exponent;
  }

  // Compression parameter holding the exponent (1 Byte).
  buffer[0] = exponent;

  bit_buffer bit_buf = bit_buffer::from_bytes(buffer.last(buffer.size() - 1));
  pack_bytes(bit_buf, quantized, compr_params.data_width);
}
