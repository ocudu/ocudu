// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Common SCTP socket option parameters shared across application configurations.
struct sctp_appconfig {
  /// SCTP initial RTO value in milliseconds (-1 to use system default).
  int rto_initial_ms = 120;
  /// SCTP RTO min in milliseconds (-1 to use system default).
  int rto_min_ms = 120;
  /// SCTP RTO max in milliseconds (-1 to use system default).
  int rto_max_ms = 500;
  /// SCTP init max attempts (-1 to use system default).
  int init_max_attempts = 3;
  /// SCTP max init timeout in milliseconds (-1 to use system default).
  int max_init_timeo_ms = 500;
  /// SCTP heartbeat interval in milliseconds (-1 to use system default).
  int hb_interval_ms = 30000;
  /// SCTP association max retransmissions (-1 to use system default).
  int assoc_max_retx = 10;
  /// Send SCTP messages as soon as possible without any Nagle-like algorithm.
  bool nodelay = false;
};

} // namespace ocudu
