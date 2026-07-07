// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "upper_phy_operation_controller_impl.h"

using namespace ocudu;

namespace {

/// Dummy timing notifier used to initialize the proxy pointers (so they are never null) and as the
/// forwarding target while the controller is stopped.
class upper_phy_timing_notifier_dummy : public upper_phy_timing_notifier
{
public:
  void on_tti_boundary(const upper_phy_timing_context& context) override {}
};

} // namespace

static upper_phy_timing_notifier_dummy dummy_timing_notifier;

upper_phy_operation_controller_impl::timing_notifier_proxy::timing_notifier_proxy() :
  connected_notifier(&dummy_timing_notifier), active_notifier(&dummy_timing_notifier)
{
}

void upper_phy_operation_controller_impl::timing_notifier_proxy::set_active(bool active_state)
{
  active_notifier.store(active_state ? connected_notifier : &dummy_timing_notifier, std::memory_order_relaxed);
}
