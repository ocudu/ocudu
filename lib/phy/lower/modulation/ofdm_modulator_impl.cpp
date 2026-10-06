// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ofdm_modulator_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/ran/subcarrier_spacing.h"

using namespace ocudu;

ofdm_symbol_modulator_impl::ofdm_symbol_modulator_impl(const ofdm_modulator_configuration& ofdm_config,
                                                       ofdm_modulator_dependencies         dependencies) :
  dft_size(ofdm_config.dft_size),
  rg_size(ofdm_config.bw_rb * NOF_SUBCARRIERS_PER_RB),
  cp(ofdm_config.cp),
  scs(to_subcarrier_spacing(ofdm_config.numerology)),
  sampling_rate_Hz(to_sampling_rate_Hz(scs, dft_size)),
  scale(ofdm_config.scale),
  dft(std::move(dependencies.dft)),
  phase_compensation_table(to_subcarrier_spacing(ofdm_config.numerology),
                           ofdm_config.cp,
                           ofdm_config.dft_size,
                           ofdm_config.center_freq_Hz,
                           true),
  current_center_freq_Hz(ofdm_config.center_freq_Hz),
  next_center_freq_Hz(ofdm_config.center_freq_Hz)
{
  ocudu_assert(std::isnormal(scale), "Invalid scaling factor {}", scale);
  ocudu_assert(
      dft_size > rg_size, "The DFT size ({}) must be greater than the resource grid size ({})", dft_size, rg_size);

  // Fill DFT input with zeros.
  ocuduvec::zero(dft->get_input());
}

void ofdm_symbol_modulator_impl::modulate(span<ci16_t>                output,
                                          const resource_grid_reader& grid,
                                          span<const cf_t>            port_weights,
                                          unsigned                    i_symbol_sf)
{
  // Beamforming weights must match the number of ports in the grid.
  ocudu_assert(port_weights.size() == grid.get_nof_ports(),
               "Beamforming weights size ({}) must match the number of ports in the resource grid ({}).",
               port_weights.size(),
               grid.get_nof_ports());

  // Recalculate phase compensation if the center frequency has changed.
  double center_freq_Hz = next_center_freq_Hz.load(std::memory_order::memory_order_relaxed);
  if (center_freq_Hz != current_center_freq_Hz) {
    phase_compensation_table = phase_compensation_lut(scs, cp, dft_size, center_freq_Hz, true);
    current_center_freq_Hz   = center_freq_Hz;
  }

  // Calculate number of symbols per slot.
  unsigned nof_symbols_per_slot = get_nsymb_per_slot(cp);

  unsigned i_symbol = i_symbol_sf % nof_symbols_per_slot;

  // Calculate cyclic prefix length.
  unsigned cp_len = cp.get_length(i_symbol_sf, scs).to_samples(sampling_rate_Hz);

  // Make sure output buffer matches the symbol size.
  ocudu_assert(output.size() == (cp_len + dft_size),
               "The output buffer size ({}) does not match the symbol index {} size ({}+{}={}). SCS={}kHz.",
               output.size(),
               i_symbol_sf,
               cp_len,
               dft_size,
               cp_len + dft_size,
               scs_to_khz(scs));

  span<cf_t> lower_dft_input = dft->get_input().last(rg_size / 2);
  span<cf_t> upper_dft_input = dft->get_input().first(rg_size / 2);

  bool is_empty = true;
  for (unsigned i_port = 0, nof_ports = port_weights.size(); i_port != nof_ports; ++i_port) {
    // Select port weights.
    cf_t port_weight = port_weights[i_port];

    // Skip port if the coefficient is zero, nan or infinity.
    if (!std::isnormal(std::norm(port_weight))) {
      continue;
    }

    // Extract resource allocation for the selected port and OFDM symbol.
    crb_interval       allocation           = grid.get_allocation_range(i_port, i_symbol);
    interval<unsigned> full_subc_allocation = {allocation.start() * NOF_SUBCARRIERS_PER_RB,
                                               allocation.stop() * NOF_SUBCARRIERS_PER_RB};

    interval<unsigned> lower_subc_allocation = interval(full_subc_allocation).intersect({0, rg_size / 2});
    interval<unsigned> upper_subc_allocation = interval(full_subc_allocation).intersect({rg_size / 2, rg_size});

    // Skip port if the allocation is empty.
    if (allocation.empty()) {
      continue;
    }

    // Extract resource grid.
    span<const cbf16_t> ofdm_symbol = grid.get_view(i_port, i_symbol);

    // If it is empty, then apply the port weight on the entire OFDM symbol.
    if (is_empty && (!lower_subc_allocation.empty() || !upper_subc_allocation.empty())) {
      ocuduvec::sc_prod(lower_dft_input, ofdm_symbol.first(rg_size / 2), port_weight);
      ocuduvec::sc_prod(upper_dft_input, ofdm_symbol.last(rg_size / 2), port_weight);
      is_empty = false;
      continue;
    }

    // Accumulate lower subcarrier allocation if available.
    if (!lower_subc_allocation.empty()) {
      unsigned nof_subc = lower_subc_allocation.length();
      ocuduvec::sc_prod_and_add(lower_dft_input.first(nof_subc),
                                ofdm_symbol.first(rg_size / 2).first(nof_subc),
                                lower_dft_input.first(nof_subc),
                                port_weight);
    }

    // Accumulate upper subcarrier allocation if available.
    if (!upper_subc_allocation.empty()) {
      unsigned nof_subc = upper_subc_allocation.length();
      ocuduvec::sc_prod_and_add(upper_dft_input.last(nof_subc),
                                ofdm_symbol.last(rg_size / 2).last(nof_subc),
                                upper_dft_input.last(nof_subc),
                                port_weight);
    }
  }

  // Skip modulator there is nothing to be modulated.
  if (is_empty) {
    ocuduvec::zero(output);
    return;
  }

  // Execute DFT.
  span<const cf_t> dft_output = dft->run();

  // Get phase correction (TS138.211, Section 5.4)
  cf_t phase_compensation = phase_compensation_table.get_coefficient(i_symbol_sf);

  // Apply scaling and phase compensation, including the conversion from cf to ci16.
  ocuduvec::sc_prod(
      output.last(dft_size), dft_output, phase_compensation * scale * ocuduvec::scaling_factor_cf_to_ci16);

  // Copy cyclic prefix.
  ocuduvec::copy(output.first(cp_len), output.last(cp_len));
}

unsigned ofdm_slot_modulator_impl::get_slot_size(unsigned slot_index) const
{
  unsigned nsymb = get_nsymb_per_slot(cp);
  unsigned count = 0;

  // Iterate all symbols of the slot and accumulate
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    count += symbol_modulator->get_symbol_size(nsymb * slot_index + symbol_idx);
  }

  return count;
}
void ofdm_slot_modulator_impl::set_center_frequency(double center_frequency_Hz)
{
  symbol_modulator->set_center_frequency(center_frequency_Hz);
}

void ofdm_slot_modulator_impl::modulate(span<ci16_t>                output,
                                        const resource_grid_reader& grid,
                                        span<const cf_t>            port_weights,
                                        unsigned                    slot_index)
{
  unsigned nsymb = get_nsymb_per_slot(cp);

  unsigned nslots_per_subframe = get_nof_slots_per_subframe(to_subcarrier_spacing(numerology));
  ocudu_assert(slot_index < nslots_per_subframe,
               "Slot index within the subframe {} exceeds the number of slots per subframe {}.",
               slot_index,
               nslots_per_subframe);

  // For each symbol in the slot.
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    // Get the current symbol size.
    unsigned symbol_sz = symbol_modulator->get_symbol_size(nsymb * slot_index + symbol_idx);

    // Modulate symbol.
    symbol_modulator->modulate(output.first(symbol_sz), grid, port_weights, nsymb * slot_index + symbol_idx);

    // Advance output buffer.
    output = output.last(output.size() - symbol_sz);
  }
}
