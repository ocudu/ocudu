// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_high/du_high_ntn_sib19_update_handler_factory.h"
#include "du_high_ntn_sib19_update_handler_impl.h"

using namespace ocudu;

std::unique_ptr<ocudu_ntn::ntn_sib19_update_handler>
ocudu::create_ntn_sib19_update_handler(odu::du_configurator& du_cfgtr)
{
  auto sib19_msg_update_handler = std::make_unique<odu::du_high_ntn_sib19_update_handler_impl>(du_cfgtr);
  return sib19_msg_update_handler;
}
