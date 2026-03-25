// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/units.h"

namespace ocudu {

struct harq_ack_event {
  du_ue_index_t              ue_index;
  rnti_t                     rnti;
  du_cell_index_t            cell_index;
  slot_point                 sl_ack_rx;
  harq_id_t                  h_id;
  mac_harq_ack_report_status ack;
  units::bytes               tbs;
};

struct sr_event {
  du_ue_index_t ue_index;
  rnti_t        rnti;
};

struct csi_report_event {
  du_ue_index_t   ue_index;
  rnti_t          rnti;
  slot_point      sl_rx;
  csi_report_data csi;
};

} // namespace ocudu
