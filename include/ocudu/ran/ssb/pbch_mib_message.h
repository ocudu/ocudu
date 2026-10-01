// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/ssb/ssb_properties.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <cstdint>

namespace ocudu {

/// \brief Collects the PBCH message contents for the Master Information Block (MIB) parameters.
///
/// The MIB is defined by Information Element \e MIB in TS38.331 Section 6.2.2.
struct pbch_mib_message {
  /// System Frame Number (0..1023). Parameter \e systemFrameNumber.
  uint32_t sfn;
  /// \c true if the SS/PBCH block transmission is in an odd half frame, \c false otherwise.
  bool hrf;
  /// Parameter \e subCarrierSpacingCommon.
  subcarrier_spacing scs_common;
  /// Parameter \e ssb-SubcarrierOffset.
  ssb_subcarrier_offset subcarrier_offset;
  /// Field \e dmrs-TypeA-Position.
  dmrs_typeA_position dmrs_typeA_pos;
  /// Field \e pdcch-ConfigSIB1.
  uint8_t pdcch_config_sib1;
  /// Field \e pdcch-ConfigSIB1.
  bool cell_barred;
  /// Field \e intraFreqReselection.
  bool intrafreq_reselection;
  /// SSB opportunity index in a 5ms burst.
  uint8_t ssb_block_index;
};

} // namespace ocudu
