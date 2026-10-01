// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "phase_compensation_lut.h"
#include "ocudu/phy/generic_functions/dft_processor.h"
#include "ocudu/phy/lower/modulation/ofdm_demodulator.h"
#include <memory>
#include <vector>

namespace ocudu {

/// OFDM demodulator dependencies. Provides the necessary dependency instances.
struct ofdm_demodulator_dependencies {
  /// DFT instance. The ownership is transferred to the demodulator.
  std::unique_ptr<dft_processor> dft;
};

/// Describes a generic OFDM symbol demodulator.
class ofdm_symbol_demodulator_impl : public ofdm_symbol_demodulator
{
  /// Indicates the DFT size.
  unsigned dft_size;
  /// Resource grid bandwidth in resource elements.
  unsigned rg_size;
  /// Half resource grid bandwidth in resource elements.
  unsigned half_rg_size;
  /// Cyclic prefix type.
  cyclic_prefix cp;
  /// DFT window offset.
  unsigned nof_samples_window_offset;
  /// Numerology.
  subcarrier_spacing scs;
  /// Sampling rate in Hz.
  unsigned sampling_rate_Hz;
  /// Scaling factor at the DFT output.
  float scale;
  /// DFT processor.
  std::unique_ptr<dft_processor> dft;
  /// Phase compensation table.
  phase_compensation_lut phase_compensation_table;
  /// Next center frequency in Hertz.
  std::atomic<double> next_center_freq_Hz;
  /// Current center frequency in Hertz.
  double current_center_freq_Hz;
  /// DFT window offset phase compensation.
  std::vector<cf_t> window_phase_compensation;

public:
  /// \brief Constructs an OFDM symbol demodulator.
  /// \param[in] ofdm_config  Provides generic OFDM configuration parameters.
  /// \param[in] dependencies Provides specific dependencies.
  ofdm_symbol_demodulator_impl(const ofdm_demodulator_configuration& ofdm_config,
                               ofdm_demodulator_dependencies         dependencies);

  /// \brief Gets the resource grid bandwidth in resource elements.
  /// \return The number of resource elements in the grid.
  unsigned get_rg_size() const { return rg_size; }

  /// \brief Gets the offset in samples to the start of (the cyclic prefix of) a given symbol.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \param[in] slot_index Slot index within the subframe containing the symbol to demodulate.
  /// \return The number of samples preceding the given symbol.
  unsigned get_cp_offset(unsigned symbol_index, unsigned slot_index) const;

  // See interface for documentation.
  unsigned get_symbol_size(unsigned symbol_index) const override
  {
    return cp.get_length(symbol_index, scs).to_samples(sampling_rate_Hz) + dft_size;
  }

  // See interface for documentation.
  void set_center_frequency(double center_frequency_Hz) override
  {
    next_center_freq_Hz.store(center_frequency_Hz, std::memory_order_relaxed);
  }

  // See interface for documentation.
  void
  demodulate(resource_grid_writer& grid, span<const ci16_t> input, unsigned port_index, unsigned symbol_index) override;
};

/// Describes a generic OFDM slot demodulator.
class ofdm_slot_demodulator_impl : public ofdm_slot_demodulator
{
  /// Cyclic prefix type.
  cyclic_prefix cp;
  /// Instance of symbol demodulator.
  std::unique_ptr<ofdm_symbol_demodulator> symbol_demodulator;

public:
  /// \brief Constructs an OFDM slot demodulator.
  /// \param[in] ofdm_config       OFDM factory parameters.
  /// \param[in] symbol_modulator_ OFDM symbol demodulator instance.
  ofdm_slot_demodulator_impl(const ofdm_demodulator_configuration&    ofdm_config,
                             std::unique_ptr<ofdm_symbol_demodulator> symbol_demodulator_) :
    cp(ofdm_config.cp), symbol_demodulator(std::move(symbol_demodulator_))
  {
    ocudu_assert(symbol_demodulator, "Invalid OFDM symbol demodulator.");
  }

  // See interface for documentation;
  unsigned get_slot_size(unsigned slot_index) const override;

  // See interface for documentation;
  void
  demodulate(resource_grid_writer& grid, span<const ci16_t> input, unsigned port_index, unsigned slot_index) override;
};

} // namespace ocudu
