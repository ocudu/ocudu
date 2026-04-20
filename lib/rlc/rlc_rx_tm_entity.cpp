// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rlc_rx_tm_entity.h"

using namespace ocudu;

rlc_rx_tm_entity::rlc_rx_tm_entity(gnb_du_id_t                       gnb_du_id,
                                   du_ue_index_t                     ue_index,
                                   rb_id_t                           rb_id,
                                   const rlc_rx_tm_config&           config,
                                   rlc_rx_upper_layer_data_notifier& upper_dn_,
                                   rlc_bearer_metrics_collector&     metrics_coll_,
                                   rlc_pcap&                         pcap_,
                                   task_executor&                    ue_executor,
                                   timer_manager&                    timers) :
  rlc_rx_entity(gnb_du_id, ue_index, rb_id, upper_dn_, metrics_coll_, pcap_, ue_executor, timers),
  cfg(config),
  pcap_context(ue_index, rb_id, /* is_uplink */ true)
{
  metrics.metrics_set_mode(rlc_mode::tm);
  logger.log_info("RLC TM created. {}", cfg);
}

void rlc_rx_tm_entity::handle_pdu(byte_buffer_slice buf)
{
  size_t pdu_len = buf.length();
  metrics.metrics_add_pdus(1, pdu_len);
  if (pdu_len == 0) {
    logger.log_warning("Dropped empty PDU.");
    return;
  }

  pcap.push_pdu(pcap_context, buf);

  expected<byte_buffer_chain> sdu = byte_buffer_chain::create(std::move(buf));
  if (!sdu) {
    logger.log_error("Dropped SDU, failed to create SDU buffer. sdu_len={}", pdu_len);
    metrics.metrics_add_lost_pdus(1);
    return;
  }

  logger.log_info(sdu.value().begin(), sdu.value().end(), "RX SDU. sdu_len={}", sdu.value().length());
  metrics.metrics_add_sdus(1, sdu.value().length());
  upper_dn.on_new_sdu(std::move(sdu.value()));
}
