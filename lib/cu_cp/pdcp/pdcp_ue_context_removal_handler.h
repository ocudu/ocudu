// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

/// Handler to remove per-UE PDCP state (SRB PDCP entities) owned by the DU processor.
class pdcp_ue_context_removal_handler
{
public:
  virtual ~pdcp_ue_context_removal_handler() = default;

  /// \brief Remove the SRB PDCP entities for the given UE.
  /// \param[in] ue_index The index of the UE whose PDCP entities are to be removed.
  virtual void remove_ue_context(cu_cp_ue_index_t ue_index) = 0;
};

} // namespace ocudu::ocucp
