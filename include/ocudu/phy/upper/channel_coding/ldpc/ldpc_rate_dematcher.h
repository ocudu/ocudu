// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC rate dematching.
#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/codeblock_metadata.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include "ocudu/ran/sch/modulation_scheme.h"

namespace ocudu {

/// LDPC rate dematcher interface. It reverts the operations of ldpc_rate_matcher.
class ldpc_rate_dematcher
{
public:
  /// Default virtual destructor.
  virtual ~ldpc_rate_dematcher() = default;

  /// \brief Recovers a full codeblock from its rate-matched version.
  ///
  /// This function also takes care of combining repeated codeblocks for H-ARQ schemes (both chase combining and
  /// incremental redundancy).
  ///
  /// Log-likelihood ratios corresponding to filler bits are set to \c INT8_MAX. Log-likelihood ratios that cannot
  /// be determined from the input are set to zero.
  /// \param[in,out] output          Full-length codeblock (log-likelihood ratios).
  /// \param[in]     input           Rate-matched codeblock (log-likelihood ratios).
  /// \param[in]     new_data        Boolean flag that indicates whether the input corresponds to a new codeblock (if
  ///                                true) or to a retransmission of an old one (if false).
  /// \param[in]     cfg             Configuration parameters.
  /// \remark The sizes of \c input and \c output determine the behavior of the rate recovering algorithm.
  virtual void rate_dematch(span<log_likelihood_ratio>       output,
                            span<const log_likelihood_ratio> input,
                            bool                             new_data,
                            const codeblock_metadata&        cfg) = 0;
};

} // namespace ocudu
