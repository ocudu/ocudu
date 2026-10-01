// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../phy_test_utils.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/phy/upper/channel_processors/pdsch/factories.h"
#include <random>

namespace ocudu {

class pdsch_encoder_spy : public pdsch_encoder
{
private:
  struct entry_t {
    configuration        config;
    std::vector<uint8_t> transport_block;
    std::vector<uint8_t> codeword;
  };
  std::vector<entry_t> entries;
  std::mt19937         rgen;

public:
  void encode(span<uint8_t> codeword, span<const uint8_t> transport_block, const configuration& cfg) override
  { // Generate new entry.
    entry_t entry = {};
    entry.config  = cfg;
    entry.codeword.resize(codeword.size());

    // Fill data.
    entry.transport_block.resize(transport_block.size());
    ocuduvec::copy(entry.transport_block, transport_block);

    // Generate encoded message.
    for (unsigned i = 0, len = codeword.size(); i != len; ++i) {
      codeword[i]       = static_cast<uint8_t>(rgen() & 0x1);
      entry.codeword[i] = codeword[i];
    }

    // Append entry.
    entries.emplace_back(entry);
  }

  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(pdsch_encoder);

} // namespace ocudu
