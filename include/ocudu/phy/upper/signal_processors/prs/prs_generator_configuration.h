// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/precoding_beamforming_configuration.h"
#include "ocudu/ran/prs/prs.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// \brief Positioning Reference Signals (PRS) transmission parameters.
///
/// \remark Not all combinations of comb size and time domain duration are valid, see \ref
/// prs_valid_num_symbols_and_comb_size for more information.
struct prs_generator_configuration {
  /// Slot and subcarrier spacing.
  slot_point slot;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// \brief Sequence identifier.
  ///
  /// Parameter \f$n_{ID,seq}^{PRS}\f$ in TS38.211 Section 7.4.1.7.2. The range is {0, ..., 4095}. It is given by the
  /// higher layer parameter \e dl-PRS-SequenceID.
  ///
  uint16_t n_id_prs;
  /// \brief Transmission comb size.
  ///
  /// Parameter \f$K_{comb}^{PRS}\f$ in TS38.211 Section 7.4.1.7.3. It is given by the higher layer parameter \e
  /// dl-PRS-CombSizeN.
  prs_comb_size comb_size;
  /// \brief Transmission comb offset.
  ///
  /// Parameter \f$k_{offset}^{PRS}\f$ in TS38.211 Section 7.4.1.7.3. It is given by the higher layer parameter \e
  /// dl-PRS-CombSizeN-AndReOffset
  uint8_t comb_offset;
  /// \brief Transmission time domain duration.
  ///
  /// Parameter \f$L_{PRS}\f$ in TS38.211 Section 7.4.1.7.3. It is given by the higher layer parameter \e
  /// dl-PRS-NumSymbols.
  prs_num_symbols duration;
  /// \brief First symbol index of the transmission.
  ///
  /// Parameter \f$l_{start}^{PRS}\f$ in TS38.211 Section 7.4.1.7.3. It is given by the higher layer parameter \e
  /// dl-PRS-ResourceSymbolOffset.
  uint8_t start_symbol;
  /// \brief Physical resource block allocation start PRB.
  ///
  /// It is given by the higher layer parameter \e dl-PRS-StartPRB. The range is {0, ..., 2176}.
  ///
  /// It expresses the number of PRB between the PointA given by the higher layer parameter \e dl-PRS-PointA and the
  /// lowest PRB of the PRS transmission.
  uint16_t prb_start;
  /// \brief Physical resource block allocation expressed as a start and number of physical resource blocks.
  ///
  /// The start position is relative to the lowest resource grid physical resource block. Set the start position equal
  /// to \c prb_start, if the higher layer parameter \e dl-PRS-PointA matches with the lowest subcarrier of the resource
  /// grid.
  ///
  /// The number of PRB is derived from the higher layer parameter \e dl-PRS-ResourceBandwidth. The range is
  /// {24, ..., 276} in steps of 4.
  interval<uint16_t> freq_alloc;
  /// Ratio of PRS data EPRE to SSS EPRE in decibels.
  float power_offset_dB;
  /// Precoding and beamforming configuration.
  precoding_beamforming_configuration precoding_and_beamforming;
};

} // namespace ocudu
