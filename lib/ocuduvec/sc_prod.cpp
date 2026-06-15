// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/ocuduvec/simd.h"

using namespace ocudu;
using namespace ocuduvec;

static void sc_prod_fff_simd(float* z, const float* x, float h, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_F_SIZE
  simd_f_t b = ocudu_simd_f_set1(h);
  for (unsigned end = (len / OCUDU_SIMD_F_SIZE) * OCUDU_SIMD_F_SIZE; i != end; i += OCUDU_SIMD_F_SIZE) {
    simd_f_t a = ocudu_simd_f_loadu(x + i);

    simd_f_t r = ocudu_simd_f_mul(a, b);

    ocudu_simd_f_storeu(z + i, r);
  }
#endif // OCUDU_SIMD_F_SIZE

  for (; i != len; ++i) {
    z[i] = x[i] * h;
  }
}

static void sc_prod_and_add_fff_simd(float* z, const float* x, const float* y, float h, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_F_SIZE
  simd_f_t h_simd = ocudu_simd_f_set1(h);
  for (unsigned end = (len / OCUDU_SIMD_F_SIZE) * OCUDU_SIMD_F_SIZE; i != end; i += OCUDU_SIMD_F_SIZE) {
    simd_f_t a = ocudu_simd_f_loadu(x + i);
    simd_f_t b = ocudu_simd_f_loadu(y + i);

    simd_f_t r = ocudu_simd_f_add(ocudu_simd_f_mul(a, h_simd), b);

    ocudu_simd_f_storeu(z + i, r);
  }
#endif // OCUDU_SIMD_F_SIZE

  for (; i != len; ++i) {
    z[i] = x[i] * h + y[i];
  }
}

template <typename OutComplexType, typename InComplexType>
void sc_prod_cfc_simd(OutComplexType* z, const InComplexType* x, float h, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  simd_f_t b = ocudu_simd_f_set1(h);
  for (unsigned end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != end; i += OCUDU_SIMD_CF_SIZE) {
    simd_cf_t a = ocudu_simd_loadu(x + i);

    simd_cf_t r = ocudu_simd_cf_mul(a, b);

    ocudu_simd_storeu(z + i, r);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    z[i] = to_cf(x[i]) * h;
  }
}

template <typename OutComplexType, typename InComplexType>
void sc_prod_ccc_simd(OutComplexType* z, const InComplexType* x, cf_t h, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  simd_cf_t b = ocudu_simd_cf_set1(h);
  for (unsigned end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != end; i += OCUDU_SIMD_CF_SIZE) {
    simd_cf_t a = ocudu_simd_loadu(x + i);

    simd_cf_t r = ocudu_simd_cf_prod(a, b);

    ocudu_simd_storeu(z + i, r);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    z[i] = to_cf(x[i]) * h;
  }
}

template <typename OutComplexType, typename InComplexType>
static void
sc_prod_and_add_cfc(OutComplexType* z, const InComplexType* x, const InComplexType* y, float h, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  simd_f_t h_simd = ocudu_simd_f_set1(h);
  for (std::size_t end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != end; i += OCUDU_SIMD_CF_SIZE) {
    simd_cf_t a = ocudu_simd_loadu(x + i);
    simd_cf_t b = ocudu_simd_loadu(y + i);

    simd_cf_t r = ocudu_simd_cf_add(ocudu_simd_cf_mul(a, h_simd), b);

    ocudu_simd_storeu(z + i, r);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    z[i] = to_cf(x[i]) * h + to_cf(y[i]);
  }
}

template <typename OutComplexType, typename InXComplexType, typename InYComplexType>
static void
sc_prod_and_add_ccc(OutComplexType* z, const InXComplexType* x, const InYComplexType* y, cf_t h, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  simd_cf_t h_simd = ocudu_simd_cf_set1(h);

  for (unsigned end = (len / OCUDU_SIMD_CF_SIZE) * OCUDU_SIMD_CF_SIZE; i != end; i += OCUDU_SIMD_CF_SIZE) {
    simd_cf_t a = ocudu_simd_loadu(x + i);
    simd_cf_t b = ocudu_simd_loadu(y + i);

    simd_cf_t r = ocudu_simd_cf_add(ocudu_simd_cf_prod(a, h_simd), b);

    ocudu_simd_storeu(z + i, r);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    z[i] = to_cf(x[i]) * h + to_cf(y[i]);
  }
}

void ocuduvec::sc_prod(span<cf_t> z, span<const cf_t> x, cf_t h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_ccc_simd(z.data(), x.data(), h, x.size());
}

void ocuduvec::sc_prod(span<cf_t> z, span<const cf_t> x, float h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_fff_simd(reinterpret_cast<float*>(z.data()), reinterpret_cast<const float*>(x.data()), h, 2 * x.size());
}

void ocuduvec::sc_prod(span<float> z, span<const float> x, float h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_fff_simd(z.data(), x.data(), h, x.size());
}

void ocuduvec::sc_prod(span<cbf16_t> z, span<const cbf16_t> x, cf_t h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_ccc_simd(z.data(), x.data(), h, x.size());
}

void ocuduvec::sc_prod(span<cbf16_t> z, span<const cbf16_t> x, float h)
{
  ocudu_ocuduvec_assert_size(x, z);

  sc_prod_cfc_simd(z.data(), x.data(), h, x.size());
}

void ocuduvec::sc_prod(span<cbf16_t> z, span<const cf_t> x, float h)
{
  ocudu_ocuduvec_assert_size(x, z);
  sc_prod_cfc_simd(z.data(), x.data(), h, x.size());
}

void ocuduvec::sc_prod(span<cbf16_t> z, span<const cf_t> x, cf_t h)
{
  ocudu_ocuduvec_assert_size(x, z);
  sc_prod_ccc_simd(z.data(), x.data(), h, x.size());
}

void ocuduvec::sc_prod_and_add(span<cf_t> z, span<const cf_t> x, span<const cf_t> y, cf_t h)
{
  ocudu_ocuduvec_assert_size(x, z);
  sc_prod_and_add_ccc(z.data(), x.data(), y.data(), h, x.size());
}

void ocuduvec::sc_prod_and_add(span<cf_t> z, span<const cf_t> x, span<const cf_t> y, float h)
{
  ocudu_ocuduvec_assert_size(x, z);
  sc_prod_and_add_fff_simd(reinterpret_cast<float*>(z.data()),
                           reinterpret_cast<const float*>(x.data()),
                           reinterpret_cast<const float*>(y.data()),
                           h,
                           2 * x.size());
}

void ocuduvec::sc_prod_and_add(span<float> z, span<const float> x, span<const float> y, float h)
{
  ocudu_ocuduvec_assert_size(x, z);
  sc_prod_and_add_fff_simd(z.data(), x.data(), y.data(), h, x.size());
}

void ocuduvec::sc_prod_and_add(span<cbf16_t> z, span<const cbf16_t> x, span<const cbf16_t> y, cf_t h)
{
  ocudu_ocuduvec_assert_size(x, z);
  sc_prod_and_add_ccc(z.data(), x.data(), y.data(), h, x.size());
}

void ocuduvec::sc_prod_and_add(span<cbf16_t> z, span<const cbf16_t> x, span<const cbf16_t> y, float h)
{
  ocudu_ocuduvec_assert_size(x, z);
  sc_prod_and_add_cfc(z.data(), x.data(), y.data(), h, x.size());
}

void ocuduvec::sc_prod_and_add(span<cf_t> z, span<const cbf16_t> x, span<const cf_t> y, cf_t h)
{
  ocudu_ocuduvec_assert_size(x, z);
  sc_prod_and_add_ccc(z.data(), x.data(), y.data(), h, x.size());
}
