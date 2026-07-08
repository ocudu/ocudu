// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pdcch/dci_format.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include <vector>

namespace ocudu {

class cell_configuration;
struct search_space_info;
struct pusch_config_common;
struct pusch_time_domain_resource_allocation;

/// \brief Returns the list of all applicable PUSCH Time Domain resource indexes for a given PDCCH slot, based on the
/// cell or UE configuration, and on FDD/TDD configuration.
///
/// For TDD DL-heavy, applicable means that only k2  <= min k1 are considered for a given PDCCH slot.
///
/// \param[in] pdcch_slot Slot at which the PDCCH is supposed to be scheduled.
/// \param[in] tdd_cfg_common TDD configuration, if present.
/// \param[in] pusch_td_list List of applicable PUSCH Time Domain resource allocations (cell-common or UE-dedicated).
/// \param[in] min_k1 Minimum k1 value applicable for the PDCCH slot.
/// \param[in] dci_format UL DCI format that will schedule the PUSCH. DCI format 0_0 identifies the fallback scheduler,
/// which does not multiplex UCI on PUSCH.
/// \return List of PUSCH Time Domain resource indexes.
static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>
get_pusch_td_resource_indices(slot_point                                        pdcch_slot,
                              const std::optional<tdd_ul_dl_config_common>&     tdd_cfg_common,
                              span<const pusch_time_domain_resource_allocation> pusch_td_list,
                              unsigned                                          min_k1,
                              dci_ul_format                                     dci_format = dci_ul_format::f0_1);

/// Returns the list circularly indexed by slot containing the list of applicable PUSCH Time Domain resource indexes per
/// slot.
///
/// In case of FDD, the list returned contains applicable PUSCH Time Domain resource indexes for only one slot.
///
/// In case of DL heavy TDD pattern, the list (i) contains at most 1 index per DL slot; (ii) the resulting k2 values are
/// such that DL_idx_n + k2(DL_idx_n) < DL_idx_m + k2(DL_idx_m) for any DL_idx_n < DL_idx_m (where the inequality has to
/// be considered in the mod(x , TDD_period) context); (iii) k2 >= min_k2; (iv) every UL slot has a DL slot that maps
/// to it.
///
/// In case of UL heavy TDD pattern, this function ensures that UL PDCCH scheduling the PUSCH are fairly distributed
/// across DL slots.
///
/// \param[in] scs SCS common value.
/// \param[in] tdd_cfg_common TDD configuration, if present.
/// \param[in] pusch_cfg_common PUSCH common configuration.
/// \param[in] dl_data_to_ul_ack List of viable k1 values.
/// \param[in] ss_info SearchSpace information.
/// \return A list circularly indexed by slot containing the list of applicable PUSCH Time Domain resource indexes per
/// slot.
/// \remark The periodicity of the circularly indexed list is equal to nof. slots in a TDD period in case of TDD and 1
/// slot in case of FDD.
/// \remark If \c ss_info is nullptr, then minimum k1 is taken from \c cell_cfg.
/// \remark The list of applicable PUSCH Time Domain resources would be empty for UL slots as UL PDCCHs are scheduled
/// only over DL slots.
/// \remark In case of FDD, the list returned would contain applicable PUSCH Time Domain resource indexes for only one
/// slot.
std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>>
get_pusch_td_resource_indices_per_slot(subcarrier_spacing                            scs,
                                       const std::optional<tdd_ul_dl_config_common>& tdd_cfg_common,
                                       const pusch_config_common&                    pusch_cfg_common,
                                       span<const uint8_t>                           dl_data_to_ul_ack,
                                       const search_space_info*                      ss_info = nullptr);
} // namespace ocudu
