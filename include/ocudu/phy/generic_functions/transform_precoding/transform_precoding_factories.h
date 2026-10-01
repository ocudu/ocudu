// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Transform precoding factories.

#pragma once

#include "ocudu/phy/generic_functions/generic_functions_factories.h"
#include "ocudu/phy/generic_functions/transform_precoding/transform_precoder.h"
#include <memory>

namespace ocudu {

/// Factory that builds transform precoder objects.
class transform_precoder_factory
{
public:
  /// Default destructor.
  virtual ~transform_precoder_factory() = default;

  /// Creates and returns a transform precoder object.
  virtual std::unique_ptr<transform_precoder> create() = 0;
};

/// \brief Creates and returns a transform precoder factory based on the DFT algorithm.
///
/// \return A transform precoder factory.
std::shared_ptr<transform_precoder_factory>
create_dft_transform_precoder_factory(std::shared_ptr<dft_processor_factory>, unsigned max_nof_prb);

} // namespace ocudu
