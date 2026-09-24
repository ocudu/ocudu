// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstdint>

namespace ocudu {
namespace ecpri {

/// eCPRI decoder metrics.
struct ecpri_decoder_metrics {
  /// Number of corrupted messages.
  uint64_t nof_corrupted_messages;
  /// Number of received messages with a sequence identifier from the past.
  uint64_t nof_past_seq_id_messages;
  /// Number of sequence identifiers skipped by the received messages.
  uint64_t nof_future_seq_id_messages;
};

} // namespace ecpri
} // namespace ocudu
