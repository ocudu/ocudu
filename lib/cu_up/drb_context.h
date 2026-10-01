// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "adapters/f1u_adapters.h"
#include "adapters/gw_adapters.h"
#include "adapters/pdcp_adapters.h"
#include "qos_flow_context.h"
#include "ocudu/f1u/cu_up/f1u_config.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/pdcp/pdcp_entity.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include <map>
#include <optional>

namespace ocudu {
namespace ocuup {

/// \brief DRB context with map to all QoS flows.
struct drb_context {
  drb_context(const drb_id_t& drb_id_, gtpu_teid_pool& ngu_teid_allocator_) :
    drb_id(drb_id_), ngu_teid_allocator(ngu_teid_allocator_)
  {
  }
  ~drb_context() { stop(); }

  void stop()
  {
    if (!stopped) {
      if (ingress_dl_data_forwarding_tnl_info.has_value()) {
        (void)ngu_teid_allocator.release_teid(ingress_dl_data_forwarding_tnl_info->gtp_teid);
      }
      if (pdcp) {
        pdcp->stop();
      }
      if (f1u) {
        f1u->stop();
      }
      if (f1u_gw_bearer) {
        f1u_gw_bearer->stop();
      }
      stopped = true;
    }
  }

  bool stopped = false;

  drb_id_t    drb_id;
  gtpu_teid_t f1u_ul_teid;
  f1u_config  f1u_cfg;

  /// Local endpoint of the DL data forwarding tunnel of this DRB, where this node receives the forwarded data.
  /// Allocated when the gNB-CU-CP requests DRB level data forwarding (TS 37.483 section 9.3.2.5).
  std::optional<up_transport_layer_info> ingress_dl_data_forwarding_tnl_info;

  /// Peer endpoint of the DL data forwarding tunnel of this DRB, where this node sends the data it still holds for
  /// the UE (TS 37.483 section 9.3.2.6).
  std::optional<up_transport_layer_info> egress_dl_data_forwarding_tnl_info;

  /// Pool to release the TEID of the DL data forwarding tunnel on.
  gtpu_teid_pool& ngu_teid_allocator;

  std::unique_ptr<f1u_cu_up_gateway_bearer> f1u_gw_bearer;
  std::unique_ptr<f1u_bearer>               f1u;
  std::unique_ptr<pdcp_entity>              pdcp;

  // Adapter PDCP->SDAP
  // FIXME: Currently, we assume only one DRB per PDU session and only one QoS flow per DRB.
  pdcp_sdap_adapter pdcp_to_sdap_adapter;
  pdcp_f1u_adapter  pdcp_to_f1u_adapter;
  f1u_pdcp_adapter  f1u_to_pdcp_adapter;

  // Adapter PDCP->CU-UP Manager.
  // Used for control events.
  pdcp_rx_cu_up_mngr_adapter pdcp_rx_to_cu_up_mngr_adapter;
  pdcp_tx_cu_up_mngr_adapter pdcp_tx_to_cu_up_mngr_adapter;

  // Adapter F1-U gateway -> NR-U
  f1u_gateway_rx_nru_adapter f1u_gateway_rx_to_nru_adapter;

  uint8_t cell_group_id; /// This can/should be a list of cell groups.

  std::map<qos_flow_id_t, std::unique_ptr<qos_flow_context>> qos_flows; // key is qos_flow_id
};

} // namespace ocuup
} // namespace ocudu
