// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "ocudu/ran/ptrs/ptrs_constants.h"

namespace ocudu {

/// Generic implementation of PT-RS for PDSCH generator.
class ptrs_pdsch_generator_generic_impl : public ptrs_pdsch_generator
{
public:
  /// Constructs a PT-RS for PDSCH generator with the required dependencies.
  ptrs_pdsch_generator_generic_impl(std::unique_ptr<pseudo_random_generator> pseudo_random_gen_,
                                    std::unique_ptr<resource_grid_mapper>    mapper_) :
    pseudo_random_gen(std::move(pseudo_random_gen_)), mapper(std::move(mapper_))
  {
    ocudu_assert(pseudo_random_gen, "Invalid sequence generator.");
  }

  // See interface for documentation.
  void generate(resource_grid_writer& grid, const configuration& config) override;

private:
  /// Pseudo-random sequence generator.
  std::unique_ptr<pseudo_random_generator> pseudo_random_gen;
  /// Resource grid mapper.
  std::unique_ptr<resource_grid_mapper> mapper;
  /// Temporary sequence.
  static_vector<cf_t, MAX_NOF_SUBCARRIERS>                      dmrs_sequence;
  static_re_buffer<ptrs_constants::max_nof_ports, MAX_NOF_PRBS> sequence;
};

} // namespace ocudu
