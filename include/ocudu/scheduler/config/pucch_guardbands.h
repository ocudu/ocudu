// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"

namespace ocudu {

struct pucch_resource;

/// \brief Computes a CRB bitmap marking all CRBs occupied by PUCCH (common + dedicated resources).
///
/// \param[in] ul_bwp_crbs        CRB interval of the initial UL BWP.
/// \param[in] pucch_res_common   Higher-layer parameter \e pucch-ResourceCommon (index into TS38.213 Table 9.2.1-1).
/// \param[in] ded_pucch_resources Span of dedicated PUCCH resources configured for the cell.
/// \return A CRB bitmap of size \c ul_bwp_crbs.length() with bits set for every CRB used by PUCCH.
crb_bitmap
compute_pucch_crbs(crb_interval ul_bwp_crbs, unsigned pucch_res_common, span<const pucch_resource> ded_pucch_resources);

} // namespace ocudu
