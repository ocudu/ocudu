// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

/// \brief Concurrent SSB processor pool.
///
/// Wraps a concurrent pool of SSB processors. It assigns a SSB processor to each thread that processes SSB.
class ssb_processor_pool : public ssb_processor
{
public:
  /// SSB processor pool type.
  using ssb_processor_pool_type = bounded_unique_object_pool<ssb_processor>;

  /// Creates a SSB processor pool from a list of processors. Ownership is transferred to the pool.
  explicit ssb_processor_pool(std::shared_ptr<ssb_processor_pool_type> processors_) :
    logger(ocudulog::fetch_basic_logger("PHY")), processors(std::move(processors_))
  {
    ocudu_assert(processors, "Invalid processor pool.");
  }

  // See interface for documentation.
  void process(resource_grid_writer& grid, const pdu_t& pdu) override
  {
    auto processor = processors->get();
    if (!processor) {
      logger.error(pdu.slot.sfn(), pdu.slot.slot_index(), "Failed to retrieve SS/PBCH block processor.");
      return;
    }
    return processor->process(grid, pdu);
  }

private:
  ocudulog::basic_logger&                  logger;
  std::shared_ptr<ssb_processor_pool_type> processors;
};

} // namespace ocudu
