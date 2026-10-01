// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/ofh_constants.h"
#include "ocudu/support/ocudu_assert.h"
#include <array>
#include <atomic>

namespace ocudu {
namespace ofh {

/// Sequence identifier generator.
class sequence_identifier_generator
{
  std::array<std::atomic<uint8_t>, MAX_SUPPORTED_EAXC_ID_VALUE> counters;

public:
  /// Default constructor.
  explicit sequence_identifier_generator(unsigned init_value = 0)
  {
    for (unsigned i = 0; i != MAX_SUPPORTED_EAXC_ID_VALUE; ++i) {
      counters[i] = init_value;
    }
  }

  /// Generates a new sequence identifier and returns it.
  uint8_t generate(unsigned eaxc)
  {
    ocudu_assert(eaxc < MAX_SUPPORTED_EAXC_ID_VALUE,
                 "Invalid eAxC value '{}'. Maximum eAxC value is '{}'",
                 eaxc,
                 MAX_SUPPORTED_EAXC_ID_VALUE);

    return counters[eaxc].fetch_add(1, std::memory_order_relaxed);
  }
};

} // namespace ofh
} // namespace ocudu
