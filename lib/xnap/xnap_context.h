// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/ran/cu_cp_cell_configuration.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/xnap/xn_setup.h"

namespace ocudu::ocucp {

struct xnap_context {
  gnb_id_t                               gnb_id;
  std::vector<supported_tracking_area>   tai_support_list;
  std::vector<xnap_amf_region_info_item> amf_region_info;
  std::vector<cu_cp_served_cell_info>    list_of_served_cells_nr;
};

} // namespace ocudu::ocucp
