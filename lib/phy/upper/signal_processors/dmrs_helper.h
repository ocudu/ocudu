// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/phy/support/mask_types.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"
#include <array>

namespace ocudu {

/// \brief Helper function to generate DM-RS sequences.
///
/// This function generates DM-RS sequences based on a pseudo-random sequence generation. It generates \c
/// nof_dmrs_per_rb for every active resource block given in \c crb_mask and skips the \c nof_dmrs_per_rb for every
/// inactive resource block.
///
/// \param[out]    sequence             Destination storage for the generated sequence.
/// \param[in,out] prg                  Pseudo-random generator to use.
/// \param[in]     amplitude            Generated sequence amplitude.
/// \param[in]     reference_point_k_rb Reference Common Resource Block index (CRB).
/// \param[in]     nof_dmrs_per_rb      Number of DM-RS to generate per active CRB.
/// \param[in]     crb_mask             Active resource blocks.
/// \note It assumes that the pseudo-random generator is initialized.
/// \note The sequence size must be consistent with the number of active RBs and the number of DM-RS per RB.
void dmrs_sequence_generate(span<cf_t>               sequence,
                            pseudo_random_generator& prg,
                            float                    amplitude,
                            unsigned                 reference_point_k_rb,
                            unsigned                 nof_dmrs_per_rb,
                            const crb_bitmap&        crb_mask);

/// Parameters for PDSCH and PUSCH DM-RS.
struct dmrs_pxsch_parameters {
  /// DM-RS frequency allocation within a resource block.
  re_prb_mask re_pattern;
  /// Frequency domain weights.
  std::array<float, 2> w_f;
  /// Time domain weights.
  std::array<float, 2> w_t;
};

/// \brief Retrieves DM-RS parameters for both PUSCH and PDSCH channels.
///
/// The DM-RS parameters for PUSCH type 1 and 2 configuration are given in TS38.211 Table 6.4.1.1.3-1 and Table
/// 6.4.1.1.3-2, respectively.
///
/// The DM-RS parameters for PDSCH type 1 and 2 configuration are given in TS38.211 Table 7.4.1.1.2-1 and Table
/// 7.4.1.1.2-2, respectively.
///
/// \param type        Specifies the DM-RS configuration type.
/// \param i_dmrs_port The DM-RS port index, indicating the specific configuration to retrieve.
/// \return The DM-RS parameters for the specified channel and configuration type.
dmrs_pxsch_parameters get_pxsch_dmrs_params(dmrs_config_type type, unsigned i_dmrs_port);

} // namespace ocudu
