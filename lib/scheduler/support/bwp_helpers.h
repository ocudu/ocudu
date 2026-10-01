// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "config_helpers.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/ran/pdcch/search_space.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/result/dci_info.h"
#include "ocudu/support/error_handling.h"

namespace ocudu {

/// \brief Convert PRB within a BWP into a Common RB, which use pointA as reference point.
/// The CRB and PRB are assumed to have the same numerology of the provided BWP configuration.
/// The existence of a CORESET#0 may also affect the rules for CRB<->PRB conversion.
/// \param bwp_cfg BWP configuration of the respective PRB.
/// \param prb PRB to be converted to CRB.
/// \return Calculated CRB.
inline unsigned prb_to_crb(const bwp_configuration& bwp_cfg, unsigned prb)
{
  ocudu_sanity_check(prb <= bwp_cfg.crbs.length(), "PRB={} falls outside BWP limits={}", prb, bwp_cfg.crbs);
  return prb + bwp_cfg.crbs.start();
}

/// Convert PRBs within a BWP into Common RBs, which use pointA as reference point. CRBs and PRBs are assumed to have
/// the same numerology of the provided BWP configuration.
/// \param bwp_cfg BWP configuration of the respective PRB interval.
/// \param prbs PRBs to be converted to CRBs.
/// \return Calculated CRB interval.
inline crb_interval prb_to_crb(const bwp_configuration& bwp_cfg, prb_interval prbs)
{
  return {prb_to_crb(bwp_cfg, prbs.start()), prb_to_crb(bwp_cfg, prbs.stop())};
}

/// \brief Convert CRB within a BWP into a PRB.
/// The CRB and PRB are assumed to use the same numerology as reference.
/// \param bwp_cfg BWP configuration of the respective CRB.
/// \param crb CRB to be converted to PRB.
/// \return Calculated PRB.
inline unsigned crb_to_prb(const bwp_configuration& bwp_cfg, unsigned crb)
{
  return crb_to_prb(bwp_cfg.crbs, crb);
}

/// Convert CRBs to PRBs within a BWP. CRBs and PRBs are assumed to have the same numerology of the provided
/// BWP configuration.
inline prb_interval crb_to_prb(const bwp_configuration& bwp_cfg, crb_interval crbs)
{
  return crb_to_prb(bwp_cfg.crbs, crbs);
}

} // namespace ocudu
