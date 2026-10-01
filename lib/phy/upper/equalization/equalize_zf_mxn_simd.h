// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "gram_matrix.h"
#include "matrix_inverse.h"
#include "ocudu/ocuduvec/simd.h"

namespace ocudu {

template <unsigned NofLayers, unsigned NofPorts>
void equalize_zf_mxn_simd(simd_cf_t eq_symbols[NofLayers],
                          simd_f_t  noise_vars[NofLayers],
                          simd_cf_t ch_symbols[NofPorts],
                          simd_cf_t h[NofPorts][NofLayers],
                          float     noise_var_est)
{
  const simd_f_t  infinity     = ocudu_simd_f_set1(std::numeric_limits<float>::infinity());
  const simd_cf_t complex_zero = ocudu_simd_cf_set1(0);

  // Calculate Gram matrix.
  simd_cf_t h_gram[NofLayers][NofLayers];
  squared_gram_matrix<NofPorts, NofLayers>(h_gram, h);

  // Invert Gram matrix. h_gram becomes an identity matrix.
  simd_cf_t  h_gram_inv[NofLayers][NofLayers];
  simd_sel_t isnormal_mask = squared_matrix_inverse<NofLayers>(h_gram_inv, h_gram);

  // Channel matrix actual inverse. h becomes an identity matrix.
  simd_cf_t h_inv[NofPorts][NofLayers];
  for (unsigned i_layer = 0; i_layer != NofLayers; ++i_layer) {
    for (unsigned i_port = 0; i_port != NofPorts; ++i_port) {
      simd_cf_t result = ocudu_simd_cf_set1(0);
      for (unsigned k = 0; k != NofLayers; ++k) {
        result += ocudu_simd_cf_conjprod(h_gram_inv[k][i_layer], h[i_port][k]);
      }
      h_inv[i_port][i_layer] = result;
    }
  }

  // Calculate equalized symbols.
  for (unsigned i_layer = 0; i_layer != NofLayers; ++i_layer) {
    // Calculate the equalized result for the layer.
    simd_cf_t eq_symbols_res = ocudu_simd_cf_set1(0);
    for (unsigned i_port = 0; i_port != NofPorts; ++i_port) {
      eq_symbols_res += h_inv[i_port][i_layer] * ch_symbols[i_port];
    }

    // Calculate the noise for the layer.
    simd_f_t noise_var_res = ocudu_simd_cf_re(h_gram_inv[i_layer][i_layer]) * ocudu_simd_f_set1(noise_var_est);

    // Discard invalid results.
    eq_symbols[i_layer] = ocudu_simd_cf_select(complex_zero, eq_symbols_res, isnormal_mask);
    noise_vars[i_layer] = ocudu_simd_f_select(infinity, noise_var_res, isnormal_mask);
  }
}

} // namespace ocudu
