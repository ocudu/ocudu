// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/receiver/ofh_sequence_id_checker.h"

namespace ocudu {
namespace ofh {

/// Open Fronthaul sequence identifier checker dummy implementation.
class sequence_id_checker_dummy_impl : public sequence_id_checker
{
public:
  // See interface for documentation.
  int update_and_compare_seq_id(unsigned eaxc, uint8_t seq_id) override { return 0; }
};

} // namespace ofh
} // namespace ocudu
