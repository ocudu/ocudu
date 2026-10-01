// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/upper/signal_processors/prs/prs_generator.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

/// \brief Concurrent PRS generator pool.
///
/// Wraps a concurrent pool of PRS generators. It assigns a generator to each thread.
class prs_generator_pool : public prs_generator
{
public:
  /// PRS generator pool type.
  using generator_pool_type = bounded_unique_object_pool<prs_generator>;

  /// Creates a SSB processor pool from a list of processors. Ownership is transferred to the pool.
  explicit prs_generator_pool(std::shared_ptr<generator_pool_type> processors_) :
    logger(ocudulog::fetch_basic_logger("PHY")), processors(std::move(processors_))
  {
    ocudu_assert(processors, "Invalid processor pool.");
  }

  // See interface for documentation.
  void generate(resource_grid_writer& grid, const prs_generator_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error("Failed to retrieve PRS generator.");
      return;
    }
    return processor->generate(grid, config);
  }

private:
  ocudulog::basic_logger&              logger;
  std::shared_ptr<generator_pool_type> processors;
};

} // namespace ocudu
