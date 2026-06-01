// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/ocuduvec/simd.h"

using namespace ocudu;
using namespace ocuduvec;

static void sc_prod_fff_simd(const float* x, float h, float* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_F_SIZE
  simd_f_t b = ocudu_simd_f_set1(h);
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(z)) {
    for (unsigned i_end = (len / OCUDU_SIMD_F_SIZE) * OCUDU_SIMD_F_SIZE; i != i_end; i += OCUDU_SIMD_F_SIZE) {
      simd_f_t a = ocudu_simd_f_load(x + i);

      simd_f_t r = ocudu_simd_f_mul(a, b);

      ocudu_simd_f_store(z + i, r);
    }
  } else {
    for (unsigned i_end = (len / OCUDU_SIMD_F_SIZE) * OCUDU_SIMD_F_SIZE; i != i_end; i += OCUDU_SIMD_F_SIZE) {
      simd_f_t a = ocudu_simd_f_loadu(x + i);

      simd_f_t r = ocudu_simd_f_mul(a, b);

      ocudu_simd_f_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * h;
  }
}

static void sc_prod_ccc_simd(const cf_t* x, cf_t h, cf_t* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  simd_cf_t b = ocudu_simd_cf_set1(h);
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(z)) {
    for (; i + OCUDU_SIMD_CF_SIZE < len + 1; i += OCUDU_SIMD_F_SIZE) {
      simd_cf_t a = ocudu_simd_load(x + i);

      simd_cf_t r = ocudu_simd_cf_prod(a, b);

      ocudu_simd_store(z + i, r);
    }
  } else {
    for (; i + OCUDU_SIMD_CF_SIZE < len + 1; i += OCUDU_SIMD_F_SIZE) {
      simd_cf_t a = ocudu_simd_loadu(x + i);

      simd_cf_t r = ocudu_simd_cf_prod(a, b);

      ocudu_simd_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * h;
  }
}

static void sc_prod_ccc_simd(const cbf16_t* x, cf_t h, cbf16_t* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  simd_cf_t b = ocudu_simd_cf_set1(h);
  for (unsigned i_end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != i_end; i += OCUDU_SIMD_CF_SIZE) {
    simd_cf_t a = ocudu_simd_loadu(x + i);

    simd_cf_t r = ocudu_simd_cf_prod(a, b);

    ocudu_simd_storeu(z + i, r);
  }
#endif

  for (; i != len; ++i) {
    z[i] = to_cbf16(to_cf(x[i]) * h);
  }
}

static void sc_prod_sss_simd(const int16_t* x, int16_t h, int16_t* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_S_SIZE
  simd_s_t b = ocudu_simd_s_set1(h);
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(z)) {
    for (unsigned i_end = (len / OCUDU_SIMD_S_SIZE) * OCUDU_SIMD_S_SIZE; i != i_end; i += OCUDU_SIMD_S_SIZE) {
      simd_s_t a = ocudu_simd_s_load(x + i);

      simd_s_t r = ocudu_simd_s_mul(a, b);

      ocudu_simd_s_store(z + i, r);
    }
  } else {
    for (unsigned i_end = (len / OCUDU_SIMD_S_SIZE) * OCUDU_SIMD_S_SIZE; i != i_end; i += OCUDU_SIMD_S_SIZE) {
      simd_s_t a = ocudu_simd_s_loadu(x + i);

      simd_s_t r = ocudu_simd_s_mul(a, b);

      ocudu_simd_s_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] * h;
  }
}

static void sc_prod_cfc_simd(const cf_t* x, float h, cbf16_t* z, unsigned len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE

  simd_f_t b = ocudu_simd_f_set1(h);

  for (unsigned end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != end; i += OCUDU_SIMD_CF_SIZE) {
    // Load interleaved complex float values into SIMD registers.
    simd_cf_t a = ocudu_simd_loadu(x + i);

    // Multiply with the scaling factor.
    simd_cf_t r = ocudu_simd_cf_mul(a, b);

    // Convert complex float to complex brain float and store the result back to memory.
    ocudu_simd_storeu(z + i, r);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    z[i] = to_cbf16(x[i] * h);
  }
}

static void sc_prod_cfc_simd(const cbf16_t* x, float h, cbf16_t* z, unsigned len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE

  simd_f_t b = ocudu_simd_f_set1(h);

  for (unsigned end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != end; i += OCUDU_SIMD_CF_SIZE) {
    // Load interleaved complex float values into SIMD registers.
    simd_cf_t a = ocudu_simd_loadu(x + i);

    // Multiply with the scaling factor.
    simd_cf_t r = ocudu_simd_cf_mul(a, b);

    // Convert complex float to complex brain float and store the result back to memory.
    ocudu_simd_storeu(z + i, r);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    z[i] = to_cbf16(to_cf(x[i]) * h);
  }
}

void ocudu::ocuduvec::sc_prod(span<cf_t> z, span<const cf_t> x, cf_t h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_ccc_simd(x.data(), h, z.data(), x.size());
}

void ocudu::ocuduvec::sc_prod(span<cf_t> z, span<const cf_t> x, float h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_fff_simd(reinterpret_cast<const float*>(x.data()), h, reinterpret_cast<float*>(z.data()), 2 * x.size());
}

void ocudu::ocuduvec::sc_prod(span<float> z, span<const float> x, float h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_fff_simd(x.data(), h, z.data(), x.size());
}

void ocudu::ocuduvec::sc_prod(span<cbf16_t> z, span<const cbf16_t> x, cf_t h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_ccc_simd(x.data(), h, z.data(), x.size());
}

void ocudu::ocuduvec::sc_prod(span<cbf16_t> z, span<const cbf16_t> x, float h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_cfc_simd(x.data(), h, z.data(), x.size());
}

void ocudu::ocuduvec::sc_prod(span<int16_t> z, span<const int16_t> x, int16_t h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_sss_simd(x.data(), h, z.data(), x.size());
}

void ocudu::ocuduvec::sc_prod(span<cbf16_t> z, span<const cf_t> x, float h)
{
  ocudu_ocuduvec_assert_size(x, z);
  sc_prod_cfc_simd(x.data(), h, z.data(), x.size());
}
