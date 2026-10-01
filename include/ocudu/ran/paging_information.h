// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/ran/time/radio_frame.h"

namespace ocudu {

/// Type of Paging. RAN initiated or CN initiated.
enum class paging_identity_type { ran_ue_paging_identity, cn_ue_paging_identity };

/// Description of eDRX paging information.
struct paging_edrx_information {
  /// Number of radio frames of the eDRX cycle in idle.
  radio_frames cycle_idle;
  /// Paging time window (PTW) length for eDRX, in multiples of 1.28 seconds as per TS 38.473.
  std::optional<radio_frames> ptw_len;
  /// \brief UE_ID_H of 13 bits specified in TS 38.413 and TS 38.473, and used for eDRX as per TS 38.304.
  uint32_t hashed_ue_identity;
};

struct paging_information {
  /// Cells at which to perform Paging of UE.
  std::vector<du_cell_index_t> paging_cells;
  /// Type of Paging Identity assigned to UE.
  paging_identity_type paging_type_indicator;
  /// Paging identity assigned to UE. Possible values are \c I-RNTI-Value (Bit string of size 40) and \c NG-5G-S-TMSI
  /// (Bit string of size 48). See TS 38.331.
  uint64_t paging_identity;
  /// UE_ID: 5G-S-TMSI mod 1024 if eDRX not enabled, and 5G-S-TMSI mod 4096, if enabled. Used by the paging scheduler
  /// to calculate the Paging Frame and Paging Occasion.
  /// \remark See TS 38.304, clause 7.1.
  unsigned ue_identity;
  /// Paging DRX cycle specified by upper layers.
  std::optional<radio_frames> paging_drx;
  /// Paging eDRX information, if available.
  std::optional<paging_edrx_information> edrx;
  /// Values {1,...,8}. Lower value indicates higher priority.
  std::optional<unsigned> paging_priority;
  bool                    is_paging_origin_non_3gpp_access{false};
};

} // namespace ocudu
