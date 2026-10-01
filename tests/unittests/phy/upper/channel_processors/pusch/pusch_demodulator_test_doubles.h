// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../phy_test_utils.h"
#include "ocudu/phy/upper/channel_processors/pusch/factories.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_demodulator_notifier.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include <random>

namespace ocudu {

class pusch_demodulator_spy : public pusch_demodulator
{
public:
  struct entry_t {
    std::vector<log_likelihood_ratio>   codeword;
    dynamic_bit_buffer                  scrambling_seq;
    const resource_grid_reader*         grid;
    const dmrs_pusch_estimator_results* est_results;
    configuration                       config;
  };

  pusch_demodulator_spy() : llr_dist(log_likelihood_ratio::min().to_int(), log_likelihood_ratio::max().to_int()) {}

  void demodulate(pusch_codeword_buffer&              codeword_buffer,
                  pusch_demodulator_notifier&         notifier,
                  const resource_grid_reader&         grid,
                  const dmrs_pusch_estimator_results& est_results,
                  const configuration&                config) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.codeword.resize(codeword_size);
    entry.scrambling_seq.resize(codeword_size);
    entry.grid        = &grid;
    entry.est_results = &est_results;
    entry.config      = config;

    std::generate(
        entry.codeword.begin(), entry.codeword.end(), [this]() { return log_likelihood_ratio(llr_dist(rgen)); });
    std::generate(entry.scrambling_seq.get_buffer().begin(), entry.scrambling_seq.get_buffer().end(), [this]() {
      return rgen();
    });
    codeword_buffer.on_new_block(entry.codeword, entry.scrambling_seq);
    codeword_buffer.on_end_codeword();

    // Report empty demodulation statistics.
    notifier.on_end_stats(pusch_demodulator_notifier::demodulation_stats());
  }

  void set_codeword_size(unsigned codeword_size_) { codeword_size = codeword_size_; }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  unsigned                                                        codeword_size = 0;
  std::vector<entry_t>                                            entries;
  std::mt19937                                                    rgen;
  std::uniform_int_distribution<log_likelihood_ratio::value_type> llr_dist;
};

PHY_SPY_FACTORY_TEMPLATE(pusch_demodulator);

} // namespace ocudu
