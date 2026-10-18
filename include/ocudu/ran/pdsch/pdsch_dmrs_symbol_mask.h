// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/dmrs/dmrs.h"

namespace ocudu {

/// \brief Collects the necessary parameters to calculate the DMRS symbol mask for a TypeA PDSCH mapping and single
/// duration DMRS.
struct pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration {
  /// \brief Indicates the first OFDM symbol within the slot carrying DMRS.
  ///
  /// This value is given by higher layer parameter \c dmrs-TypeA-Position. Possible values are {2, 3}.
  dmrs_typeA_position typeA_pos;
  /// \brief Indicates the number of additional DMRS positions in time domain.
  ///
  /// This value is given by higher layer parameter \c dmrs-AdditionalPosition. Possible values are {0...3}.
  dmrs_additional_positions additional_position;
  /// \brief Last OFDM symbol scheduled for PDSCH
  ///
  /// It indicates the number of symbols between the first OFDM symbol of the slot and the last OFDM symbol of the
  /// scheduled PDSCH resources in the slot for PDSCH in OFDM symbols. It corresponds to parameter \f$l_d\f$ as
  /// per TS38.211, Section 7.4.1.1.2.
  ///
  /// Possible values are {3, ..., 14}.
  bounded_integer<uint8_t, 3, 14> last_symbol;
  /// Indicates the higher layer parameter \c lte-CRS-ToMatchAround is configured.
  bool lte_crs_match_around;
  /// Indicates the UE has indicated it is capable of \c additionalDMRS-DL-Alt.
  bool ue_capable_additional_dmrs_dl_alt;
};

/// \brief Calculates the DMRS for PDSCH symbol mask for single duration.
///
/// It is implemented as per TS38.211 Table 5.1.2.1.1-2.
///
/// Any configuration that is not covered in the table triggers an assertion.
dmrs_symbol_mask pdsch_dmrs_symbol_mask_mapping_type_A_single_get(
    const pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration& config);

/// \brief Collects the necessary parameters to calculate the DMRS symbol mask for a TypeB PDSCH mapping and single
/// duration DMRS.
struct pdsch_dmrs_symbol_mask_mapping_type_B_single_configuration {
  /// \brief Number of OFDM symbols scheduled for PDSCH.
  ///
  /// It corresponds to parameter \f$l_d\f$ as per TS38.211, Section 7.4.1.1.2. For mapping type B, \f$l_d\f$ is the
  /// duration of the allocation and the DMRS positions are relative to its first symbol.
  ///
  /// Possible values are {2, ..., 13}, as per TS38.214 Table 5.1.2.1-1 for normal cyclic prefix.
  bounded_integer<uint8_t, 2, 13> duration;
  /// \brief Index of the first OFDM symbol scheduled for PDSCH within the slot.
  ///
  /// It corresponds to \f$l_0\f$ as per TS38.211, Section 7.4.1.1.2, which for mapping type B is the first symbol of
  /// the allocation rather than a fixed position in the slot.
  bounded_integer<uint8_t, 0, 13> start_symbol;
  /// \brief Indicates the number of additional DMRS positions in time domain.
  ///
  /// This value is given by higher layer parameter \c dmrs-AdditionalPosition. Possible values are {0...3}.
  dmrs_additional_positions additional_position;
};

/// \brief Calculates the DMRS for PDSCH symbol mask for mapping type B and single duration.
///
/// It is implemented as per TS38.211 Table 7.4.1.1.2-3, columns for PDSCH mapping type B.
///
/// Any configuration that is not covered in the table triggers an assertion.
dmrs_symbol_mask pdsch_dmrs_symbol_mask_mapping_type_B_single_get(
    const pdsch_dmrs_symbol_mask_mapping_type_B_single_configuration& config);

} // namespace ocudu
