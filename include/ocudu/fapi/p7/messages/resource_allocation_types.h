// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/resource_allocation/rb_interval.h"

namespace ocudu {
namespace fapi {

/// Holds the parameters for the Resource Allocation Type 1.
struct resource_allocation_type_1 {
  vrb_interval vrbs;
};

} // namespace fapi
} // namespace ocudu
