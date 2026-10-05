// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/f1ap/common/f1ap_cho_types.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/f1ap/ue_context_management_configs.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/serv_cell_index.h"

namespace ocudu {
namespace odu {

/// \brief F1AP sends this request to the DU to create a new UE context. This happens in the particular case
/// of a F1AP UE Context Setup Request received without associated logical F1-connection.
struct f1ap_ue_context_creation_request {
  du_ue_index_t   ue_index;
  du_cell_index_t pcell_index;
};

/// \brief Response from the DU back to the F1AP with the created UE index.
struct f1ap_ue_context_creation_response {
  bool result;
  /// C-RNTI allocated during the UE creation, that the F1AP can send to the CU-CP in its response.
  rnti_t crnti;
};

/// \brief SCell to be setup in the UE context.
struct f1ap_scell_to_setup {
  serv_cell_index_t serv_cell_index;
  du_cell_index_t   cell_index;
};

struct f1ap_serving_cell_mo_list_item {
  uint8_t  serving_cell_mo;
  uint32_t ssb_freq;
};

/// \brief Request from DU F1AP to DU manager to modify existing UE configuration.
struct f1ap_ue_context_update_request {
  du_ue_index_t                                              ue_index;
  std::optional<nr_cell_global_id_t>                         spcell_id;
  std::optional<uint8_t>                                     serving_cell_mo;
  std::optional<std::vector<f1ap_serving_cell_mo_list_item>> serving_cell_mo_list;
  /// New SRBs to setup.
  std::vector<srb_id_t> srbs_to_setup;
  /// List of new DRBs to setup.
  std::vector<f1ap_drb_to_setup> drbs_to_setup;
  /// List of DRBs to modify.
  std::vector<f1ap_drb_to_modify> drbs_to_mod;
  /// List of DRBs to remove.
  std::vector<drb_id_t>            drbs_to_rem;
  std::vector<f1ap_scell_to_setup> scells_to_setup;
  std::vector<serv_cell_index_t>   scells_to_rem;
  /// \brief If true, the gnb-DU shall generate a cell group configuration using full configuration. Otherwise, delta,
  /// should be used.
  bool full_config_required;
  /// \brief measConfig selected by the CU-CP. If non-empty, the gnb-DU shall deduce which changes to measConfig need
  /// to be applied as per TS 38.473, 8.3.1.2.
  byte_buffer meas_cfg;
  /// \brief Optional HO preparation information. If present, the gnb-DU shall proceed with a reconfiguration with sync
  /// as defined in TS 38.331, and TS 38.473, 8.3.1.2.
  byte_buffer ho_prep_info;
  /// \brief If a source cell group config is included, the gnb-DU shall generate a cell group configuration using
  /// full configuration. Otherwise, delta configuration is allowed, as per TS 38.473, 8.3.1.2.
  byte_buffer source_cell_group_cfg;
  /// Container with the UE-CapabilityRAT-ContainerList, as per TS 38.331.
  byte_buffer ue_cap_rat_list;
  /// \brief Container with the LocationMeasurementInfo of the UE, as per TS 38.331. If non-empty, the gNB-DU configures
  /// a measurement gap for the location measurements of the UE.
  byte_buffer location_meas_info;
  /// Indiction that the CU-CP has received the RRC reconfiguration complete.
  bool rrc_recfg_complete_ind;
  /// \brief Optional CHO trigger. If set, this context update is part of a Conditional Handover procedure.
  std::optional<f1ap_cho_trigger> cho_trigger;
  /// \brief Optional target gNB-DU-UE-F1AP-ID for the inter-DU CHO replace case.
  std::optional<gnb_du_ue_f1ap_id_t> cho_target_gnb_du_ue_f1ap_id;
};

/// \brief Response from DU manager to DU F1AP with the result of the UE context update.
struct f1ap_ue_context_update_response {
  bool result;
  /// List of DRBs that were successfully setup.
  std::vector<f1ap_drb_setupmod> drbs_setup;
  /// List of DRBs that were successfully modified.
  std::vector<f1ap_drb_setupmod> drbs_mod;
  /// List of DRBs that failed to be setup.
  std::vector<f1ap_drb_failed_to_setupmod> failed_drbs_setups;
  /// List of DRBs that failed to be modified.
  std::vector<f1ap_drb_failed_to_setupmod> failed_drb_mods;
  /// List of servingCellMOs that have been encoded in CellGroupConfig IE.
  std::vector<uint8_t> serving_cell_mo_encoded_in_cgc_list;
  byte_buffer          cell_group_cfg;
  byte_buffer          meas_gap_cfg;
  bool                 full_config_present = false;
};

/// \brief Request Command for F1AP UE CONTEXT Release Request.
struct f1ap_ue_context_release_request {
  enum class cause_type { rlf_mac, rlf_rlc, other };

  du_ue_index_t ue_index;
  cause_type    cause;
};

/// \brief Notification that a UE has successfully accessed a target cell, (e.g., during CHO execution). See TS 38.473
/// Section 8.3.8.
struct f1ap_access_success_event {
  du_ue_index_t ue_index;
  /// NR-CGI of the cell where the UE successfully performed random access.
  nr_cell_global_id_t cgi;
};

/// \brief Request Command for F1AP UE CONTEXT Modification Required.
struct f1ap_ue_context_modification_required {};

} // namespace odu
} // namespace ocudu
