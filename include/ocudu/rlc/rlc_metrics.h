// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/rlc/rlc_rx_metrics.h"
#include "ocudu/rlc/rlc_tx_metrics.h"

namespace ocudu {

/// Container to hold TX/RX metrics
struct rlc_metrics {
  gnb_du_id_t    du_index;
  du_ue_index_t  ue_index;
  rb_id_t        rb_id;
  rlc_tx_metrics tx;
  rlc_rx_metrics rx;
  unsigned       counter;
  timer_duration metrics_period;
};

/// \brief Notifier interface used to report RLC metrics.
class rlc_metrics_notifier
{
public:
  virtual ~rlc_metrics_notifier() = default;

  /// \brief This method will be called periodically to report the latest RLC metrics statistics.
  virtual void report_metrics(const rlc_metrics& metrics) = 0;
};
} // namespace ocudu
