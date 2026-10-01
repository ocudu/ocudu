// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/rrc/rrc_du_factory.h"
#include "rrc_du_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/rrc/rrc_config.h"

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<rrc_du> ocudu::ocucp::create_rrc_du(const rrc_cfg_t& cfg)
{
  return std::make_unique<rrc_du_impl>(cfg);
}
