// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "test_rng.h"

namespace ocudu::test_rng {

/// Called once during a process lifetime to initialize a base seed.
void init_base_seed(uint32_t base_seed_);

/// Get the base seed.
uint32_t base_seed();

/// Called on the start of each new iteration, to generate a running seed based on the iteration index and base seed.
void advance_iter_seed(uint32_t iter);

/// Called on the start of a new test to rewind generator state.
void rewind_rng();

/// Compute base_seed that at iteration 0 leads to the current iter_seed.
uint32_t compute_base_seed_at_iter0(uint32_t iter);

} // namespace ocudu::test_rng
