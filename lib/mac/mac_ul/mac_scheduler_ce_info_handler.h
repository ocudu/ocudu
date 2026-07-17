// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ul_bsr.h"
#include "ocudu/mac/ue_con_res_id.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/logical_channel/lcid_dl_sch.h"
#include "ocudu/ran/logical_channel/phr_report.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {

/// \brief Information and context relative to a decoded MAC CE BSR.
struct mac_bsr_ce_info {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
  bsr_format      bsr_fmt;
  /// \brief List of BSR reports decoded from the MAC CE BSR for different LCG IDs.
  lcg_bsr_report_list lcg_reports;
};

/// \brief Command used by MAC to force the scheduling of an UL grant for a UE in the scheduler.
struct mac_ul_scheduling_command {
  du_cell_index_t cell_index;
  slot_point      sl_rx;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
};

/// \brief Command used by MAC to trigger the scheduling of a DL MAC CE for a UE.
struct mac_ce_scheduling_command {
  du_ue_index_t ue_index;
  lcid_dl_sch_t ce_lcid;
};

/// \brief Information and context relative to a decoded MAC CE PHR.
struct mac_phr_ce_info {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
  slot_point      slot_rx;
  phr_report      phr;
};

/// \brief Interface between MAC and scheduler that is used by MAC to forward MAC CE information and force UL grants.
class mac_scheduler_ce_info_handler
{
public:
  virtual ~mac_scheduler_ce_info_handler() = default;

  /// \brief Forward to scheduler any decoded UL BSRs for a given UE.
  virtual void handle_ul_bsr_indication(const mac_bsr_ce_info& bsr) = 0;

  /// \brief Force the UL grant scheduling for a given UE.
  virtual void handle_ul_sched_command(const mac_ul_scheduling_command& sched_cmd) = 0;

  /// \brief Command scheduling of DL MAC CE for a given UE.
  /// \param mac_ce DL MAC CE to be scheduled.
  virtual void handle_dl_mac_ce_indication(const mac_ce_scheduling_command& mac_ce) = 0;

  /// \brief Forward to scheduler any decoded UL PHRs for a given UE.
  virtual void handle_ul_phr_indication(const mac_phr_ce_info& phr) = 0;

  /// \brief Forward to scheduler any notification of a received MAC CRNTI CE.
  virtual void handle_crnti_ce_indication(du_ue_index_t old_ue_index, du_cell_index_t cell_index) = 0;

  /// \brief Resolves the TC-RNTI allocated to a 2-step RACH (MsgA) PUSCH transmission, given its RA-RNTI and RAPID.
  /// If resolved, also registers the UE Contention Resolution Identity decoded from the same CCCH SDU, so it is
  /// later echoed back in the successRAR MAC subPDU when this preamble's MsgB is scheduled.
  /// \return The TC-RNTI, if a matching, non-expired entry exists; std::nullopt otherwise.
  virtual std::optional<rnti_t> handle_msga_ccch_sdu(du_cell_index_t        cell_index,
                                                     rnti_t                 ra_rnti,
                                                     uint8_t                rapid,
                                                     slot_point             sl_rx,
                                                     const ue_con_res_id_t& con_res_id) = 0;
};

} // namespace ocudu
