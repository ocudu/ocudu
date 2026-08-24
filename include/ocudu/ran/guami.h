// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/plmn_identity.h"

namespace ocudu {

/// 3GPP TS 38.413 section 9.3.3.3, GUAMI (Globally unique AMF identifier)
struct guami_t {
  plmn_identity plmn          = plmn_identity::test_value();
  uint16_t      amf_set_id    = 0;
  uint8_t       amf_pointer   = 0;
  uint8_t       amf_region_id = 0;
};

} // namespace ocudu
