// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_flexible_o_du_low.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi_adaptor/mac/operation_controller.h"

using namespace ocudu;

split6_flexible_o_du_low::split6_flexible_o_du_low(split6_flexible_o_du_low_dependencies dependencies) :
  odu_low_session_manager(std::move(dependencies.odu_low_session_factory)),
  mac_p5_adaptor(std::move(dependencies.mac_p5_adaptor)),
  phy_p5_adaptor(std::move(dependencies.phy_p5_adaptor))
{
  ocudu_assert(mac_p5_adaptor, "Invalid MAC-FAPI P5 sector adaptor");
  ocudu_assert(phy_p5_adaptor, "Invalid PHY-FAPI P5 sector adaptor");

  // Connect MAC-PHY FAPI P5 adaptors.
  phy_p5_adaptor->set_p5_operational_change_request_notifier(odu_low_session_manager);
  phy_p5_adaptor->set_error_indication_notifier(mac_p5_adaptor->get_error_indication_notifier());
  phy_p5_adaptor->set_p5_responses_notifier(mac_p5_adaptor->get_p5_responses_notifier());
}

void split6_flexible_o_du_low::start()
{
  mac_p5_adaptor->get_operation_controller().start();
}

void split6_flexible_o_du_low::stop()
{
  mac_p5_adaptor->get_operation_controller().stop();

  // Clear running session.
  odu_low_session_manager.on_stop_request();
}
