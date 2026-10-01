// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/ran/pusch/pusch_time_domain_resource.h"

namespace ocudu {

struct search_space_configuration;
class sched_bwp_config;

/// \brief Determines the default PUSCH time-domain resource allocation A.
///
/// The time domain allocation configuration is described in TS38.214 Table 5.1.2.1.1-2 for normal cyclic prefix and
/// TS38.214 Table 5.1.2.1.1-3 for extended cyclic prefix.
///
/// \param[in] cp        Cyclic prefix.
/// \param[in] row_index Row index.
/// \param[in] scs       PUSCH subcarrier spacing.
/// \return A valid PUSCH time-domain allocation configuration is the provided parameters are valid. Otherwise,
/// invalid resource.
pusch_time_domain_resource_allocation
pusch_default_time_allocation_default_A_get(cyclic_prefix cp, unsigned row_index, subcarrier_spacing scs);

/// \brief Determines the table of default PUSCH time-domain resource allocation A.
///
/// \param[in] cp  Cyclic prefix.
/// \param[in] scs PUSCH subcarrier spacing.
/// \return A list of valid PUSCH time-domain allocation configurations to choose from.
span<const pusch_time_domain_resource_allocation>
pusch_default_time_allocations_default_A_table(cyclic_prefix cp, subcarrier_spacing scs);

/// \brief Determines the time domain resource allocation table to be used for PUSCH as per TS 38.214, clause 6.1.2.1.1.
///
/// \param is_common_ss Whether the active searchSpace is a common searchSpace.
/// \param cs_id Coreset ID associated with the chosen searchSpace.
/// \param active_bwp Active BWP configuration.
/// \return A list of valid PUSCH time-domain allocation configurations to choose from.
span<const pusch_time_domain_resource_allocation>
get_c_rnti_pusch_time_domain_list(bool is_common_ss, coreset_id cs_id, const sched_bwp_config& active_bwp);

/// \brief Determines the time domain resource allocation table to be used for PUSCH as per TS 38.214, clause 6.1.2.1.1.
///
/// \param ss_cfg Search Space configuration.
/// \param active_bwp Active BWP configuration.
/// \return A list of valid PUSCH time-domain allocation configurations to choose from.
span<const pusch_time_domain_resource_allocation>
get_c_rnti_pusch_time_domain_list(const search_space_configuration& ss_cfg, const sched_bwp_config& active_bwp);

} // namespace ocudu
