// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_ue_cond_mobility_manager.h"

using namespace ocudu;
using namespace ocudu::odu;

void du_ue_cond_mobility_manager::set_success_access_required()
{
  success_access_required = true;
}

bool du_ue_cond_mobility_manager::is_success_access_required() const
{
  return success_access_required;
}

bool du_ue_cond_mobility_manager::handle_crnti_ce_indication()
{
  if (!success_access_required) {
    return false;
  }
  success_access_required = false;
  return true;
}
