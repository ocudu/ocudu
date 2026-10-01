// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/processors/downlink/downlink_processor.h"
#include "ocudu/phy/lower/processors/downlink/pdxch/pdxch_processor_factories.h"
#include "ocudu/phy/lower/sampling_rate.h"
#include <memory>

namespace ocudu {

/// Describes a sector configuration.
struct downlink_processor_configuration {
  /// Sector identifier.
  unsigned sector_id;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Cyclic prefix configuration.
  cyclic_prefix cp;
  /// Baseband sampling rate.
  sampling_rate rate;
  /// Bandwidth in PRB.
  unsigned bandwidth_prb;
  /// Center frequency in Hz.
  double center_frequency_Hz;
  /// Transmit antenna topology.
  antenna_topology tx_ant_topology;
  /// Number of slots notified in advance in the TTI boundary event.
  unsigned nof_slot_tti_in_advance;
};

/// Lower physical layer downlink processor - Factory interface.
class lower_phy_downlink_processor_factory
{
public:
  /// Default destructor.
  virtual ~lower_phy_downlink_processor_factory() = default;

  /// Creates a lower PHY downlink processor.
  virtual std::unique_ptr<lower_phy_downlink_processor> create(const downlink_processor_configuration& config,
                                                               task_executor& modulation_executor) = 0;
};

/// Creates a software based downlink processor factory.
std::shared_ptr<lower_phy_downlink_processor_factory>
create_downlink_processor_factory_sw(std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory);

} // namespace ocudu
