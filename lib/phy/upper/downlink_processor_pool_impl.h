// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "processor_pool_helpers.h"
#include "ocudu/phy/upper/downlink_processor.h"

namespace ocudu {

/// Describes all downlink processors in a pool.
struct downlink_processor_pool_impl_config {
  /// Downlink processors for a given numerology.
  struct sector_dl_processor {
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Pointers to the actual downlink processors.
    std::vector<std::unique_ptr<downlink_processor_base>> procs;
  };

  /// Collection of all downlink processors, organized by numerology.
  std::vector<sector_dl_processor> procs;
};

/// Implementation of a downlink processor pool.
class downlink_processor_pool_impl : public downlink_processor_pool
{
public:
  /// \brief Constructs a downlink processor pool using the given configuration.
  ///
  /// \param[in] dl_processors Pool configuration.
  explicit downlink_processor_pool_impl(downlink_processor_pool_impl_config dl_processors);

  // See interface for documentation.
  downlink_processor_controller& get_processor_controller(slot_point slot) override;

  // See interface for documentation.
  void stop() override;

private:
  /// Container for downlink processors. Each entry belongs to a different sector.
  processor_pool_repository<downlink_processor_base> processors;
};

} // namespace ocudu
