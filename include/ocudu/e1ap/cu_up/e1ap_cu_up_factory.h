// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/cu_up/e1ap_configuration.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up.h"

namespace ocudu::ocuup {

/// Creates an instance of an E1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e1ap_interface> create_e1ap(const e1ap_configuration&           e1ap_cfg,
                                            const e1ap_cu_up_impl_dependencies& e1ap_dependencies);

} // namespace ocudu::ocuup
