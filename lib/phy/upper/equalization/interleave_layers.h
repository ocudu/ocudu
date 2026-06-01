// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ocuduvec/simd.h"

namespace ocudu {

template <unsigned NofLayers>
void interleave_layers_generic(span<cf_t> out, simd_cf_t in[NofLayers])
{
  unsigned nof_re = out.size() / NofLayers;
  for (unsigned i_layer = 0; i_layer != NofLayers; ++i_layer) {
    std::array<cf_t, OCUDU_SIMD_CF_SIZE> temp_eq_symbols;
    ocudu_simd_storeu(temp_eq_symbols.data(), in[i_layer]);

    for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
      out[NofLayers * i_re + i_layer] = temp_eq_symbols[i_re];
    }
  }
}

template <unsigned NofLayers>
void interleave_layers_simd(span<cf_t> out, simd_cf_t in[NofLayers])
{
  interleave_layers_generic<NofLayers>(out, in);
}

template <>
inline void interleave_layers_simd<2>(span<cf_t> out, simd_cf_t in[2])
{
  ocudu_simd_storeu(&out[OCUDU_SIMD_CF_SIZE * 0], ocudu_simd_cf_interleave_low(in[0], in[1]));
  ocudu_simd_storeu(&out[OCUDU_SIMD_CF_SIZE * 1], ocudu_simd_cf_interleave_high(in[0], in[1]));
}

template <>
inline void interleave_layers_simd<4>(span<cf_t> out, simd_cf_t in[4])
{
  simd_cf_t low02  = ocudu_simd_cf_interleave_low(in[0], in[2]);
  simd_cf_t low13  = ocudu_simd_cf_interleave_low(in[1], in[3]);
  simd_cf_t high02 = ocudu_simd_cf_interleave_high(in[0], in[2]);
  simd_cf_t high13 = ocudu_simd_cf_interleave_high(in[1], in[3]);

  ocudu_simd_storeu(&out[OCUDU_SIMD_CF_SIZE * 0], ocudu_simd_cf_interleave_low(low02, low13));
  ocudu_simd_storeu(&out[OCUDU_SIMD_CF_SIZE * 1], ocudu_simd_cf_interleave_high(low02, low13));
  ocudu_simd_storeu(&out[OCUDU_SIMD_CF_SIZE * 2], ocudu_simd_cf_interleave_low(high02, high13));
  ocudu_simd_storeu(&out[OCUDU_SIMD_CF_SIZE * 3], ocudu_simd_cf_interleave_high(high02, high13));
}

template <unsigned NofLayers>
void interleave_layers_generic(span<float> out, simd_f_t in[NofLayers])
{
  unsigned nof_re = out.size() / NofLayers;
  for (unsigned i_layer = 0; i_layer != NofLayers; ++i_layer) {
    std::array<float, OCUDU_SIMD_CF_SIZE> temp_eq_symbols;
    ocudu_simd_f_storeu(temp_eq_symbols.data(), in[i_layer]);

    for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
      out[NofLayers * i_re + i_layer] = temp_eq_symbols[i_re];
    }
  }
}

template <unsigned NofLayers>
void interleave_layers_simd(span<float> out, simd_f_t in[NofLayers])
{
  interleave_layers_generic<NofLayers>(out, in);
}

template <>
inline void interleave_layers_simd<2>(span<float> out, simd_f_t in[2])
{
  ocudu_simd_f_storeu(&out[OCUDU_SIMD_F_SIZE * 0], ocudu_simd_f_interleave_low(in[0], in[1]));
  ocudu_simd_f_storeu(&out[OCUDU_SIMD_F_SIZE * 1], ocudu_simd_f_interleave_high(in[0], in[1]));
}

template <>
inline void interleave_layers_simd<4>(span<float> out, simd_f_t in[4])
{
  simd_f_t low02  = ocudu_simd_f_interleave_low(in[0], in[2]);
  simd_f_t low13  = ocudu_simd_f_interleave_low(in[1], in[3]);
  simd_f_t high02 = ocudu_simd_f_interleave_high(in[0], in[2]);
  simd_f_t high13 = ocudu_simd_f_interleave_high(in[1], in[3]);

  ocudu_simd_f_storeu(&out[OCUDU_SIMD_F_SIZE * 0], ocudu_simd_f_interleave_low(low02, low13));
  ocudu_simd_f_storeu(&out[OCUDU_SIMD_F_SIZE * 1], ocudu_simd_f_interleave_high(low02, low13));
  ocudu_simd_f_storeu(&out[OCUDU_SIMD_F_SIZE * 2], ocudu_simd_f_interleave_low(high02, high13));
  ocudu_simd_f_storeu(&out[OCUDU_SIMD_F_SIZE * 3], ocudu_simd_f_interleave_high(high02, high13));
}

} // namespace ocudu
