// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/upper_phy_operation_controller.h"
#include "ocudu/phy/upper/upper_phy_timing_notifier.h"
#include <atomic>

namespace ocudu {

/// \brief Concrete upper PHY operation controller used in the monolithic DU.
///
/// Gates the propagation of timing notifications towards higher layers: a private proxy
/// implementing \ref upper_phy_timing_notifier sits between the upper PHY timing handler and the
/// external notifier, forwarding slot boundary events only while the controller is started. As the
/// FAPI P5 procedures drive start() and stop(), this provides full control over when slot
/// indications are generated for the MAC without extending the FAPI or PHY interfaces.
///
/// The proxy starts in the forwarding state so cells that never exercise the FAPI P5 lifecycle
/// observe no behaviour change, and the first activation of each cell at DU init (which cannot
/// complete the START handshake while the control executors are not yet running) finds slot
/// indications already flowing.
class upper_phy_operation_controller_impl : public upper_phy_operation_controller
{
public:
  // See interface for documentation.
  void start() override { proxy.set_active(true); }

  // See interface for documentation.
  void stop() override { proxy.set_active(false); }

  /// Connects the external notifier that receives the forwarded slot boundary events.
  void connect_timing_notifier(upper_phy_timing_notifier& notifier) { proxy.connect(notifier); }

  /// Returns the timing notifier proxy to be registered into the upper PHY timing handler.
  upper_phy_timing_notifier& get_timing_notifier_proxy() { return proxy; }

private:
  /// Forwards timing notifications to the connected notifier while the controller is started.
  class timing_notifier_proxy : public upper_phy_timing_notifier
  {
  public:
    timing_notifier_proxy();

    // See interface for documentation.
    void on_tti_boundary(const upper_phy_timing_context& context) override
    {
      if (active.load(std::memory_order_relaxed)) {
        notifier->on_tti_boundary(context);
      }
    }

    /// Enables or disables forwarding. The new state takes effect on the next slot boundary.
    void set_active(bool active_state) { active.store(active_state, std::memory_order_relaxed); }

    /// Connects the notifier that receives the forwarded events.
    void connect(upper_phy_timing_notifier& notif) { notifier = &notif; }

  private:
    /// Forwarding state. Read on the cell executor on every slot boundary; written on the control
    /// executor by the FAPI P5 START and STOP procedures, hence atomic.
    std::atomic<bool> active{true};
    /// Connected notifier. Points to a dummy until \ref connect is called, so it is never null and no
    /// per-slot check is needed. Connected once at init before the slot path runs, hence not atomic.
    upper_phy_timing_notifier* notifier;
  };

  timing_notifier_proxy proxy;
};

} // namespace ocudu
