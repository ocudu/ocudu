// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rlc_rx_entity.h"

namespace ocudu {

class rlc_rx_tm_entity : public rlc_rx_entity
{
public:
  rlc_rx_tm_entity(gnb_du_id_t                       gnb_du_id,
                   du_ue_index_t                     ue_index,
                   rb_id_t                           rb_id,
                   const rlc_rx_tm_config&           config,
                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                   rlc_bearer_metrics_collector&     metrics_coll_,
                   rlc_pcap&                         pcap_,
                   task_executor&                    ue_executor,
                   timer_manager&                    timers);

  void stop() final
  {
    // Stop all timers.
    high_metrics_timer.stop();
  }

  // Interfaces for higher layers
  void handle_pdu(byte_buffer_slice buf) override;

private:
  // Config storage
  const rlc_rx_tm_config cfg;

  pcap_rlc_pdu_context pcap_context;
};

} // namespace ocudu
