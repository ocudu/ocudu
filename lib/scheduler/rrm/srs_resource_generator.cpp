// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "srs_resource_generator.h"

using namespace ocudu;

std::vector<du_srs_resource> ocudu::generate_cell_srs_list(const ran_cell_config& cell_cfg, bool use_special_slot_only)
{
  std::vector<du_srs_resource> srs_res_list;
  // TX comb offsets values, depending on the TX comb value, as per TS 38.331, \c transmissionComb, \c SRS-Resource,
  // \c SRS-Config.
  std::vector<unsigned> tx_comb_offsets = cell_cfg.init_bwp.srs_cfg.tx_comb == ocudu::tx_comb_size::n2
                                              ? std::vector<unsigned>{0U, 1U}
                                              : std::vector<unsigned>{0U, 1U, 2U, 3U};

  // Cyclic Shifts values, depending on the TX comb value, as per TS 38.331, \c cyclicShift, \c SRS-Resource,
  // \c SRS-Config.
  const unsigned        max_cs  = cell_cfg.init_bwp.srs_cfg.tx_comb == ocudu::tx_comb_size::n2 ? 8U : 12U;
  const unsigned        cs_step = max_cs / static_cast<unsigned>(cell_cfg.init_bwp.srs_cfg.cyclic_shift_reuse_factor);
  std::vector<unsigned> cs_values;
  for (unsigned cs = 0; cs < max_cs; cs += cs_step) {
    cs_values.push_back(cs);
  }

  // Compute the available Sequence IDs.
  // NOTE: we only consider the number of orthogonal sequences that can be generated, as per TS 38.211,
  // Section 6.4.1.4.2, which is 30.
  // NOTE: Contiguous PCIs will be assigned different (sequence ID % 30) values, which means that their
  // SRS resources will be orthogonal.
  constexpr unsigned max_seq_id_values = 30U;
  constexpr unsigned nof_sequence_ids  = 1024;
  const unsigned     seq_id_step =
      max_seq_id_values / static_cast<unsigned>(cell_cfg.init_bwp.srs_cfg.sequence_id_reuse_factor);
  std::vector<unsigned> seq_id_values;
  for (unsigned seq_id = 0; seq_id < max_seq_id_values; seq_id += seq_id_step) {
    seq_id_values.push_back((static_cast<unsigned>(cell_cfg.pci) + seq_id) % nof_sequence_ids);
  }

  unsigned starting_sym = 0;
  if (cell_cfg.tdd_cfg.has_value() and use_special_slot_only) {
    const auto& tdd_cfg = cell_cfg.tdd_cfg.value();
    // For special slot allocation only, we only consider the allocation on the special slot.
    starting_sym = std::max(starting_sym, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - tdd_cfg.pattern1.nof_ul_symbols);
    if (tdd_cfg.pattern2.has_value() and (tdd_cfg.pattern2.value().nof_ul_symbols != 0)) {
      starting_sym = std::max(starting_sym, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - tdd_cfg.pattern2.value().nof_ul_symbols);
    }
  } else {
    // At this point, the SRS resource is not assigned to a given slot, and we need to consider all possible UL symbols
    // where the SRS can be placed. The viable symbols for SRS are defined by the user configuration, through \c
    // max_nof_symbols for fully-UL slots, or by the number of UL symbols for partially-UL slots. We take the min of
    // these 2 values as starting symbol, and we cap it to the 6th last symbol, which is sey by the standard, as per
    // TS 38.211, Section 6.4.1.4.1.
    // NOTE: not all SRS resources are compatible with all slots; the SRS periodic resource manager will check which
    // resource can be allocated to a given UE as a function of the offset of the periodic resource.
    starting_sym = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.init_bwp.srs_cfg.max_nof_symbols.value();
    if (cell_cfg.tdd_cfg.has_value()) {
      const auto& tdd_cfg = cell_cfg.tdd_cfg.value();
      starting_sym        = std::min(starting_sym, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - tdd_cfg.pattern1.nof_ul_symbols);
      if (tdd_cfg.pattern2.has_value()) {
        starting_sym =
            std::min(starting_sym, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - tdd_cfg.pattern2.value().nof_ul_symbols);
      }
    }
  }
  // Cap the starting symbol to the 6th last symbol of the slot (\c cell_cfg.srs_cfg.max_nof_symbols.max()), as per
  // TS 38.211, Section 6.4.1.4.1.
  starting_sym =
      std::max(starting_sym, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.init_bwp.srs_cfg.max_nof_symbols.max());
  // We use the counter to define the cell resource ID.
  unsigned srs_res_cnt = 0;
  for (unsigned sym_start =
           NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - static_cast<unsigned>(cell_cfg.init_bwp.srs_cfg.nof_symbols);
       sym_start >= starting_sym;
       sym_start -= static_cast<unsigned>(cell_cfg.init_bwp.srs_cfg.nof_symbols)) {
    const ofdm_symbol_range srs_res_symbols{sym_start,
                                            sym_start + static_cast<unsigned>(cell_cfg.init_bwp.srs_cfg.nof_symbols)};
    for (auto tx_comb_offset : tx_comb_offsets) {
      for (auto cs : cs_values) {
        for (auto seq_id : seq_id_values) {
          du_srs_resource srs_res;
          srs_res.cell_res_id    = srs_res_cnt;
          srs_res.tx_comb_offset = tx_comb_offset;
          srs_res.symbols        = srs_res_symbols;
          srs_res.sequence_id    = seq_id;
          srs_res.cs             = cs;
          srs_res_list.push_back(srs_res);
          ++srs_res_cnt;
        }
      }
    }
  }
  return srs_res_list;
}
