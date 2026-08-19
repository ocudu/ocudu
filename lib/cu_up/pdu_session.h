// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "adapters/gtpu_adapters.h"
#include "adapters/sdap_adapters.h"
#include "drb_context.h"
#include "ocudu/adt/batched_dispatch_queue.h"
#include "ocudu/gtpu/gtpu_demux.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/gtpu/gtpu_tunnel_ngu.h"
#include "ocudu/ran/up_transport_layer_info.h"

namespace ocudu::ocuup {

#define MAX_NUM_PDU_SESSIONS_PER_UE (8) /// Todo: find 3GPP spec reference

/// \brief Context for PDU session with session-wide parameters and all contained DRBs.
struct pdu_session {
  pdu_session(const e1ap_pdu_session_res_to_setup_item& session,
              gtpu_teid_t      local_teid_, // the local teid used by the gNB for this PDU session
              gtpu_demux_ctrl& gtpu_rx_demux_,
              gtpu_teid_pool&  n3_teid_allocator_) :
    pdu_session_id(session.pdu_session_id),
    snssai(session.snssai),
    security_ind(session.security_ind),
    local_teid(local_teid_),
    ul_tunnel_info(session.ng_ul_up_tnl_info),
    gtpu_rx_demux(gtpu_rx_demux_),
    n3_teid_allocator(n3_teid_allocator_)
  {
  }
  ~pdu_session() { stop(); }

  void stop()
  {
    if (not stopped) {
      gtpu_rx_demux.remove_tunnel(local_teid);
      (void)n3_teid_allocator.release_teid(local_teid);

      if (dl_data_forwarding_tnl_info.has_value()) {
        (void)n3_teid_allocator.release_teid(dl_data_forwarding_tnl_info->gtp_teid);
      }

      if (dispatch_queue != nullptr) {
        dispatch_queue->stop();
      }
      gtpu->stop();

      // Stop DRBs
      for (const auto& drb : drbs) {
        drb.second->stop();
      }
    }
    stopped = true;
  }

  bool stopped = false;

  std::unique_ptr<sdap_entity>     sdap;
  std::unique_ptr<gtpu_tunnel_ngu> gtpu;

  // Adapters between SDAP and GTPU
  gtpu_sdap_adapter gtpu_to_sdap_adapter;
  sdap_gtpu_adapter sdap_to_gtpu_adapter;

  // Adapters between GTP-U and NG-U/F1-U
  gtpu_network_gateway_adapter gtpu_to_udp_adapter;
  pdcp_f1u_adapter             pdcp_to_f1u_adapter;

  // PDU Session parameters
  pdu_session_id_t        pdu_session_id;
  s_nssai_t               snssai;
  security_indication_t   security_ind;
  gtpu_teid_t             local_teid;     // the local teid used by the gNB for this PDU session
  up_transport_layer_info ul_tunnel_info; // the peer GTP-U address and TEID

  /// Local endpoint of the PDU session level DL data forwarding tunnel, allocated when the gNB-CU-CP requests PDU
  /// session level data forwarding (TS 37.483 section 9.3.2.5).
  std::optional<up_transport_layer_info> dl_data_forwarding_tnl_info;

  // GTP-U demux parameters
  gtpu_demux_ctrl&                           gtpu_rx_demux;     // The demux entity to register/remove the tunnel.
  gtpu_teid_pool&                            n3_teid_allocator; // Pool to de-allocate TEID on release
  std::unique_ptr<gtpu_demux_dispatch_queue> dispatch_queue;

  // DRB contexts
  drb_context*                                     default_drb = nullptr; // non-owning pointer to default DRB, if any
  std::map<drb_id_t, std::unique_ptr<drb_context>> drbs;                  // key is drb_id
};

} // namespace ocudu::ocuup
