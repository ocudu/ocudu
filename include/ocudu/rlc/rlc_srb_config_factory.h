// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/rlc/rlc_config.h"

namespace ocudu {

/// \brief SRB0 default configuration (only implementation-specific parameters)
inline rlc_config make_default_srb0_rlc_config()
{
  rlc_config cfg             = {};
  cfg.mode                   = rlc_mode::tm;
  cfg.tm.tx.queue_size       = 8;
  cfg.tm.tx.queue_size_bytes = 8 * 2000;
  cfg.metrics_period         = std::chrono::milliseconds(0); // disable metrics reporting for SRBs
  return cfg;
}

/// \brief SRB1, SRB2 and SRB3 default configuration as per TS 38.331, 9.2.1.
inline rlc_config make_default_srb_rlc_config()
{
  rlc_config cfg              = {};
  cfg.mode                    = rlc_mode::am;
  cfg.am.tx.sn_field_length   = rlc_am_sn_size::size12bits;
  cfg.am.tx.pdcp_sn_len       = pdcp_sn_size::size12bits;
  cfg.am.tx.t_poll_retx       = 45;
  cfg.am.tx.poll_pdu          = -1;
  cfg.am.tx.poll_byte         = -1;
  cfg.am.tx.max_retx_thresh   = 8;
  cfg.am.tx.queue_size        = 32;
  cfg.am.tx.queue_size_bytes  = 64000;
  cfg.am.rx.sn_field_length   = rlc_am_sn_size::size12bits;
  cfg.am.rx.t_reassembly      = 35;
  cfg.am.rx.t_status_prohibit = 0;
  cfg.am.rx.max_sn_per_status = {};
  cfg.metrics_period          = std::chrono::milliseconds(0); // disable metrics reporting for SRBs
  return cfg;
}

} // namespace ocudu
