// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/upper/signal_processors/srs/srs_estimator.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

/// Concurrent Sounding Reference Signal propagation channel estimator pool.
class srs_estimator_pool : public srs_estimator
{
public:
  using estimator_pool = bounded_unique_object_pool<srs_estimator>;

  /// Creates a pool from a list of estimators. Ownership is transferred to the pool.
  explicit srs_estimator_pool(std::shared_ptr<estimator_pool> estimators_) :
    logger(ocudulog::fetch_basic_logger("PHY")), estimators(std::move(estimators_))
  {
  }

  // See interface for documentation.
  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override
  {
    auto estimator = estimators->get();
    if (!estimator) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve NZP-CSI-RS generator.");
      return {};
    }
    return estimator->estimate(grid, config);
  }

private:
  ocudulog::basic_logger&         logger;
  std::shared_ptr<estimator_pool> estimators;
};

} // namespace ocudu
