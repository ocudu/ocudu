// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_fapi_p7_sector_fastpath_adaptor_factory_impl.h"
#include "mac_fapi_p7_sector_fastpath_adaptor_impl.h"
#include "mac_fapi_p7_sector_fastpath_adaptor_impl_config.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace fapi_adaptor;

std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor> ocudu::fapi_adaptor::create_mac_fapi_p7_sector_fastpath_adaptor(
    const mac_fapi_p7_sector_fastpath_adaptor_config&     config,
    mac_fapi_p7_sector_fastpath_adaptor_impl_dependencies dependencies)
{
  return std::make_unique<mac_fapi_p7_sector_fastpath_adaptor_impl>(config, std::move(dependencies));
}
