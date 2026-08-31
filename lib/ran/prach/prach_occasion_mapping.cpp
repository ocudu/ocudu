// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/prach/prach_occasion_mapping.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/prach/prach_configuration.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include "ocudu/ran/ssb/ssb_mapping.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

/// N_gap, as per TS 38.213, Table 8.1-2.
static unsigned get_n_gap(const prach_configuration& prach_cfg, subcarrier_spacing ul_scs)
{
  // As per TS 38.213, Section 8.1, N_gap is zero for preamble format B4.
  if (prach_cfg.format == prach_format_type::B4) {
    return 0;
  }
  // Long preamble formats use a 1.25kHz or 5kHz preamble subcarrier spacing.
  if (is_long_preamble(prach_cfg.format)) {
    return 0;
  }
  switch (ul_scs) {
    case subcarrier_spacing::kHz15:
    case subcarrier_spacing::kHz30:
    case subcarrier_spacing::kHz60:
    case subcarrier_spacing::kHz120:
      return 2;
    default:
      report_fatal_error("Unsupported PRACH subcarrier spacing {}", to_string(ul_scs));
  }
}

/// PRACH configuration derived from the PRACH configuration index of the cell.
static const prach_configuration& get_prach_config(const prach_helper::prach_occasion_mapping_config& config)
{
  return prach_configuration_get(band_helper::get_freq_range(config.band),
                                 band_helper::get_duplex_mode(config.band),
                                 config.rach_cfg.rach_cfg_generic.prach_config_index);
}

/// Last symbol occupied by an SS/PBCH block, indexed by slot within the SSB period, as per TS 38.213 Section 4.1.
/// Slots that hold no SS/PBCH block are set to -1.
static std::vector<int8_t> make_ssb_symbol_table(const prach_helper::prach_occasion_mapping_config& config)
{
  const ssb_configuration& ssb_cfg        = config.ssb_cfg;
  const ssb_pattern_case   ssb_case       = band_helper::get_ssb_pattern(config.band, ssb_cfg.scs);
  const unsigned           nsymb_per_slot = get_nsymb_per_slot(config.cp);
  const unsigned           ssb_period_slots =
      static_cast<unsigned>(ssb_cfg.ssb_period) * get_nof_slots_per_subframe(config.ul_scs);

  // The SS/PBCH block symbol indexes are expressed in the SSB numerology, so they are rescaled to the uplink one.
  const int scs_shift =
      static_cast<int>(to_numerology_value(config.ul_scs)) - static_cast<int>(to_numerology_value(ssb_cfg.scs));

  std::vector<int8_t> last_ssb_symbol(ssb_period_slots, -1);
  for (size_t ssb_idx : ssb_cfg.ssb_bitmap.get_bit_positions()) {
    // First symbol of the SS/PBCH block within the half-frame.
    unsigned l_first  = ssb_get_l_first(ssb_case, static_cast<uint8_t>(ssb_idx));
    unsigned nof_symb = NOF_SSB_SYMB;
    if (scs_shift >= 0) {
      l_first <<= scs_shift;
      nof_symb <<= scs_shift;
    } else {
      l_first >>= -scs_shift;
      nof_symb = std::max(1U, nof_symb >> -scs_shift);
    }

    const unsigned ssb_slot  = l_first / nsymb_per_slot;
    const unsigned last_symb = std::min(nsymb_per_slot, (l_first % nsymb_per_slot) + nof_symb) - 1;
    ocudu_assert(ssb_slot < ssb_period_slots, "SS/PBCH block slot out of the SSB period");
    last_ssb_symbol[ssb_slot] = std::max<int8_t>(last_ssb_symbol[ssb_slot], static_cast<int8_t>(last_symb));
  }
  return last_ssb_symbol;
}

prach_helper::prach_occasion_mapping::prach_occasion_mapping(const prach_occasion_mapping_config& config) :
  td_mapping(config.band, config.ul_scs, config.rach_cfg.rach_cfg_generic.prach_config_index),
  paired_spectrum(band_helper::is_paired_spectrum(config.band)),
  cp(config.cp),
  tdd_cfg(config.tdd_cfg),
  n_gap(get_n_gap(get_prach_config(config), config.ul_scs)),
  ul_scs(config.ul_scs),
  nof_slots_per_frame(get_nof_slots_per_subframe(config.ul_scs) * NOF_SUBFRAMES_PER_FRAME),
  nof_td_occasions(std::max<unsigned>(1, get_prach_config(config).nof_occasions_within_slot)),
  nof_fd_occasions(std::max<unsigned>(1, config.rach_cfg.rach_cfg_generic.msg1_fdm)),
  first_td_occasion_start(get_prach_duration_info(get_prach_config(config), config.ul_scs).start_symbol_pusch_scs),
  td_occasion_duration(get_prach_duration_info(get_prach_config(config), config.ul_scs).nof_symbols / nof_td_occasions),
  nof_burst_slots(td_mapping.has_long_preamble() ? td_mapping.prach_burst_length_slots() : 1),
  last_ssb_symbol(make_ssb_symbol_table(config))
{
  report_error_if_not(not tdd_cfg.has_value() or tdd_cfg->ref_scs == config.ul_scs,
                      "The TDD reference subcarrier spacing must match the uplink subcarrier spacing");
}

unsigned prach_helper::prach_occasion_mapping::ssb_period_frames() const
{
  return std::max<unsigned>(1, last_ssb_symbol.size() / nof_slots_per_frame);
}

ofdm_symbol_range prach_helper::prach_occasion_mapping::get_occasion_symbols(unsigned slot_offset,
                                                                             unsigned td_occasion_idx) const
{
  if (nof_burst_slots == 1) {
    const unsigned start = first_td_occasion_start + td_occasion_idx * td_occasion_duration;
    return {static_cast<uint8_t>(start), static_cast<uint8_t>(start + td_occasion_duration)};
  }

  // A long preamble runs from its starting symbol in the first slot of the burst to the end of the preamble in the
  // last one, filling every slot in between.
  const unsigned nsymb_per_slot = get_nsymb_per_slot(cp);
  const unsigned start          = slot_offset == 0 ? first_td_occasion_start : 0;
  const unsigned stop           = slot_offset + 1 < nof_burst_slots
                                      ? nsymb_per_slot
                                      : first_td_occasion_start + td_occasion_duration - slot_offset * nsymb_per_slot;
  return {static_cast<uint8_t>(start), static_cast<uint8_t>(stop)};
}

bool prach_helper::prach_occasion_mapping::is_valid_ro(slot_point sl, ofdm_symbol_range prach_symbols) const
{
  ocudu_assert(sl.scs() == ul_scs, "Slot subcarrier spacing does not match the uplink one");

  // As per TS 38.213, Section 8.1, all the PRACH occasions of a paired spectrum or supplementary uplink band are
  // valid.
  if (paired_spectrum or not tdd_cfg.has_value()) {
    return true;
  }

  // For unpaired spectrum, an occasion is valid either when it falls in uplink symbols, or when it starts far enough
  // after everything the cell transmits in the slot, so that the UE has time to switch to transmission.
  const unsigned slot_idx = sl.sfn() * nof_slots_per_frame + sl.slot_index();

  if (get_active_tdd_ul_symbols(*tdd_cfg, slot_idx, cp).contains(prach_symbols)) {
    return true;
  }

  // The occasion must not precede an SS/PBCH block of the slot either, which the comparison against its last symbol
  // also covers.
  const ofdm_symbol_range dl_symbols = get_active_tdd_dl_symbols(*tdd_cfg, slot_idx, cp);
  int                     last_busy  = dl_symbols.empty() ? -1 : static_cast<int>(dl_symbols.stop()) - 1;
  if (not last_ssb_symbol.empty()) {
    last_busy = std::max(last_busy, static_cast<int>(last_ssb_symbol[slot_idx % last_ssb_symbol.size()]));
  }

  return static_cast<int>(prach_symbols.start()) >= last_busy + 1 + static_cast<int>(n_gap);
}

bool prach_helper::prach_occasion_mapping::is_valid_occasion(slot_point sl, unsigned td_occasion_idx) const
{
  for (unsigned slot_offset = 0; slot_offset != nof_burst_slots; ++slot_offset) {
    if (not is_valid_ro(sl + slot_offset, get_occasion_symbols(slot_offset, td_occasion_idx))) {
      return false;
    }
  }
  return true;
}

bool prach_helper::prach_occasion_mapping::is_valid_prach_slot(slot_point sl) const
{
  if (not td_mapping.has_prach_occasion(sl)) {
    return false;
  }
  for (unsigned td = 0; td != nof_td_occasions; ++td) {
    if (not is_valid_occasion(sl, td)) {
      return false;
    }
  }
  return true;
}
