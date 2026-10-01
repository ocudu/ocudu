// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rx_buffer_codeblock_pool.h"
#include "rx_buffer_impl.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/upper/rx_buffer_pool.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <vector>

namespace ocudu {

/// Implements a PUSCH rate matcher buffer pool.
class rx_buffer_pool_impl : public rx_buffer_pool_controller, private rx_buffer_pool
{
private:
  /// No expiration time value.
  const slot_point null_expiration = slot_point();

  /// Set to true when the buffer pool stopped.
  std::atomic<bool> stopped = {false};
  /// Codeblock buffer pool.
  rx_buffer_codeblock_pool codeblock_pool;
  /// Actual buffer pool.
  std::vector<rx_buffer_impl> buffers;
  /// \brief List of buffer identifiers.
  ///
  /// Maps buffer identifiers to buffers. Each position indicates the buffer identifier assign to each of the buffers.
  /// Buffers with \c trx_buffer_identifier::invalid() identifier are not reserved.
  std::vector<trx_buffer_identifier> identifiers;
  /// Tracks expiration times.
  std::vector<slot_point> expirations;

  /// Indicates the lifetime of a buffer reservation as a number of slots.
  unsigned expire_timeout_slots;
  /// Logger.
  ocudulog::basic_logger& logger;

  // See interface for documentation.
  unique_rx_buffer reserve(slot_point slot, trx_buffer_identifier id, unsigned nof_codeblocks, bool new_data) override;

  // See interface for documentation.
  void run_slot(slot_point slot) override;

public:
  /// \brief Creates a generic receiver buffer pool.
  /// \param[in] config Provides the pool required parameters.
  rx_buffer_pool_impl(const rx_buffer_pool_config& config) :
    codeblock_pool(config.nof_codeblocks, config.max_codeblock_size, config.external_soft_bits),
    buffers(config.nof_buffers, rx_buffer_impl(codeblock_pool)),
    identifiers(config.nof_buffers, trx_buffer_identifier::invalid()),
    expirations(config.nof_buffers, null_expiration),
    expire_timeout_slots(config.expire_timeout_slots),
    logger(ocudulog::fetch_basic_logger("PHY", true))
  {
  }

  // See rx_buffer_pool_controller interface for documentation.
  ~rx_buffer_pool_impl() override
  {
    ocudu_assert(std::none_of(buffers.begin(), buffers.end(), [](const auto& buffer) { return buffer.is_locked(); }),
                 "A buffer is still locked.");
  }

  // See rx_buffer_pool_controller interface for documentation.
  rx_buffer_pool& get_pool() override;

  // See rx_buffer_pool_controller interface for documentation.
  void stop() override;
};

} // namespace ocudu
