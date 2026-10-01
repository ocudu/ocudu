// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/ran/paging_information.h"
#include "ocudu/ran/time/radio_frame.h"

namespace ocudu {

struct sched_paging_information {
  /// Type of Paging. RAN initiated or CN initiated.
  paging_identity_type paging_type_indicator;
  /// Paging identity assigned to UE. Possible values are \c I-RNTI-Value (Bit string of size 40) and \c NG-5G-S-TMSI
  /// (Bit string of size 48). See TS 38.331.
  uint64_t paging_identity;
  /// Cells at which to perform Paging of UE.
  static_vector<du_cell_index_t, MAX_NOF_DU_CELLS> paging_cells;
  /// \brief UE_ID: 5G-S-TMSI mod 1024 for non eDRX and 5G-S-TMSI mode 4096 for eDRX.
  /// Used by the paging scheduler to calculate the Paging Frame.
  /// \remark See TS 38.304, clause 7.1.
  uint32_t ue_identity;
  /// Paging DRX cycle in radio frames, if provided by upper layers.
  std::optional<radio_frames> paging_drx;
  /// Paging eDRX cycle in radio frames.
  std::optional<paging_edrx_information> edrx;
};

/// Scheduler interface to handle paging a UE.
class scheduler_paging_handler
{
public:
  virtual ~scheduler_paging_handler() = default;

  /// Forward paging information to scheduler.
  virtual void handle_paging_information(const sched_paging_information& pi) = 0;
};

} // namespace ocudu
