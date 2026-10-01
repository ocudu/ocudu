// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_flexible_o_du_low_session.h"
#include "ocudu/du/du_low/du_low.h"
#include "ocudu/du/du_operation_controller.h"
#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_adaptor.h"
#include "ocudu/fapi_adaptor/phy/p7/phy_fapi_p7_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_sector_fastpath_adaptor.h"
#include "ocudu/phy/upper/upper_phy.h"
#include "ocudu/ru/ru_controller.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

split6_flexible_o_du_low_session::~split6_flexible_o_du_low_session()
{
  // Stop RU.
  ru->get_controller().get_operation_controller().stop();

  // Stop O-DU low.
  odu_low->get_operation_controller().stop();
}

void split6_flexible_o_du_low_session::set_dependencies(
    std::unique_ptr<fapi_adaptor::mac_fapi_p7_sector_adaptor> slot_msg_adaptor,
    std::unique_ptr<odu::o_du_low>                            du,
    std::unique_ptr<radio_unit>                               radio,
    unique_timer                                              timer)
{
  ocudu_assert(slot_msg_adaptor, "Invalid FAPI P7 message adaptor");
  ocudu_assert(du, "Invalid O-DU low");
  ocudu_assert(radio, "Invalid Radio Unit");

  mac_p7_adaptor = std::move(slot_msg_adaptor);
  odu_low        = std::move(du);
  ru             = std::move(radio);

  // Connect the RU adaptor to the RU.
  ru_dl_rg_adapt.connect(ru->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru->get_uplink_plane_handler());

  // Connect all the sectors of the DU low to the RU adaptors.
  for (unsigned i = 0; i != NOF_CELLS_SUPPORTED; ++i) {
    // Make connections between DU and RU.
    auto& upper = odu_low->get_du_low().get_upper_phy(i);
    ru_ul_adapt.map_handler(i, upper.get_rx_symbol_handler());
    ru_timing_adapt.map_handler(i, upper.get_timing_handler());
    ru_error_adapt.map_handler(i, upper.get_error_handler());

    // Connect adaptor with O-DU low.
    auto& fapi_adaptor = odu_low->get_phy_fapi_fastpath_adaptor().get_sector_adaptor(i).get_p7_sector_adaptor();
    fapi_adaptor.set_p7_slot_indication_notifier(mac_p7_adaptor->get_p7_slot_indication_notifier());
    fapi_adaptor.set_p7_indications_notifier(mac_p7_adaptor->get_p7_indications_notifier());
    fapi_adaptor.set_error_indication_notifier(mac_p7_adaptor->get_error_indication_notifier());
  }

  odu_low->get_operation_controller().start();
  ru->get_controller().get_operation_controller().start();

  // Initialize metrics collector.
  if (report_period.count() != 0 && notifier) {
    metrics_collector = split6_o_du_low_metrics_collector_impl(
        odu_low->get_metrics_collector(), ru->get_metrics_collector(), notifier, std::move(timer), report_period);
  }
}
