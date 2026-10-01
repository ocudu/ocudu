// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/signal_processors/srs/srs_estimator_configuration_validator.h"

namespace ocudu {

/// \brief Implements a Sounding Reference Signal channel estimator configuration validator.
///
/// The validator is aligned with \ref srs_estimator_generic_impl implementation of \ref srs_estimator.
class srs_validator_generic_impl : public srs_estimator_configuration_validator
{
public:
  /// Constructs an SRS estimator configuration validator with the maximum number of PRB.
  srs_validator_generic_impl(unsigned max_nof_prb_) : max_nof_prb(max_nof_prb_) {}

  // See interface for documentation.
  error_type<std::string> is_valid(const srs_estimator_configuration& config) const override;

private:
  unsigned max_nof_prb;
};

} // namespace ocudu
