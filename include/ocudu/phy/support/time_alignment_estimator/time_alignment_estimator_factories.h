// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/generic_functions/generic_functions_factories.h"
#include "ocudu/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include <memory>

namespace ocudu {

/// Time alignment estimator factory interface.
class time_alignment_estimator_factory
{
public:
  /// Default destructor.
  virtual ~time_alignment_estimator_factory() = default;

  /// Creates a time alignment estimator.
  virtual std::unique_ptr<time_alignment_estimator> create() = 0;
};

/// Creates a time alignment estimator factory based on DFT.
std::shared_ptr<time_alignment_estimator_factory>
create_time_alignment_estimator_dft_factory(std::shared_ptr<dft_processor_factory> dft_proc_factory);

} // namespace ocudu
