// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1u_bearer_impl.h"

using namespace ocudu;
using namespace ocuup;

f1u_bearer_impl::f1u_bearer_impl(uint32_t                       ue_index,
                                 drb_id_t                       drb_id_,
                                 const up_transport_layer_info& ul_tnl_info_,
                                 const f1u_config&              config,
                                 f1u_tx_pdu_notifier&           tx_pdu_notifier_,
                                 f1u_rx_delivery_notifier&      rx_delivery_notifier_,
                                 f1u_rx_sdu_notifier&           rx_sdu_notifier_,
                                 timer_factory                  ue_dl_timer_factory,
                                 timer_factory                  ue_ctrl_timer_factory,
                                 unique_timer&                  ue_inactivity_timer_,
                                 task_executor&                 dl_exec_,
                                 task_executor&                 ul_exec_,
                                 task_executor&                 ctrl_exec_) :
  logger("CU-F1-U", {ue_index, drb_id_, ul_tnl_info_}),
  metrics_period(config.metrics_period),
  metrics_timer(ue_ctrl_timer_factory.create_timer()),
  metrics_agg(ue_index, drb_id_, ul_tnl_info_, metrics_period, config.metrics_notifier, ctrl_exec_),
  tx_metrics(metrics_agg.get_metrics_period().count()),
  rx_metrics(metrics_agg.get_metrics_period().count()),
  cfg(config),
  tx_pdu_notifier(tx_pdu_notifier_),
  rx_delivery_notifier(rx_delivery_notifier_),
  rx_sdu_notifier(rx_sdu_notifier_),
  ul_tnl_info(ul_tnl_info_),
  dl_exec(dl_exec_),
  ul_exec(ul_exec_),
  dl_notif_timer(ue_dl_timer_factory.create_timer()),
  ue_inactivity_timer(ue_inactivity_timer_)
{
  if (metrics_agg.get_metrics_period().count()) {
    metrics_timer = ue_ctrl_timer_factory.create_timer();
    metrics_timer.set(std::chrono::milliseconds(metrics_agg.get_metrics_period().count()), [this]() {
      if (stopped) {
        return;
      }
      metrics_agg.push_tx_metrics(tx_metrics.get_metrics_and_reset());
      metrics_agg.push_rx_metrics(rx_metrics.get_metrics_and_reset());
      metrics_timer.run();
    });
    metrics_timer.run();
  }

  dl_notif_timer.set(std::chrono::milliseconds(config.dl_t_notif_timer), [this]() { on_expired_dl_notif_timer(); });

  auto dispatch_fn = [this](span<nru_ul_message> msg_span) {
    for (nru_ul_message& msg : msg_span) {
      handle_pdu_impl(std::move(msg));
    }
  };
  ul_batched_queue = std::make_unique<nru_ul_batched_queue>(
      "CU-NRU-UL", config.queue_size, ul_exec, logger.get_basic_logger(), dispatch_fn, config.batch_size);

  logger.log_info("F1-U bearer configured. {}", cfg);
}

void f1u_bearer_impl::handle_pdu(nru_ul_message msg)
{
  if (stopped) {
    return;
  }
  if (not ul_batched_queue->try_push(std::move(msg))) {
    if (!cfg.warn_on_drop) {
      logger.log_info("Dropped F1-U PDU, queue is full.");
    } else {
      rx_metrics.add_dropped_pdus(1);
      logger.log_warning("Dropped F1-U PDU, queue is full.");
    }
  }
}

void f1u_bearer_impl::handle_pdu_impl(nru_ul_message msg)
{
  if (stopped) {
    return;
  }

  rx_metrics.add_pdus(1);
  logger.log_debug("F1-U bearer received PDU");

  // handle T-PDU
  if (msg.t_pdu.has_value() && !msg.t_pdu->empty()) {
    ue_inactivity_timer.run(); // restart inactivity timer due to UL PDU
    rx_metrics.add_sdus(1, msg.t_pdu->length());
    logger.log_debug("Delivering T-PDU of size={}", msg.t_pdu->length());
    rx_sdu_notifier.on_new_sdu(std::move(*msg.t_pdu));
  }

  // handle data delivery status (transmit notifications)
  if (msg.data_delivery_status.has_value()) {
    rx_metrics.add_dds(1);
    nru_dl_data_delivery_status& status = *msg.data_delivery_status;

    // Desired buffer size
    if (not dl_exec.defer([this, status]() {
          rx_delivery_notifier.on_desired_buffer_size_notification(status.desired_buffer_size_for_drb);
        })) {
      rx_metrics.add_dds_failures(1);
      logger.log_warning("Could not pass desired buffer size notification to PDCP");
    }

    // Highest transmitted PDCP SN
    if (status.highest_transmitted_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_transmitted_pdcp_sn.value();
      if (pdcp_sn != notif_highest_transmitted_pdcp_sn) {
        ue_inactivity_timer.run(); // restart inactivity timer due to confirmed transmission of DL PDU
        logger.log_debug("Notifying highest transmitted pdcp_sn={}", pdcp_sn);
        notif_highest_transmitted_pdcp_sn = pdcp_sn;
        if (not dl_exec.defer([this, pdcp_sn]() { rx_delivery_notifier.on_transmit_notification(pdcp_sn); })) {
          rx_metrics.add_dds_failures(1);
          logger.log_warning("Could not pass desired buffer size notification to PDCP");
        }
      } else {
        logger.log_debug("Ignored duplicate notification of highest transmitted pdcp_sn={}", pdcp_sn);
      }
    }
    // Highest successfully delivered PDCP SN
    if (status.highest_delivered_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_delivered_pdcp_sn.value();
      if (pdcp_sn != notif_highest_delivered_pdcp_sn) {
        logger.log_debug("Notifying highest successfully delivered pdcp_sn={}", pdcp_sn);
        notif_highest_delivered_pdcp_sn = pdcp_sn;
        if (not dl_exec.defer([this, pdcp_sn]() { rx_delivery_notifier.on_delivery_notification(pdcp_sn); })) {
          rx_metrics.add_dds_failures(1);
          logger.log_warning("Could not pass highest delivered notification to PDCP");
        }
      } else {
        logger.log_debug("Ignored duplicate notification of highest successfully delivered pdcp_sn={}", pdcp_sn);
      }
    }
    // Highest retransmitted PDCP SN
    if (status.highest_retransmitted_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_retransmitted_pdcp_sn.value();
      logger.log_debug("Notifying highest retransmitted pdcp_sn={}", pdcp_sn);
      if (not dl_exec.defer([this, pdcp_sn]() { rx_delivery_notifier.on_retransmit_notification(pdcp_sn); })) {
        rx_metrics.add_dds_failures(1);
        logger.log_warning("Could not pass highest retransmitted notification to PDCP");
      }
    }
    // Highest successfully delivered retransmitted PDCP SN
    if (status.highest_delivered_retransmitted_pdcp_sn.has_value()) {
      uint32_t pdcp_sn = status.highest_delivered_retransmitted_pdcp_sn.value();
      logger.log_debug("Notifying highest successfully delivered retransmitted pdcp_sn={}", pdcp_sn);
      if (not dl_exec.defer(
              [this, pdcp_sn]() { rx_delivery_notifier.on_delivery_retransmitted_notification(pdcp_sn); })) {
        rx_metrics.add_dds_failures(1);
        logger.log_warning("Could not pass highest retransmitted notification to PDCP");
      }
    }
  }
}

void f1u_bearer_impl::handle_sdu(byte_buffer sdu, bool is_retx)
{
  if (stopped) {
    return;
  }

  tx_metrics.add_sdus(1, sdu.length());
  logger.log_debug("F1-U bearer received SDU. size={} is_retx={}", sdu.length(), is_retx);
  nru_dl_message msg = {};

  // attach the SDU
  msg.t_pdu = std::move(sdu);

  // set retransmission flag
  msg.dl_user_data.retransmission_flag = is_retx;

  // attach discard blocks (if any)
  fill_discard_blocks(msg);

  // stop backoff timer
  dl_notif_timer.stop();

  tx_metrics.add_pdus(1);
  tx_pdu_notifier.on_new_pdu(std::move(msg));
}

void f1u_bearer_impl::discard_sdu(uint32_t pdcp_sn)
{
  tx_metrics.add_sdu_discards(1);
  // start backoff timer
  if (!dl_notif_timer.is_running()) {
    dl_notif_timer.run();
  }
  if (discard_blocks.empty()) {
    discard_blocks.push_back(nru_pdcp_sn_discard_block{});
    nru_pdcp_sn_discard_block& block = discard_blocks.back();
    block.pdcp_sn_start              = pdcp_sn;
    block.block_size                 = 1;
    logger.log_debug("Queued first SDU discard block with pdcp_sn={}", pdcp_sn);
  } else {
    nru_pdcp_sn_discard_block& last_block = discard_blocks.back();
    if (last_block.pdcp_sn_start + last_block.block_size == pdcp_sn) {
      last_block.block_size++;
      logger.log_debug("Expanded previous SDU discard block with pdcp_sn={}. pdcp_sn_start={} block_size={}",
                       pdcp_sn,
                       last_block.pdcp_sn_start,
                       last_block.block_size);
    } else {
      discard_blocks.push_back(nru_pdcp_sn_discard_block{});
      nru_pdcp_sn_discard_block& block = discard_blocks.back();
      block.pdcp_sn_start              = pdcp_sn;
      block.block_size                 = 1;
      logger.log_debug("Queued next SDU discard block with pdcp_sn={}", pdcp_sn);
      if (discard_blocks.full()) {
        logger.log_debug("Flushing SDU discard block notification. nof_blocks={}", discard_blocks.capacity());
        flush_discard_blocks();
      }
    }
  }
}

void f1u_bearer_impl::fill_discard_blocks(nru_dl_message& msg)
{
  if (!discard_blocks.empty()) {
    msg.dl_user_data.discard_blocks = std::move(discard_blocks);
    discard_blocks                  = {};
  }
}

void f1u_bearer_impl::on_expired_dl_notif_timer()
{
  if (stopped) {
    return;
  }
  logger.log_debug("DL notification timer expired");
  flush_discard_blocks();
}

void f1u_bearer_impl::flush_discard_blocks()
{
  nru_dl_message msg = {};

  // attach discard blocks (if any)
  fill_discard_blocks(msg);

  // stop backoff timer
  dl_notif_timer.stop();

  if (msg.dl_user_data.discard_blocks.has_value()) {
    logger.log_debug("Sending discard blocks");
    tx_metrics.add_pdus(1);
    tx_pdu_notifier.on_new_pdu(std::move(msg));
  }
}
