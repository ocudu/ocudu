// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rnti_value_table.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {

using du_rnti_table = rnti_value_table<du_ue_index_t, du_ue_index_t::INVALID_DU_UE_INDEX>;

/// \brief Extends DU RNTI Table with the ability to allocate unique RNTIs for UEs.
class rnti_manager : public du_rnti_table
{
public:
  rnti_manager(rnti_t initial_rnti = to_rnti(0x4601)) :
    rnti_counter(to_value(initial_rnti) - to_value(rnti_t::MIN_CRNTI))
  {
    ocudu_assert(is_crnti(initial_rnti), "Invalid initial c-rnti={}", initial_rnti);
  }

  /// \brief Allocates new unique TC-RNTI.
  rnti_t allocate(bool is_cs_rnti = false)
  {
    // For C-RNTI allocation, we assume 1 C-RNTI per UE.
    // If the allocation is for CS-RNTI, the nof_ues() counter won't be increased.
    const uint16_t max_supported_nof_ues = is_cs_rnti ? MAX_NOF_DU_UES + 1 : MAX_NOF_DU_UES;
    if (this->nof_ues() >= max_supported_nof_ues) {
      ocudu_sanity_check(not is_cs_rnti,
                         "This event should not happen. This UE should have been rejected at C-RNTI allocation");
      // [For CRNTI only] If the number of CRNTIs is already maximum, ignore RACH.
      return rnti_t::INVALID_RNTI;
    }
    // Increments RNTI counter until it finds an available temp C-RNTI.
    rnti_t temp_crnti;
    do {
      uint16_t prev_counter = rnti_counter.fetch_add(1, std::memory_order_relaxed) % CRNTI_RANGE;
      temp_crnti            = to_rnti(prev_counter + to_value(rnti_t::MIN_CRNTI));
    } while (this->has_rnti(temp_crnti));
    return temp_crnti;
  }

private:
  static constexpr int CRNTI_RANGE = to_value(rnti_t::MAX_CRNTI) + 1 - to_value(rnti_t::MIN_CRNTI);

  std::atomic<std::underlying_type_t<rnti_t>> rnti_counter;
};

} // namespace ocudu
