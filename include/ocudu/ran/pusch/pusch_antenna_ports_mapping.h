// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/srs/srs_resource_configuration.h"

namespace ocudu {

/// PUSCH antenna ports mapping of a row in TS38.212 Tables 7.3.1.1.2-6 to 7.3.1.1.2-23.
struct pusch_antenna_ports_mapping {
  /// Row index, signalled in the DCI format 0_1 antenna ports field.
  unsigned row_index;
  /// Number of DM-RS CDM groups without data.
  unsigned nof_dmrs_cdm_groups_without_data;
  /// DM-RS port mask, with bit \f$i\f$ set when DM-RS port \f$i\f$ carries a layer.
  dmrs_port_mask dmrs_ports = dmrs_port_mask(DMRS_MAX_NPORTS);
};

/// \brief Returns the PUSCH antenna ports mapping in TS38.212 Tables 7.3.1.1.2-6 to 7.3.1.1.2-23.
/// \param[in] nof_layers         Number of layers.
/// \param[in] transform_precoder Set to \c true if the transform precoder is used.
/// \param[in] dmrs_cfg_type      PUSCH DM-RS configuration type.
/// \param[in] dmrs_max_len       PUSCH DM-RS maximum number of OFDM symbols.
/// \return The PUSCH antenna ports mapping.
/// \remark An assertion is triggered if \c nof_layers is not in the range 1 to \ref pusch_constants::MAX_NOF_LAYERS.
/// \remark An assertion is triggered if \c dmrs_cfg_type is not \ref dmrs_config_type::type1.
/// \remark An assertion is triggered if \c dmrs_max_len is not \ref dmrs_max_length::len1.
/// \remark An assertion is triggered if \c transform_precoder is \c true and \c nof_layers is not one.
pusch_antenna_ports_mapping get_pusch_antenna_port_mapping(unsigned         nof_layers,
                                                           bool             transform_precoder,
                                                           dmrs_config_type dmrs_cfg_type,
                                                           dmrs_max_length  dmrs_max_len);

/// \brief Returns the PUSCH antenna ports mapping row index in TS 38.212, table 7.3.1.2.2-6 to 7.3.1.2.2-23.
/// \param[in] nof_layers         Number of layers.
/// \param[in] transform_precoder Set to \c true if the transform precoder is used.
/// \param[in] dmrs_cfg_type      PUSCH DM-RS configuration type.
/// \param[in] dmrs_max_len       PUSCH DM-RS maximum number of OFDM symbols.
/// \return The PUSCH antenna ports mapping row index.
unsigned get_pusch_antenna_port_mapping_row_index(unsigned         nof_layers,
                                                  bool             transform_precoder,
                                                  dmrs_config_type dmrs_cfg_type,
                                                  dmrs_max_length  dmrs_max_len);

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
