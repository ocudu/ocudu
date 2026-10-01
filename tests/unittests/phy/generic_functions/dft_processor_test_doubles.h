// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/generic_functions/generic_functions_factories.h"
#include <random>

namespace ocudu {

class dft_processor_spy : public dft_processor
{
public:
  struct entry {
    std::vector<cf_t> input;
    std::vector<cf_t> output;
  };

  dft_processor_spy(unsigned seed, const configuration& config) :
    size(config.size), dir(config.dir), input_buffer(config.size), rgen(seed), dist(-1, 1)
  {
    // Do nothing.
  }
  direction        get_direction() const override { return dir; }
  unsigned         get_size() const override { return size; }
  span<cf_t>       get_input() override { return input_buffer; }
  span<const cf_t> run() override
  {
    // Generate some random output.
    std::vector<cf_t> output_buffer;
    output_buffer.reserve(size);
    std::generate_n(std::back_inserter(output_buffer), size, [this]() { return cf_t{dist(rgen), dist(rgen)}; });

    entries.emplace_back();
    entry& e = entries.back();
    e.input  = input_buffer;
    e.output = output_buffer;

    return e.output;
  }

  void clear_entries() { entries.clear(); }

  const std::vector<entry>& get_entries() const { return entries; }

private:
  unsigned                              size;
  direction                             dir;
  std::vector<cf_t>                     input_buffer;
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> dist;
  std::vector<entry>                    entries;
};

class dft_processor_factory_spy : public dft_processor_factory
{
private:
  struct entry {
    dft_processor_spy* dft;
    unsigned           seed;
  };

  unsigned seed = 1234;

  std::vector<entry> entries;

public:
  std::unique_ptr<dft_processor> create(const dft_processor::configuration& config) override
  {
    entries.emplace_back();
    entry& e = entries.back();
    e.dft    = new dft_processor_spy(seed, config);
    e.seed   = seed;

    // Increment seed.
    ++seed;

    return std::unique_ptr<dft_processor_spy>(e.dft);
  }

  void clear_entries() { entries.clear(); }

  const std::vector<entry>& get_entries() const { return entries; }
};

} // namespace ocudu
