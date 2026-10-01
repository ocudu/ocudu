// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/cu_up/f1u_rx_metrics.h"
#include "ocudu/f1u/cu_up/f1u_rx_pdu_handler.h"
#include "ocudu/f1u/cu_up/f1u_tx_metrics.h"
#include "ocudu/f1u/cu_up/f1u_tx_sdu_handler.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include "ocudu/support/timers.h"

namespace ocudu::ocuup {

struct f1u_metrics_container {
  uint32_t                 ue_index;
  drb_id_t                 drb_id;
  f1u_tx_metrics_container tx;
  f1u_rx_metrics_container rx;
  timer_duration           metrics_period;
};

class f1u_bearer_disconnector;

class f1u_bearer
{
public:
  f1u_bearer()          = default;
  virtual ~f1u_bearer() = default;

  f1u_bearer(const f1u_bearer&)            = delete;
  f1u_bearer(f1u_bearer&&)                 = delete;
  f1u_bearer& operator=(const f1u_bearer&) = delete;
  f1u_bearer& operator=(f1u_bearer&&)      = delete;

  virtual void                stop()               = 0;
  virtual f1u_rx_pdu_handler& get_rx_pdu_handler() = 0;
  virtual f1u_tx_sdu_handler& get_tx_sdu_handler() = 0;
};

/// This class represents the interface through which a F1-U bearer disconnects from its gateway upon destruction
class f1u_bearer_disconnector
{
public:
  virtual ~f1u_bearer_disconnector() = default;

  virtual void disconnect_cu_bearer(const up_transport_layer_info& ul_up_tnl_info) = 0;
};

} // namespace ocudu::ocuup

namespace fmt {
template <>
struct formatter<ocudu::ocuup::f1u_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::ocuup::f1u_metrics_container& m, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "ue={} drb={} tx=[{}] rx=[{}]",
                     m.ue_index,
                     m.drb_id,
                     ocudu::ocuup::format_f1u_tx_metrics(m.metrics_period, m.tx),
                     ocudu::ocuup::format_f1u_rx_metrics(m.metrics_period, m.rx));
  }
};
} // namespace fmt
