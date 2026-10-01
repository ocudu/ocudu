// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_processor.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

/// \brief Concurrent PDCCH processor pool.
///
/// Wraps a concurrent pool of PDCCH processors. It assigns a PDCCH processor to each thread that processes PDCCH.
class pdcch_processor_pool : public pdcch_processor
{
public:
  /// PDCCH processor pool type.
  using pdcch_processor_pool_type = bounded_unique_object_pool<pdcch_processor>;

  /// Creates a PDCCH processor pool from a list of processors. Ownership is transferred to the pool.
  explicit pdcch_processor_pool(std::shared_ptr<pdcch_processor_pool_type> processors_) :
    logger(ocudulog::fetch_basic_logger("PHY")), processors(std::move(processors_))
  {
    ocudu_assert(processors, "Invalid processor pool.");
  }

  // See interface for documentation.
  void process(resource_grid_writer& grid, const pdu_t& pdu) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(pdu.slot.sfn(), pdu.slot.slot_index(), "Failed to retrieve PDCCH processor.");
      return;
    }
    processor->process(grid, pdu);
  }

private:
  ocudulog::basic_logger&                    logger;
  std::shared_ptr<pdcch_processor_pool_type> processors;
};

} // namespace ocudu
