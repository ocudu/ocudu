// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_bearer.h"

namespace ocudu {
namespace odu {

/// \brief Bearer container for a UE object in the DU manager.
class du_ue_bearer_manager
{
public:
  /// Add new SRB with specified SRB-Id and configuration.
  du_ue_srb& add_srb(srb_id_t srb_id);

  /// Add new bearer to list of bearers.
  void add_drb(std::unique_ptr<du_ue_drb> drb);

  std::unique_ptr<du_ue_drb> remove_drb(drb_id_t drb_id);

  const slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS>& srbs() const { return srbs_; }
  slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS>&       srbs() { return srbs_; }
  const std::map<drb_id_t, std::unique_ptr<du_ue_drb>>&      drbs() const { return drbs_; }

  std::optional<lcid_t> allocate_lcid() const;

private:
  slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS> srbs_;
  std::map<drb_id_t, std::unique_ptr<du_ue_drb>>      drbs_;
};

} // namespace odu
} // namespace ocudu
