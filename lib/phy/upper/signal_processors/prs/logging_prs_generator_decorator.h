// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/support_formatters.h"
#include "ocudu/phy/upper/signal_processors/prs/formatters.h"
#include "ocudu/phy/upper/signal_processors/prs/prs_generator.h"
#include "ocudu/phy/upper/signal_processors/prs/prs_generator_configuration.h"

namespace ocudu {

template <typename Func>
std::chrono::nanoseconds time_execution(Func&& func)
{
  auto start = std::chrono::steady_clock::now();
  func();
  auto end = std::chrono::steady_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

class logging_prs_generator_decorator : public prs_generator
{
public:
  logging_prs_generator_decorator(ocudulog::basic_logger& logger_, std::unique_ptr<prs_generator> generator_) :
    logger(logger_), generator(std::move(generator_))
  {
    ocudu_assert(generator, "Invalid PRS generator.");
  }

  void generate(resource_grid_writer& grid, const prs_generator_configuration& config) override
  {
    const auto&& func = [&]() { generator->generate(grid, config); };

    std::chrono::nanoseconds time_ns = time_execution(func);

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all config fields.
      logger.debug(config.slot.sfn(), config.slot.slot_index(), "PRS: {:s} {}\n  {:n}", config, time_ns, config);
      return;
    }
    // Single line log entry.
    logger.info(config.slot.sfn(), config.slot.slot_index(), "PRS: {:s} {}", config, time_ns);
  }

private:
  ocudulog::basic_logger&        logger;
  std::unique_ptr<prs_generator> generator;
};

} // namespace ocudu
