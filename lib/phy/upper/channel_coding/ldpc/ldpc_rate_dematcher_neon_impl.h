// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ldpc_rate_dematcher_impl.h"

namespace ocudu {

/// \brief LDPC Rate dematcher specialized with NEON instruction set.
///
/// It implements \ref ldpc_rate_dematcher interface. It relies on NEON instruction sets.
class ldpc_rate_dematcher_neon_impl : public ldpc_rate_dematcher_impl
{
protected:
  // See interface for documentation.
  void combine_softbits(span<log_likelihood_ratio>       out,
                        span<const log_likelihood_ratio> in0,
                        span<const log_likelihood_ratio> in1) const override;

  // See interface for documentation.
  void deinterleave_qpsk(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;

  // See interface for documentation.
  void deinterleave_qam16(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;

  // See interface for documentation.
  void deinterleave_qam64(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;

  // See interface for documentation.
  void deinterleave_qam256(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const override;
};

} // namespace ocudu
