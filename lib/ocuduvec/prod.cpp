// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/prod.h"
#include "ocudu/ocuduvec/simd.h"
#include "ocudu/support/math/math_utils.h"
#include <cmath>

using namespace ocudu;
using namespace ocuduvec;

static void prod_fff_simd(const float* x, const float* y, float* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_F_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (; i + OCUDU_SIMD_F_SIZE < len + 1; i += OCUDU_SIMD_F_SIZE) {
      simd_f_t a = ocudu_simd_f_load(x + i);
      simd_f_t b = ocudu_simd_f_load(y + i);

      simd_f_t r = ocudu_simd_f_mul(a, b);

      ocudu_simd_f_store(z + i, r);
    }
  } else {
    for (; i + OCUDU_SIMD_F_SIZE < len + 1; i += OCUDU_SIMD_F_SIZE) {
      simd_f_t a = ocudu_simd_f_loadu(x + i);
      simd_f_t b = ocudu_simd_f_loadu(y + i);

      simd_f_t r = ocudu_simd_f_mul(a, b);

      ocudu_simd_f_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * y[i];
  }
}

static void prod_sss_simd(const int16_t* x, const int16_t* y, int16_t* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (; i + OCUDU_SIMD_S_SIZE < len + 1; i += OCUDU_SIMD_S_SIZE) {
      simd_s_t a = ocudu_simd_s_load(x + i);
      simd_s_t b = ocudu_simd_s_load(y + i);

      simd_s_t r = ocudu_simd_s_mul(a, b);

      ocudu_simd_s_store(z + i, r);
    }
  } else {
    for (; i + OCUDU_SIMD_S_SIZE < len + 1; i += OCUDU_SIMD_S_SIZE) {
      simd_s_t a = ocudu_simd_s_loadu(x + i);
      simd_s_t b = ocudu_simd_s_loadu(y + i);

      simd_s_t r = ocudu_simd_s_mul(a, b);

      ocudu_simd_s_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * y[i];
  }
}

template <typename OutComplexType, typename InComplexType>
static void prod_ccc_simd(OutComplexType* z, const InComplexType* x, const InComplexType* y, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != i_end; i += OCUDU_SIMD_CF_SIZE) {
      simd_cf_t a = ocudu_simd_load(x + i);
      simd_cf_t b = ocudu_simd_load(y + i);

      simd_cf_t r = ocudu_simd_cf_prod(a, b);

      ocudu_simd_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != i_end; i += OCUDU_SIMD_CF_SIZE) {
      simd_cf_t a = ocudu_simd_loadu(x + i);
      simd_cf_t b = ocudu_simd_loadu(y + i);

      simd_cf_t r = ocudu_simd_cf_prod(a, b);

      ocudu_simd_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * y[i];
  }
}

template <typename OutComplexType, typename InComplexType>
static void prod_conj_ccc_simd(OutComplexType* z, const InComplexType* x, const InComplexType* y, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != i_end; i += OCUDU_SIMD_CF_SIZE) {
      simd_cf_t a = ocudu_simd_load(x + i);
      simd_cf_t b = ocudu_simd_load(y + i);

      simd_cf_t r = ocudu_simd_cf_conjprod(a, b);

      ocudu_simd_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != i_end; i += OCUDU_SIMD_CF_SIZE) {
      simd_cf_t a = ocudu_simd_loadu(x + i);
      simd_cf_t b = ocudu_simd_loadu(y + i);

      simd_cf_t r = ocudu_simd_cf_conjprod(a, b);

      ocudu_simd_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * std::conj(y[i]);
  }
}

template <typename OutComplexType, typename InComplexType>
static void prod_cexp_simd(OutComplexType* out, const InComplexType* in, float cfo, float initial_phase, unsigned len)
{
  // Phase increment for each sample.
  cf_t osc = std::polar<float>(1.0F, TWOPI * cfo);
  // Current phase, initially with the initial phase.
  cf_t phase = std::polar<float>(1.0F, initial_phase);
  // Current sample index.
  unsigned i = 0;

#if OCUDU_SIMD_CF_SIZE
  if (len >= OCUDU_SIMD_CF_SIZE) {
    // Prepare current phase vector with the initial phases.
    std::array<cf_t, OCUDU_SIMD_CF_SIZE> temp_phase;
    temp_phase[0] = phase;
    for (int k = 1; k != OCUDU_SIMD_CF_SIZE; ++k) {
      temp_phase[k] = temp_phase[k - 1] * osc;
    }

    // Load the current phase SIMD register with the initial phases;
    simd_cf_t simd_phase = ocudu_simd_loadu(temp_phase.data());

    // Prepare SIMD oscillator phase shift.
    simd_cf_t simd_osc = ocudu_simd_cf_set1(std::polar<float>(1.0F, OCUDU_SIMD_CF_SIZE * TWOPI * cfo));

    // Process in blocks of the SIMD register size.
    for (unsigned end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != end; i += OCUDU_SIMD_CF_SIZE) {
      // Load input.
      simd_cf_t simd_in = ocudu_simd_loadu(&in[i]);

      // Apply current phase to the input.
      simd_cf_t simd_out = ocudu_simd_cf_prod(simd_in, simd_phase);

      // Store result.
      ocudu_simd_storeu(&out[i], simd_out);

      // Increment current phase.
      simd_phase = ocudu_simd_cf_prod(simd_phase, simd_osc);
    }

    // Store phase SIMD register and update the current phase with the next phase.
    ocudu_simd_storeu(temp_phase.data(), simd_phase);
    phase = temp_phase.front();
  }
#endif

  for (; i != len; ++i) {
    out[i] = to_cf(in[i]) * phase;
    phase *= osc;
  }
}

void ocudu::ocuduvec::prod(span<cf_t> z, span<const cf_t> x, span<const cf_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  prod_ccc_simd(z.data(), x.data(), y.data(), x.size());
}

void ocudu::ocuduvec::prod(span<cbf16_t> z, span<const cf_t> x, span<const cf_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  prod_ccc_simd(z.data(), x.data(), y.data(), x.size());
}

void ocudu::ocuduvec::prod(span<float> z, span<const float> x, span<const float> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  prod_fff_simd(x.data(), y.data(), z.data(), x.size());
}

void ocudu::ocuduvec::prod(span<int16_t> z, span<const int16_t> x, span<const int16_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  prod_sss_simd(x.data(), y.data(), z.data(), x.size());
}

void ocudu::ocuduvec::prod_conj(span<cf_t> z, span<const cf_t> x, span<const cf_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  prod_conj_ccc_simd(z.data(), x.data(), y.data(), x.size());
}

void ocudu::ocuduvec::prod_cexp(span<cf_t> out, span<const cf_t> in, float norm_cfo, float initial_phase)
{
  ocudu_ocuduvec_assert_size(out, in);

  prod_cexp_simd(out.data(), in.data(), norm_cfo, initial_phase, in.size());
}

void ocudu::ocuduvec::prod_cexp(span<cbf16_t> out, span<const cbf16_t> in, float norm_cfo, float initial_phase)
{
  ocudu_ocuduvec_assert_size(out, in);

  prod_cexp_simd(out.data(), in.data(), norm_cfo, initial_phase, in.size());
}

void ocudu::ocuduvec::prod_cexp(span<cbf16_t> out, span<const cf_t> in, float norm_cfo, float initial_phase)
{
  ocudu_ocuduvec_assert_size(out, in);

  prod_cexp_simd(out.data(), in.data(), norm_cfo, initial_phase, in.size());
}
