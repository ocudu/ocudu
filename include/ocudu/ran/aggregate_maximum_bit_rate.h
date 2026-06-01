// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

struct aggregate_maximum_bit_rate_t {
  uint64_t dl = 0;
  uint64_t ul = 0;
};

} // namespace ocudu
