// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PDCCH Modulation and Signalling parameter derivation as per TS38.211, 7.3.2.3 and 7.4.1.3.

#pragma once

#include "ocudu/ran/pci.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/ran/pdcch/search_space.h"

namespace ocudu {

/// Calculates \f$n_ID\f$ as per TS38.211 7.3.2.3.
/// \return integer within values: {0,1,...,65535}.
inline unsigned
get_scrambling_n_ID(pci_t pci, const coreset_configuration& cs_cfg, const search_space_configuration& ss_cfg)
{
  // For a UE-specific search space [...] equals the higher-layer parameter pdcch-DMRS-ScramblingID if configured,
  if (not ss_cfg.is_common_search_space() and cs_cfg.get_pdcch_dmrs_scrambling_id().has_value()) {
    return *cs_cfg.get_pdcch_dmrs_scrambling_id();
  }
  // \f$n_id = N_{ID}^{cell}\f$ otherwise.
  return pci;
}

/// Calculates \f$n_{RNTI}\f$ as per TS38.211, 7.3.2.3.
/// \return integer within values: {0,1,...,65535}.
inline unsigned
get_scrambling_n_RNTI(rnti_t rnti, const coreset_configuration& cs_cfg, const search_space_configuration& ss_cfg)
{
  // \f$n_{RNTI}\f$ is given by the C-RNTI for a PDCCH in a UE-speicfic search space if the higher-layer parameter
  // pdcch-DMRS-ScramblingID is configured.
  if (not ss_cfg.is_common_search_space() and cs_cfg.get_pdcch_dmrs_scrambling_id().has_value()) {
    return to_underlying(rnti);
  }
  return 0;
}

/// Calculates \f$N_{ID}\f$ as per TS38.211, 7.4.1.3.1.
inline unsigned get_N_ID_dmrs(pci_t pci, const coreset_configuration& cs_cfg)
{
  if (cs_cfg.get_pdcch_dmrs_scrambling_id().has_value()) {
    return cs_cfg.get_pdcch_dmrs_scrambling_id().value();
  }
  return pci;
}

} // namespace ocudu
