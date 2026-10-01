// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/ocuduvec/simd.h"

namespace ocudu {

#if OCUDU_SIMD_CF_SIZE

template <unsigned N, unsigned M>
inline void squared_gram_matrix(simd_cf_t out[M][M], const simd_cf_t in[N][M])
{
  for (unsigned i = 0; i != M; ++i) {
    for (unsigned j = 0; j != M; ++j) {
      simd_cf_t result = ocudu_simd_cf_set1(0);
      for (unsigned k = 0; k != N; ++k) {
        result += ocudu_simd_cf_conjprod(in[k][i], in[k][j]);
      }
      out[i][j] = result;
    }
  }
}

#endif // OCUDU_SIMD_CF_SIZE

} // namespace ocudu
