// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/support_factories.h"
#include "ocudu/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "ocudu/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include <memory>

namespace ocudu {

/// Phase Tracking Reference Signals (PT-RS) for PDSCH generator factory interface.
class ptrs_pdsch_generator_factory
{
public:
  /// Default destructor.
  virtual ~ptrs_pdsch_generator_factory() = default;

  /// Creates a PT-RS for PDSCH generator.
  virtual std::unique_ptr<ptrs_pdsch_generator> create() = 0;
};

/// \brief Creates a generic PT-RS for PDSCH generator factory.
/// \param[in] pseudo_random_gen_factory Pseudo-random sequence generator factory.
/// \return A PT-RS for PDSCH generator factory instance.
std::shared_ptr<ptrs_pdsch_generator_factory>
create_ptrs_pdsch_generator_generic_factory(std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory,
                                            std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory);

} // namespace ocudu
