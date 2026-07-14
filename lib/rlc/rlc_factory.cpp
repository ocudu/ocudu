// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/rlc/rlc_factory.h"
#include "rlc_am_entity.h"
#include "rlc_tm_entity.h"
#include "rlc_um_entity.h"
#include "ocudu/rlc/rlc_window_seg_pools.h"

using namespace ocudu;

std::unique_ptr<rlc_entity> ocudu::create_rlc_entity(const rlc_entity_creation_message& msg)
{
  switch (msg.config.mode) {
    case rlc_mode::tm:
      return std::make_unique<rlc_tm_entity>(msg.gnb_du_id,
                                             msg.ue_index,
                                             msg.rb_id,
                                             msg.config.tm,
                                             msg.config.metrics_period,
                                             msg.rlc_metrics_notif,
                                             *msg.rx_upper_dn,
                                             *msg.tx_upper_dn,
                                             *msg.tx_upper_cn,
                                             *msg.tx_lower_dn,
                                             *msg.pcap_writer,
                                             *msg.pcell_executor,
                                             *msg.ue_executor,
                                             *msg.timers);
    case rlc_mode::um_unidir_dl:
    case rlc_mode::um_unidir_ul:
    case rlc_mode::um_bidir:
      return std::make_unique<rlc_um_entity>(msg.gnb_du_id,
                                             msg.ue_index,
                                             msg.rb_id,
                                             msg.config.um,
                                             msg.config.metrics_period,
                                             msg.rlc_metrics_notif,
                                             *msg.rx_upper_dn,
                                             *msg.tx_upper_dn,
                                             *msg.tx_upper_cn,
                                             *msg.tx_lower_dn,
                                             *msg.pcap_writer,
                                             *msg.pcell_executor,
                                             *msg.ue_executor,
                                             *msg.timers,
                                             get_rlc_drb_um_rx_window_seg_pool());
    case rlc_mode::am:
      return std::make_unique<rlc_am_entity>(
          msg.gnb_du_id,
          msg.ue_index,
          msg.rb_id,
          msg.config.am,
          msg.config.metrics_period,
          msg.rlc_metrics_notif,
          *msg.rx_upper_dn,
          *msg.tx_upper_dn,
          *msg.tx_upper_cn,
          *msg.tx_lower_dn,
          *msg.pcap_writer,
          *msg.pcell_executor,
          *msg.ue_executor,
          *msg.timers,
          msg.rb_id.is_srb() ? get_rlc_srb_am_rx_window_seg_pool() : get_rlc_drb_am_rx_window_seg_pool(),
          msg.rb_id.is_srb() ? get_rlc_srb_am_tx_window_seg_pool() : get_rlc_drb_am_tx_window_seg_pool());
    default:
      ocudu_terminate("RLC mode not supported.");
  }
  return nullptr;
}
