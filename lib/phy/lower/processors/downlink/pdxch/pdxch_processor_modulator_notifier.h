// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"

namespace ocudu {

struct resource_grid_context;

/// Physical downlink modulator notifier interface.
class pdxch_processor_modulator_notifier
{
public:
  /// Default destructor.
  virtual ~pdxch_processor_modulator_notifier() = default;

  /// \brief Notifies the completion of the OFDM modulation for a given slot.
  /// \param[in] buffer  Baseband buffer containing the modulated slot.
  /// \param[in] context Modulated resource grid context.
  virtual void on_modulation_completion(baseband_gateway_buffer_ptr buffer, const resource_grid_context& context) = 0;
};

} // namespace ocudu
