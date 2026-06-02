// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/ranac.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/tac.h"
#include "ocudu/rrc/meas_types.h"
#include "ocudu/security/security.h"
#include <string>
#include <vector>

namespace ocudu::ocucp {

/// Arguments for the RRC Reconfiguration procedure.

struct rrc_srb_to_add_mod {
  bool                       reestablish_pdcp_present = false;
  bool                       discard_on_pdcp_present  = false;
  srb_id_t                   srb_id                   = srb_id_t::nulltype;
  std::optional<pdcp_config> pdcp_cfg;
};

struct rrc_cn_assoc {
  std::optional<uint8_t>       eps_bearer_id;
  std::optional<sdap_config_t> sdap_cfg;
};

struct rrc_drb_to_add_mod {
  bool                        reestablish_pdcp_present = false;
  bool                        recover_pdcp_present     = false;
  std::optional<rrc_cn_assoc> cn_assoc;
  drb_id_t                    drb_id = drb_id_t::invalid;
  std::optional<pdcp_config>  pdcp_cfg;
};

struct rrc_security_algorithm_config {
  security::ciphering_algorithm                ciphering_algorithm;
  std::optional<security::integrity_algorithm> integrity_prot_algorithm;
};

struct rrc_security_config {
  std::optional<rrc_security_algorithm_config> security_algorithm_cfg;
  std::optional<std::string>                   key_to_use;
};

struct rrc_radio_bearer_config {
  /// \brief Returns true if at least one of the optional vectors/fields contains an element.
  bool contains_values() const
  {
    return (srb_to_add_mod_list.empty() || drb_to_add_mod_list.empty() || drb_to_release_list.empty() ||
            !security_cfg.has_value());
  }
  slotted_id_vector<srb_id_t, rrc_srb_to_add_mod> srb_to_add_mod_list;
  slotted_id_vector<drb_id_t, rrc_drb_to_add_mod> drb_to_add_mod_list;
  std::vector<drb_id_t>                           drb_to_release_list;
  std::optional<rrc_security_config>              security_cfg;
  bool                                            srb3_to_release_present = false;
};

struct rrc_master_key_upd {
  bool        key_set_change_ind = false;
  uint8_t     next_hop_chaining_count;
  byte_buffer nas_container;
};

struct rrc_delay_budget_report_cfg {
  std::string type;
  std::string delay_budget_report_prohibit_timer;
};

struct rrc_other_cfg {
  std::optional<rrc_delay_budget_report_cfg> delay_budget_report_cfg;
};

struct rrc_recfg_v1530_ies {
  bool                              full_cfg_present = false;
  byte_buffer                       master_cell_group;
  std::vector<byte_buffer>          ded_nas_msg_list;
  std::optional<rrc_master_key_upd> master_key_upd;
  byte_buffer                       ded_sib1_delivery;
  byte_buffer                       ded_sys_info_delivery;
  std::optional<rrc_other_cfg>      other_cfg;

  // TODO: Add rrc_recfg_v1540_ies_s
  // std::optional<rrc_recfg_v1540_ies> non_crit_ext;
};

using cond_recfg_id_t = ocudu::bounded_integer<uint8_t, 1, 8>;

struct cu_cp_ue_cho_candidate {
  cond_recfg_id_t     cond_recfg_id;
  pci_t               target_pci;
  nr_cell_global_id_t target_cgi;         // Target cell CGI (includes NCI) for measId lookup
  byte_buffer         prepared_rrc_recfg; // Plain ASN.1-encoded RRC Reconfiguration for target cell
};

struct rrc_reconfiguration_procedure_request {
  std::optional<rrc_radio_bearer_config> radio_bearer_cfg;
  byte_buffer                            secondary_cell_group;
  std::optional<rrc_meas_cfg>            meas_cfg;
  // Optional measurement gap config to include in Reconfiguration.
  byte_buffer                        meas_gap_cfg;
  std::optional<rrc_recfg_v1530_ies> non_crit_ext;

  /// When true the message is packed as plain ASN.1 (no DL-DCCH wrapper, no PDCP protection)
  /// for embedding in condRRCReconfiguration-r16. Default false (regular handover path).
  bool is_cho_preparation = false;
  /// CHO-execution fields.
  std::optional<std::vector<cu_cp_ue_cho_candidate>> cho_candidates;
  std::optional<cho_meas_id_map_t>                   cho_nci_to_meas_ids; // Maps NCI to measIds for CHO candidates
  /// Conditional reconfig IDs to remove (CHO cancellation). Adds condRecfgToRemList-r16 when set.
  std::optional<std::vector<cond_recfg_id_t>> cho_cancellation_ids;
};

struct rrc_ue_capability_transfer_request {
  // Empty for now but should include ratType and capabilityRequestFilter, etc.
};

struct rrc_ran_area_cfg_t {
  tac_t                tac;
  std::vector<ranac_t> ran_area_code_list;
};

struct rrc_plmn_ran_area_cfg_t {
  std::optional<plmn_identity>    plmn_id;
  std::vector<rrc_ran_area_cfg_t> ran_area;
};

struct rrc_plmn_ran_area_cell_t {
  std::optional<plmn_identity>  plmn_id;
  std::vector<nr_cell_identity> ran_area_cells;
};

using rrc_ran_notification_area_info_t =
    std::variant<std::vector<rrc_plmn_ran_area_cell_t>, std::vector<rrc_plmn_ran_area_cfg_t>>;

} // namespace ocudu::ocucp
