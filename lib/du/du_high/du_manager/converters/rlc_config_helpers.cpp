// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rlc_config_helpers.h"

using namespace ocudu;
using namespace odu;

template <typename Bearer>
static void fill_rlc_entity_creation_message_common(rlc_entity_creation_message&                msg,
                                                    gnb_du_id_t                                 du_id,
                                                    du_ue_index_t                               ue_index,
                                                    du_cell_index_t                             pcell_index,
                                                    Bearer&                                     bearer,
                                                    const rlc_config&                           rlc_cfg,
                                                    const du_manager_params::service_params&    du_services,
                                                    rlc_tx_upper_layer_control_notifier&        rlc_rlf_notifier,
                                                    const du_manager_params::rlc_config_params& rlc_params,
                                                    const du_manager_resources::rlc_resources&  rlc_resources)
{
  msg.gnb_du_id      = du_id;
  msg.ue_index       = ue_index;
  msg.config         = rlc_cfg;
  msg.rx_upper_dn    = &bearer.connector.rlc_rx_sdu_notif;
  msg.tx_upper_dn    = &bearer.connector.rlc_tx_data_notif;
  msg.tx_upper_cn    = &rlc_rlf_notifier;
  msg.tx_lower_dn    = &bearer.connector.rlc_tx_buffer_state_notif;
  msg.timers         = &du_services.timers;
  msg.pcell_executor = &du_services.cell_execs.rlc_lower_executor(pcell_index);
  msg.ue_executor    = &du_services.ue_execs.ctrl_executor(ue_index);
  msg.pcap_writer    = &rlc_params.pcap_writer;
  msg.drb_am_rx_pool = &rlc_resources.drb_rx_window_seg_pool->get_pool_of_type<rlc_rx_am_sdu_info>();
  msg.drb_am_tx_pool = &rlc_resources.drb_tx_window_seg_pool->get_pool_of_type<rlc_tx_am_sdu_info>();
  msg.drb_um_rx_pool = &rlc_resources.drb_rx_window_seg_pool->get_pool_of_type<rlc_rx_um_sdu_info>();
  msg.srb_am_rx_pool = &rlc_resources.srb_rx_window_seg_pool->get_pool_of_type<rlc_rx_am_sdu_info>();
  msg.srb_am_tx_pool = &rlc_resources.srb_tx_window_seg_pool->get_pool_of_type<rlc_tx_am_sdu_info>();
}

// for SRBs
rlc_entity_creation_message
ocudu::odu::make_rlc_entity_creation_message(gnb_du_id_t                                 gnb_du_id,
                                             du_ue_index_t                               ue_index,
                                             du_cell_index_t                             pcell_index,
                                             du_ue_srb&                                  bearer,
                                             const rlc_config&                           rlc_cfg,
                                             const du_manager_params::service_params&    du_services,
                                             rlc_tx_upper_layer_control_notifier&        rlc_rlf_notifier,
                                             const du_manager_params::rlc_config_params& rlc_params,
                                             const du_manager_resources::rlc_resources&  rlc_resources)
{
  rlc_entity_creation_message msg;
  fill_rlc_entity_creation_message_common(
      msg, gnb_du_id, ue_index, pcell_index, bearer, rlc_cfg, du_services, rlc_rlf_notifier, rlc_params, rlc_resources);
  msg.rb_id             = bearer.srb_id;
  msg.rlc_metrics_notif = rlc_params.rlc_metrics_notif;
  return msg;
}

// for DRBs
rlc_entity_creation_message
ocudu::odu::make_rlc_entity_creation_message(gnb_du_id_t                                 gnb_du_id,
                                             du_ue_index_t                               ue_index,
                                             du_cell_index_t                             pcell_index,
                                             du_ue_drb&                                  bearer,
                                             const rlc_config&                           rlc_cfg,
                                             const du_manager_params::service_params&    du_services,
                                             rlc_tx_upper_layer_control_notifier&        rlc_rlf_notifier,
                                             const du_manager_params::rlc_config_params& rlc_params,
                                             const du_manager_resources::rlc_resources&  rlc_resources)
{
  rlc_entity_creation_message msg;
  fill_rlc_entity_creation_message_common(
      msg, gnb_du_id, ue_index, pcell_index, bearer, rlc_cfg, du_services, rlc_rlf_notifier, rlc_params, rlc_resources);
  msg.rb_id             = bearer.drb_id;
  msg.rlc_metrics_notif = rlc_params.rlc_metrics_notif;
  if (msg.rlc_metrics_notif == nullptr) {
    msg.config.metrics_period = timer_duration{0};
  }
  return msg;
}
