// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lower_phy_sector_timing_handler.h"

using namespace ocudu;

namespace {

/// Lower PHY sector timing notifier dummy implementation.
class lower_phy_sector_timing_notifier_dummy : public lower_phy_timing_notifier
{
public:
  // See interface for documentation.
  void on_tti_boundary(const lower_phy_timing_context& context) override {}

  // See interface for documentation.
  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override {}

  // See interface for documentation.
  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override {}
};

} // namespace

static lower_phy_sector_timing_notifier_dummy dummy_notifier;

lower_phy_sector_timing_handler::lower_phy_sector_timing_handler(lower_phy_timing_notifier* notifier_) :
  notifier(notifier_ ? notifier_ : &dummy_notifier)
{
}

void lower_phy_sector_timing_handler::on_tti_boundary(const lower_phy_timing_context& context)
{
  notifier->on_tti_boundary(context);
}

void lower_phy_sector_timing_handler::on_ul_half_slot_boundary(const lower_phy_timing_context& context)
{
  notifier->on_ul_half_slot_boundary(context);
}

void lower_phy_sector_timing_handler::on_ul_full_slot_boundary(const lower_phy_timing_context& context)
{
  notifier->on_ul_full_slot_boundary(context);
}
