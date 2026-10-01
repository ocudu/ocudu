// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../phy_test_utils.h"
#include "ocudu/phy/upper/channel_processors/ssb/factories.h"
#include "ocudu/support/test_utils.h"
#include <vector>

namespace ocudu {

class pbch_encoder_spy : public pbch_encoder
{
private:
  struct entry_t {
    pbch_msg_t           msg;
    std::vector<uint8_t> encoded;
  };
  std::vector<entry_t> entries;

public:
  void encode(span<uint8_t> encoded, const pbch_msg_t& pbch_msg) override
  {
    // Generate new entry
    entry_t entry = {};
    entry.msg     = pbch_msg;
    entry.encoded.resize(encoded.size());

    // Generate message
    for (unsigned i = 0, len = encoded.size(); i != len; ++i) {
      encoded[i]       = static_cast<uint8_t>(i & 0xff);
      entry.encoded[i] = encoded[i];
    }

    // Append entry
    entries.emplace_back(entry);
  }

  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(pbch_encoder);

} // namespace ocudu
