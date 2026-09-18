// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/srs/srs_constants.h"
#include "ocudu/ran/srs/srs_context.h"
#include "ocudu/ran/srs/srs_resource_configuration.h"
#include <cstdint>

namespace ocudu {

/// Parameters required to receive the Sounding Reference Signals described in 3GPP TS38.211 Section 6.4.1.4.
struct srs_estimator_configuration {
  /// Context information.
  std::optional<srs_context> context;
  /// Numerology, SFN and slot index.
  slot_point slot;
  /// \brief Slot delay with which the message is processed.
  ///
  /// Determines the number of slot that have passed since the UE transmitted to the gNB received. In other words,
  /// the UE transmitted in slot \e n while the gNB is receiving in slot <em>n + slot_offset</em>.
  ///
  /// The effect of this offset is applied to the sequence generators that depend on the slot.
  ///
  /// This parameter is derived from the higher layers K-MAC which is specific for NTN.
  unsigned slot_offset;
  /// SRS resource configuration.
  srs_resource_configuration resource;
  /// Receive port list.
  static_vector<uint8_t, srs_constants::max_nof_rx_ports> ports;
};

} // namespace ocudu
