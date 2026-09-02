// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/frame_types.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/math/bit_ops.h"

namespace ocudu {

class pucch_helper
{
private:
  // Pseudo-random sequence generator instance
  std::unique_ptr<pseudo_random_generator> prg;

public:
  pucch_helper(std::unique_ptr<pseudo_random_generator> prg_) : prg(std::move(prg_))
  {
    ocudu_assert(prg, "Invalid PRG.");
  }

  /// Computes the NR-PUCCH group sequence (TS38.211 clause 6.3.2.2.1 Group and sequence hopping).
  /// \param[in] group_hopping Group hopping configuration.
  /// \param[in] n_id          Scrambling identifier.
  /// \return A pair of sequence group u and sequence number v.
  static std::pair<unsigned, unsigned> compute_group_sequence(pucch_group_hopping group_hopping, unsigned n_id)
  {
    unsigned f_gh = 0;
    unsigned f_ss = 0;

    switch (group_hopping) {
      case pucch_group_hopping::NEITHER:
        f_ss = n_id % low_papr_sequence_collection::NOF_GROUPS;
        break;
      case pucch_group_hopping::ENABLE:
        ocudu_terminate("Group hopping is not implemented");
      case pucch_group_hopping::DISABLE:
        ocudu_terminate("Hopping is not implemented");
    }

    unsigned u = (f_gh + f_ss) % low_papr_sequence_collection::NOF_GROUPS;
    unsigned v = 0;
    return {u, v};
  }

  /// \brief Computes the NR alpha index (1-NOF_SUBCARRIERS_PER_RB) (TS38.211 clause 6.3.2.2.2 Cyclic shift hopping)
  ///
  /// \param slot[in]    Current slot
  /// \param cp[in]      Cyclic prefix type
  /// \param n_id[in]    Higher layer parameter hoppingID if configured, physical cell id otherwise
  /// \param symbol[in]  OFDM symbol index in the slot
  /// \param m0[in]      Initial cyclic shift
  /// \param m_cs[in]    Cyclic shift
  /// \return NR alpha index
  unsigned
  get_alpha_index(slot_point slot, cyclic_prefix cp, unsigned n_id, unsigned symbol, unsigned m0, unsigned m_cs) const
  {
    // Initialize pseudo-random sequence with the seed set to nid
    unsigned cinit = n_id;

    // Pseudo-random sequence length for max numerology (i.e. 4)
    const size_t max_seq_length  = 8U * MAX_NSYMB_PER_SLOT * NOF_SUBFRAMES_PER_FRAME * (1U << 4U);
    unsigned     sequence_length = 8U * get_nsymb_per_slot(cp) * slot.nof_slots_per_frame();

    // Create a zero array
    static_bit_buffer<max_seq_length> cs(sequence_length);

    prg->init(cinit);
    prg->generate(cs);

    // Slot number
    unsigned n_slot = slot.slot_index();

    // Create n_cs parameter
    unsigned base_idx = get_nsymb_per_slot(cp) * n_slot + symbol;
    unsigned n_cs     = reverse_byte(cs.get_byte(base_idx));

    unsigned alpha_idx = (m0 + m_cs + n_cs) % NOF_SUBCARRIERS_PER_RB;
    return alpha_idx;
  }
};

} // namespace ocudu
