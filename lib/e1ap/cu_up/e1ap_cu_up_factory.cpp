// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "e1ap_cu_up_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;
using namespace ocuup;

std::unique_ptr<e1ap_interface> ocudu::ocuup::create_e1ap(const e1ap_configuration&           e1ap_cfg,
                                                          const e1ap_cu_up_impl_dependencies& e1ap_dependencies)
{
  return std::make_unique<e1ap_cu_up_impl>(e1ap_cfg, e1ap_dependencies);
}
