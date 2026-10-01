// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <array>

namespace ocudu {
namespace test_helper {

inline slot_point generate_random_slot_point(subcarrier_spacing scs)
{
  static std::array<std::uniform_int_distribution<uint32_t>, NOF_NUMEROLOGIES> scs_dists = {
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz15)) - 1},
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz30)) - 1},
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz60)) - 1},
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz120)) - 1},
      std::uniform_int_distribution<uint32_t>{0, (10240 * get_nof_slots_per_subframe(subcarrier_spacing::kHz240)) - 1}};

  uint32_t count = scs_dists[to_numerology_value(scs)](test_rng::tls_gen());
  return slot_point{scs, count};
}

} // namespace test_helper
} // namespace ocudu
