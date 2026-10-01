// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/support_formatters.h"
#include "ocudu/phy/upper/signal_processors/srs/formatters.h"
#include "ocudu/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "ocudu/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"

namespace ocudu {

/// Logging Sounding Reference Signal channel estimator decorator.
class logging_srs_estimator_decorator : public srs_estimator
{
  template <typename Func>
  static std::chrono::nanoseconds time_execution(Func&& func)
  {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  }

public:
  /// Creates a logging decorator for SRS channel estimator.
  logging_srs_estimator_decorator(ocudulog::basic_logger& logger_, std::unique_ptr<srs_estimator> srs_) :
    logger(logger_), srs(std::move(srs_))
  {
    ocudu_assert(srs, "Invalid SRS estimator instance.");
  }

  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override
  {
    srs_estimator_result     result;
    std::chrono::nanoseconds time_ns = time_execution([&]() { result = srs->estimate(grid, config); });

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all PDU fields.
      logger.debug(config.slot.sfn(),
                   config.slot.slot_index(),
                   "SRS: {:s} {:s} {:s} {}\n  {:n}\n  {:n}\n  {:n}",
                   config,
                   config.resource,
                   result,
                   time_ns,
                   config,
                   config.resource,
                   result);
    } else {
      // Single line log entry.
      logger.info(config.slot.sfn(),
                  config.slot.slot_index(),
                  "SRS: {:s} {:s} {:s} {}",
                  config,
                  config.resource,
                  result,
                  time_ns);
    }

    return result;
  }

private:
  ocudulog::basic_logger&        logger;
  std::unique_ptr<srs_estimator> srs;
};

} // namespace ocudu
