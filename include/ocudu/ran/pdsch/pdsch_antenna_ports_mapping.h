// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include <optional>

namespace ocudu {

/// Antenna ports mapping information for PDSCH when using DCI format 1_1. See TS 38.212 Tables 7.3.1.2.2-1/2/3/4.
struct pdsch_antenna_ports_mapping {
  /// Number of DMRS CDM group(s) without data.
  unsigned nof_dmrs_cdm_groups_without_data;
  /// DM-RS ports used. Bit \f$p\f$ corresponds to the antenna port \f$1000+p\f$.
  dmrs_port_mask dmrs_ports;
  /// Number of front-load DM-RS symbols.
  unsigned nof_front_load_symbols;

  bool operator==(const pdsch_antenna_ports_mapping& other) const
  {
    return nof_dmrs_cdm_groups_without_data == other.nof_dmrs_cdm_groups_without_data &&
           dmrs_ports == other.dmrs_ports && nof_front_load_symbols == other.nof_front_load_symbols;
  }
  bool operator!=(const pdsch_antenna_ports_mapping& other) const { return !(*this == other); }
};

/// \brief Gets the PDSCH antenna ports mapping table.
///
/// Retrieves the antenna port mapping table for PDSCH transmission based on the specified DM-RS configuration
/// parameters. The mapping table defines how antenna ports are assigned to PDSCH resources according to TS 38.212
/// Section 7.3.1.2.2.
///
/// \param dmrs_cfg_type        UE dedicated DM-RS type configuration.
/// \param dmrs_max_len         UE dedicated DM-RS maximum length configuration.
/// \param are_both_cws_enabled Set to true if both codewords are enabled for PDSCH transmission.
/// \return A constant span view to the PDSCH antenna ports mapping table associated with the given parameters. The view
///         is empty if TS 38.212 does not define a table for two codewords with the given DM-RS configuration.
/// \remark A fatal failure is triggered if the DM-RS type or maximum length are not set.
span<const pdsch_antenna_ports_mapping> get_pdsch_antenna_port_mapping_table(dmrs_config_type dmrs_cfg_type,
                                                                             dmrs_max_length  dmrs_max_len,
                                                                             bool             are_both_cws_enabled);

/// \brief Gets the size of the Antenna port(s) field of DCI format 1_1, as per TS 38.212 Section 7.3.1.2.2.
/// \param dmrs_cfg_type PDSCH DM-RS configuration type.
/// \param dmrs_max_len  PDSCH DM-RS maximum number of OFDM symbols.
/// \return The number of bits of the field, i.e., 4, 5 or 6.
unsigned get_pdsch_antenna_ports_field_size(dmrs_config_type dmrs_cfg_type, dmrs_max_length dmrs_max_len);

/// \brief Finds the row of TS 38.212 Tables 7.3.1.2.2-1/2/3/4 that signals the given PDSCH antenna ports mapping.
///
/// The table is selected from the DM-RS configuration and from the number of codewords, which is given by the number of
/// DM-RS ports as per TS 38.211 Table 7.3.1.3-1.
///
/// \param dmrs_cfg_type PDSCH DM-RS configuration type.
/// \param dmrs_max_len  PDSCH DM-RS maximum number of OFDM symbols.
/// \param mapping       DM-RS ports, number of CDM groups without data and number of front-load symbols to signal.
/// \return The row index, i.e., the value of the Antenna port(s) field of DCI format 1_1, or \c std::nullopt if no row
///         matches the given mapping.
std::optional<unsigned> find_pdsch_antenna_port_mapping_row_index(dmrs_config_type                   dmrs_cfg_type,
                                                                  dmrs_max_length                    dmrs_max_len,
                                                                  const pdsch_antenna_ports_mapping& mapping);

} // namespace ocudu
