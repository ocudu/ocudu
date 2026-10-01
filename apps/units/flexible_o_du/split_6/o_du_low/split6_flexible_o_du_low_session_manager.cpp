// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_flexible_o_du_low_session_manager.h"
#include "split6_flexible_o_du_low_session_factory.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

bool split6_flexible_o_du_low_session_manager::on_start_request(const fapi::cell_configuration& config)
{
  // Call the factory.
  flexible_odu_low = odu_low_session_factory->create_o_du_low_session(config);

  // Return true when the flexible O-DU low was successfully created, otherwise false.
  return flexible_odu_low != nullptr;
}
