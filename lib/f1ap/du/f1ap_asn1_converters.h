// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/du/f1ap_du_connection_manager.h"

namespace asn1 {
namespace f1ap {

struct served_cell_info_s;

}
} // namespace asn1

namespace ocudu {
namespace odu {

asn1::f1ap::served_cell_info_s make_asn1_served_cell_info(const du_served_cell_info& served_cell,
                                                          span<const s_nssai_t>      slices);

}
} // namespace ocudu
