// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/srs/srs_resource_configuration.h"
#include <optional>

namespace ocudu {

/// Antenna ports mapping information for PUSCH when using DCI format 0_1. See TS 38.212 Tables 7.3.1.1.2-6 to
/// 7.3.1.1.2-23.
struct pusch_antenna_ports_mapping {
  /// Number of DMRS CDM group(s) without data.
  unsigned nof_dmrs_cdm_groups_without_data;
  /// DM-RS ports used. Bit \f$p\f$ corresponds to the DM-RS port \f$p\f$.
  dmrs_port_mask dmrs_ports;
  /// Number of front-load DM-RS symbols.
  unsigned nof_front_load_symbols;

  bool operator==(const pusch_antenna_ports_mapping& other) const
  {
    return nof_dmrs_cdm_groups_without_data == other.nof_dmrs_cdm_groups_without_data &&
           dmrs_ports == other.dmrs_ports && nof_front_load_symbols == other.nof_front_load_symbols;
  }
  bool operator!=(const pusch_antenna_ports_mapping& other) const { return !(*this == other); }
};

/// \brief Gets the PUSCH antenna ports mapping table.
///
/// The size of the Antenna ports field of DCI format 0_1 is not given by the size of the table, as the field has the
/// same size for all the numbers of layers (see TS 38.212 Section 7.3.1.1.2).
///
/// \param transform_precoder Set to \c true if the transform precoder is used.
/// \param dmrs_cfg_type      PUSCH DM-RS configuration type.
/// \param dmrs_max_len       PUSCH DM-RS maximum number of OFDM symbols.
/// \param nof_layers         Number of layers of the PUSCH.
/// \return A constant span view to the TS 38.212 table of Tables 7.3.1.1.2-6 to 7.3.1.1.2-23 associated with the given
///         parameters. The view is empty if the table is not supported, i.e., for DM-RS type 2, or if the number of
///         layers is not valid.
span<const pusch_antenna_ports_mapping> get_pusch_antenna_port_mapping_table(bool             transform_precoder,
                                                                             dmrs_config_type dmrs_cfg_type,
                                                                             dmrs_max_length  dmrs_max_len,
                                                                             unsigned         nof_layers);

/// \brief Finds the row of TS 38.212 Tables 7.3.1.1.2-6 to 7.3.1.1.2-23 that signals the given PUSCH antenna ports
/// mapping.
///
/// The table is selected from the transform precoder, the DM-RS configuration and the number of layers, which is the
/// number of DM-RS ports.
///
/// \param transform_precoder Set to \c true if the transform precoder is used.
/// \param dmrs_cfg_type      PUSCH DM-RS configuration type.
/// \param dmrs_max_len       PUSCH DM-RS maximum number of OFDM symbols.
/// \param mapping            DM-RS ports, number of CDM groups without data and number of front-load symbols to signal.
///                           The DM-RS ports mask must have \c DMRS_MAX_NPORTS bits.
/// \return The row index, i.e., the value of the Antenna ports field of DCI format 0_1, or \c std::nullopt if no row
///         matches the given mapping.
std::optional<unsigned> find_pusch_antenna_port_mapping_row_index(bool                               transform_precoder,
                                                                  dmrs_config_type                   dmrs_cfg_type,
                                                                  dmrs_max_length                    dmrs_max_len,
                                                                  const pusch_antenna_ports_mapping& mapping);

/// \brief Returns the PUSCH precoding information mapping row index in TS 38.212, table 7.3.1.1.2-2/3/4/5.
/// \param[in] nof_layers Number of layers.
/// \param[in] max_rank   Maximum number of transmission layers.
/// \param[in] tpmi       PUSCH TPMI.
/// \return The PUSCH precoding information mapping row index.
unsigned get_pusch_precoding_info_row_index(unsigned                                      nof_layers,
                                            unsigned                                      max_rank,
                                            srs_resource_configuration::one_two_four_enum nof_srs_ports,
                                            unsigned                                      tpmi);

} // namespace ocudu
