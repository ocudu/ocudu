// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/shared_prach_buffer.h"
#include "ocudu/phy/upper/uplink_request_processor.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

class upper_phy_rx_symbol_request_notifier;
class shared_resource_grid;

/// \brief Implementation of the uplink request processor interface.
///
/// This implementation processes uplink data requests according to the following steps.
///     1. It resolves all the dependencies.
///     2. It calls a method from the internal upper_phy_rx_symbol_request_notifier object. This will produce and notify
///        an event that requests the capture of slot symbol data.
class uplink_request_processor_impl : public uplink_request_processor
{
public:
  uplink_request_processor_impl(upper_phy_rx_symbol_request_notifier&       rx_symbol_request_notifier_,
                                std::vector<std::unique_ptr<prach_buffer>>& prach_buffers,
                                ocudulog::basic_logger&                     logger_);

  // See interface for documentation.
  void process_prach_request(const prach_buffer_context& context) override;

  // See interface for documentation.
  void process_uplink_slot_request(const resource_grid_context& context, const shared_resource_grid& grid) override;

private:
  /// Symbol request notifier.
  upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier;
  /// PRACH buffer pool.
  prach_buffer_pool       prach_pool;
  ocudulog::basic_logger& logger;
};

} // namespace ocudu
