// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/dot_prod.h"
#include "ocudu/ocuduvec/simd.h"

using namespace ocudu;
using namespace ocuduvec;

cf_t ocudu::ocuduvec::dot_prod(span<const cf_t> x, span<const cf_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);

  cf_t     result = 0;
  unsigned i      = 0;
  unsigned len    = x.size();

#if OCUDU_SIMD_CF_SIZE
  if (len >= OCUDU_SIMD_CF_SIZE) {
    simd_cf_t simd_result = ocudu_simd_cf_zero();
    for (unsigned simd_end = OCUDU_SIMD_CF_SIZE * (len / OCUDU_SIMD_CF_SIZE); i != simd_end; i += OCUDU_SIMD_CF_SIZE) {
      simd_cf_t simd_x = ocudu_simd_loadu(x.data() + i);
      simd_cf_t simd_y = ocudu_simd_loadu(y.data() + i);

      simd_result = ocudu_simd_cf_add(ocudu_simd_cf_conjprod(simd_x, simd_y), simd_result);
    }

    alignas(SIMD_BYTE_ALIGN) std::array<cf_t, OCUDU_SIMD_CF_SIZE> simd_vector_sum;
    ocudu_simd_store(simd_vector_sum.data(), simd_result);
    result = std::accumulate(simd_vector_sum.begin(), simd_vector_sum.end(), cf_t());
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    result += x[i] * std::conj(y[i]);
  }

  return result;
}

cf_t ocudu::ocuduvec::dot_prod(span<const cbf16_t> x, span<const cf_t> y)
{
  ocudu_ocuduvec_assert_size(x, y);

  cf_t     result = 0;
  unsigned i      = 0;
  unsigned len    = x.size();

#if OCUDU_SIMD_CF_SIZE
  if (len >= OCUDU_SIMD_CF_SIZE) {
    simd_cf_t simd_result = ocudu_simd_cf_zero();
    for (unsigned simd_end = OCUDU_SIMD_CF_SIZE * (len / OCUDU_SIMD_CF_SIZE); i != simd_end; i += OCUDU_SIMD_CF_SIZE) {
      simd_cf_t simd_x = ocudu_simd_loadu(x.data() + i);
      simd_cf_t simd_y = ocudu_simd_loadu(y.data() + i);

      simd_result = ocudu_simd_cf_add(ocudu_simd_cf_conjprod(simd_x, simd_y), simd_result);
    }

    alignas(SIMD_BYTE_ALIGN) std::array<cf_t, OCUDU_SIMD_CF_SIZE> simd_vector_sum;
    ocudu_simd_store(simd_vector_sum.data(), simd_result);
    result = std::accumulate(simd_vector_sum.begin(), simd_vector_sum.end(), cf_t());
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    result += to_cf(x[i]) * std::conj(y[i]);
  }

  return result;
}

cf_t ocudu::ocuduvec::dot_prod(span<const cf_t> x, span<const cbf16_t> y)
{
  return std::conj(dot_prod(y, x));
}

float ocudu::ocuduvec::average_power(span<const cf_t> x)
{
  float    result = 0;
  unsigned i      = 0;
  unsigned len    = x.size();

  if (len == 0) {
    return 0.0F;
  }

#if OCUDU_SIMD_CF_SIZE
  if (len >= OCUDU_SIMD_CF_SIZE) {
    simd_f_t simd_result = ocudu_simd_f_zero();
    for (unsigned simd_end = OCUDU_SIMD_CF_SIZE * (len / OCUDU_SIMD_CF_SIZE); i != simd_end; i += OCUDU_SIMD_CF_SIZE) {
      simd_cf_t simd_x = ocudu_simd_loadu(x.data() + i);

      simd_result = ocudu_simd_f_add(ocudu_simd_cf_norm_sq(simd_x), simd_result);
    }

    alignas(SIMD_BYTE_ALIGN) std::array<float, OCUDU_SIMD_F_SIZE> simd_vector_sum;
    ocudu_simd_f_store(simd_vector_sum.data(), simd_result);
    result = std::accumulate(simd_vector_sum.begin(), simd_vector_sum.end(), 0.0F);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    result += std::norm(x[i]);
  }

  return result / static_cast<float>(len);
}

float ocudu::ocuduvec::average_power(span<const cbf16_t> x)
{
  float    result = 0;
  unsigned i      = 0;
  unsigned len    = x.size();

  if (len == 0) {
    return 0.0F;
  }

#if OCUDU_SIMD_CF_SIZE
  if (len >= OCUDU_SIMD_CF_SIZE) {
    simd_f_t simd_result = ocudu_simd_f_zero();
    for (unsigned simd_end = OCUDU_SIMD_CF_SIZE * (len / OCUDU_SIMD_CF_SIZE); i != simd_end; i += OCUDU_SIMD_CF_SIZE) {
      simd_cf_t simd_x = ocudu_simd_loadu(x.data() + i);

      simd_result = ocudu_simd_f_add(ocudu_simd_cf_norm_sq(simd_x), simd_result);
    }

    alignas(SIMD_BYTE_ALIGN) std::array<float, OCUDU_SIMD_F_SIZE> simd_vector_sum;
    ocudu_simd_f_store(simd_vector_sum.data(), simd_result);
    result = std::accumulate(simd_vector_sum.begin(), simd_vector_sum.end(), 0.0F);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    result += std::norm(to_cf(x[i]));
  }

  return result / static_cast<float>(len);
}

float ocudu::ocuduvec::average_power(span<const ci16_t> x, float scale)
{
  if (x.empty()) {
    return 0.0F;
  }

  uint64_t energy_sum = 0;
  unsigned i          = 0;
  unsigned len        = x.size();

#if OCUDU_SIMD_CI16_SIZE
  simd_u64_t energy_accum = ocudu_simd_u64_zero();
  for (unsigned simd_end = OCUDU_SIMD_CI16_SIZE * (len / OCUDU_SIMD_CI16_SIZE); i != simd_end;
       i += OCUDU_SIMD_CI16_SIZE) {
    simd_i16_t simd_v    = ocudu_simd_ci16_loadu(x.data() + i);
    simd_i_t   simd_abs2 = ocudu_simd_ci16_norm_sq(simd_v);
    energy_accum         = ocudu_simd_u64_accumulate_i(energy_accum, simd_abs2);
  }
  energy_sum = ocudu_simd_u64_accum_reduce(energy_accum);
#endif // OCUDU_SIMD_CI16_SIZE

  for (; i != len; ++i) {
    int32_t re = x[i].real();
    int32_t im = x[i].imag();
    energy_sum += static_cast<uint64_t>(re) * static_cast<uint64_t>(re);
    energy_sum += static_cast<uint64_t>(im) * static_cast<uint64_t>(im);
  }

  float scale_sq = scale * scale;
  return static_cast<float>(energy_sum) / (static_cast<float>(len) * scale_sq);
}
