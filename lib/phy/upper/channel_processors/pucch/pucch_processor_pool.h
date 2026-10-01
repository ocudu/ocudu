// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

/// Concurrent PUCCH processor pool.
class pucch_processor_pool : public pucch_processor
{
public:
  using processor_pool = bounded_unique_object_pool<pucch_processor>;

  /// Creates a PUCCH processor pool from a shared pool of processors.
  explicit pucch_processor_pool(std::shared_ptr<processor_pool> processors_) :
    logger(ocudulog::fetch_basic_logger("PHY")), processors(std::move(processors_))
  {
    ocudu_assert(processors, "Invalid PUCCH processor pool.");
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve PUCCH processor for Format 0.");
      return {};
    }
    return processor->process(grid, config);
  }

  // See interface for documentation.
  pucch_format1_map<pucch_processor_result> process(const resource_grid_reader&        grid,
                                                    const format1_batch_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.common_config.slot.sfn(),
                   config.common_config.slot.slot_index(),
                   "Failed to retrieve PUCCH processor for Format 1.");
      return {};
    }
    return processor->process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve PUCCH processor for Format 2.");
      return {};
    }
    return processor->process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve PUCCH processor for Format 3.");
      return {};
    }
    return processor->process(grid, config);
  }

  // See interface for documentation.
  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(config.slot.sfn(), config.slot.slot_index(), "Failed to retrieve PUCCH processor for Format 4.");
      return {};
    }
    return processor->process(grid, config);
  }

private:
  ocudulog::basic_logger&         logger;
  std::shared_ptr<processor_pool> processors;
};

} // namespace ocudu
