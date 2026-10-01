// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/mac/mac_factory.h"
#include "mac_impl.h"

using namespace ocudu;

std::unique_ptr<mac_interface> ocudu::create_mac(const mac_config& mac_cfg)
{
  std::unique_ptr<mac_interface> mac_obj = std::make_unique<mac_impl>(mac_cfg);
  return mac_obj;
}
