// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_intra_cu_ho_types.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ngap/ngap_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/guami.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/qos/qos_flow_id.h"
#include "ocudu/support/async/async_task.h"
#include <map>

namespace ocudu {

namespace ocucp {
class du_processor_repository;
class cu_cp_mobility_command_handler;
} // namespace ocucp

struct cu_handover_control_config {
  nr_cell_global_id_t                                    target_cell_id;
  std::map<pdu_session_id_t, std::vector<qos_flow_id_t>> pdu_sessions_for_ho;
  std::map<drb_id_t, std::vector<qos_flow_id_t>>         drbs_for_ho;
  std::vector<nr_cell_global_id_t>                       secondary_cells_to_be_added;
};

class cu_configurator
{
public:
  virtual ~cu_configurator() = default;

  /// Get UE index for a given GNB-CU-UE-F1AP-ID.
  virtual cu_cp_ue_index_t get_ue_index(const ocucp::amf_ue_id_t&  amf_ue_id,
                                        const guami_t&             guami,
                                        const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id) const = 0;

  /// Get index of the DU that serves the given UE.
  virtual cu_cp_du_index_t get_du_index(const cu_cp_ue_index_t& ue_index) const = 0;

  /// Get DU index for a given NR Cell Global ID.
  virtual cu_cp_du_index_t get_du_index(const nr_cell_global_id_t& nr_cgi) const = 0;

  /// Get PCI for a given NR Cell Global ID.
  virtual pci_t get_pci(const nr_cell_global_id_t& nr_cgi) const = 0;

  /// Trigger UE Handover.
  virtual async_task<ocucp::cu_cp_intra_cu_handover_response>
  trigger_handover(const cu_cp_du_index_t&                       source_du_index,
                   const ocucp::cu_cp_intra_cu_handover_request& handover_req) = 0;
};

} // namespace ocudu
