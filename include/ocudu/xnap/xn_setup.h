// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/plmn_identity.h"
#include <cstdint>

namespace ocudu::ocucp {

struct xnap_amf_region_info_item {
  plmn_identity plmn = plmn_identity::test_value();
  uint8_t       amf_region_id;
};

} // namespace ocudu::ocucp
