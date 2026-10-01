// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "ocudu/phy/lower/modulation/modulation_factories.h"
#include "ocudu/phy/lower/processors/downlink/pdxch/pdxch_processor.h"
#include "ocudu/phy/lower/sampling_rate.h"
#include "ocudu/ran/antenna_topology.h"
#include <memory>

namespace ocudu {

class task_executor;

/// Collects the necessary parameters to create a PDxCH processor.
struct pdxch_processor_configuration {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Sampling rate.
  sampling_rate srate;
  /// Sector bandwidth as a number of resource blocks.
  unsigned bandwidth_rb;
  /// Uplink center frequency in Hz.
  double center_freq_Hz;
  /// Transmit antenna topology.
  antenna_topology tx_ant_topology;
};

/// Lower physical layer PDxCH processor - Factory interface.
class pdxch_processor_factory
{
public:
  /// Default destructor.
  virtual ~pdxch_processor_factory() = default;

  /// Creates a lower PHY PDxCH processor.
  virtual std::unique_ptr<pdxch_processor> create(const pdxch_processor_configuration& config,
                                                  task_executor&                       modulation_executor) = 0;
};

/// \brief Creates a software based PDxCH processor factory.
///
/// \param[in] ofdm_mod_factory          OFDM modulator factory.
/// \param[in] amplitude_control_factory Amplitude control factory.
/// \return A PDxCH processor factory.
std::shared_ptr<pdxch_processor_factory>
create_pdxch_processor_factory_sw(std::shared_ptr<ofdm_modulator_factory>       ofdm_mod_factory,
                                  std::shared_ptr<amplitude_controller_factory> amplitude_control_factory);

} // namespace ocudu
