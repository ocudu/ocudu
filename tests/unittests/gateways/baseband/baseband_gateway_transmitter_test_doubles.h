// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "baseband_gateway_buffer_test_doubles.h"
#include "ocudu/gateways/baseband/baseband_gateway_transmitter.h"
#include <random>

namespace ocudu {

class baseband_gateway_transmitter_spy : public baseband_gateway_transmitter
{
public:
  struct entry_t {
    baseband_gateway_transmitter_metadata metadata;
    baseband_gateway_buffer_read_only     data;
  };

  // See interface for documentation.
  void transmit(const baseband_gateway_buffer_reader& data, const baseband_gateway_transmitter_metadata& md) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.metadata = md;
    entry.data     = baseband_gateway_buffer_read_only(data);
  }

  /// Gets all transmit entries.
  const std::vector<entry_t>& get_entries() const { return entries; }

  /// Clears all types of entries.
  void clear() { entries.clear(); }

private:
  std::vector<entry_t> entries;
};

} // namespace ocudu
