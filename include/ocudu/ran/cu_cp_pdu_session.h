// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/slotted_vector.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/s_nssai.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include <optional>

namespace ocudu::ocucp {

struct qos_flow_setup_request_item {
  qos_flow_id_t                 qos_flow_id = qos_flow_id_t::invalid;
  qos_flow_level_qos_parameters qos_flow_level_qos_params;
  std::optional<uint8_t>        erab_id;
  /// \brief Set if the source proposes this QoS flow for DL data forwarding.
  ///
  /// Reported in the Data Forwarding and Offloading Info from source NG-RAN node IE of the PDU session
  /// (TS 38.423 section 9.2.1.17).
  std::optional<bool> dl_forwarding;
};

enum class cu_cp_qos_flow_map_indication { ul = 0, dl };

struct cu_cp_associated_qos_flow {
  qos_flow_id_t                                qos_flow_id = qos_flow_id_t::invalid;
  std::optional<cu_cp_qos_flow_map_indication> qos_flow_map_ind;
};

struct cu_cp_drbs_to_qos_flows_map_item {
  drb_id_t                               drb_id = drb_id_t::invalid;
  std::vector<cu_cp_associated_qos_flow> associated_qos_flow_list;
};

struct cu_cp_pdu_session_res_setup_item {
  pdu_session_id_t                                              pdu_session_id = pdu_session_id_t::invalid;
  byte_buffer                                                   pdu_session_nas_pdu;
  s_nssai_t                                                     s_nssai;
  std::optional<uint64_t>                                       pdu_session_aggregate_maximum_bit_rate_dl;
  std::optional<uint64_t>                                       pdu_session_aggregate_maximum_bit_rate_ul;
  up_transport_layer_info                                       ul_ngu_up_tnl_info;
  pdu_session_type_t                                            pdu_session_type;
  std::optional<security_indication_t>                          security_ind;
  slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item> qos_flow_setup_request_items;
  /// \brief Set if the 5GC decided that this PDU session is not subject to data forwarding.
  ///
  /// Only carried by HANDOVER REQUEST and ignored otherwise (TS 38.413 section 9.3.4.1).
  std::optional<bool> data_forwarding_not_possible;
  /// \brief This source's own DRB-to-QoS-flow mapping for this PDU session.
  ///
  /// Reported in the Data Forwarding and Offloading Info from source NG-RAN node IE, so that the target can keep the
  /// DRB numbering of the source (TS 38.423 section 9.2.1.17).
  std::vector<cu_cp_drbs_to_qos_flows_map_item> source_drbs_to_qos_flows_map_list;
};

} // namespace ocudu::ocucp
