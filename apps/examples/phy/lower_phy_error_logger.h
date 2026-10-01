// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/lower/lower_phy_error_notifier.h"
#include "ocudu/phy/support/resource_grid_context.h"

namespace ocudu {

/// Implements a lower physical layer error adapter to an RU error adapter.
class lower_phy_error_logger : public lower_phy_error_notifier
{
public:
  /// Creates an adapter with a given logger.
  explicit lower_phy_error_logger(ocudulog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void on_late_resource_grid(const resource_grid_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in lower PHY: Downlink data late for sector {} and slot {}.",
                   context.sector,
                   context.slot);
  }

  // See interface for documentation.
  void on_prach_request_late(const prach_buffer_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in lower PHY: PRACH request late for sector {}, slot {} and start symbol {}.",
                   context.sector,
                   context.slot,
                   context.start_symbol);
  }

  // See interface for documentation.
  void on_prach_request_overflow(const prach_buffer_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in lower PHY: PRACH request overflow for sector {}, slot {} and start symbol {}.",
                   context.sector,
                   context.slot,
                   context.start_symbol);
  }

  // See interface for documentation.
  void on_puxch_request_late(const resource_grid_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in lower PHY: PUxCH request late for sector {}, slot {}.",
                   context.sector,
                   context.slot);
  }

private:
  /// Logger.
  ocudulog::basic_logger& logger;
};

} // namespace ocudu
