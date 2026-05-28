// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/ran/cause/e1ap_cause.h"
#include "ocudu/ran/cause/f1ap_cause.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/crit_diagnostics.h"
#include "ocudu/ran/cu_cp_paging.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/ranac.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/ran/tac.h"
#include "ocudu/ran/tai.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace ocudu::ocucp {

/// Notification from the E1AP/F1AP that transaction reference information for some UEs has been lost.
struct ue_transaction_info_loss_event {
  std::vector<cu_cp_ue_index_t> ues_lost;
};

/// Common interface reset message for E1AP (E1 Reset), F1AP (F1 Reset) and NGAP (NG Reset).
struct cu_cp_reset {
  std::variant<e1ap_cause_t, f1ap_cause_t, ngap_cause_t> cause;
  bool                                                   interface_reset = false;
  std::vector<cu_cp_ue_index_t>                          ues_to_reset;
};

/// QoS Configuration, i.e. 5QI and the associated PDCP
/// and SDAP configuration for DRBs
struct cu_cp_qos_config {
  pdcp_config pdcp;
};

/// <AMF Identifier> = <AMF Region ID><AMF Set ID><AMF Pointer>
/// with AMF Region ID length is 8 bits, AMF Set ID length is 10 bits and AMF Pointer length is 6 bits
struct cu_cp_amf_identifier_t {
  uint8_t  amf_region_id = 0;
  uint16_t amf_set_id    = 0;
  uint8_t  amf_pointer   = 0;
};

struct cu_cp_initial_ue_message {
  cu_cp_ue_index_t               ue_index = cu_cp_ue_index_t::invalid;
  byte_buffer                    nas_pdu;
  establishment_cause_t          establishment_cause;
  cu_cp_user_location_info_nr    user_location_info;
  std::optional<five_g_s_tmsi_t> five_g_s_tmsi;
  std::optional<uint16_t>        amf_set_id;
};

struct cu_cp_ul_nas_transport {
  cu_cp_ue_index_t            ue_index = cu_cp_ue_index_t::invalid;
  byte_buffer                 nas_pdu;
  cu_cp_user_location_info_nr user_location_info;
};

struct cu_cp_tx_bw {
  subcarrier_spacing nr_scs;
  uint16_t           nr_nrb;
};

struct cu_cp_sul_info {
  uint32_t    sul_nr_arfcn;
  cu_cp_tx_bw sul_tx_bw;
};

struct cu_cp_supported_sul_freq_band_item {
  uint16_t freq_band_ind_nr;
};

struct cu_cp_freq_band_nr_item {
  uint16_t                                        freq_band_ind_nr;
  std::vector<cu_cp_supported_sul_freq_band_item> supported_sul_band_list;
};

struct cu_cp_nr_freq_info {
  uint32_t                             nr_arfcn;
  std::optional<cu_cp_sul_info>        sul_info;
  std::vector<cu_cp_freq_band_nr_item> freq_band_list_nr;
};

struct cu_cp_fdd_info {
  cu_cp_nr_freq_info ul_nr_freq_info;
  cu_cp_nr_freq_info dl_nr_freq_info;
  cu_cp_tx_bw        ul_tx_bw;
  cu_cp_tx_bw        dl_tx_bw;
};

struct cu_cp_tdd_info {
  cu_cp_nr_freq_info nr_freq_info;
  cu_cp_tx_bw        tx_bw;
};

using cu_cp_nr_mode_info = std::variant<cu_cp_fdd_info, cu_cp_tdd_info>;

struct cu_cp_served_cell_info {
  nr_cell_global_id_t        nr_cgi;
  pci_t                      nr_pci;
  std::optional<tac_t>       five_gs_tac;
  std::vector<plmn_identity> served_plmns;
  cu_cp_nr_mode_info         nr_mode_info;
  byte_buffer                meas_timing_cfg;
  std::optional<ranac_t>     ranac;

  cu_cp_served_cell_info() = default;
  cu_cp_served_cell_info(const cu_cp_served_cell_info& other) :
    nr_cgi(other.nr_cgi),
    nr_pci(other.nr_pci),
    five_gs_tac(other.five_gs_tac),
    served_plmns(other.served_plmns),
    nr_mode_info(other.nr_mode_info),
    meas_timing_cfg(other.meas_timing_cfg.copy())
  {
  }
  cu_cp_served_cell_info& operator=(const cu_cp_served_cell_info& other)
  {
    if (this != &other) {
      nr_cgi          = other.nr_cgi;
      nr_pci          = other.nr_pci;
      five_gs_tac     = other.five_gs_tac;
      served_plmns    = other.served_plmns;
      nr_mode_info    = other.nr_mode_info;
      meas_timing_cfg = other.meas_timing_cfg.copy();
    }
    return *this;
  }
};

struct cu_cp_gnb_du_sys_info {
  byte_buffer mib_msg;
  byte_buffer sib1_msg;

  cu_cp_gnb_du_sys_info() = default;
  cu_cp_gnb_du_sys_info(const cu_cp_gnb_du_sys_info& other) : mib_msg(other.mib_msg), sib1_msg(other.sib1_msg) {}
  cu_cp_gnb_du_sys_info& operator=(const cu_cp_gnb_du_sys_info& other)
  {
    if (this != &other) {
      mib_msg  = other.mib_msg.copy();
      sib1_msg = other.sib1_msg.copy();
    }
    return *this;
  }
};

struct cu_cp_du_served_cells_item {
  cu_cp_served_cell_info               served_cell_info;
  std::optional<cu_cp_gnb_du_sys_info> gnb_du_sys_info; // not optional for NG-RAN
};

/// NR carrier target for RRC Release with redirection (TS 38.331 Sec. 5.3.8.3).
struct cu_cp_release_redirect_nr_info {
  uint32_t           arfcn;                               ///< Target NR downlink ARFCN
  subcarrier_spacing ssb_scs = subcarrier_spacing::kHz15; ///< SSB subcarrier spacing
};

/// Command sent from the CU-CP to the lower layers to release the UE context.
struct cu_cp_ue_context_release_command {
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  ngap_cause_t     cause;
  // If true, the lower layers will send an RRC message e.g. RRCReject or RRCRelease to the UE as part of the context
  // release procedure.
  bool                                          requires_rrc_message = true;
  std::optional<std::chrono::seconds>           release_wait_time    = std::nullopt;
  std::optional<cu_cp_release_redirect_nr_info> redirect_nr_info     = std::nullopt;
};

/// Request sent from the lower layers to the CU-CP to request the release of an UE context.
struct cu_cp_ue_context_release_request {
  cu_cp_ue_index_t                              ue_index = cu_cp_ue_index_t::invalid;
  std::vector<pdu_session_id_t>                 pdu_session_res_list_cxt_rel_req;
  ngap_cause_t                                  cause;
  std::optional<cu_cp_release_redirect_nr_info> redirect_nr_info = std::nullopt;
};

/// \brief Indication from a DU that a UE has successfully accessed a target cell (CHO execution).
struct cu_cp_access_success_indication {
  cu_cp_ue_index_t    ue_index        = cu_cp_ue_index_t::invalid; ///< Target UE index (sender of Access Success).
  cu_cp_ue_index_t    source_ue_index = cu_cp_ue_index_t::invalid; ///< Resolved CHO source UE index.
  nr_cell_global_id_t cgi;
};

struct cu_cp_ue_context_release_complete {
  cu_cp_ue_index_t                           ue_index = cu_cp_ue_index_t::invalid;
  std::optional<cu_cp_user_location_info_nr> user_location_info;
  std::optional<cu_cp_info_on_recommended_cells_and_ran_nodes_for_paging>
                                    info_on_recommended_cells_and_ran_nodes_for_paging;
  std::vector<pdu_session_id_t>     pdu_session_res_list_cxt_rel_cpl;
  std::optional<crit_diagnostics_t> crit_diagnostics;
};

struct cu_cp_bearer_context_release_request {
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  ngap_cause_t     cause;
};

struct cu_cp_inactivity_notification {
  cu_cp_ue_index_t              ue_index    = cu_cp_ue_index_t::invalid;
  bool                          ue_inactive = false;
  std::vector<drb_id_t>         inactive_drbs;
  std::vector<pdu_session_id_t> inactive_pdu_sessions;
};

struct cu_cp_rrc_resume_request {
  cu_cp_ue_index_t    ue_index = cu_cp_ue_index_t::invalid;
  nr_cell_global_id_t cgi;
  rnti_t              new_c_rnti;
  resume_cause_t      cause;
};

} // namespace ocudu::ocucp
