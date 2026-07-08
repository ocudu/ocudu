// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pdcch/dci_format.h"
#include "ocudu/ran/pucch/pucch_td_helper.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include <optional>
#include <vector>

namespace ocudu {

struct tdd_ul_dl_config_common;
struct pusch_time_domain_resource_allocation;

/// \brief Computes the list of valid PUCCH k1 values that can be used for a given DL slot.
///
/// For TDD, the returned vector is circularly indexed by slot within the TDD period (size = TDD period length). For
/// FDD, the vector contains a single entry (list) that applies to every DL slot.
///
/// The valid k1 values per slot are derived as follows:
/// - \b FDD: all values in \c dl_data_to_ul_ack are valid for every slot.
/// - \b TDD UL-heavy (more full-UL slots than DL slots): all values in \c dl_data_to_ul_ack are valid for every DL
///   slot; UL slots and non-DL slots get an empty list.
/// - \b TDD DL-heavy (nof DL slots >= full-UL slots): for each DL slot, only k1 values that are >= the slot's
///   assigned k2 (from \c pusch_td_resource_indices_per_slot) are valid, ensuring that a PUSCH cannot be scheduled
///   before any PUCCH carrying the HARQ-ACK scheduled for the same slot. DL slots with no assigned PUSCH carry the full
///   k1 list. UL slots get an empty list.
///
/// \param[in] dl_data_to_ul_ack   List of candidate k1 values (dl-DataToUL-ACK), in ascending order.
/// \param[in] tdd_cfg_common      TDD UL/DL configuration. If absent, the cell operates in FDD mode.
/// \param[in] pusch_td_alloc_list PUSCH time-domain resource allocation table for the cell.
/// \param[in] pusch_td_resource_indices_per_slot Per-slot list of indices into \c pusch_td_alloc_list that are
/// applicable for PUSCH scheduling in that slot, as produced by \c get_pusch_td_resource_indices_per_slot(). Used in
/// the DL-heavy TDD case to determine the minimum k2 assigned to each DL slot and, hence, the minimum valid k1.
/// \return Vector of valid k1 lists, one entry per slot in the TDD period (or one entry for FDD). Empty inner lists
///   indicate slots for which no PUCCH k1 scheduling is applicable (UL or non-DL slots).
std::vector<static_vector<uint8_t, pucch_td_helper::MAX_K1_CANDIDATES>>
get_pucch_k1_list_per_slot(span<const uint8_t>                                       dl_data_to_ul_ack,
                           const std::optional<tdd_ul_dl_config_common>&             tdd_cfg_common,
                           const std::vector<pusch_time_domain_resource_allocation>& pusch_td_alloc_list,
                           const std::vector<static_vector<uint8_t, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>&
                               pusch_td_resource_indices_per_slot);

} // namespace ocudu
