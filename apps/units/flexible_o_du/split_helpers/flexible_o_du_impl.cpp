// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "flexible_o_du_impl.h"
#include "ocudu/du/du_low/du_low.h"
#include "ocudu/du/du_low/o_du_low.h"
#include "ocudu/du/o_du.h"
#include "ocudu/phy/upper/upper_phy.h"
#include "ocudu/ru/ru.h"
#include "ocudu/ru/ru_controller.h"

using namespace ocudu;

flexible_o_du_impl::flexible_o_du_impl(unsigned nof_cells_, flexible_o_du_metrics_notifier* notifier) :
  nof_cells(nof_cells_),
  ru_ul_adapt(nof_cells_),
  ru_timing_adapt(nof_cells_),
  ru_error_adapt(nof_cells_),
  odu_metrics_handler(notifier)
{
}

flexible_o_du_impl::~flexible_o_du_impl()
{
  // Detach from the RU here as well as in stop(): the RU is destroyed before the DU, whose NTN update timers can
  // still reach this adapter. Covers the paths that destroy the O-DU without stopping it first.
  ru_doppler_adapt.disconnect();
}

void flexible_o_du_impl::start()
{
  // Start the RU before the DU. The DU activates its first cell during start(), and that activation
  // drives the FAPI P5 START procedure, which only completes once the first slot indication arrives.
  // Slot indications are produced by the RU, so it must already be running; otherwise the START
  // handshake times out and the cell fails to activate at startup.
  ru->get_controller().get_operation_controller().start();
  du->get_operation_controller().start();
}

void flexible_o_du_impl::stop()
{
  // Detach from the RU before any member is destroyed.
  ru_doppler_adapt.disconnect();
  ru->get_controller().get_operation_controller().stop();
  du->get_operation_controller().stop();
}

void flexible_o_du_impl::add_ru(std::unique_ptr<radio_unit> active_ru)
{
  ru = std::move(active_ru);
  ocudu_assert(ru, "Invalid Radio Unit");

  // Connect the RU adaptor to the RU.
  ru_dl_rg_adapt.connect(ru->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru->get_uplink_plane_handler());
  ru_doppler_adapt.connect(ru->get_controller());

  // Update the RU metrics collector.
  if (auto* collector = ru->get_metrics_collector()) {
    odu_metrics_handler.set_ru_metrics_collector(*collector);
  }
}

void flexible_o_du_impl::add_du(std::unique_ptr<odu::o_du> active_du)
{
  du = std::move(active_du);
  ocudu_assert(du, "Cannot set an invalid DU");

  // Connect all the sectors of the DU low to the RU adaptors.
  for (unsigned i = 0; i != nof_cells; ++i) {
    auto& upper = du->get_o_du_low().get_du_low().get_upper_phy(i);
    // Make connections between DU and RU.
    ru_ul_adapt.map_handler(i, upper.get_rx_symbol_handler());
    ru_timing_adapt.map_handler(i, upper.get_timing_handler());
    ru_error_adapt.map_handler(i, upper.get_error_handler());
  }
}
