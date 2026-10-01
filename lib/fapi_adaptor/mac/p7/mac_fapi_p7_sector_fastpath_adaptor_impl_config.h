// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor_config.h"

namespace ocudu {

class mac_cell_slot_handler;

namespace fapi_adaptor {

/// MAC-FAPI P7 sector fastpath adaptor implementation dependencies.
struct mac_fapi_p7_sector_fastpath_adaptor_impl_dependencies {
  mac_fapi_p7_sector_fastpath_adaptor_dependencies base_dependencies;
  mac_cell_slot_handler&                           slot_handler;
};

} // namespace fapi_adaptor
} // namespace ocudu
