// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"

namespace ocudu {

struct pucch_resource;
struct ran_cell_config;

/// \brief Computes a CRB bitmap marking all CRBs occupied by PUCCH (common + dedicated resources).
///
/// \param[in] ul_bwp_crbs        CRB interval of the initial UL BWP.
/// \param[in] pucch_res_common   Higher-layer parameter \e pucch-ResourceCommon (index into TS38.213 Table 9.2.1-1).
/// \param[in] ded_pucch_resources Span of dedicated PUCCH resources configured for the cell.
/// \return A CRB bitmap of size \c ul_bwp_crbs.length() with bits set for every CRB used by PUCCH.
crb_bitmap
compute_pucch_crbs(crb_interval ul_bwp_crbs, unsigned pucch_res_common, span<const pucch_resource> ded_pucch_resources);

/// \brief Computes a CRB bitmap marking all CRBs occupied by PUCCH, with the arguments of the overload above derived
/// from the cell configuration (the dedicated resources being the ones the cell generates for its UEs).
///
/// \param[in] cell_cfg Cell configuration. Its initial UL BWP must have PUCCH Config Common set.
/// \return A CRB bitmap of the size of the initial UL BWP, indexed relative to the start of that BWP.
crb_bitmap compute_pucch_crbs(const ran_cell_config& cell_cfg);

/// \brief Computes the CRB interval, within the UL BWP, that is free of the common PUCCH resources.
///
/// \param[in] ul_bwp_crbs CRB interval of the initial UL BWP.
/// \param[in] pucch_res_common Higher-layer parameter \e pucch-ResourceCommon (index into TS38.213 Table 9.2.1-1).
/// \return CRB interval, within \c ul_bwp_crbs, that is free of common PUCCH resources.
crb_interval compute_available_crbs_without_common_pucch(crb_interval ul_bwp_crbs, unsigned pucch_res_common);

} // namespace ocudu
