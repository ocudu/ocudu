// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../phy_test_utils.h"
#include "ocudu/phy/upper/channel_processors/pucch/factories.h"
#include "ocudu/phy/upper/channel_processors/uci/uci_decoder.h"
#include <random>

namespace ocudu {

class uci_decoder_spy : public uci_decoder
{
public:
  struct entry_t {
    std::vector<log_likelihood_ratio> llr;
    configuration                     config;
    std::vector<uint8_t>              message;
    uci_status                        status;
  };

  uci_decoder_spy() : rgen(0), bin_dist(0, 1)
  {
    // Do nothing.
  }

  uci_status decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.config   = config;

    // Copy input soft bits.
    entry.llr.resize(llr.size());
    ocuduvec::copy(entry.llr, llr);

    // Generate random decoded message.
    std::generate(message.begin(), message.end(), [this]() { return rgen() & 1; });
    entry.message.resize(message.size());
    ocuduvec::copy(entry.message, message);

    // Generate random status.
    entry.status = (bin_dist(rgen) == 1) ? uci_status::valid : uci_status::invalid;

    return entry.status;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t>                    entries;
  std::mt19937                            rgen;
  std::uniform_int_distribution<unsigned> bin_dist;
};

PHY_SPY_FACTORY_TEMPLATE(uci_decoder);

} // namespace ocudu
