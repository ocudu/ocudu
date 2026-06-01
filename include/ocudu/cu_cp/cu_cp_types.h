// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/ran/cause/common.h"
#include "ocudu/ran/cause/e1ap_cause.h"
#include "ocudu/ran/cause/f1ap_cause.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/crit_diagnostics.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/i_rnti.h"
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
#include <string>
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

struct cu_cp_five_g_s_tmsi {
  cu_cp_five_g_s_tmsi() = default;

  cu_cp_five_g_s_tmsi(const bounded_bitset<48>& five_g_s_tmsi_) : five_g_s_tmsi(five_g_s_tmsi_)
  {
    ocudu_assert(five_g_s_tmsi_.size() == 48, "Invalid size for 5G-S-TMSI ({})", five_g_s_tmsi_.size());
  }

  cu_cp_five_g_s_tmsi(uint64_t amf_set_id, uint64_t amf_pointer, uint64_t five_g_tmsi)
  {
    five_g_s_tmsi.emplace();
    five_g_s_tmsi->resize(48);
    five_g_s_tmsi->from_uint64((amf_set_id << 38U) + (amf_pointer << 32U) + five_g_tmsi);
  }

  uint16_t get_amf_set_id() const
  {
    ocudu_assert(five_g_s_tmsi.has_value(), "five_g_s_tmsi is not set");
    return five_g_s_tmsi.value().to_uint64() >> 38U;
  }

  uint8_t get_amf_pointer() const
  {
    ocudu_assert(five_g_s_tmsi.has_value(), "five_g_s_tmsi is not set");
    return (five_g_s_tmsi.value().to_uint64() & 0x3f00000000) >> 32U;
  }

  uint32_t get_five_g_tmsi() const
  {
    ocudu_assert(five_g_s_tmsi.has_value(), "five_g_s_tmsi is not set");
    return (five_g_s_tmsi.value().to_uint64() & 0xffffffff);
  }

  uint64_t to_number() const { return five_g_s_tmsi->to_uint64(); }

private:
  std::optional<bounded_bitset<48>> five_g_s_tmsi;
};

struct cu_cp_initial_ue_message {
  cu_cp_ue_index_t                   ue_index = cu_cp_ue_index_t::invalid;
  byte_buffer                        nas_pdu;
  establishment_cause_t              establishment_cause;
  cu_cp_user_location_info_nr        user_location_info;
  std::optional<cu_cp_five_g_s_tmsi> five_g_s_tmsi;
  std::optional<uint16_t>            amf_set_id;
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

struct cu_cp_recommended_cell_item {
  nr_cell_global_id_t     ngran_cgi;
  std::optional<uint16_t> time_stayed_in_cell;
};

struct cu_cp_recommended_cells_for_paging {
  std::vector<cu_cp_recommended_cell_item> recommended_cell_list;
};

struct cu_cp_global_gnb_id {
  plmn_identity plmn_id = plmn_identity::test_value();
  gnb_id_t      gnb_id;
};

struct cu_cp_amf_paging_target {
  bool                               is_global_ran_node_id;
  bool                               is_tai;
  std::optional<cu_cp_global_gnb_id> global_ran_node_id;
  std::optional<tai_t>               tai;
};

struct cu_cp_recommended_ran_node_item {
  cu_cp_amf_paging_target amf_paging_target;
};

struct cu_cp_recommended_ran_nodes_for_paging {
  std::vector<cu_cp_recommended_ran_node_item> recommended_ran_node_list;
};

struct cu_cp_info_on_recommended_cells_and_ran_nodes_for_paging {
  cu_cp_recommended_cells_for_paging     recommended_cells_for_paging;
  cu_cp_recommended_ran_nodes_for_paging recommended_ran_nodes_for_paging;
};

struct cu_cp_ue_context_release_complete {
  cu_cp_ue_index_t                           ue_index = cu_cp_ue_index_t::invalid;
  std::optional<cu_cp_user_location_info_nr> user_location_info;
  std::optional<cu_cp_info_on_recommended_cells_and_ran_nodes_for_paging>
                                    info_on_recommended_cells_and_ran_nodes_for_paging;
  std::vector<pdu_session_id_t>     pdu_session_res_list_cxt_rel_cpl;
  std::optional<crit_diagnostics_t> crit_diagnostics;
};

struct cu_cp_tai_list_for_paging_item {
  tai_t tai;
};

struct cu_cp_ue_radio_cap_for_paging {
  byte_buffer ue_radio_cap_for_paging_of_nr;
};

struct cu_cp_assist_data_for_recommended_cells {
  cu_cp_recommended_cells_for_paging recommended_cells_for_paging;
};

struct cu_cp_paging_attempt_info {
  uint8_t                    paging_attempt_count;
  uint8_t                    intended_nof_paging_attempts;
  std::optional<std::string> next_paging_area_scope;
};

struct cu_cp_assist_data_for_paging {
  std::optional<cu_cp_assist_data_for_recommended_cells> assist_data_for_recommended_cells;
  std::optional<cu_cp_paging_attempt_info>               paging_attempt_info;
};

struct cu_cp_paging_edrx_info {
  float                  nr_paging_edrx_cycle;
  std::optional<uint8_t> nr_paging_time_window;
};

struct cu_cp_paging_message {
  uint64_t                                         ue_id_idx_value = 0;
  std::variant<cu_cp_five_g_s_tmsi, full_i_rnti_t> ue_paging_id;
  std::optional<uint16_t>                          paging_drx;
  std::vector<cu_cp_tai_list_for_paging_item>      tai_list_for_paging;
  std::optional<uint8_t>                           paging_prio;
  std::optional<cu_cp_ue_radio_cap_for_paging>     ue_radio_cap_for_paging;
  std::optional<bool>                              paging_origin;
  std::optional<cu_cp_assist_data_for_paging>      assist_data_for_paging;
  std::optional<uint64_t>                          extended_ue_id_idx_value;
  std::optional<cu_cp_paging_edrx_info>            paging_edrx_info;
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
