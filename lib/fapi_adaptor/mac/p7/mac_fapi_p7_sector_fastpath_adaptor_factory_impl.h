// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "mac_fapi_p7_sector_fastpath_adaptor_impl_config.h"
#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor.h"
#include <memory>

namespace ocudu {
namespace fapi_adaptor {

struct mac_fapi_p7_sector_fastpath_adaptor_config;

/// Creates a MAC-FAPI P7 sector fastpath adaptor.
std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor>
create_mac_fapi_p7_sector_fastpath_adaptor(const mac_fapi_p7_sector_fastpath_adaptor_config&     config,
                                           mac_fapi_p7_sector_fastpath_adaptor_impl_dependencies dependencies);

} // namespace fapi_adaptor
} // namespace ocudu
