// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/result/csi_rs_info.h"
#include "ocudu/scheduler/result/pdcch_info.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include "ocudu/scheduler/result/prach_info.h"
#include "ocudu/scheduler/result/pucch_info.h"
#include "ocudu/scheduler/result/pusch_info.h"
#include "ocudu/scheduler/result/srs_info.h"

namespace ocudu {

/// Scheduler decision made for DL in a given slot.
struct dl_sched_result {
  /// Number of DL symbols active for this slot.
  unsigned nof_dl_symbols;
  /// Allocated DL PDCCHs. Includes both SIB, RAR and Data PDCCHs.
  pdcch_dl_info_list dl_pdcchs;
  /// Allocated UL PDCCHs.
  pdcch_ul_info_list ul_pdcchs;
  /// Allocation of SSB and SIBs.
  dl_broadcast_allocation bc;
  /// Allocation of dedicated RARs.
  static_vector<rar_information, MAX_RAR_PDUS_PER_SLOT> rar_grants;
  /// Allocation of Paging messages.
  static_vector<dl_paging_allocation, MAX_PAGING_PDUS_PER_SLOT> paging_grants;
  /// Allocation of dedicated UE messages.
  static_vector<dl_msg_alloc, MAX_UE_PDUS_PER_SLOT> ue_grants;
  /// Allocation of CSI-RS messages.
  static_vector<csi_rs_info, MAX_CSI_RS_PDUS_PER_SLOT> csi_rs;
};

/// Scheduler decision made for UL in a given slot.
struct ul_sched_result {
  /// Number of UL symbols active for this slot.
  unsigned nof_ul_symbols;
  /// PUSCH grants allocated in the current slot.
  static_vector<ul_sched_info, MAX_PUSCH_PDUS_PER_SLOT> puschs;
  /// PRACH occasions within the given slot.
  static_vector<prach_occasion_info, MAX_PRACH_OCCASIONS_PER_SLOT> prachs;
  /// PUCCH grants allocated in the current slot.
  static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT> pucchs;
  /// SRS grants allocated in the current slot.
  static_vector<srs_info, MAX_SRS_PDUS_PER_SLOT> srss;
};

/// Failed allocation attempts in a given slot.
struct failed_alloc_attempts {
  /// Number of failed DL PDCCH allocation attempts (common and UE-dedicated SearchSpaces).
  unsigned dl_pdcch = 0;
  /// Number of failed UL PDCCH allocation attempts (common and UE-dedicated SearchSpaces).
  unsigned ul_pdcch = 0;
  /// Number of failed DL PDCCH allocation attempts in common SearchSpaces. Subset of \c dl_pdcch.
  unsigned common_dl_pdcch = 0;
  /// Number of failed UL PDCCH allocation attempts in common SearchSpaces. Subset of \c ul_pdcch.
  unsigned common_ul_pdcch = 0;
  /// Number of failed UCI allocation attempts.
  unsigned uci = 0;
  /// Number of fallback DL grants dropped because the common HARQ-ACK PUCCH collided on every candidate k1.
  unsigned fallback_uci_allocs = 0;
};

/// Scheduler decision made for DL and UL in a given slot.
struct sched_result {
  /// Whether the scheduling for the given slot was successful.
  bool success;
  /// DL allocations for the given slot.
  dl_sched_result dl;
  /// UL allocations for the given slot.
  ul_sched_result ul;
  /// Information about failed allocation attempts for the given slot.
  failed_alloc_attempts failed_attempts;
};

} // namespace ocudu
