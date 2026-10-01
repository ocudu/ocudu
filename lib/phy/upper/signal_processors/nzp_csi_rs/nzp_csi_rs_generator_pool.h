// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/signal_processors/nzp_csi_rs/nzp_csi_rs_generator.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

/// \brief Concurrent NZP-CSI-RS generator pool.
///
/// Wraps a concurrent pool of NZP-CSI-RS generators. It assigns a generator to each thread.
class nzp_csi_rs_generator_pool : public nzp_csi_rs_generator
{
public:
  /// NZP-CSI-RS generator pool type.
  using generator_pool_type = bounded_unique_object_pool<nzp_csi_rs_generator>;

  /// Creates a NZP-CSI-RS generator pool from a list of processors. Ownership is transferred to the pool.
  explicit nzp_csi_rs_generator_pool(std::shared_ptr<generator_pool_type> generators_) :
    logger(ocudulog::fetch_basic_logger("PHY")), generators(std::move(generators_))
  {
    ocudu_assert(generators, "Invalid generator pool.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override
  {
    auto processor = generators->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve NZP-CSI-RS generator.");
      return;
    }
    processor->map(grid, config);
  }

private:
  ocudulog::basic_logger&              logger;
  std::shared_ptr<generator_pool_type> generators;
};

} // namespace ocudu
