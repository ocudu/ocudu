// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/five_g_s_tmsi.h"
#include "ocudu/ran/i_rnti.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/tai.h"
#include <variant>

namespace ocudu::ocucp {

struct cu_cp_recommended_cell_item {
  nr_cell_global_id_t     ngran_cgi;
  std::optional<uint16_t> time_stayed_in_cell;
};

struct cu_cp_recommended_cells_for_paging {
  std::vector<cu_cp_recommended_cell_item> recommended_cell_list;
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
  uint64_t                                     ue_id_idx_value = 0;
  std::variant<five_g_s_tmsi_t, full_i_rnti_t> ue_paging_id;
  std::optional<uint16_t>                      paging_drx;
  std::vector<cu_cp_tai_list_for_paging_item>  tai_list_for_paging;
  std::optional<uint8_t>                       paging_prio;
  std::optional<cu_cp_ue_radio_cap_for_paging> ue_radio_cap_for_paging;
  std::optional<bool>                          paging_origin;
  std::optional<cu_cp_assist_data_for_paging>  assist_data_for_paging;
  std::optional<uint64_t>                      extended_ue_id_idx_value;
  std::optional<cu_cp_paging_edrx_info>        paging_edrx_info;
};

} // namespace ocudu::ocucp
