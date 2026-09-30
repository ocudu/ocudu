// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/qos/qos_flow_id.h"
#include "ocudu/ran/qos/qos_parameters.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/rlc_mode.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include <map>

namespace ocudu::ocucp {

/// List of all supported 5QIs and their corresponding PDCP/SDAP configs.
struct up_resource_manager_cfg {
  /// Configuration for available 5QI.
  std::map<five_qi_t, cu_cp_qos_config> five_qi_config;
  uint8_t                               max_nof_drbs_per_ue;
};

struct up_qos_flow_context {
  qos_flow_id_t                 qfi = qos_flow_id_t::invalid;
  qos_flow_level_qos_parameters qos_params;
};

struct up_drb_context {
  ocudu::drb_id_t  drb_id         = drb_id_t::invalid;
  pdu_session_id_t pdu_session_id = pdu_session_id_t::invalid;
  s_nssai_t        s_nssai        = {};
  bool             default_drb    = false;
  rlc_mode         rlc_mod;
  /// DRB QoS params.
  qos_flow_level_qos_parameters qos_params;
  /// QoS flow IDs of all QoS flows mapped to this DRB.
  std::map<qos_flow_id_t, up_qos_flow_context> qos_flows;
  /// Allocated by CU-UP.
  std::vector<up_transport_layer_info> ul_up_tnl_info_to_be_setup_list;
  // True if, during inter-gNB handover admission, this DRB ID was confirmed-reused from the source's own
  // DRB-to-QoS-flow mapping (rather than arbitrarily/sequentially assigned).
  bool source_drb_id_confirmed = false;

  pdcp_config   pdcp_cfg;
  sdap_config_t sdap_cfg;
};

struct up_pdu_session_context {
  up_pdu_session_context(pdu_session_id_t id_, pdu_session_type_t type_, up_transport_layer_info ul_ngu_up_tnl_info_) :
    id(id_), type(type_), ul_ngu_up_tnl_info(ul_ngu_up_tnl_info_)
  {
  }

  pdu_session_id_t                   id = pdu_session_id_t::invalid;
  pdu_session_type_t                 type;
  up_transport_layer_info            ul_ngu_up_tnl_info;
  std::map<drb_id_t, up_drb_context> drbs;

  integrity_protection_result_t       integrity_protection_result = integrity_protection_result_t::performed;
  confidentiality_protection_result_t confidentiality_protection_result =
      confidentiality_protection_result_t::performed;
};

/// \brief This struct holds the UP configuration currently in place.
struct up_context {
  std::map<pdu_session_id_t, up_pdu_session_context> pdu_sessions;

  std::map<drb_id_t, pdu_session_id_t> drb_map;
  std::map<qos_flow_id_t, drb_id_t>    qos_flow_map;

  /// \brief Set of DRB IDs that were used with particular KgNB key, tracked to satisfy 3GPP TS 38.331 section 5.3.1.2.
  ///
  /// Before DRB ID reuse the gNB has to derive a new KgNB by triggering an intra-cell handover.
  std::bitset<MAX_NOF_DRBS> used_drb_ids;
};

} // namespace ocudu::ocucp
