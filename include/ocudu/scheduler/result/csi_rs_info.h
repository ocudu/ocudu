// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_rs/csi_rs_types.h"
#include "ocudu/ran/precoding/precoding_and_beamforming_info.h"
#include "ocudu/scheduler/config/bwp_configuration.h"

namespace ocudu {

/// Stores the information associated with a CSI-RS signalling.
struct csi_rs_info {
  const bwp_configuration* bwp_cfg;
  /// Range of RBs where this CSI resource starts and ends, with relation to CRB#0. Only multiples of 4 are allowed.
  crb_interval crbs;
  csi_rs_type  type;
  /// \brief Row entry into the CSI Resource location table, as per 3GP TS 38.211, sec 7.4.1.5.3 and table 7.4.1.5.3-1.
  /// Values: {1,...,18}.
  uint8_t row;
  /// \brief Bitmap defining the frequencyDomainAllocation as per 3GPP TS 38.211, sec 7.4.1.5.3 and 3GPP TS 38.331
  /// "CSIResource Mapping".
  bounded_bitset<12, false> freq_domain;
  /// \brief The time domain location l0 and firstOFDMSymbolInTimeDomain as per 3GPP TS 38.211, sec 7.4.1.5.3.
  /// Values: {0,...,13}.
  uint8_t symbol0;
  /// \brief The time domain location l1 and firstOFDMSymbolInTimeDomain2 as per 3GPP TS 38.211, sec 7.4.1.5.3.
  uint8_t                  symbol1;
  csi_rs_cdm_type          cdm_type;
  csi_rs_freq_density_type freq_density;
  /// \brief ScramblingID of the CSI-RS as per 3GPP TS 38.214, sec 5.2.2.3.1. Values: {0,...,1023}.
  uint16_t scrambling_id;
  /// Ratio of PDSCH EPRE to NZP CSI-RS EPRE as per 3GPP TS 38.214, clause 5.2.2.3.1. Values: {-8,...,15}.
  int8_t power_ctrl_offset;
  /// Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE. Values: {-3,0,3,6}.
  int8_t power_ctrl_offset_ss;
  /// \brief Precoding and beamforming of this CSI-RS.
  /// \remark Nothing is transmitted in a ZP CSI-RS, which therefore ignores this field.
  precoding_and_beamforming_info precoding_and_beamforming;
};

} // namespace ocudu
