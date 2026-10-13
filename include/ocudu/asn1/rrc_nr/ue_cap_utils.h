// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/rrc_nr/ue_cap.h"

namespace asn1::rrc_nr {

/// Returns UE-NR-Capability-v1700 when its extension chain is present, as per TS 38.331, Section 6.3.3.
inline const ue_nr_cap_v1700_s* get_ue_nr_cap_v1700(const ue_nr_cap_s& capabilities)
{
  auto next = [](const auto* extension) {
    return extension != nullptr && extension->non_crit_ext_present ? &extension->non_crit_ext : nullptr;
  };
  const auto* v1530 = next(&capabilities);
  const auto* v1540 = next(v1530);
  const auto* v1550 = next(v1540);
  const auto* v1560 = next(v1550);
  const auto* v1570 = next(v1560);
  const auto* v1610 = next(v1570);
  const auto* v1640 = next(v1610);
  const auto* v1650 = next(v1640);
  const auto* v1690 = next(v1650);
  return next(v1690);
}

} // namespace asn1::rrc_nr
