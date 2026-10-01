// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/accumulate.h"
#include "ocudu/ocuduvec/simd.h"
#include "ocudu/support/ocudu_assert.h"
#include <numeric>

using namespace ocudu;
using namespace ocuduvec;

static float accumulate_f_simd(const float* x, unsigned len)
{
  float    result = 0;
  unsigned i      = 0;

#if OCUDU_SIMD_F_SIZE
  if (len >= OCUDU_SIMD_F_SIZE) {
    simd_f_t simd_result = ocudu_simd_f_loadu(x + i);
    i += OCUDU_SIMD_F_SIZE;

    for (unsigned simd_end = OCUDU_SIMD_F_SIZE * (len / OCUDU_SIMD_F_SIZE); i != simd_end; i += OCUDU_SIMD_F_SIZE) {
      simd_f_t simd_x = ocudu_simd_f_loadu(x + i);

      simd_result = ocudu_simd_f_add(simd_x, simd_result);
    }

    alignas(SIMD_BYTE_ALIGN) std::array<float, OCUDU_SIMD_F_SIZE> simd_vector_sum;
    ocudu_simd_f_store(simd_vector_sum.data(), simd_result);
    result = std::accumulate(simd_vector_sum.begin(), simd_vector_sum.end(), result);
  }
#endif // OCUDU_SIMD_F_SIZE

  for (; i != len; ++i) {
    result += x[i];
  }

  return result;
}

float ocudu::ocuduvec::accumulate(span<const float> x)
{
  return accumulate_f_simd(x.data(), x.size());
}
