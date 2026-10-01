// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ofdm_demodulator_impl.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/prod.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;

ofdm_symbol_demodulator_impl::ofdm_symbol_demodulator_impl(const ofdm_demodulator_configuration& ofdm_config,
                                                           ofdm_demodulator_dependencies         dependencies) :
  dft_size(ofdm_config.dft_size),
  rg_size(ofdm_config.bw_rb * NOF_SUBCARRIERS_PER_RB),
  half_rg_size(rg_size / 2),
  cp(ofdm_config.cp),
  nof_samples_window_offset(ofdm_config.nof_samples_window_offset),
  scs(to_subcarrier_spacing(ofdm_config.numerology)),
  sampling_rate_Hz(to_sampling_rate_Hz(scs, dft_size)),
  scale(ofdm_config.scale),
  dft(std::move(dependencies.dft)),
  phase_compensation_table(to_subcarrier_spacing(ofdm_config.numerology),
                           ofdm_config.cp,
                           ofdm_config.dft_size,
                           ofdm_config.center_freq_Hz,
                           false),
  next_center_freq_Hz(ofdm_config.center_freq_Hz),
  current_center_freq_Hz(ofdm_config.center_freq_Hz)
{
  report_fatal_error_if_not(std::isnormal(scale), "Invalid scaling factor {}.", scale);
  report_fatal_error_if_not(
      dft_size > rg_size, "The DFT size ({}) must be greater than the resource grid size ({}).", dft_size, rg_size);

  // Fill DFT input with zeros.
  ocuduvec::zero(dft->get_input());

  if (ofdm_config.nof_samples_window_offset != 0) {
    // Verify the window is valid.
    ocudu_assert(ofdm_config.nof_samples_window_offset < (144 * ofdm_config.dft_size) / 2048,
                 "The DFT window offset (i.e., {}) must be lower than {}.",
                 ofdm_config.nof_samples_window_offset,
                 (144 * ofdm_config.dft_size) / 2048);

    // Prepare phase compensation vector.
    window_phase_compensation.resize(dft_size);

    // Discrete frequency of the complex exponential.
    float omega = static_cast<float>(ofdm_config.nof_samples_window_offset) * static_cast<float>(2.0 * M_PI) /
                  static_cast<float>(dft_size);
    for (unsigned i = 0; i != dft_size; ++i) {
      window_phase_compensation[i] = std::polar(1.0F, omega * static_cast<float>(i));
    }
  }
}

unsigned ofdm_symbol_demodulator_impl::get_cp_offset(unsigned symbol_index, unsigned slot_index) const
{
  // Calculate number of symbols per slot.
  unsigned nsymb = get_nsymb_per_slot(cp);

  // Calculate the offset in samples to the start of the symbol CP within the current slot
  unsigned cp_offset = 0;
  for (unsigned symb_idx = 0; symb_idx != symbol_index; ++symb_idx) {
    cp_offset += cp.get_length(nsymb * slot_index + symb_idx, scs).to_samples(sampling_rate_Hz) + dft_size;
  }

  return cp_offset;
}

void ofdm_symbol_demodulator_impl::demodulate(resource_grid_writer& grid,
                                              span<const ci16_t>    input,
                                              unsigned              port_index,
                                              unsigned              symbol_index)
{
  // Recalculate phase compensation if the center frequency has changed.
  double center_freq_Hz = next_center_freq_Hz.load(std::memory_order::memory_order_relaxed);
  if (center_freq_Hz != current_center_freq_Hz) {
    phase_compensation_table = phase_compensation_lut(scs, cp, dft_size, center_freq_Hz, false);
    current_center_freq_Hz   = center_freq_Hz;
  }

  // Calculate number of symbols per slot.
  unsigned nsymb = get_nsymb_per_slot(cp);

  // Calculate cyclic prefix length.
  unsigned cp_len = cp.get_length(symbol_index, scs).to_samples(sampling_rate_Hz);

  // Make sure output buffer matches the symbol size.
  ocudu_assert(input.size() == (cp_len + dft_size),
               "The input buffer size ({}) does not match the symbol index {} size ({}+{}={}). SCS={}kHz.",
               input.size(),
               symbol_index,
               cp_len,
               dft_size,
               cp_len + dft_size,
               scs_to_khz(scs));
  // Get phase correction (TS138.211, Section 5.4) and apply frequency offset compensation.
  cf_t phase_compensation = phase_compensation_table.get_coefficient(symbol_index);

  // Prepare the DFT inputs, while skipping the cyclic prefix. Include the conversion from ci16 to cf.
  ocuduvec::sc_prod(dft->get_input().first(dft_size),
                    input.subspan(cp_len - nof_samples_window_offset, dft_size),
                    phase_compensation * scale / ocuduvec::scaling_factor_ci16_to_cf);

  // Execute DFT and get output span.
  span<const cf_t> dft_output = dft->run();

  // Extract view of the destination frequency domain for the corresponding OFDM symbol.
  span<cbf16_t> symbol_view = grid.get_view(port_index, symbol_index % nsymb);

  // Compensate DFT window offset phase shift.
  if (!window_phase_compensation.empty()) {
    span<const cf_t> phase_shift(window_phase_compensation);

    // DFT window shift compensation and map the upper bound frequency domain data.
    ocuduvec::prod(symbol_view.first(half_rg_size), phase_shift.last(half_rg_size), dft_output.last(half_rg_size));

    // DFT window shift compensation and map the lower bound frequency domain data.
    ocuduvec::prod(symbol_view.last(half_rg_size), phase_shift.first(half_rg_size), dft_output.first(half_rg_size));
  } else {
    // Map the upper bound frequency domain data.
    ocuduvec::convert(symbol_view.first(half_rg_size), dft_output.last(half_rg_size));

    // Map the lower bound frequency domain data.
    ocuduvec::convert(symbol_view.last(half_rg_size), dft_output.first(half_rg_size));
  }
}

unsigned ofdm_slot_demodulator_impl::get_slot_size(unsigned slot_index) const
{
  unsigned nsymb = get_nsymb_per_slot(cp);
  unsigned count = 0;

  // Iterate all symbols of the slot and accumulate
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    count += symbol_demodulator->get_symbol_size(nsymb * slot_index + symbol_idx);
  }

  return count;
}

void ofdm_slot_demodulator_impl::demodulate(resource_grid_writer& grid,
                                            span<const ci16_t>    input,
                                            unsigned              port_index,
                                            unsigned              slot_index)
{
  unsigned nsymb = get_nsymb_per_slot(cp);

  // For each symbol in the slot.
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    // Get the current symbol size.
    unsigned symbol_sz = symbol_demodulator->get_symbol_size(nsymb * slot_index + symbol_idx);

    // Demodulate symbol.
    symbol_demodulator->demodulate(grid, input.first(symbol_sz), port_index, nsymb * slot_index + symbol_idx);

    input = input.last(input.size() - symbol_sz);
  }
}
