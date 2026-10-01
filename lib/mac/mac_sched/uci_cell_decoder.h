// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../rnti_value_table.h"
#include "rlf_detector.h"
#include "ocudu/adt/spmc_slot_ring.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/mac/mac_cell_control_information_handler.h"
#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"

namespace ocudu {

struct ul_sched_info;
struct pucch_info;

using du_rnti_table = rnti_value_table<du_ue_index_t, du_ue_index_t::INVALID_DU_UE_INDEX>;

class uci_cell_decoder
{
public:
  uci_cell_decoder(const sched_cell_configuration_request_message& cell_cfg,
                   const du_rnti_table&                            rnti_table_,
                   rlf_detector&                                   rlf_hdlr_);

  /// \brief Store information relative to expected UCIs to be decoded.
  void
  store_uci(slot_point uci_sl, span<const pucch_info> scheduled_pucchs, span<const ul_sched_info> scheduled_puschs);

  /// \brief Decode received MAC UCI indication and convert it to scheduler UCI indication.
  uci_indication decode_uci(const mac_uci_indication_message& msg);

private:
  struct uci_context {
    rnti_t                   rnti = rnti_t::INVALID_RNTI;
    csi_report_configuration csi_rep_cfg;
  };

  /// Expected UCI reports for a single slot, stored by store_uci and looked up by decode_uci.
  using slot_ucis = static_vector<uci_context, MAX_PUCCH_PDUS_PER_SLOT + MAX_PUSCH_PDUS_PER_SLOT>;

  const du_rnti_table&    rnti_table;
  du_cell_index_t         cell_index;
  bool                    aperiodic_csi_report;
  rlf_detector&           rlf_handler;
  ocudulog::basic_logger& logger;

  /// Ring buffer of expected UCI reports, keyed by slot. The slot-indication path (store_uci) is the single writer;
  /// the PHY may deliver a slot's UCIs concurrently across several worker threads, so decode_uci readers access it
  /// concurrently.
  spmc_slot_ring<slot_ucis> expected_uci_report_grid;
};

} // namespace ocudu
