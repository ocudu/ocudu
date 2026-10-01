// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/lower/lower_phy_timing_notifier.h"

namespace ocudu {

class lower_phy_sector_timing_handler : public lower_phy_timing_notifier
{
  lower_phy_timing_notifier* notifier = nullptr;

public:
  explicit lower_phy_sector_timing_handler(lower_phy_timing_notifier* notifier_);

  // See interface for documentation.
  void on_tti_boundary(const lower_phy_timing_context& context) override;

  // See interface for documentation.
  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override;

  // See interface for documentation.
  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override;
};

} // namespace ocudu
