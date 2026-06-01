// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/add.h"
#include "ocudu/ocuduvec/simd.h"

using namespace ocudu;
using namespace ocuduvec;

static void add_fff_simd(const float* x, const float* y, float* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_F_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / OCUDU_SIMD_F_SIZE) * OCUDU_SIMD_F_SIZE; i != i_end; i += OCUDU_SIMD_F_SIZE) {
      simd_f_t a = ocudu_simd_f_load(x + i);
      simd_f_t b = ocudu_simd_f_load(y + i);

      simd_f_t r = ocudu_simd_f_add(a, b);

      ocudu_simd_f_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / OCUDU_SIMD_F_SIZE) * OCUDU_SIMD_F_SIZE; i != i_end; i += OCUDU_SIMD_F_SIZE) {
      simd_f_t a = ocudu_simd_f_loadu(x + i);
      simd_f_t b = ocudu_simd_f_loadu(y + i);

      simd_f_t r = ocudu_simd_f_add(a, b);

      ocudu_simd_f_storeu(z + i, r);
    }
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] + y[i];
  }
}

static void add_ccc_simd(const cf_t* x, const cbf16_t* y, cf_t* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_F_SIZE
  for (std::size_t i_end = (len / OCUDU_SIMD_F_SIZE) * OCUDU_SIMD_F_SIZE; i != i_end; i += OCUDU_SIMD_F_SIZE) {
    simd_cf_t a = ocudu_simd_loadu(x + i);
    simd_cf_t b = ocudu_simd_loadu(y + i);

    simd_cf_t r = ocudu_simd_cf_add(a, b);

    ocudu_simd_storeu(z + i, r);
  }
#endif

  for (; i != len; ++i) {
    z[i] = x[i] + to_cf(y[i]);
  }
}

static void add_sss_simd(const int16_t* x, const int16_t* y, int16_t* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / OCUDU_SIMD_S_SIZE) * OCUDU_SIMD_S_SIZE; i != i_end; i += OCUDU_SIMD_S_SIZE) {
      simd_s_t a = ocudu_simd_s_load(x + i);
      simd_s_t b = ocudu_simd_s_load(y + i);

      simd_s_t r = ocudu_simd_s_add(a, b);

      ocudu_simd_s_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / OCUDU_SIMD_S_SIZE) * OCUDU_SIMD_S_SIZE; i != i_end; i += OCUDU_SIMD_S_SIZE) {
      simd_s_t a = ocudu_simd_s_loadu(x + i);
      simd_s_t b = ocudu_simd_s_loadu(y + i);

      simd_s_t r = ocudu_simd_s_add(a, b);

      ocudu_simd_s_storeu(z + i, r);
    }
  }
#endif /* OCUDU_SIMD_S_SIZE */

  for (; i != len; ++i) {
    z[i] = x[i] + y[i];
  }
}

static void add_bbb_simd(const int8_t* x, const int8_t* y, int8_t* z, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_S_SIZE
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(y) && SIMD_IS_ALIGNED(z)) {
    for (std::size_t i_end = (len / OCUDU_SIMD_B_SIZE) * OCUDU_SIMD_B_SIZE; i != i_end; i += OCUDU_SIMD_B_SIZE) {
      simd_b_t a = ocudu_simd_b_load(x + i);
      simd_b_t b = ocudu_simd_b_load(y + i);

      simd_b_t r = ocudu_simd_b_add(a, b);

      ocudu_simd_b_store(z + i, r);
    }
  } else {
    for (std::size_t i_end = (len / OCUDU_SIMD_B_SIZE) * OCUDU_SIMD_B_SIZE; i != i_end; i += OCUDU_SIMD_B_SIZE) {
      simd_b_t a = ocudu_simd_b_loadu(x + i);
      simd_b_t b = ocudu_simd_b_loadu(y + i);

      simd_b_t r = ocudu_simd_b_add(a, b);

      ocudu_simd_b_storeu(z + i, r);
    }
  }
#endif /* OCUDU_SIMD_S_SIZE */

  for (; i != len; ++i) {
    z[i] = x[i] + y[i];
  }
}

void ocudu::ocuduvec::add(span<cf_t> z, span<const cf_t> x, span<const cf_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  add_fff_simd(reinterpret_cast<const float*>(x.data()),
               reinterpret_cast<const float*>(y.data()),
               reinterpret_cast<float*>(z.data()),
               2 * z.size());
}

void ocudu::ocuduvec::add(span<cf_t> z, span<const cf_t> x, span<const cbf16_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  add_ccc_simd(x.data(), y.data(), z.data(), z.size());
}

void ocudu::ocuduvec::add(span<float> z, span<const float> x, span<const float> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  add_fff_simd(x.data(), y.data(), z.data(), z.size());
}

void ocudu::ocuduvec::add(span<int16_t> z, span<const int16_t> x, span<const int16_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  add_sss_simd(x.data(), y.data(), z.data(), z.size());
}

void ocudu::ocuduvec::add(span<int8_t> z, span<const int8_t> x, span<const int8_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);
  ocudu_ocuduvec_assert_size(x, z);

  add_bbb_simd(x.data(), y.data(), z.data(), z.size());
}
