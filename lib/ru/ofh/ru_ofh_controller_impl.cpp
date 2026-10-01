// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_ofh_controller_impl.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

void ru_ofh_controller_impl::start()
{
  logger.info("Starting the operation of the Open Fronthaul interface");
  for (auto* sector : sector_controllers) {
    sector->start();
  }
  logger.info("Started the operation of the Open Fronthaul interface");
}

void ru_ofh_controller_impl::stop()
{
  logger.info("Stopping the operation of the Open Fronthaul interface");
  for (auto* sector : sector_controllers) {
    sector->stop();
  }
  logger.info("Stopped the operation of the Open Fronthaul interface");
}

void ru_ofh_controller_impl::set_sector_controllers(std::vector<ofh::operation_controller*> controllers)
{
  ocudu_assert(!controllers.empty(), "Invalid sector controllers");

  sector_controllers = std::move(controllers);

  ocudu_assert(std::all_of(sector_controllers.begin(),
                           sector_controllers.end(),
                           [](const auto& elem) { return elem != nullptr; }),
               "Invalid sector controller");
}
