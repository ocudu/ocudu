// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_up_ue_logger.h"
#include "ngu_session_manager.h"
#include "pdu_session.h"
#include "pdu_session_manager.h"
#include "ocudu/cu_up/cu_up_config.h"
#include "ocudu/cu_up/cu_up_types.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/f1u/cu_up/f1u_gateway.h"
#include "ocudu/gtpu/gtpu_demux.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/support/rate_limiting/token_bucket.h"
#include "ocudu/support/timers.h"
#include <map>

namespace ocudu::ocuup {

/// PDU session manager dependencies.
struct pdu_session_manager_dependencies {
  cu_up_ue_logger&              logger;
  unique_timer&                 ue_inactivity_timer;
  timer_factory                 ue_dl_timer_factory;
  timer_factory                 ue_ul_timer_factory;
  timer_factory                 ue_ctrl_timer_factory;
  e1ap_interface&               e1ap;
  f1u_cu_up_gateway&            f1u_gw;
  ngu_session_manager&          ngu_session_mngr;
  cu_up_manager_pdcp_interface& cu_up_mngr_pdcp_if;
  gtpu_teid_pool&               n3_teid_allocator;
  gtpu_teid_pool&               f1u_teid_allocator;
  gtpu_demux_ctrl&              gtpu_rx_demux;
  task_executor&                ue_dl_exec;
  task_executor&                ue_ul_exec;
  task_executor&                ue_ctrl_exec;
  task_executor&                crypto_exec;
  dlt_pcap&                     gtpu_pcap;
};

class pdu_session_manager_impl final : public pdu_session_manager_ctrl
{
public:
  pdu_session_manager_impl(cu_up_ue_index_t                             ue_index_,
                           std::map<five_qi_t, ocuup::cu_up_qos_config> qos_cfg_,
                           const security::sec_as_config&               security_info_,
                           const n3_interface_config&                   n3_config_,
                           const cu_up_test_mode_config&                test_mode_config_,
                           uint64_t                                     ue_dl_ambr,
                           const pdu_session_manager_dependencies&      dependencies);

  pdu_session_setup_result        setup_pdu_session(const e1ap_pdu_session_res_to_setup_item& session) override;
  pdu_session_modification_result modify_pdu_session(const e1ap_pdu_session_res_to_modify_item& session,
                                                     bool new_ul_tnl_info_required) override;
  void                            remove_pdu_session(pdu_session_id_t pdu_session_id) override;
  size_t                          get_nof_pdu_sessions() override;
  pdu_session_state_t             get_pdu_session_state() override;

  void disconnect_pdu_session(pdu_session_id_t pdu_session_id);
  void disconnect_all_pdu_sessions();
  void update_security_config(const security::sec_as_config& security_info);

  void notify_pdcp_pdu_processing_stopped();
  void restart_pdcp_pdu_processing();

  void begin_pdcp_buffering();
  void end_pdcp_buffering();

  void suspend();
  void resume();

  async_task<void> await_crypto_rx_all_pdu_sessions();
  async_task<void> await_crypto_rx_all_drbs(const std::unique_ptr<pdu_session>& pdu_session);
  async_task<void> await_crypto_tx_all_pdu_sessions();
  async_task<void> await_crypto_tx_all_drbs(const std::unique_ptr<pdu_session>& pdu_session);

  /// \brief Function used to allocate a local NG-U TEID
  /// This function allocates a new TEID based on the UE id, and PDU session ID.
  /// The allocation should look like this,
  ///   |X X X X | 0..3
  ///   |U U P P | 4..7
  /// where X is reserved, U are the bytes for UE id and P is the byte for PDU session Id.
  gtpu_teid_t allocate_local_teid(pdu_session_id_t pdu_session_id);

private:
  drb_setup_result handle_drb_to_setup_item(pdu_session&                         new_session,
                                            const e1ap_drb_to_setup_item_ng_ran& drb_to_setup);

  /// \brief Allocate the local endpoint of a DL data forwarding tunnel on the NG-U interface.
  ///
  /// The endpoint is reported to the gNB-CU-CP so that it can be advertised to the source NG-RAN node, which sends the
  /// data it still holds for the UE to it (TS 37.483 section 9.3.2.6). It shares the NG-U bind address of the PDU
  /// session, so that the forwarded and the freshly arriving DL packets reach the same gateway.
  std::optional<up_transport_layer_info> allocate_dl_data_forwarding_tnl_info(const std::string& n3_addr);

  cu_up_ue_index_t                                         ue_index;
  const std::map<five_qi_t, ocuup::cu_up_qos_config>       qos_cfg;
  security::sec_as_config                                  security_info;
  const n3_interface_config&                               n3_config;
  cu_up_test_mode_config                                   test_mode_config;
  cu_up_ue_logger&                                         logger;
  std::unique_ptr<token_bucket>                            ue_ambr_limiter;
  unique_timer&                                            ue_inactivity_timer;
  timer_factory                                            ue_dl_timer_factory;
  timer_factory                                            ue_ul_timer_factory;
  timer_factory                                            ue_ctrl_timer_factory;
  gtpu_teid_pool&                                          n3_teid_allocator;
  gtpu_teid_pool&                                          f1u_teid_allocator;
  gtpu_demux_ctrl&                                         gtpu_rx_demux;
  task_executor&                                           ue_dl_exec;
  task_executor&                                           ue_ul_exec;
  task_executor&                                           ue_ctrl_exec;
  task_executor&                                           crypto_exec;
  dlt_pcap&                                                gtpu_pcap;
  e1ap_interface&                                          e1ap;
  f1u_cu_up_gateway&                                       f1u_gw;
  ngu_session_manager&                                     ngu_session_mngr;
  cu_up_manager_pdcp_interface&                            cu_up_mngr_pdcp_if;
  std::map<pdu_session_id_t, std::unique_ptr<pdu_session>> pdu_sessions; // key is pdu_session_id
};

} // namespace ocudu::ocuup
