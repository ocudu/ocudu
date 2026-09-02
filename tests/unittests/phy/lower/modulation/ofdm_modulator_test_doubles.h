// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocuduvec/copy.h"
#include "ocudu/phy/lower/modulation/modulation_factories.h"
#include <gtest/gtest.h>
#include <random>

namespace ocudu {

class ofdm_symbol_modulator_spy : public ofdm_symbol_modulator
{
private:
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> dist;
  ofdm_modulator_configuration          configuration;

  struct modulate_entry {
    std::vector<cf_t>           output;
    const resource_grid_reader* grid;
    std::vector<cf_t>           port_weights;
    unsigned                    symbol_index;
  };
  std::vector<modulate_entry> modulate_entries;

public:
  ofdm_symbol_modulator_spy(const ofdm_modulator_configuration& config) : rgen(0), dist(-1, +1), configuration(config)
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

  void modulate(span<cf_t>                  output,
                const resource_grid_reader& grid,
                span<const cf_t>            port_weights,
                unsigned                    symbol_index) override
  {
    EXPECT_EQ(output.size(), get_symbol_size(symbol_index));

    modulate_entries.emplace_back();
    modulate_entry& entry = modulate_entries.back();
    entry.grid            = &grid;
    entry.port_weights    = std::vector<cf_t>(port_weights.begin(), port_weights.end());
    entry.symbol_index    = symbol_index;

    // Prepare output.
    entry.output.resize(output.size());
    std::generate(entry.output.begin(), entry.output.end(), [this]() { return cf_t(dist(rgen), dist(rgen)); });
    ocuduvec::copy(output, entry.output);
  }

  void clear_modulate_entries() { modulate_entries.clear(); }

  const ofdm_modulator_configuration& get_configuration() const { return configuration; }

  const std::vector<modulate_entry>& get_modulate_entries() const { return modulate_entries; }
};

class ofdm_modulator_factory_spy : public ofdm_modulator_factory
{
private:
  std::vector<ofdm_symbol_modulator_spy*> modulators;

public:
  std::unique_ptr<ofdm_symbol_modulator>
  create_ofdm_symbol_modulator(const ofdm_modulator_configuration& config) override
  {
    std::unique_ptr<ofdm_symbol_modulator_spy> ptr = std::make_unique<ofdm_symbol_modulator_spy>(config);
    modulators.push_back(ptr.get());
    return ptr;
  }
  std::unique_ptr<ofdm_slot_modulator> create_ofdm_slot_modulator(const ofdm_modulator_configuration& config) override
  {
    return nullptr;
  }

  std::vector<ofdm_symbol_modulator_spy*>& get_modulators() { return modulators; }
};

} // namespace ocudu
