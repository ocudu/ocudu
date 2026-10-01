// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"

namespace ocudu {

/// \brief Soft-demodulates QPSK modulation.
/// \param[out] llrs       Resultant log-likelihood ratios.
/// \param[in]  symbols    Input constellation symbols.
/// \param[in]  noise_vars Noise variance for each symbol in the constellation.
void demodulate_soft_QPSK(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars);

} // namespace ocudu
