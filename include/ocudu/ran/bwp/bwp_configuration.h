// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {

/// Generic parameters of a bandwidth part as defined in TS 38.211, clause 4.5 and TS 38.213, clause 12.
/// \remark See TS 38.331, Bandwidth-Part (BWP).
struct bwp_configuration {
  cyclic_prefix      cp;
  subcarrier_spacing scs;
  /// Common RBs where the BWP is located. CRB=0 overlaps with pointA.
  crb_interval crbs;

  bool operator==(const bwp_configuration& other) const
  {
    return cp == other.cp and scs == other.scs and crbs == other.crbs;
  }

  bool operator<(const bwp_configuration& other) const
  {
    return cp < other.cp or (cp == other.cp and scs < other.scs) or
           (cp == other.cp and scs == other.scs and crbs < other.crbs);
  }
};

} // namespace ocudu
