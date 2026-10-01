// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_sector_controller.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace ofh;

void sector_controller::start()
{
  ofh_tx.get_operation_controller().start();
  ofh_rx.get_operation_controller().start();
}

void sector_controller::stop()
{
  ofh_tx.get_operation_controller().stop();
  ofh_rx.get_operation_controller().stop();
  // Repositories should be cleared last as during the stop procedure entries may still be added.
  slot_repo->clear();
  prach_repo->clear();
}
