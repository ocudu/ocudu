// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocuduvec/copy.h"
#include "ocudu/phy/lower/modulation/modulation_factories.h"
#include "ocudu/support/error_handling.h"
#include <random>

namespace ocudu {

class ofdm_symbol_demodulator_spy : public ofdm_symbol_demodulator
{
public:
  struct demodulate_entry {
    std::vector<ci16_t>         input;
    const resource_grid_writer* grid;
    unsigned                    port_index;
    unsigned                    symbol_index;
  };

  ofdm_symbol_demodulator_spy(const ofdm_demodulator_configuration& config) :
    rgen(0), dist(-1, +1), configuration(config)
  {
    // Do nothing.
  }

  void set_center_frequency(double center_frequency_Hz) override {}

  unsigned get_symbol_size(unsigned symbol_index) const override
  {
    unsigned sampling_rate_Hz = (configuration.dft_size * 15000) << configuration.numerology;
    return configuration.cp.get_length(symbol_index, to_subcarrier_spacing(configuration.numerology))
               .to_samples(sampling_rate_Hz) +
           configuration.dft_size;
  }

  void
  demodulate(resource_grid_writer& grid, span<const ci16_t> input, unsigned port_index, unsigned symbol_index) override
  {
    report_fatal_error_if_not((input.size()) == (get_symbol_size(symbol_index)), "input.size() == get_symbol_size(symbol_index)");

    demodulate_entries.emplace_back();
    demodulate_entry& entry = demodulate_entries.back();
    entry.grid              = &grid;
    entry.port_index        = port_index;
    entry.symbol_index      = symbol_index;

    entry.input.resize(input.size());
    ocuduvec::copy(entry.input, input);
  }

  void clear_demodulate_entries() { demodulate_entries.clear(); }

  const ofdm_demodulator_configuration& get_configuration() const { return configuration; }

  const std::vector<demodulate_entry>& get_demodulate_entries() const { return demodulate_entries; }

private:
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> dist;
  ofdm_demodulator_configuration        configuration;
  std::vector<demodulate_entry>         demodulate_entries;
};

class ofdm_demodulator_factory_spy : public ofdm_demodulator_factory
{
public:
  std::unique_ptr<ofdm_symbol_demodulator>
  create_ofdm_symbol_demodulator(const ofdm_demodulator_configuration& config) override
  {
    std::unique_ptr<ofdm_symbol_demodulator_spy> ptr = std::make_unique<ofdm_symbol_demodulator_spy>(config);
    demodulators.push_back(ptr.get());
    return ptr;
  }
  std::unique_ptr<ofdm_slot_demodulator>
  create_ofdm_slot_demodulator(const ofdm_demodulator_configuration& config) override
  {
    return nullptr;
  }

  std::vector<ofdm_symbol_demodulator_spy*>& get_demodulators() { return demodulators; }

private:
  std::vector<ofdm_symbol_demodulator_spy*> demodulators;
};

} // namespace ocudu
